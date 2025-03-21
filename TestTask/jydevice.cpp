#include "jydevice.h"
#include "ui_jydevice.h"
#include <QComboBox>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QMessageBox>
#include <QDebug>
#include <QRegExp>
#include <QTimer>
#include <QtConcurrent>


JYDevice::JYDevice(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::JYDevice)
{
    ui->setupUi(this);
    this->setWindowTitle("仪器控制");
    initTables();  // 调用初始化函数
    // 初始化各个仪器（每个仪器在独立线程中运行）
    initdevice();

    updatePlotTimer = new QTimer(this);
    updatePlotTimer->setInterval(100); // 每100 ms更新一次，可根据需要调整
    connect(updatePlotTimer, &QTimer::timeout, this, [this](){
        updatePlot5322();
        updatePlot5323();
        updatePlot8902();
    });
    updatePlotTimer->start();
    this->close();
}

JYDevice::~JYDevice()
{
    delete ui;
}

void JYDevice::initTables()
{
    // 初始化 tableWidget_5711 为 32行, 5列，并设置列标题
    ui->tableWidget_5711->setRowCount(32);
    ui->tableWidget_5711->setColumnCount(5);
    QStringList headers;
    headers << "端口" << "类型" << "幅值/V" << "占空比/%" << "频率/Hz";
    ui->tableWidget_5711->setHorizontalHeaderLabels(headers);

    int row = 0;
    for (int i = 0; i < 16; ++i) {
         QTableWidgetItem* item = new QTableWidgetItem(QString("模拟输出端口%1").arg(i));
         item->setFlags(item->flags() & ~Qt::ItemIsEditable); // 设置为不可编辑
         ui->tableWidget_5711->setItem(row, 0, item);
         row++;
    }
    for (int i = 0; i < 12; ++i) {
         QTableWidgetItem* item = new QTableWidgetItem(QString("数字输出端口%1").arg(i));
         item->setFlags(item->flags() & ~Qt::ItemIsEditable); // 设置为不可编辑
         ui->tableWidget_5711->setItem(row, 0, item);
         row++;
    }
    for (int i = 0; i < 4; ++i) {
         QTableWidgetItem* item = new QTableWidgetItem(QString("电源输出端口%1").arg(i));
         item->setFlags(item->flags() & ~Qt::ItemIsEditable); // 设置为不可编辑
         ui->tableWidget_5711->setItem(row, 0, item);
         row++;
    }

    for (int i = 0; i < 32; ++i) {
         QComboBox *combo = new QComboBox();
         combo->addItem("关闭");
         combo->addItem("高电平");
         combo->addItem("低电平");
         combo->addItem("正弦波");
         combo->addItem("方波");
         combo->addItem("阶跃");
         combo->addItem("三角波");
         ui->tableWidget_5711->setCellWidget(i, 1, combo);
    }

    for (int i = 0; i < 32; ++i) {
         QLineEdit *lineEdit1 = new QLineEdit();
         QDoubleValidator *validator1 = new QDoubleValidator(lineEdit1);
         validator1->setBottom(0.0001);
         validator1->setTop(10);
         lineEdit1->setValidator(validator1);
         ui->tableWidget_5711->setCellWidget(i, 2, lineEdit1);
 
         // 列 3: 占空比/%，只允许正数
         QLineEdit *lineEdit2 = new QLineEdit();
         QDoubleValidator *validator2 = new QDoubleValidator(lineEdit2);
         validator2->setBottom(0.0001);
         validator2->setTop(100);
         lineEdit2->setValidator(validator2);
         ui->tableWidget_5711->setCellWidget(i, 3, lineEdit2);
 
         // 列 4: 频率/Hz，只允许正数
         QLineEdit *lineEdit3 = new QLineEdit();
         QDoubleValidator *validator3 = new QDoubleValidator(lineEdit3);
         validator3->setBottom(0.0001);
         validator3->setTop(2000000);
         lineEdit3->setValidator(validator3);
         ui->tableWidget_5711->setCellWidget(i, 4, lineEdit3);
    }

    // 初始化 tableWidget_8902 为 1行, 2列，并设置列标题
    ui->tableWidget_8902->setRowCount(1);
    ui->tableWidget_8902->setColumnCount(2);
    QStringList headers8902;
    headers8902 << "模式" << "测量项";
    ui->tableWidget_8902->setHorizontalHeaderLabels(headers8902);

    // 为第一行的单元格添加复选框
    QComboBox* comboMode = new QComboBox();
    comboMode->addItem("直流");
    comboMode->addItem("交流");
    ui->tableWidget_8902->setCellWidget(0, 0, comboMode);

    QComboBox* comboMeasure = new QComboBox();
    comboMeasure->addItem("电压");
    comboMeasure->addItem("电流");
    comboMeasure->addItem("电阻");
    ui->tableWidget_8902->setCellWidget(0, 1, comboMeasure);

    // 初始化 tableWidget_5323 为 32行, 2列
    ui->tableWidget_5323->setRowCount(32);
    ui->tableWidget_5323->setColumnCount(2);
    QStringList headers5323;
    headers5323 << "端口" << "状态" << "查看波形";
    ui->tableWidget_5323->setHorizontalHeaderLabels(headers5323);
    for(int i = 0; i < 32; i++)
    {
        QTableWidgetItem* item = new QTableWidgetItem(QString("模拟量输入端口%1").arg(i));
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget_5323->setItem(i, 0, item);
        QComboBox *combo = new QComboBox();
        combo->addItem("关闭");
        combo->addItem("开启");
        ui->tableWidget_5323->setCellWidget(i, 1, combo);
        ui->tableWidget_5323->setItem(i, 2, new QTableWidgetItem("查看"));
    }

    // 初始化 tableWidget_5322 为 16行, 2列
    ui->tableWidget_5322->setRowCount(16);
    ui->tableWidget_5322->setColumnCount(2);
    QStringList headers5322;
    headers5322 << "端口" << "状态" << "查看波形";
    ui->tableWidget_5322->setHorizontalHeaderLabels(headers5322);
    for(int i = 0; i < 16; i++)
    {
        QTableWidgetItem* item = new QTableWidgetItem(QString("数字量输入端口%1").arg(i));
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget_5322->setItem(i, 0, item);
        QComboBox *combo = new QComboBox();
        combo->addItem("关闭");
        combo->addItem("开启");
        ui->tableWidget_5322->setCellWidget(i, 1, combo);
        ui->tableWidget_5322->setItem(i, 2, new QTableWidgetItem("查看"));
    }
}

