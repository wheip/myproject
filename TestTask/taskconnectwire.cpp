#include "taskconnectwire.h"
#include "ui_taskconnectwire.h"
#include <QtConcurrent>

TaskConnectWire::TaskConnectWire(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TaskConnectWire)
{
    ui->setupUi(this);
    setWindowTitle("连接线配置");
    setModal(true);
    ui->pbbegin->setEnabled(false);
    ui->pbcomplete->setEnabled(false);
}

TaskConnectWire::~TaskConnectWire()
{
    delete ui;
}

void TaskConnectWire::setStep(const int step_id)
{
    this->step_id = step_id;
    ui->pbbegin->setEnabled(true);
}

void TaskConnectWire::setDevice(const int device_id)
{
    this->device_id = device_id;
    initimage();
}

void TaskConnectWire::initimage()
{
    label_info.clear();
    label_info_add.clear();
    delete_id.clear();
    if(labelEditingWindow) {
        ui->gridLayout->removeWidget(labelEditingWindow.get());
        ui->gridLayout->update();
    }
    QFuture<QString> future = QtConcurrent::run([this]() -> QString {
        Database threadDb(QUuid::createUuid().toString(), nullptr);
        if (!threadDb.connect()) {
            return QString("数据库连接失败");
        }
        std::vector<Device> devices;
        QString condition = "id = '" + QString::number(device_id) + "'";
        threadDb.get_device(condition, devices, true);
        if(devices.empty()) {
            return QString("设备查询失败");
        }
        LabeledingImage = QImage::fromData(devices[0].image);
        QString ErrorMessage;
        if(!threadDb.get_deviceelement(device_id, "", label_info, ErrorMessage)) {
            return ErrorMessage;
        }
        threadDb.disconnect();
        return QString("");
    });

    auto watcher = new QFutureWatcher<QString>(this);
    watcher->setFuture(future);
    connect(watcher, &QFutureWatcher<QString>::finished, this, [this, watcher]() {
        if (watcher->result().isEmpty()) {
            QMetaObject::invokeMethod(this, [this]() {
                initUI();
            }, Qt::QueuedConnection);
        }else{
            QMessageBox::warning(this, "提示", watcher->result());
        }
    });
}

void TaskConnectWire::initWaveform()
{
    QFuture<bool> future = QtConcurrent::run([this]() {
        pxie5320waveforms.clear();
        pxie5711waveforms.clear();
        data8902s.clear();
        Database threadDb(QUuid::createUuid().toString(), nullptr);
        if (!threadDb.connect()) {
            qDebug() << "线程数据库连接失败";
            return false;
        }
        QString tableName = QString::number(device_id) + "$$PXIe5320";
        QString condition = "step_id = '" + QString::number(step_id) + "'";
        threadDb.get_pxie5320waveform(tableName, condition, pxie5320waveforms);
        tableName = QString::number(device_id) + "$$PXIe5711";
        threadDb.get_pxie5711waveform(tableName, condition, pxie5711waveforms);
        tableName = QString::number(device_id) + "$$PXIe8902";
        threadDb.get_8902data(tableName, condition, data8902s);
        threadDb.disconnect();
        return true;
    });

    auto watcher = new QFutureWatcher<bool>(this);
    watcher->setFuture(future);
    connect(watcher, &QFutureWatcher<bool>::finished, this, [this, watcher]() {
        if (watcher->result()) {
            QMetaObject::invokeMethod(this, [this]() {
                beginwireconnect();
            }, Qt::QueuedConnection);
        } else {
            QMessageBox::warning(this, "提示", "波形初始化失败");
        }
        watcher->deleteLater();
    });
}

void TaskConnectWire::initUI()
{
    std::unique_lock<std::mutex> lock(uiMutex);
    labelEditingWindow = std::make_unique<LabelEditingWindow>(this, LabeledingImage, label_info, label_info_add, delete_id);
    labelEditingWindow->setOnlyViewModel();
    labelEditingWindow->setFixedWidth(1600);
    labelEditingWindow->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    ui->gridLayout->addWidget(labelEditingWindow.get(), 0, 0);
    ui->textBrowser->clear();
    is_init_ui = true;
    init_ui_cv.notify_one();
}

