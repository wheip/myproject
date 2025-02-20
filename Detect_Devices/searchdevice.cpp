#include "searchdevice.h"
#include "ui_searchdevice.h"
#include "camera.h"
#include "ui_camera.h"
#include "imageviewer.h" 
#include <QMessageBox>
#include <QBuffer>
#include <QMenu>
#include <QFileDialog>
#include <QThread>
#include <QtConcurrent>
#include <QDesktopServices>
SearchDevice::SearchDevice(QWidget *parent)
    : parent(parent), ui(new Ui::SearchDevice), db("searchdevice", this), yolomodel(YOLOModel::getInstance())
{
    ui->setupUi(this);
    qDebug() << "SearchDevice";
    adddevice = std::make_shared<Adddevice>(nullptr);
    pcbcomponentsdetect = PCBComponentsDetect::getInstance(nullptr);
    connect(pcbcomponentsdetect.get(), &PCBComponentsDetect::signal_save_image_label, this, &SearchDevice::current_Image);

    headerLabels << "设备ID" << "设备名称" << "驱动电压" << "驱动电流" << "驱动功率" << "图片" << "更新" << "删除"; // 使用 QStringList
    ui->tableWidget_devices->setColumnCount(headerLabels.size());
    ui->tableWidget_devices->setHorizontalHeaderLabels(headerLabels);
    ui->tableWidget_devices->setContextMenuPolicy(Qt::CustomContextMenu); // 设置上下文菜单策略
    ui->tableWidget_devices->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->setWindowTitle("搜索设备");
    this->close();
}

SearchDevice::~SearchDevice()
{
    delete ui;
}

void SearchDevice::populateTable(const vector<Device>& devices)
{
    clearTableWidgets(ui->tableWidget_devices);
    ui->tableWidget_devices->setHorizontalHeaderLabels(headerLabels);

    for (const Device& device : devices) {
        int row = ui->tableWidget_devices->rowCount();
        ui->tableWidget_devices->insertRow(row);
        populateDeviceRow(row, device);
    }
}

void SearchDevice::clearTableWidgets(QTableWidget *table)
{
    table->clear();
    for (int i = 0; i < table->rowCount(); ++i) {
        for (int j = 0; j < table->columnCount(); ++j) {
            QWidget *widget = table->cellWidget(i, j);
            if (widget) {
                table->removeCellWidget(i, j);
                delete widget;
            }
        }
    }
    table->setRowCount(0);
}

void SearchDevice::populateDeviceRow(int row, const Device& device) {
    ui->tableWidget_devices->setItem(row, 0, new QTableWidgetItem(device.id));
    ui->tableWidget_devices->item(row, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled); // 只可查看
    ui->tableWidget_devices->setItem(row, 1, new QTableWidgetItem(QString::fromUtf8(device.device_name)));
    ui->tableWidget_devices->setItem(row, 2, new QTableWidgetItem(QString::number(device.device_driver_Voltage)));
    ui->tableWidget_devices->setItem(row, 3, new QTableWidgetItem(QString::number(device.device_driver_Current)));
    ui->tableWidget_devices->setItem(row, 4, new QTableWidgetItem(QString::number(device.device_driver_Power)));

    // 创建图标项
    QTableWidgetItem *itemImage = new QTableWidgetItem("双击查看图片/编辑元件");
    itemImage->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget_devices->setItem(row, 5, itemImage);
    
    // 添加右键菜单
    itemImage->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    connect(ui->tableWidget_devices, &QTableWidget::customContextMenuRequested, this, [this, row, itemImage](const QPoint &pos) {
        QTableWidgetItem *itemAtPos = ui->tableWidget_devices->itemAt(pos);
        if (itemAtPos == itemImage) {
            showContextMenu(pos, row, itemImage);
        }
    });

    QPushButton *buttonUpdate = new QPushButton("更新");
    QPushButton *buttonDelete = new QPushButton("删除");
    ui->tableWidget_devices->setCellWidget(row, 6, buttonUpdate);
    ui->tableWidget_devices->setCellWidget(row, 7, buttonDelete);

    // 连接按钮的点击事件
    connect(buttonUpdate, &QPushButton::clicked, this, [this, device, row]() {
        QPushButton *button = qobject_cast<QPushButton*>(sender());
        button->setEnabled(false);
        updateDevice(device, row);
        button->setEnabled(true);
    });

    connect(buttonDelete, &QPushButton::clicked, this, [this, device, row]() {
        QPushButton *button = qobject_cast<QPushButton*>(sender());
        deleteDevice(device, row);
    });
}