void JYDevice::initdevice()
{
    init5711();
    init8902();
    init5323();
    init5322();
}
void JYDevice::init5711()
{
    device5711 = JYDeviceManager::getInstance()->getDevice5711();
}

void JYDevice::init8902()
{
    device8902 = JYDeviceManager::getInstance()->getDevice8902();
    device8902->continueAcquisition = true;
    connect(device8902, &PXIe8902::signalAcquisitionData, this, &JYDevice::on_DataFrom8902);
}

void JYDevice::init5323()
{
    device5323 = JYDeviceManager::getInstance()->getDevice5323();
    device5323->continueAcquisition = true;
    connect(device5323, &PXIe5320::signalAcquisitionData, this, &JYDevice::on_DataFrom5323);
}

void JYDevice::init5322()
{
    device5322 = JYDeviceManager::getInstance()->getDevice5322();
    device5322->continueAcquisition = true;
    connect(device5322, &PXIe5320::signalAcquisitionData, this, &JYDevice::on_DataFrom5322);
}

void JYDevice::on_Initwaveform_clicked()
{
    if(!device5711) return;
    if(!device5711->InitializeDevice())
    {
        QString errorMsg = device5711->getErrorMessage();
        QMessageBox::warning(this, "错误", errorMsg);
        return;
    }
}