void TaskConnectWire::beginwireconnect()
{
    QFuture<void> future = QtConcurrent::run([this]() {
        std::unique_lock<std::mutex> threadLock(mtx);
        
        for(auto &waveform : pxie5320waveforms) {
            if(waveform.device == 5323) {
                QMetaObject::invokeMethod(this, [this, &waveform](){
                    showTip(QString("请将模拟量输入端口%1正极连接到如图所示位置").arg(waveform.port), waveform.positive_connect_location);
                }, Qt::BlockingQueuedConnection);

                is_connect_wire = false;
                cv.wait(threadLock, [this]() { return is_connect_wire; });

                QMetaObject::invokeMethod(this, [this, &waveform](){
                    showTip(QString("请将模拟量输入端口%1负极连接到如图所示位置").arg(waveform.port), waveform.negative_connect_location);
                }, Qt::BlockingQueuedConnection);
            }else if(waveform.device == 5322) {
                QMetaObject::invokeMethod(this, [this, &waveform](){
                    showTip(QString("请将数字量输出端口%1正极连接到如图所示位置").arg(waveform.port), waveform.positive_connect_location);
                }, Qt::BlockingQueuedConnection);

                is_connect_wire = false;
                cv.wait(threadLock, [this]() { return is_connect_wire; });

                QMetaObject::invokeMethod(this, [this, &waveform](){
                    showTip(QString("请将数字量输出端口%1负极连接到如图所示位置").arg(waveform.port), waveform.negative_connect_location);
                }, Qt::BlockingQueuedConnection);
            }
            is_connect_wire = false;
            cv.wait(threadLock, [this]() { return is_connect_wire; });
        }

        for(auto &waveform : pxie5711waveforms) {
            if(waveform.channel <= 15)
            {
                QMetaObject::invokeMethod(this, [this, &waveform](){
                    showTip(QString("请将模拟量输出端口%1正极连接到如图所示位置").arg(waveform.channel), waveform.positive_connect_location);
                }, Qt::BlockingQueuedConnection);

                is_connect_wire = false;
                cv.wait(threadLock, [this]() { return is_connect_wire; });

                QMetaObject::invokeMethod(this, [this, &waveform](){
                    showTip(QString("请将模拟量输出端口%1负极连接到如图所示位置").arg(waveform.channel), waveform.negative_connect_location);
                }, Qt::BlockingQueuedConnection);
                
                is_connect_wire = false;
                cv.wait(threadLock, [this]() { return is_connect_wire; });
            }else if(waveform.channel <= 26 && waveform.channel > 15) {
                QMetaObject::invokeMethod(this, [this, &waveform](){
                    showTip(QString("请将数字量输出端口%1正极连接到如图所示位置").arg(waveform.channel - 16), waveform.positive_connect_location);
                }, Qt::BlockingQueuedConnection);

                is_connect_wire = false;
                cv.wait(threadLock, [this]() { return is_connect_wire; });

                QMetaObject::invokeMethod(this, [this, &waveform](){
                    showTip(QString("请将数字量输出端口%1负极连接到如图所示位置").arg(waveform.channel - 16), waveform.negative_connect_location);
                }, Qt::BlockingQueuedConnection);
                
                is_connect_wire = false;
                cv.wait(threadLock, [this]() { return is_connect_wire; });
            }else if(waveform.channel > 26) {
                QMetaObject::invokeMethod(this, [this, &waveform](){
                    showTip(QString("请将电源输出端口%1正极连接到如图所示位置").arg(waveform.channel - 28), waveform.positive_connect_location);
                }, Qt::BlockingQueuedConnection);

                is_connect_wire = false;    
                cv.wait(threadLock, [this]() { return is_connect_wire; });

                QMetaObject::invokeMethod(this, [this, &waveform](){
                    showTip(QString("请将电源输出端口%1负极连接到如图所示位置").arg(waveform.channel - 28), waveform.negative_connect_location);
                }, Qt::BlockingQueuedConnection);
                
                is_connect_wire = false;
                cv.wait(threadLock, [this]() { return is_connect_wire; });
            }
        }

        for(auto &data : data8902s) {
            QMetaObject::invokeMethod(this, [this, &data](){
                showTip(QString("请将万用表正极连接到如图所示位置"), data.positive_connect_location);
            }, Qt::BlockingQueuedConnection);
            
            is_connect_wire = false;
            cv.wait(threadLock, [this]() { return is_connect_wire; });

            QMetaObject::invokeMethod(this, [this, &data](){
                showTip(QString("请将万用表负极连接到如图所示位置"), data.negative_connect_location);
            }, Qt::BlockingQueuedConnection);
            
            is_connect_wire = false;
            cv.wait(threadLock, [this]() { return is_connect_wire; });
        }
    });

    auto watcher = new QFutureWatcher<void>(this);
    watcher->setFuture(future);
    
    connect(watcher, &QFutureWatcher<void>::finished, this, [this, watcher]() {
        QMetaObject::invokeMethod(this, [this]() {
            // 如果需要可以设置对话框标题和模态属性
            accept();
        }, Qt::QueuedConnection);
        watcher->deleteLater();
    });
}

void TaskConnectWire::showTip(QString text, int location)
{
    // 保存当前文本长度作为新消息的起始位置
    lastMessageStart = ui->textBrowser->document()->characterCount() - 1;
    
    // 添加新文本并设置为红色
    QTextCursor cursor = ui->textBrowser->textCursor();
    QTextCharFormat redFormat;
    redFormat.setForeground(Qt::red);
    ui->textBrowser->setCurrentCharFormat(redFormat);
    ui->textBrowser->append(text);
    
    // 保存消息结束位置
    lastMessageEnd = ui->textBrowser->document()->characterCount() - 1;
    
    // 高亮对应的PCB位置
    labelEditingWindow->triggerTableRowClickById(location);

    ui->pbcomplete->setEnabled(true);
}

void TaskConnectWire::on_pbcomplete_clicked()
{
    std::unique_lock<std::mutex> lock(uiMutex);
    
    // 将最后一条消息改为绿色
    QTextCursor cursor = ui->textBrowser->textCursor();
    cursor.setPosition(lastMessageStart);
    cursor.setPosition(lastMessageEnd, QTextCursor::KeepAnchor);
    
    QTextCharFormat greenFormat;
    greenFormat.setForeground(Qt::green);
    cursor.mergeCharFormat(greenFormat);
    
    is_connect_wire = true;
    cv.notify_one();
    ui->pbcomplete->setEnabled(false);
}

void TaskConnectWire::on_pbbegin_clicked()
{
    ui->pbbegin->setEnabled(false);
    initWaveform();
}