void SearchDevice::showContextMenu(const QPoint &pos, int row, QTableWidgetItem *itemImage) {
    QMenu contextMenu(tr("上下文菜单"), parent);
    QAction actionDelete("删除图片", this);
    QAction actionReplace("添加/更换图片", this);
    QAction actionSave("另存为", this);
    
    connect(&actionDelete, &QAction::triggered, this, [this, row, itemImage]() {
        deleteImage(row, itemImage);
    });
    
    connect(&actionReplace, &QAction::triggered, this, [this, row, itemImage]() {
        QMenu replaceMenu(tr("选择来源"), this);
        QAction actionLocal("本地", this);
        QAction actionCamera("拍摄", this);

        connect(&actionLocal, &QAction::triggered, this, [this, row, itemImage]() {
            auto reply = QMessageBox::question(this, "选择图片", "更改图片将会删除所有元件标签和对应的测试任务，确定要更改图片吗？",
                                  QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::No) {
                return;
            }
            replaceImageFromLocal(row, itemImage);
        });

        connect(&actionCamera, &QAction::triggered, this, [this, row, itemImage]() {
            auto reply = QMessageBox::question(this, "选择图片", "更改图片将会删除所有元件标签和对应的测试任务，确定要更改图片吗？",
                                  QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::No) {
                return;
            }
            pcbcomponentsdetect->startrecog();
            replaceImageFromCamera_row = row;
            replaceImageFromCamera_itemImage = itemImage;
            replaceImageFromCamera_flag = true;
        });

        replaceMenu.addAction(&actionLocal);
        replaceMenu.addAction(&actionCamera);
        replaceMenu.exec(QCursor::pos());
    });

    connect(&actionSave, &QAction::triggered, this, [this, row, itemImage]() {
        saveImageToLocal(row, itemImage);
    });

    contextMenu.addAction(&actionDelete);
    contextMenu.addAction(&actionReplace);
    contextMenu.addAction(&actionSave);
    contextMenu.exec(ui->tableWidget_devices->viewport()->mapToGlobal(pos));
}

void SearchDevice::deleteImage(int row, QTableWidgetItem *itemImage) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "删除", "确定要删除设备的图片吗？",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        Device device_update = getDeviceFromRow(row);
        device_update.image = QByteArray(); // 清空图片数据
        QString ErrorMessage;
        if(!db.update_device(device_update, ErrorMessage, true))
            QMessageBox::warning(this, "更新失败", "删除图片" + device_update.id + "失败");
        itemImage->setText(""); // 清空图片单元格
    }
}

void SearchDevice::replaceImageFromLocal(int row, QTableWidgetItem *itemImage) {
    QFileDialog dialog(parent);
    dialog.setWindowTitle("选择图片");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Images (*.png *.jpg *.jpeg *.bmp)"));
    if (dialog.exec() == QDialog::Accepted) {
        QString filePath = dialog.selectedFiles().first();
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray imageData = file.readAll();
            file.close();
            currentDevice = getDeviceFromRow(row);
            currentImage = QImage::fromData(imageData);
            cv::Mat cvImage = cv::imread(filePath.toStdString());
            cv::Mat recog_image = yolomodel->recognize(cvImage);
            currentLabels.clear();
            delete_id.clear();
            currentLabels_add = yolomodel->get_labels();

            labelediting = std::make_unique<LabelEditingWindow>(nullptr, currentImage, currentLabels, currentLabels_add, delete_id);
            labelediting->show();

            connect(labelediting.get(), &LabelEditingWindow::window_close, [this]() {
                currentLabels_add.clear();
                labelediting->labelEditing->getAllLabelItemInfo(currentLabels_add);
                saveImage();
            });
        }
    }
}

void SearchDevice::replaceImageFromCamera() {
    currentDevice = getDeviceFromRow(replaceImageFromCamera_row);
    QProcess process;
    process.start("nvidia-smi --gpu-reset");
    process.waitForFinished();
    delete_id.clear();
    labelediting = std::make_unique<LabelEditingWindow>(nullptr, currentImage, currentLabels, currentLabels_add, delete_id);
    labelediting->show();

    connect(labelediting.get(), &LabelEditingWindow::window_close, [this]() {
        currentLabels_add.clear();
        labelediting->labelEditing->getAllLabelItemInfo(currentLabels_add);
        labelediting.reset();
        saveImage();
    });
}