void JYDevice::on_start_5711_clicked()
{
    if(!device5711) return;
    if(PXIe_5711_state != 0) on_stop_5711_clicked();
    std::vector<PXIe5711Waveform> waveforms5711;
    for(int i = 0; i < 32; i++)
    {
        QComboBox *combo = static_cast<QComboBox*>(ui->tableWidget_5711->cellWidget(i, 1));
        if(combo->currentText() == "关闭") continue;
        PXIe5711Waveform waveform;
        waveform.channel = i;
        waveform.waveform_type = static_cast<PXIe5711_testtype>(combo->currentIndex() - 1);
        waveform.amplitude = static_cast<QLineEdit*>(ui->tableWidget_5711->cellWidget(i, 2))->text().toDouble();
        waveform.dutyCycle = static_cast<QLineEdit*>(ui->tableWidget_5711->cellWidget(i, 3))->text().toDouble();
        waveform.frequency = static_cast<QLineEdit*>(ui->tableWidget_5711->cellWidget(i, 4))->text().toDouble();
        waveforms5711.push_back(waveform);
    }
    if(!device5711->receivewaveform(waveforms5711)) 
    {
        QString errorMsg = device5711->getErrorMessage();
        QMessageBox::warning(this, "错误", errorMsg);
        return;
    }
    PXIe_5711_state = 1;
    ui->start_5711->setEnabled(false);
    ui->Initwaveform->setEnabled(false);
    ui->Trigger_5711->setEnabled(true);
    ui->stop_5711->setEnabled(true);
}

void JYDevice::on_Trigger_5711_clicked()
{
    if(!device5711) return;
    if(PXIe_5711_state != 1) return;
    if(!device5711->SendSoftTrigger())
    {
        QString errorMsg = device5711->getErrorMessage();
        QMessageBox::warning(this, "错误", errorMsg);
        return;
    }
    PXIe_5711_state = 2;
    ui->Trigger_5711->setEnabled(false);
}


void JYDevice::on_stop_5711_clicked()
{
    if(!device5711) return;
    device5711->CloseDevice();
    PXIe_5711_state = 0;
    ui->start_5711->setEnabled(true);
    ui->Initwaveform->setEnabled(true);
    ui->Trigger_5711->setEnabled(false);
    ui->stop_5711->setEnabled(false);
}


void JYDevice::on_start_8902_clicked()
{
    if(!device8902) return;
    if(PXIe_8902_state != 0) on_stop_8902_clicked();
    queue8902.clear();
    if(plot8902) {
        std::vector<QCPGraph*> graphs;
        for(int i = 0; i < plot8902->graphCount(); i++)
            graphs.push_back(plot8902->graph(i));
        if(!graphs.empty()) plot8902->removeLine(graphs);
    }
    int mode = static_cast<QComboBox*>(ui->tableWidget_8902->cellWidget(0, 0))->currentIndex();
    PXIe8902_testtype measure = static_cast<PXIe8902_testtype>(static_cast<QComboBox*>(ui->tableWidget_8902->cellWidget(0, 1))->currentIndex());
    vector<Data8902> data8902;
    Data8902 data;
    data.test_type = measure;
    data.model = static_cast<bool>(mode);
    data8902.push_back(data);
    if(!device8902->StartAcquisition(data8902, 1))
    {
        QString errorMsg = device8902->getErrorMessage();
        QMessageBox::warning(this, "错误", errorMsg);
        return;
    }
    PXIe_8902_state = 1;
    ui->start_8902->setEnabled(false);
    ui->Trigger_8902->setEnabled(true);
    ui->stop_8902->setEnabled(true);
}

void JYDevice::on_Trigger_8902_clicked()
{
    if(!device8902) return;
    if(PXIe_8902_state != 1) return;
    if(!device8902->SendSoftTrigger())
    {
        QString errorMsg = device8902->getErrorMessage();
        QMessageBox::warning(this, "错误", errorMsg);
        return;
    }
    PXIe_8902_state = 2;
    ui->Trigger_8902->setEnabled(false);
}


void JYDevice::on_stop_8902_clicked()
{
    if(!device8902) return;
    device8902->DeviceClose();
    PXIe_8902_state = 0;
    ui->start_8902->setEnabled(true);
    ui->Trigger_8902->setEnabled(false);
    ui->stop_8902->setEnabled(false);
}


void JYDevice::on_start_5323_clicked()
{
    if(!device5323) return;
    if(PXIe_5323_state != 0) on_stop_5323_clicked();
    queue5323.clear();
    if(plot5323) {
        std::vector<QCPGraph*> graphs;
        for(int i = 0; i < plot5323->graphCount(); i++)
            graphs.push_back(plot5323->graph(i));
        if(!graphs.empty()) plot5323->removeLine(graphs);
    }
    std::vector<PXIe5320Waveform> waveforms5323;
    for(int i = 0; i < 32; i++)
    {
        if(static_cast<QComboBox*>(ui->tableWidget_5323->cellWidget(i, 1))->currentText() == "关闭") continue;
        PXIe5320Waveform waveform;
        waveform.port = i;
        waveform.data = {};
        waveforms5323.push_back(waveform);
    }
    if(!device5323->StartAcquisition(waveforms5323, 1))
    {
        QString errorMsg = device5323->getErrorMessage();
        QMessageBox::warning(this, "错误", errorMsg);
        return;
    }
    PXIe_5323_state = 1;
    ui->start_5323->setEnabled(false);
    ui->Trigger_5323->setEnabled(true);
    ui->stop_5323->setEnabled(true);
}


void JYDevice::on_Trigger_5323_clicked()
{
    if(!device5323) return;
    if(PXIe_5323_state != 1) return;
    if(!device5323->SendSoftTrigger())
    {
        QString errorMsg = device5323->getErrorMessage();
        QMessageBox::warning(this, "错误", errorMsg);
        return;
    }
    PXIe_5323_state = 2;
    ui->Trigger_5323->setEnabled(false);
}


void JYDevice::on_stop_5323_clicked()
{
    if(!device5323) return;
    device5323->DeviceClose();
    PXIe_5323_state = 0;
    ui->start_5323->setEnabled(true);
    ui->Trigger_5323->setEnabled(false);
    ui->stop_5323->setEnabled(false);
}


void JYDevice::on_start_5322_clicked()
{
    if(!device5322) return;
    if(PXIe_5322_state != 0) on_stop_5322_clicked();
    queue5322.clear();
    if(plot5322) {
        std::vector<QCPGraph*> graphs;
        for(int i = 0; i < plot5322->graphCount(); i++)
            graphs.push_back(plot5322->graph(i));
        if(!graphs.empty()) plot5322->removeLine(graphs);
    }
    std::vector<PXIe5320Waveform> waveforms5322;
    for(int i = 0; i < 16; i++)
    {
        if(static_cast<QComboBox*>(ui->tableWidget_5322->cellWidget(i, 1))->currentText() == "关闭") continue;
        PXIe5320Waveform waveform;
        waveform.port = i;
        waveform.data = {};
        waveforms5322.push_back(waveform);
    }
    if(!device5322->StartAcquisition(waveforms5322, 1))
    {
        QString errorMsg = device5322->getErrorMessage();
        QMessageBox::warning(this, "错误", errorMsg);
        return;
    }
    PXIe_5322_state = 1;
    ui->start_5322->setEnabled(false);
    ui->Trigger_5322->setEnabled(true);
    ui->stop_5322->setEnabled(true);
}


void JYDevice::on_Trigger_5322_clicked()
{
    if(!device5322) return;
    if(PXIe_5322_state != 1) return;
    if(!device5322->SendSoftTrigger())
    {
        QString errorMsg = device5322->getErrorMessage();
        QMessageBox::warning(this, "错误", errorMsg);
        return;
    }
    PXIe_5322_state = 2;
}


void JYDevice::on_stop_5322_clicked()
{
    if(!device5322) return;
    device5322->DeviceClose();
    PXIe_5322_state = 0;
    ui->start_5322->setEnabled(true);
    ui->Trigger_5322->setEnabled(false);
    ui->stop_5322->setEnabled(false);
}

void JYDevice::on_StateChanged(const QString& state, int numb)
{
    if(numb < 0)
    {
        QMessageBox::information(this, "错误", state);
    }
}