void SearchDevice::saveImage() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "保存", "是否保存更改？",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {

        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        
        qDebug() << "currentImage.save(&buffer, \"PNG\");";
        currentImage.save(&buffer, "PNG"); // 将图像数据转换为QByteArray
        currentDevice.image = byteArray;

        qDebug() << "currentLabels.size(): " << currentLabels.size();

        QFuture<QString> updateFuture = QtConcurrent::run([this]() -> QString {
            Database ImageUpdateDb = Database("imageupdate", nullptr);
            QString ErrorMessage;
            if(!ImageUpdateDb.update_device(currentDevice, ErrorMessage, true)) {
                return ErrorMessage;
            }
            if(currentLabels.empty())
            {
                currentLabels.insert(currentLabels.end(), currentLabels_add.begin(), currentLabels_add.end());
                if(!ImageUpdateDb.delete_all_deviceelement(currentDevice.id, ErrorMessage)) {
                    return ErrorMessage;
                }
                if(!ImageUpdateDb.insert_deviceelement(currentDevice.id, currentLabels, ErrorMessage)) {
                    return ErrorMessage;
                }
            }else{
                std::vector<Label> update_temp;
                std::vector<Label> insert_temp;
                for(int i = 0; i < currentLabels_add.size(); i++)
                {
                    if(currentLabels_add[i].id > 0)
                    {
                        update_temp.push_back(currentLabels_add[i]);
                    }
                    else
                    {
                        insert_temp.push_back(currentLabels_add[i]);
                    }
                }
                if(!ImageUpdateDb.update_deviceelement(currentDevice.id, update_temp, ErrorMessage)) {
                    return ErrorMessage;
                }
                if(!ImageUpdateDb.insert_deviceelement(currentDevice.id, insert_temp, ErrorMessage)) {
                    return ErrorMessage;
                }
                if(!ImageUpdateDb.delete_deviceelement(currentDevice.id, delete_id, ErrorMessage)) {
                    return ErrorMessage;
                }
            }
            ImageUpdateDb.disconnect();
            return "";
        });
        QFutureWatcher<QString> *watcher = new QFutureWatcher<QString>();
        watcher->setFuture(updateFuture);
        QObject::connect(watcher, &QFutureWatcher<QString>::finished, [this, watcher]() {
            QString ErrorMessage = watcher->future().result();
            if (!ErrorMessage.isEmpty()) {
                QMessageBox::warning(this, "更新失败", "更新设备失败: " + ErrorMessage);
            } else {
                QMessageBox::information(this, "更新", "图片: " + currentDevice.id + " 更新成功");
            }
            watcher->deleteLater();
        });
    }
}

void SearchDevice::saveImageToLocal(int row, QTableWidgetItem *itemImage) {
    QString device_id = ui->tableWidget_devices->item(row, 0)->text();
    std::vector<Device> devices;
    if(db.get_device("id = '" + device_id + "'", devices))
    {
        QPixmap pixmap = QPixmap::fromImage(QImage::fromData(devices[0].image));
        QString filePath = QFileDialog::getSaveFileName(parent, tr("保存图片"), "", tr("Images (*.png *.jpg *.jpeg *.bmp)"));
        if (!filePath.isEmpty()) {
            pixmap.save(filePath);
            QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
        }
    }
    else
    {
        QMessageBox::warning(this, "保存失败", "设备" + device_id + "不存在");
    }
}

Device SearchDevice::getDeviceFromRow(int row) {
    Device device_update;
    device_update.id = ui->tableWidget_devices->item(row, 0)->text();
    device_update.device_name = ui->tableWidget_devices->item(row, 1)->text().toUtf8();
    device_update.device_driver_Voltage = ui->tableWidget_devices->item(row, 2)->text().toFloat();
    device_update.device_driver_Current = ui->tableWidget_devices->item(row, 3)->text().toFloat();
    device_update.device_driver_Power = ui->tableWidget_devices->item(row, 4)->text().toFloat();
    return device_update;
}

void SearchDevice::updateDevice(const Device& device, int row) {
    Device device_update = getDeviceFromRow(row);
    device_update.image = device.image;
    QString ErrorMessage;
    if(!db.update_device(device_update, ErrorMessage))
    {
        QMessageBox::warning(this, "更新失败", "更新设备" + device_update.id + "失败" + ErrorMessage);
        return;
    }

    QMessageBox::information(this, "更新", "更新设备: " + device.id);
}

void SearchDevice::deleteDevice(const Device& device, int row) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "删除", "确定要删除设备: " + device.id + " 吗？",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QFuture<QString> deleteFuture = QtConcurrent::run([this, device, row]() -> QString {
            Database DeleteDb = Database("device_delete", nullptr);
            QString ErrorMessage;
            if(!DeleteDb.delete_device(device.id, ErrorMessage))
            {
                DeleteDb.disconnect();
                return ErrorMessage;
            }
            DeleteDb.disconnect();
            return "";
        });
        QFutureWatcher<QString> *watcher = new QFutureWatcher<QString>();
        watcher->setFuture(deleteFuture);
        QObject::connect(watcher, &QFutureWatcher<QString>::finished, [this, watcher, row]() {
            QString ErrorMessage = watcher->future().result();
            if(!ErrorMessage.isEmpty())
            {
                QMessageBox::warning(this, "删除失败", ErrorMessage);
            }
            else
            {
                ui->tableWidget_devices->removeRow(row);
            }
            watcher->deleteLater();
        });
    }
}

void SearchDevice::on_pdsearch_clicked()
{
    QString device_id = ui->lineEdit_id->text();
    QString device_name = ui->lineEdit_name->text();
    QString sql;

    if (device_id.isEmpty() && device_name.isEmpty()) {
        vector<Device> devices;
        db.get_device("", devices);
        populateTable(devices);
    } else {
        if (!device_id.isEmpty()) {
            sql += "id = '" + device_id + "'";
        }
        if (!device_name.isEmpty()) {
            if (!sql.isEmpty()) {
                sql += " AND ";
            }
            sql += "device_name = '" + device_name.toUtf8() + "'";
        }

        vector<Device> devices;
        db.get_device(sql, devices);
        populateTable(devices);
    }
}

void SearchDevice::on_tableWidget_devices_itemDoubleClicked(QTableWidgetItem *item) {
    if (item->column() == 5) {
        QString device_id = ui->tableWidget_devices->item(item->row(), 0)->text();
        currentDevice = getDeviceFromRow(item->row());
        QFuture<QString> future = QtConcurrent::run([this, device_id, item]() -> QString {
            QString ErrorMessage;
            Database DeviceDb = Database("device_doubleclicked", nullptr);
            std::vector<Device> devices;
            if(DeviceDb.get_device("id = '" + device_id + "'", devices, true))
            {
                if(!devices.empty())
                {   
                    QPixmap pixmap = QPixmap::fromImage(QImage::fromData(devices[0].image));
                    QPixmap scaledPixmap = pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation); // 缩小图片
                    item->setText("");
                    item->setIcon(QIcon(scaledPixmap));
                    currentImage = QImage::fromData(devices[0].image);
                }
                currentLabels.clear();
                if(!DeviceDb.get_deviceelement(device_id, "", currentLabels, ErrorMessage))
                {
                    return ErrorMessage;
                }
            }
            DeviceDb.disconnect();
            return "";
        });
        QFutureWatcher<QString> *watcher = new QFutureWatcher<QString>();
        watcher->setFuture(future);
        QObject::connect(watcher, &QFutureWatcher<QString>::finished, [this, watcher]() {
            QString ErrorMessage = watcher->future().result();
            if(!ErrorMessage.isEmpty())
            {   
                QMessageBox::warning(this, "查看失败", ErrorMessage);
            }else{
                currentLabels_add.clear();
                delete_id.clear();
                labelediting = std::make_unique<LabelEditingWindow>(nullptr, currentImage, currentLabels, currentLabels_add, delete_id);
                labelediting->show();
                connect(labelediting.get(), &LabelEditingWindow::window_close, [this]() {
                    currentLabels_add.clear();
                    labelediting->labelEditing->getAllLabelItemInfo(currentLabels_add);
                    saveImage();
                });
            }
            watcher->deleteLater();
        });
    }
}

void SearchDevice::current_Image(const cv::Mat &image, const std::vector<Label>& labels) {
    if(image.empty())
    {
        QMessageBox::warning(this, "图片错误", "图片为空");
        return;
    }
    if (replaceImageFromCamera_flag) {
        pcbcomponentsdetect->close();
        cv::Mat image_cv;
        cv::cvtColor(image, image_cv, cv::COLOR_BGR2RGB);
        QImage tempImage = QImage(image_cv.data, image_cv.cols, image_cv.rows, image_cv.step, QImage::Format_RGB888);
        currentImage = tempImage.copy();
        currentLabels.clear();
        currentLabels_add.clear();
        currentLabels_add.insert(currentLabels_add.end(), labels.begin(), labels.end());
        replaceImageFromCamera();
    }
}


void SearchDevice::on_pbadddevice_clicked()
{
    adddevice->show();
    adddevice->raise();
    adddevice->activateWindow();
}