void JYDevice::on_DataFrom5323(const std::vector<PXIe5320Waveform> collectdata, int serial_number)
{
    if (!plot5323) {
         plot5323 = new UESTCQCustomPlot();
         plot5323->setWindowFlags(Qt::Window);
         plot5323->setWindowModality(Qt::NonModal);
         plot5323->resize(800,600);
         plot5323->setWindowTitle("5323 数据显示");
         plot5323->show();
    }
    
    // 遍历采集数据，更新对应通道的队列数据
    for (const auto &waveform : collectdata) {
         int port = waveform.port;
         // 确保 queue5323 容量足够
         if (port >= static_cast<int>(queue5323.size())) {
             queue5323.resize(port + 1);
         }
         // 将新采集的数据追加到对应通道的队列中
         for (float sample : waveform.data) {
             queue5323[port].push(sample);
         }
    }
}

void JYDevice::on_DataFrom5322(const std::vector<PXIe5320Waveform> collectdata, int serial_number)
{
    if (!plot5322) {
         plot5322 = new UESTCQCustomPlot();
         plot5322->setWindowFlags(Qt::Window);
         plot5322->setWindowModality(Qt::NonModal);
         plot5322->resize(800,600);
         plot5322->setWindowTitle("5322 数据显示");
         plot5322->show();
    }
    
    // 遍历采集数据，更新对应通道的队列数据
    for (const auto &waveform : collectdata) {
         int port = waveform.port;
         // 确保 queue5322 容量足够
         if (port >= static_cast<int>(queue5322.size())) {
             queue5322.resize(port + 1);
         }
         // 将新采集的数据追加到对应通道的队列中
         for (float sample : waveform.data) {
             queue5322[port].push(sample);
         }
    }
}

void JYDevice::on_DataFrom8902(const std::vector<PXIe5320Waveform> collectdata, int serial_number)
{
    if (!plot8902) {
        plot8902 = new UESTCQCustomPlot();
        plot8902->setWindowFlags(Qt::Window);
        plot8902->setWindowModality(Qt::NonModal);
        plot8902->resize(800,600);
        plot8902->setWindowTitle("8902 数据显示");
        plot8902->show();
    }
    // 遍历采集数据，更新对应通道的队列数据
    for (const auto &waveform : collectdata) {
        int port = abs(waveform.port + 1);
        // 确保 queue8902 容量足够
        if (port >= static_cast<int>(queue8902.size())) {
            queue8902.resize(port + 1);
        }
        // 将新采集的数据追加到对应通道的队列中
        for (float sample : waveform.data) {
            queue8902[port].push(sample);
        }
    }
}

void JYDevice::updatePlot5323()
{
    if (!plot5323) return;
    QVector<QCPGraph*> graphs;
    QVector<QList<double>> data;
    for (size_t port = 0; port < queue5323.size(); ++port) {
        if (queue5323[port].empty())
            continue;

        QString graphName = QString("端口 %1").arg(port);
        QCPGraph *graph = nullptr;

        // 查找已有的图形
        for (int i = 0; i < plot5323->graphCount(); ++i) {
            if (plot5323->graph(i)->name() == graphName) {
                graph = plot5323->graph(i);
                break;
            }
        }

        // 如果图形不存在则添加实时曲线
        if (!graph) {
            graph = plot5323->addRealTimeLine(graphName);
        }

        // 累计该端口的新数据
        QList<double> newData;
        int totalPoints = queue5323[port].size();
        for (int i = 0; i < totalPoints; ++i) {
            double value = queue5323[port].front();
            queue5323[port].pop();
            newData.append(value);
        }
        // 只有当有新数据时，才把该图形和数据添加进数组，确保图形与数据一一对应
        if (!newData.isEmpty()){
            graphs.push_back(graph);
            data.push_back(newData);
        }
    }
    if(!graphs.isEmpty() && graphs.size() == data.size())
        plot5323->updateRealTimeLines(graphs, data);
}

void JYDevice::updatePlot5322()
{
    if (!plot5322) return;
    QVector<QCPGraph*> graphs;
    QVector<QList<double>> data;
    // 遍历每个端口的队列数据
    for (size_t port = 0; port < queue5322.size(); ++port) {
        if (queue5322[port].empty())
            continue;

        QString graphName = QString("端口 %1").arg(port);
        QCPGraph *graph = nullptr;

        // 查找已有的图形
        for (int i = 0; i < plot5322->graphCount(); ++i) {
            if (plot5322->graph(i)->name() == graphName) {
                graph = plot5322->graph(i);
                break;
            }
        }

        // 如果图形不存在则添加实时曲线
        if (!graph) {
            graph = plot5322->addRealTimeLine(graphName);
        }

        // 累计该端口的新数据
        QList<double> newData;
        int totalPoints = queue5322[port].size();
        for (int i = 0; i < totalPoints; ++i) {
            double value = queue5322[port].front();
            queue5322[port].pop();
            newData.append(value);
        }
        // 只有当有新数据时，才把该图形和数据添加进数组，确保图形与数据一一对应
        if (!newData.isEmpty()){
            graphs.push_back(graph);
            data.push_back(newData);
        }
    }
    if(!graphs.isEmpty() && graphs.size() == data.size())
        plot5322->updateRealTimeLines(graphs, data);
}

void JYDevice::updatePlot8902()
{
    if (!plot8902) return;
    QVector<QCPGraph*> graphs;
    QVector<QList<double>> data;
    for (size_t port = 0; port < queue8902.size(); ++port) {
        if (queue8902[port].empty())
            continue;

        QString graphName;
        if(port == 0)
        {
            graphName = QString("电压");
        }else if(port == 1){
            graphName = QString("电流");
        }else{
            graphName = QString("电阻");
        }
        QCPGraph *graph = nullptr;

        // 查找已有的图形
        if(plot8902->graphCount() > 0)
        {
            if (plot8902->graph(0)->name() == graphName) graph = plot8902->graph(0);
        }

        // 如果图形不存在则添加实时曲线
        if (!graph) graph = plot8902->addRealTimeLine(graphName);

        // 累计该端口的新数据
        QList<double> newData;
        int totalPoints = queue8902[port].size();
        for (int i = 0; i < totalPoints; ++i) {
            double value = queue8902[port].front();
            queue8902[port].pop();
            newData.append(value);
        }
        // 只有当有新数据时，才把该图形和数据添加进数组，确保图形与数据一一对应
        if (!newData.isEmpty()){
            graphs.push_back(graph);
            data.push_back(newData);
        }
    }
    if(!graphs.isEmpty() && graphs.size() == data.size())
        plot8902->updateRealTimeLines(graphs, data);
}

void JYDevice::on_pbshow5322_clicked()
{
    if(plot5322) {
        plot5322->show();
    }
}

void JYDevice::on_pbshow5323_clicked()
{
    if(plot5323) {
        plot5323->show();
    }
}

void JYDevice::on_pbshow_8902_clicked()
{
    if(plot8902) {
        plot8902->show();
    }
}

void JYDevice::on_pushButton_clicked()
{
    if(ui->checkBox_5711->isChecked() && device5711) on_start_5711_clicked();
    if(ui->checkBox_8902->isChecked() && device8902) on_start_8902_clicked();
    if(ui->checkBox_5322->isChecked() && device5322) on_start_5322_clicked();
    if(ui->checkBox_5323->isChecked() && device5323) on_start_5323_clicked();
}


void JYDevice::on_pushButton_2_clicked()
{
    if(ui->checkBox_5711->isChecked() && device5711) on_Trigger_5711_clicked();
    if(ui->checkBox_8902->isChecked() && device8902) on_Trigger_8902_clicked();
    if(ui->checkBox_5322->isChecked() && device5322) on_Trigger_5322_clicked();
    if(ui->checkBox_5323->isChecked() && device5323) on_Trigger_5323_clicked();
}


void JYDevice::on_pushButton_3_clicked()
{
    if(ui->checkBox_5711->isChecked() && device5711) on_stop_5711_clicked();
    if(ui->checkBox_8902->isChecked() && device8902) on_stop_8902_clicked();
    if(ui->checkBox_5322->isChecked() && device5322) on_stop_5322_clicked();
    if(ui->checkBox_5323->isChecked() && device5323) on_stop_5323_clicked();
}

