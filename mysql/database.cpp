#include "database.h"
#include <QDebug>
#include <QMessageBox>
#include <QFile>

// 构造函数
Database::Database(const QString& connectionName, QWidget *parent)
    : connectionName(connectionName)
    , parent(parent)
{
    if (!connect()) {
        QMessageBox::critical(parent, "错误", "无法连接到数据库: " + db.lastError().text(), QMessageBox::Ok);
    }
}

// 析构函数
Database::~Database()
{
    disconnect();
}

// 连接数据库
bool Database::connect()
{
    if (db.isOpen()) {
        return true;
    }

    db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
    db.setHostName("localhost");
    db.setDatabaseName("fault_detect");
    db.setUserName("root");
    db.setPassword("211200");
    db.setConnectOptions("MYSQL_OPT_LOCAL_INFILE=1"); // 添加 --local-infile 选项
    if (!db.open()) {
        QMessageBox::critical(parent, "错误", "无法连接到数据库: " + db.lastError().text(), QMessageBox::Ok);
        return false;
    }

    query = QSqlQuery(db);
    if (!query.exec("SET NAMES 'GBK'")) {
        QMessageBox::critical(parent, "错误", "设置字符集错误: " + query.lastError().text(), QMessageBox::Ok);
        return false;
    }

    return true;
}

// 断开数据库连接
bool Database::disconnect()
{
    if (db.isOpen()) {
        db.close();
    }
    QSqlDatabase::removeDatabase(connectionName);
    return true;
}

bool Database::get_table_name(const QString& condition, std::vector<QString>& TableNames)
{
    query.prepare("SHOW TABLES LIKE '" + condition + "'");
    if (!query.exec()) {
        QMessageBox::critical(parent, "错误", "无法查询表名: " + query.lastError().text(), QMessageBox::Ok);
        return false;
    }
    while (query.next()) {
        TableNames.push_back(query.value(0).toString());
    }
    return true;
}

bool Database::delete_table(const QString& tableName)
{
    query.prepare("DROP TABLE IF EXISTS " + tableName);
    if (!query.exec()) {
        QMessageBox::critical(parent, "错误", "无法删除表: " + query.lastError().text(), QMessageBox::Ok);
        return false;
    }
    return true;
}

// 对 Device 表的增删改查函数
bool Database::insert_device(const Device& device, QString& ErrorMessage)
{
    QString imagePath;

    // 数据库中不存储图片路径
    query.prepare("INSERT INTO devices (id, device_name, device_driver_Voltage, device_driver_Current, device_driver_Power, image) VALUES (:id, :device_name, :device_driver_Voltage, :device_driver_Current, :device_driver_Power, :image)");
    query.bindValue(":id", device.id);
    query.bindValue(":device_name", device.device_name);
    query.bindValue(":device_driver_Voltage", device.device_driver_Voltage);
    query.bindValue(":device_driver_Current", device.device_driver_Current);
    query.bindValue(":device_driver_Power", device.device_driver_Power);
    query.bindValue(":image", QByteArray());  // 存储空值
    
    if (!query.exec()) {
        ErrorMessage = query.lastError().text();
        return false;
    }
    
    QString device_elementTableName = device.id + "$$DeviceElement";
    query.prepare("CREATE TABLE " + device_elementTableName + "(id INT AUTO_INCREMENT PRIMARY KEY, label BLOB, device_id VARCHAR(50) NOT NULL, point_x DOUBLE NOT NULL, point_y DOUBLE NOT NULL, width DOUBLE NOT NULL, height DOUBLE NOT NULL, position_number VARCHAR(50), notes BLOB, FOREIGN KEY (device_id) REFERENCES devices(id) ON DELETE CASCADE)ENGINE=InnoDB;");
    if (!query.exec()) {
        ErrorMessage = query.lastError().text();
        return false;
    }
    QString testTaskTableName = device.id + "$$TestTask";
    query.prepare("CREATE TABLE " + testTaskTableName + "(id VARCHAR(50) PRIMARY KEY, element_id INT NOT NULL, connection_image_data LONGBLOB, FOREIGN KEY (element_id) REFERENCES " + device.id + "$$DeviceElement(id) ON DELETE CASCADE)ENGINE=InnoDB;");
    if (!query.exec()) {
        ErrorMessage = query.lastError().text();
        return false;
    }
    QString stepTableName = device.id + "$$Step";
    query.prepare("CREATE TABLE " + stepTableName + "(id VARCHAR(50) PRIMARY KEY, test_task_id VARCHAR(50) NOT NULL, collecttime DOUBLE NOT NULL, step_number INT NOT NULL, continue_step TINYINT(1), isthermometry TINYINT(1), thermometry_pause_time DOUBLE, FOREIGN KEY (test_task_id) REFERENCES " + testTaskTableName + "(id) ON DELETE CASCADE)ENGINE=InnoDB;");
    if (!query.exec()) {
        ErrorMessage = query.lastError().text();
        return false;
    }
    QString pxie5711TableName = device.id + "$$PXIe5711";
    query.prepare("CREATE TABLE " + pxie5711TableName + "(id VARCHAR(50) PRIMARY KEY, step_id VARCHAR(50) NOT NULL, channel INT NOT NULL, waveform_type VARCHAR(50) NOT NULL, amplitude DOUBLE NOT NULL, frequency DOUBLE NOT NULL, dutyCycle DOUBLE NOT NULL, positive_connect_location INT, negative_connect_location INT, FOREIGN KEY (step_id) REFERENCES " + stepTableName + "(id) ON DELETE CASCADE)ENGINE=InnoDB;");
    if (!query.exec()) {
        ErrorMessage = query.lastError().text();
        return false;
    }
    QString pxie5320TableName = device.id + "$$PXIe5320";
    query.prepare("CREATE TABLE " + pxie5320TableName + "(id VARCHAR(50) PRIMARY KEY, step_id VARCHAR(50) NOT NULL, device INT NOT NULL, port INT NOT NULL, data LONGBLOB, positive_connect_location INT, negative_connect_location INT, FOREIGN KEY (step_id) REFERENCES " + stepTableName + "(id) ON DELETE CASCADE)ENGINE=InnoDB;");
    if (!query.exec()) {
        ErrorMessage = query.lastError().text();
        return false;
    }
    QString imageTableName = device.id + "$$image";
    query.prepare("CREATE TABLE " + imageTableName + "(id VARCHAR(50) PRIMARY KEY, step_id VARCHAR(50) NOT NULL, task_table_name VARCHAR(50) NOT NULL, device VARCHAR(50) NOT NULL, image_data LONGBLOB NOT NULL, temp_data LONGBLOB, temp_width DOUBLE, temp_height DOUBLE, FOREIGN KEY (step_id) REFERENCES " + stepTableName + "(id) ON DELETE CASCADE)ENGINE=InnoDB;");
    if (!query.exec()) {
        ErrorMessage = query.lastError().text();
        return false;
    }
    QString pxie8902TableName = device.id + "$$PXIe8902";
    query.prepare("CREATE TABLE " + pxie8902TableName + "(id VARCHAR(50) PRIMARY KEY, step_id VARCHAR(50) NOT NULL, test_type VARCHAR(50) NOT NULL, positive_connect_location INT, negative_connect_location INT, FOREIGN KEY (step_id) REFERENCES " + stepTableName + "(id) ON DELETE CASCADE)ENGINE=InnoDB;");
    if (!query.exec()) {
        ErrorMessage = query.lastError().text();
        return false;
    }

    // 保存图片到文件系统
    if (!device.image.isEmpty()) {
        imagePath = "./IMAGE/" + device.id + ".jpg";
        QFile file(imagePath);
        if (!file.open(QIODevice::WriteOnly)) {
            ErrorMessage = "无法创建图片文件: " + file.errorString();
            return false;
        }
        file.write(device.image);
        file.close();

        try{
            std::vector<std::string> imageFiles;
            imageFiles.push_back(imagePath.toStdString());
            SIFT_MATCHER.appendToDatabase(imageFiles);

        }
        catch(const std::exception& e)
        {
            ErrorMessage = "无法将图片添加到数据库: " + QString::fromStdString(e.what());
            return false;
        }
    }

    return true;
}

bool Database::get_device(const QString& condition, std::vector<Device>& devices, bool includeImage)
{
    QString selectQuery = "SELECT id, device_name, device_driver_Voltage, device_driver_Current, device_driver_Power FROM devices";
    if(!condition.isEmpty()) {
        selectQuery += " WHERE " + condition;
    }
    
    query.prepare(selectQuery);
    if (!query.exec()) {
        QMessageBox::critical(parent, "错误", "无法查询 Device 数据: " + query.lastError().text(), QMessageBox::Ok);
        return false;
    }

    while (query.next()) {
        Device device;
        device.id = query.value("id").toString();
        device.device_name = query.value("device_name").toByteArray();
        device.device_driver_Voltage = query.value("device_driver_Voltage").toDouble();
        device.device_driver_Current = query.value("device_driver_Current").toDouble();
        device.device_driver_Power = query.value("device_driver_Power").toDouble();
        
        if (includeImage) {
            QString imagePath = "./IMAGE/" + device.id + ".jpg";
            if (QFile::exists(imagePath)) {
                QFile file(imagePath);
                if (file.open(QIODevice::ReadOnly)) {
                    device.image = file.readAll();
                    file.close();
                }
            }
        }
        
        devices.push_back(device);
    }
    return true;
}

bool Database::update_device(const Device& device, QString& ErrorMessage, bool includeImage)
{
    // 数据库中不更新图片字段
    query.prepare("UPDATE devices SET device_name = :device_name, "
                 "device_driver_Voltage = :device_driver_Voltage, "
                 "device_driver_Current = :device_driver_Current, "
                 "device_driver_Power = :device_driver_Power "
                 "WHERE id = :id");
    
    query.bindValue(":device_name", device.device_name);
    query.bindValue(":device_driver_Voltage", device.device_driver_Voltage);
    query.bindValue(":device_driver_Current", device.device_driver_Current);
    query.bindValue(":device_driver_Power", device.device_driver_Power);
    query.bindValue(":id", device.id);

    if (!query.exec()) {
        ErrorMessage = query.lastError().text();
        return false;
    }

    if(includeImage && !device.image.isEmpty())
    {
        // 保存新图片
        QString imagePath = "./IMAGE/" + device.id + ".jpg";
        QFile file(imagePath);
        if (!file.open(QIODevice::WriteOnly)) {
            ErrorMessage = "无法创建图片文件: " + file.errorString();
            return false;
        }
        file.write(device.image);
        file.close();

        try{
            if(SIFT_MATCHER.removeFromDatabase(std::string(device.id.toStdString())))
            {
                std::vector<std::string> imageFiles;
                imageFiles.push_back(imagePath.toStdString());
                SIFT_MATCHER.appendToDatabase(imageFiles);
            }
        }
        catch(const std::exception& e)
        {
            ErrorMessage = "无法将图片添加到SIFT数据库: " + QString::fromStdString(e.what());
            return false;
        }
    }

    return true;
}

bool Database::delete_device(const QString& id, QString& ErrorMessage)
{

    query.exec("SET FOREIGN_KEY_CHECKS = 0;");
    query.prepare("DELETE FROM devices WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        ErrorMessage = query.lastError().text();
        query.exec("SET FOREIGN_KEY_CHECKS = 1;");
        return false;
    }

    std::vector<QString> TableNames;
    QString condition = id + "$$%";
    if (!get_table_name(condition, TableNames)) {
        ErrorMessage = query.lastError().text();
        query.exec("SET FOREIGN_KEY_CHECKS = 1;");
        return false;
    }

    for(auto &TableName : TableNames)
    {
        query.prepare("DROP TABLE IF EXISTS " + TableName);
        if (!query.exec()) {
            ErrorMessage = query.lastError().text();
            query.exec("SET FOREIGN_KEY_CHECKS = 1;");
            return false;
        }
    }
    query.exec("SET FOREIGN_KEY_CHECKS = 1;");

    // 删除图片文件
    QString imagePath = "./IMAGE/" + id + ".jpg";
    if (QFile::exists(imagePath)) {
        QFile::remove(imagePath);
        if(!SIFT_MATCHER.removeFromDatabase(std::string(id.toStdString())))
        {
            ErrorMessage = "无法删除SIFT数据库中的图片: " + id;
        }
    }
    return true;
}

// 对 DeviceElement 表的增删改查函数
bool Database::insert_deviceelement(const QString& device_id, const std::vector<Label>& labels, QString& ErrorMessage)
{
    QString sql = "INSERT INTO " + device_id + "$$DeviceElement" + " (label, device_id, point_x, point_y, width, height, position_number, notes) VALUES ";
    QStringList valueStrings;
    for(auto &label : labels)
    {
        QByteArray label_data = label.label.toUtf8();
        valueStrings.append(QString("('%1', '%2', %3, %4, %5, %6, '%7', '%8')")
                            .arg(label_data)
                            .arg(device_id)
                            .arg(label.point_x)
                            .arg(label.point_y)
                            .arg(label.width)
                            .arg(label.height)
                            .arg(label.position_number)
                            .arg(label.notes));
    }
    sql += valueStrings.join(", ") + ";";
    query.prepare(sql);
    if (!query.exec()) {
        ErrorMessage = query.lastError().text();
        return false;
    }
    return true;
}

bool Database::get_deviceelement(const QString& device_id, const QString& condition, std::vector<Label>& labels, QString& ErrorMessage)
{
    QString sqlQuery = "SELECT * FROM " + device_id + "$$DeviceElement";
    if (!condition.isEmpty()) {
        sqlQuery += " WHERE " + condition;
    }
    query.prepare(sqlQuery);
    if (!query.exec()) {
        ErrorMessage = query.lastError().text();
        return false;
    }
    while (query.next()) {
        Label label;
        label.id = query.value("id").toInt();
        label.label = QString::fromUtf8(query.value("label").toByteArray());
        label.point_x = query.value("point_x").toDouble();
        label.point_y = query.value("point_y").toDouble();
        label.width = query.value("width").toDouble();
        label.height = query.value("height").toDouble();
        label.position_number = query.value("position_number").toString();
        label.notes = query.value("notes").toByteArray();
        labels.push_back(label);
    }
    return true;
}

bool Database::update_deviceelement(const QString& device_id, const std::vector<Label>& labels, QString& ErrorMessage)
{
    for(auto &label : labels)
    {
        QByteArray label_data = label.label.toUtf8();
        QString updateQuery = QString("UPDATE %1$$DeviceElement SET label = ?, point_x = ?, point_y = ?, width = ?, height = ?, position_number = ?, notes = ? WHERE id = ?")
                             .arg(device_id);
        query.prepare(updateQuery);
        query.addBindValue(label_data);
        query.addBindValue(label.point_x);
        query.addBindValue(label.point_y); 
        query.addBindValue(label.width);
        query.addBindValue(label.height);
        query.addBindValue(label.position_number);
        query.addBindValue(label.notes);
        query.addBindValue(label.id);

        if (!query.exec()) {
            ErrorMessage = query.lastError().text();
            return false;
        }
    }
    return true;
}

bool Database::delete_deviceelement(const QString& device_id, const std::vector<int>& ids, QString& ErrorMessage)
{
    if(ids.empty())
    {
        return true;
    }

    // 先删除关联的测试任务
    for(auto &id : ids)
    {
        std::vector<TestTask> testtasks;
        get_testtask(device_id + "$$TestTask", "element_id = " + QString::number(id), testtasks, ErrorMessage);
        if(!testtasks.empty())
        {
            qDebug() << "删除设备元素" << id << "关联的 TestTask 数据" << testtasks.size() << "个";
            for(auto &testtask : testtasks)
            {
                delete_testtask(device_id + "$$TestTask", testtask.id, ErrorMessage);
            }
        }
    }

    // 构建删除元素的SQL语句
    QString idList;
    for(size_t i = 0; i < ids.size(); i++) {
        if(i > 0) idList += ",";
        idList += QString::number(ids[i]);
    }
    
    // 使用prepare和bindValue来避免SQL注入和语法错误
    QString tableName = device_id + "$$DeviceElement";
    QString deleteQuery = "DELETE FROM " + tableName + " WHERE id IN (" + idList + ")";
    query.prepare(deleteQuery);

    // 执行删除
    if (!query.exec()) {
        ErrorMessage = query.lastError().text();
        qDebug() << "删除设备元素失败:" << ErrorMessage;
        qDebug() << "执行的SQL:" << deleteQuery;
        return false;
    }

    // 检查是否真的删除了记录
    if(query.numRowsAffected() == 0) {
        ErrorMessage = "没有找到要删除的记录";
        qDebug() << "删除设备元素失败: 没有找到要删除的记录";
        return false;
    }

    return true;
}

bool Database::delete_all_deviceelement(const QString& device_id, QString& ErrorMessage)
{
    std::vector<TestTask> testtasks;
    get_testtask(device_id + "$$TestTask", "", testtasks, ErrorMessage);
    for(auto &testtask : testtasks)
    {
        delete_testtask(device_id + "$$TestTask", testtask.id, ErrorMessage);
    }
    query.prepare("DELETE FROM " + device_id + "$$DeviceElement");
    if (!query.exec()) {
        ErrorMessage = query.lastError().text();
        return false;
    }
    query.prepare("ALTER TABLE " + device_id + "$$DeviceElement AUTO_INCREMENT = 1");
    if (!query.exec()) {
        ErrorMessage = query.lastError().text();
        return false;
    }
    return true;
}

// 对 TestTask 表的增删改查函数
bool Database::insert_testtask(const QString& tableName, const TestTask& testtask, QString& ErrorMessage)
{
    query.prepare("INSERT INTO " + tableName + " (id, element_id, connection_image_data) VALUES (:id, :element_id, :connection_image_data)");
    query.bindValue(":id", testtask.id);
    query.bindValue(":element_id", testtask.element_id);
    query.bindValue(":connection_image_data", testtask.connection_image_data);
    if (!query.exec()) {
        ErrorMessage = (query.lastError().text() == "") ? "设备错误，请重新添加设备！" : query.lastError().text();
        return false;
    }
    return true;
}

bool Database::update_testtask(const QString& tableName, const TestTask& testtask)
{
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT COUNT(*) FROM " + tableName + " WHERE id = :id");
    checkQuery.bindValue(":id", testtask.id);
    if (!checkQuery.exec() || !checkQuery.next() || checkQuery.value(0).toInt() == 0) {
        // 如果记录不存在
        return false;
    }
    
    query.prepare("UPDATE " + tableName + " SET element_id = :element_id, connection_image_data = :connection_image_data WHERE id = :id");
    query.bindValue(":element_id", testtask.element_id);
    query.bindValue(":connection_image_data", testtask.connection_image_data);
    query.bindValue(":id", testtask.id);
    if (!query.exec()) {
        // QMessageBox::critical(parent, "错误", "无法更新 TestTask 数据: " + query.lastError().text(), QMessageBox::Ok);
        return false;
    }
    return true;
}

bool Database::get_testtask(const QString& tableName, const QString& condition, std::vector<TestTask>& testtasks, QString& ErrorMessage)
{
    QString sqlQuery = "SELECT * FROM " + tableName;
    if (!condition.isEmpty()) {
        sqlQuery += " WHERE " + condition;
    }
    query.prepare(sqlQuery);
    if (!query.exec()) {
        ErrorMessage = query.lastError().text();
        if(ErrorMessage == "")
        {
            testtasks = {};
            return true;
        }
        return false;
    }
    while (query.next()) {
        TestTask testtask;
        testtask.id = query.value("id").toString();
        testtask.element_id = query.value("element_id").toInt();
        testtask.connection_image_data = query.value("connection_image_data").toByteArray();
        testtasks.push_back(testtask);
    }
    return true;
}

bool Database::delete_testtask(const QString& tableName, const QString& id, QString& ErrorMessage)
{
    query.prepare("DELETE FROM " + tableName + " WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        ErrorMessage = query.lastError().text();
        return false;
    }
    std::vector<QString> TableNames;
    QString condition = tableName.split("$$")[0] + "$$" + id + "_%";
    if (!get_table_name(condition, TableNames)) {
        ErrorMessage = "无法删除 TestTask 数据: " + query.lastError().text();
        return false;
    }

    QString image_id = tableName.split("$$")[1];
    QString image_table_name = tableName.split("$$")[0] + "$$Image";
    for(auto &TableName : TableNames)
    {
        if(!delete_TestDate(TableName, ErrorMessage))
        {
            return false;
        }
    }
    return true;
}

bool Database::delete_TestDate(const QString& tableName, QString& ErrorMessage)
{
    query.prepare("DROP TABLE IF EXISTS " + tableName);
    if (!query.exec()) {
        ErrorMessage = query.lastError().text();
        return false;
    }
    
    QString image_table_name = tableName.split("$$")[0] + "$$Image";
    QString task_table_name = tableName.split("$$")[1];
    QString condition = "task_table_name = " + task_table_name;
    if(!delete_image(image_table_name, condition))
    {
        ErrorMessage = "温度图片删除失败！";
        return false;
    }
    return true;
}

// 对 Step 表的增删改查函数
bool Database::insert_step(const QString& tableName, const Step& step)
{
    query.prepare("INSERT INTO " + tableName + " (id, test_task_id, step_number, collecttime, continue_step, isthermometry, thermometry_pause_time) VALUES (:id, :test_task_id, :step_number, :collecttime, :continue_step, :isthermometry, :thermometry_pause_time)");
    query.bindValue(":id", step.id);
    query.bindValue(":test_task_id", step.test_task_id);
    query.bindValue(":step_number", step.step_number);
    query.bindValue(":collecttime", step.collecttime);
    query.bindValue(":continue_step", step.continue_step);
    query.bindValue(":isthermometry", step.isthermometry);
    query.bindValue(":thermometry_pause_time", step.thermometry_pause_time);
    if (!query.exec()) {
        QMessageBox::critical(parent, "错误", "无法插入 Step 数据: " + query.lastError().text(), QMessageBox::Ok);
        return false;
    }
    return true;
}

bool Database::get_step(const QString& tableName, const QString& condition, std::vector<Step>& steps)
{
    QString sqlQuery = "SELECT * FROM " + tableName;
    if (!condition.isEmpty()) {
        sqlQuery += " WHERE " + condition;
    }
    query.prepare(sqlQuery);
    if (!query.exec()) {
        QMessageBox::critical(parent, "错误", "无法查询 Step 数据: " + query.lastError().text(), QMessageBox::Ok);
        return false;
    }
    while (query.next()) {
        Step step;
        step.id = query.value("id").toString();
        step.test_task_id = query.value("test_task_id").toString();
        step.step_number = query.value("step_number").toInt();
        step.collecttime = query.value("collecttime").toDouble();
        step.continue_step = query.value("continue_step").toBool();
        step.isthermometry = query.value("isthermometry").toBool();
        step.thermometry_pause_time = query.value("thermometry_pause_time").toDouble();
        steps.push_back(step);
    }
    return true;
}

bool Database::update_step(const QString& tableName, const Step& step)
{
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT COUNT(*) FROM " + tableName + " WHERE id = :id");
    checkQuery.bindValue(":id", step.id);
    if (!checkQuery.exec() || !checkQuery.next() || checkQuery.value(0).toInt() == 0) {
        // 如果记录不存在
        return false;
    }

    query.prepare("UPDATE " + tableName + " SET test_task_id = :test_task_id, step_number = :step_number, collecttime = :collecttime, continue_step = :continue_step, isthermometry = :isthermometry, thermometry_pause_time = :thermometry_pause_time WHERE id = :id");
    query.bindValue(":test_task_id", step.test_task_id);
    query.bindValue(":step_number", step.step_number);
    query.bindValue(":collecttime", step.collecttime);
    query.bindValue(":continue_step", step.continue_step);
    query.bindValue(":isthermometry", step.isthermometry);
    query.bindValue(":thermometry_pause_time", step.thermometry_pause_time);
    query.bindValue(":id", step.id);
    if (!query.exec()) {
        // QMessageBox::critical(parent, "错误", "无法更新 Step 数据: " + query.lastError().text(), QMessageBox::Ok);
        return false;
    }

    return true;
}

bool Database::delete_step(const QString& tableName, const QString& id, QString& ErrorMessage)
{
    qDebug() << "删除 Step 数据: " + id;
    query.prepare("DELETE FROM " + tableName + " WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        ErrorMessage = query.lastError().text();
        return false;
    }
    qDebug() << "删除 Step 数据成功: " + id;
    return true;
}

// 对 PXIe5711Waveform 表的增删改查函数
bool Database::insert_pxie5711waveform(const QString& tableName, const PXIe5711Waveform& waveform)
{
    query.prepare("INSERT INTO " + tableName + 
        " (id, step_id, channel, waveform_type, amplitude, frequency, dutyCycle, positive_connect_location, negative_connect_location) "
        "VALUES (:id, :step_id, :channel, :waveform_type, :amplitude, :frequency, :dutyCycle, :positive_connect_location, :negative_connect_location)");
    
    query.bindValue(":id", waveform.id);
    query.bindValue(":step_id", waveform.step_id);
    query.bindValue(":channel", waveform.channel);
    query.bindValue(":waveform_type", waveform.waveform_type);
    query.bindValue(":amplitude", waveform.amplitude);
    query.bindValue(":frequency", waveform.frequency);
    query.bindValue(":dutyCycle", waveform.dutyCycle);
    query.bindValue(":positive_connect_location", waveform.positive_connect_location);
    query.bindValue(":negative_connect_location", waveform.negative_connect_location);
    
    if (!query.exec()) {
        QMessageBox::critical(parent, "错误", "无法插入 PXIe5711Waveform 数据: " + query.lastError().text(), QMessageBox::Ok);
        return false;
    }
    return true;
}

bool Database::get_pxie5711waveform(const QString& tableName, const QString& condition, std::vector<PXIe5711Waveform>& waveforms)
{
    QString sqlQuery = "SELECT * FROM " + tableName;
    if (!condition.isEmpty()) {
        sqlQuery += " WHERE " + condition;
    }
    query.prepare(sqlQuery);
    if (!query.exec()) {
        // QMessageBox::critical(parent, "错误", "无法查询 PXIe5711Waveform 数据: " + query.lastError().text(), QMessageBox::Ok);
        return false;
    }
    while (query.next()) {
        PXIe5711Waveform waveform;
        waveform.id = query.value("id").toString();
        waveform.step_id = query.value("step_id").toString();
        waveform.channel = query.value("channel").toInt();
        waveform.waveform_type = query.value("waveform_type").toString();
        waveform.amplitude = query.value("amplitude").toDouble();
        waveform.frequency = query.value("frequency").toDouble();
        waveform.dutyCycle = query.value("dutyCycle").toDouble();
        waveform.positive_connect_location = query.value("positive_connect_location").toInt();
        waveform.negative_connect_location = query.value("negative_connect_location").toInt();
        waveforms.push_back(waveform);
    }
    return true;
}

bool Database::update_pxie5711waveform(const QString& tableName, const PXIe5711Waveform& waveform)
{
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT COUNT(*) FROM " + tableName + " WHERE id = :id");
    checkQuery.bindValue(":id", waveform.id);
    if (!checkQuery.exec() || !checkQuery.next() || checkQuery.value(0).toInt() == 0) {
        // 如果记录不存在
        return false;
    }

    query.prepare("UPDATE " + tableName + 
        " SET step_id = :step_id, channel = :channel, waveform_type = :waveform_type, "
        "amplitude = :amplitude, frequency = :frequency, dutyCycle = :dutyCycle, "
        "positive_connect_location = :positive_connect_location, negative_connect_location = :negative_connect_location "
        "WHERE id = :id");
    
    query.bindValue(":step_id", waveform.step_id);
    query.bindValue(":channel", waveform.channel);
    query.bindValue(":waveform_type", waveform.waveform_type);
    query.bindValue(":amplitude", waveform.amplitude);
    query.bindValue(":frequency", waveform.frequency);
    query.bindValue(":dutyCycle", waveform.dutyCycle);
    query.bindValue(":positive_connect_location", waveform.positive_connect_location);
    query.bindValue(":negative_connect_location", waveform.negative_connect_location);
    query.bindValue(":id", waveform.id);
    
    if (!query.exec()) {
        QMessageBox::critical(parent, "错误", "无法更新 PXIe5711Waveform 数据: " + query.lastError().text(), QMessageBox::Ok);
        return false;
    }
    return true;
}

bool Database::delete_pxie5711waveform(const QString& tableName, const QString& id)
{
    query.prepare("DELETE FROM " + tableName + " WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        QMessageBox::critical(parent, "错误", "无法删除 PXIe5711Waveform 数据: " + query.lastError().text(), QMessageBox::Ok);
        return false;
    }
    return true;
}

// 对 PXIe5320Waveform 表的增删改查函数
bool Database::insert_pxie5320waveform(const QString& tableName, const PXIe5320Waveform& waveform)
{
    query.prepare("INSERT INTO " + tableName + 
        " (id, step_id, device, port, data, positive_connect_location, negative_connect_location) "
        "VALUES (:id, :step_id, :device, :port, :data, :positive_connect_location, :negative_connect_location)");
    
    query.bindValue(":id", waveform.id);
    query.bindValue(":step_id", waveform.step_id);
    query.bindValue(":device", waveform.device);
    query.bindValue(":port", waveform.port);
    QByteArray byteArray(reinterpret_cast<const char*>(waveform.data.data()), waveform.data.size() * sizeof(float));
    query.bindValue(":data", byteArray);
    query.bindValue(":positive_connect_location", waveform.positive_connect_location);
    query.bindValue(":negative_connect_location", waveform.negative_connect_location);
    
    if (!query.exec()) {
        QMessageBox::critical(parent, "错误", "无法插入 PXIe5320Waveform 数据: " + query.lastError().text(), QMessageBox::Ok);
        return false;
    }
    return true;
}

bool Database::get_pxie5320waveform(const QString& tableName, const QString& condition, std::vector<PXIe5320Waveform>& waveforms)
{
    QString sqlQuery = "SELECT * FROM " + tableName;
    if (!condition.isEmpty()) {
        sqlQuery += " WHERE " + condition;
    }
    query.prepare(sqlQuery);
    if (!query.exec()) {
        QMessageBox::critical(parent, "错误", "无法查询 PXIe5320Waveform 数据: " + query.lastError().text(), QMessageBox::Ok);
        return false;
    }
    while (query.next()) {
        PXIe5320Waveform waveform;
        waveform.id = query.value("id").toString();
        waveform.step_id = query.value("step_id").toString();
        waveform.device = query.value("device").toInt();
        waveform.port = query.value("port").toInt();
        QByteArray byteArray = query.value("data").toByteArray();
        waveform.data.resize(byteArray.size() / sizeof(float));
        memcpy(waveform.data.data(), byteArray.data(), byteArray.size());
        waveform.positive_connect_location = query.value("positive_connect_location").toInt();
        waveform.negative_connect_location = query.value("negative_connect_location").toInt();
        waveforms.push_back(waveform);
    }
    return true;
}

bool Database::update_pxie5320waveform(const QString& tableName, const PXIe5320Waveform& waveform)
{
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT COUNT(*) FROM " + tableName + " WHERE id = :id");
    checkQuery.bindValue(":id", waveform.id);
    if (!checkQuery.exec() || !checkQuery.next() || checkQuery.value(0).toInt() == 0) {
        // 如果记录不存在
        return false;
    }
    
    query.prepare("UPDATE " + tableName + 
        " SET step_id = :step_id, device = :device, port = :port, data = :data, "
        "positive_connect_location = :positive_connect_location, "
        "negative_connect_location = :negative_connect_location WHERE id = :id");
    
    query.bindValue(":step_id", waveform.step_id);
    query.bindValue(":device", waveform.device);
    query.bindValue(":port", waveform.port);
    QByteArray byteArray(reinterpret_cast<const char*>(waveform.data.data()), waveform.data.size() * sizeof(float));
    query.bindValue(":data", byteArray);
    query.bindValue(":positive_connect_location", waveform.positive_connect_location);
    query.bindValue(":negative_connect_location", waveform.negative_connect_location);
    query.bindValue(":id", waveform.id);
    if (!query.exec()) {
        qDebug() << "无法更新 PXIe5320Waveform 数据: " + query.lastError().text();
        QMessageBox::critical(parent, "错误", "无法更新 PXIe5320Waveform 数据: " + query.lastError().text(), QMessageBox::Ok);
        return false;
    }
    return true;
}

bool Database::delete_pxie5320waveform(const QString& tableName, const QString& id)
{
    query.prepare("DELETE FROM " + tableName + " WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        QMessageBox::critical(parent, "错误", "无法删除 PXIe5320Waveform 数据: " + query.lastError().text(), QMessageBox::Ok);
        return false;
    }
    return true;
}

bool Database::insert_8902data(const QString& tableName, const Data8902& data)
{
    query.prepare("INSERT INTO " + tableName + " (id, step_id, test_type, positive_connect_location, negative_connect_location) VALUES (:id, :step_id, :test_type, :positive_connect_location, :negative_connect_location)");
    query.bindValue(":id", data.id);
    query.bindValue(":step_id", data.step_id);
    query.bindValue(":test_type", data.test_type);
    query.bindValue(":positive_connect_location", data.positive_connect_location);
    query.bindValue(":negative_connect_location", data.negative_connect_location);
    if (!query.exec()) {
        QMessageBox::critical(parent, "错误", "无法插入 Data8902 数据: " + query.lastError().text(), QMessageBox::Ok);
        return false;
    }
    return true;
}

bool Database::get_8902data(const QString& tableName, const QString& condition, std::vector<Data8902>& datas)
{
    QString sqlQuery = "SELECT * FROM " + tableName;
    if (!condition.isEmpty()) {
        sqlQuery += " WHERE " + condition;
    }
    query.prepare(sqlQuery);
    if (!query.exec()) {
        QMessageBox::critical(parent, "错误", "无法查询 Data8902 数据: " + query.lastError().text(), QMessageBox::Ok);
        return false;
    }
    while (query.next()) {
        Data8902 data;
        data.id = query.value("id").toString();
        data.step_id = query.value("step_id").toString();
        data.test_type = query.value("test_type").toString();
        data.positive_connect_location = query.value("positive_connect_location").toInt();
        data.negative_connect_location = query.value("negative_connect_location").toInt();
        datas.push_back(data);
    }
    return true;
}

bool Database::delete_8902data(const QString& tableName, const QString& id)
{
    query.prepare("DELETE FROM " + tableName + " WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        QMessageBox::critical(parent, "错误", "无法删除 Data8902 数据: " + query.lastError().text(), QMessageBox::Ok);
        return false;
    }
    return true;
}

bool Database::update_8902data(const QString& tableName, const Data8902& data)
{
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT COUNT(*) FROM " + tableName + " WHERE id = :id");
    checkQuery.bindValue(":id", data.id);
    if (!checkQuery.exec() || !checkQuery.next() || checkQuery.value(0).toInt() == 0) {
        // 如果记录不存在
        return false;
    }

    query.prepare("UPDATE " + tableName + " SET test_type = :test_type, positive_connect_location = :positive_connect_location, negative_connect_location = :negative_connect_location WHERE id = :id");
    query.bindValue(":test_type", data.test_type);
    query.bindValue(":positive_connect_location", data.positive_connect_location);
    query.bindValue(":negative_connect_location", data.negative_connect_location);
    query.bindValue(":id", data.id);
    if (!query.exec()) {
        QMessageBox::critical(parent, "错误", "无法更新 Data8902 数据: " + query.lastError().text(), QMessageBox::Ok);
        return false;
    }
    return true;
}

bool Database::Build_5320data_table(const QString& TableName)
{
    query.prepare("CREATE TABLE IF NOT EXISTS " + TableName + " (id VARCHAR(50) PRIMARY KEY, serial_number INT, waveformid VARCHAR(50), data LONGBLOB)");
    if(!query.exec())
    {
        qDebug() << "无法创建 " + TableName + " 表: " + query.lastError().text();
        return false;
    }
    qDebug() << "创建 5320data 表成功: " + TableName;
    return true;
}

bool Database::insert_5320data(const QString& sqlFilePath)
{
    qDebug() << "插入 5320data 数据: " + sqlFilePath;
    QFile sqlFile(sqlFilePath);
    if (!sqlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开 SQL 文件: " + sqlFile.errorString();
        return false;
    }

    QTextStream in(&sqlFile);
    QString command = in.readAll();
    sqlFile.close();
    if(command.isEmpty())
    {
        return true;
    }
    if (!query.exec(command)) {
        qDebug() << "无法执行 SQL 脚本: " + query.lastError().text();
        return false;
    }
    return true;
}

bool Database::get_5320data(const QString& tableName, const QString& condition, std::vector<Data5320>& datas, QString& ErrorMessage)
{
    qDebug() << "查询 5320data 数据: " + tableName + " " + condition;
    QString sqlQuery = "SELECT * FROM " + tableName;
    if (!condition.isEmpty()) {
        sqlQuery += " WHERE " + condition;
    }
    query.prepare(sqlQuery);
    if (!query.exec()) {
        ErrorMessage = "无法查询 5320data 数据: " + query.lastError().text();
        return false;
    }
    while (query.next()) {
        Data5320 data;
        data.id = query.value("id").toString();
        data.serial_number = query.value("serial_number").toInt();
        data.waveform_id = query.value("waveformid").toString();
        QByteArray byteArray = QByteArray::fromBase64(query.value("data").toByteArray());
        QVector<float> floatVector(byteArray.size() / sizeof(float));
        memcpy(floatVector.data(), byteArray.data(), byteArray.size());
        data.data = std::vector<float>(floatVector.begin(), floatVector.end());
        datas.push_back(data);
    }
    qDebug() << "查询 5320data 数据成功: " << tableName << " 数据量: " << datas.size();
    return true;
}

bool Database::update_5320data(const Data5320& data)
{
    query.prepare("UPDATE 5320data SET serial_number = :serial_number, waveformid = :waveformid, data = :data WHERE id = :id");
    query.bindValue(":serial_number", data.serial_number);
    query.bindValue(":waveformid", data.waveform_id);
    QByteArray byteArray(reinterpret_cast<const char*>(data.data.data()), data.data.size() * sizeof(float));
    query.bindValue(":data", byteArray);
    query.bindValue(":id", data.id);
    if (!query.exec()) {
        qDebug() << "无法更新 5320data 数据: " + query.lastError().text();
        QMessageBox::critical(parent, "错误", "无法更新 5320data 数据: " + query.lastError().text(), QMessageBox::Ok);
        return false;
    }
    return true;
}

bool Database::insert_5320data_batch(const std::vector<Data5320>& datas)
{
    QSqlDatabase::database().transaction(); // 开始事务
    QString tableName = "5320data" + QString::number(datas[0].serial_number % 10);
    query.prepare("INSERT INTO " + tableName + " (serial_number, waveformid, data) VALUES (:serial_number, :waveformid, :data)");
    for(auto &data : datas)
    {
        // query.bindValue(":id", data.id);
        query.bindValue(":serial_number", data.serial_number);
        query.bindValue(":waveformid", data.waveform_id);
        QByteArray byteArray(reinterpret_cast<const char*>(data.data.data()), data.data.size() * sizeof(float));
        query.bindValue(":data", byteArray);
        if(!query.exec())
        {
            qDebug() << "批量插入数据失败: " + query.lastError().text();
            return false;
        }
    }
    QSqlDatabase::database().commit(); // 提交事务
    qDebug() << "批量插入数据成功";
    return true;
}
bool Database::DISABLE_KEYS(const QString& tableName)
{
    query.prepare("ALTER TABLE " + tableName + " DISABLE KEYS;");
    return query.exec();
}

bool Database::ENABLE_KEYS(const QString& tableName)
{
    query.prepare("ALTER TABLE " + tableName + " ENABLE KEYS;");
    return query.exec();
}

bool Database::loadCSVFile(const QString& filePath, const QString& tableName)
{   
    QString csvFilePath = filePath;
    qDebug() << "加载 CSV 文件: " << csvFilePath;
    QFile file(csvFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开 CSV 文件: " << csvFilePath;
        return false;
    }

    if(file.size() == 0)
    {
        qDebug() << "CSV 文件为空: " << csvFilePath;
        return true;
    }

    QSqlQuery localQuery(db); // 使用局部查询对象以支持并发操作
    // 构建 LOAD DATA LOCAL INFILE 语句
    QString loadDataQuery = QString("LOAD DATA LOCAL INFILE '%1' "
                                     "INTO TABLE %2 "
                                     "FIELDS TERMINATED BY ',' "
                                     "ENCLOSED BY '\"' "
                                     "LINES TERMINATED BY '\r\n' "
                                     "IGNORE 1 LINES;")
                            .arg(csvFilePath)
                            .arg(tableName);
    localQuery.prepare(loadDataQuery);
    qDebug() << "执行加载 CSV 文件的 SQL 语句: " << loadDataQuery;
    if (!localQuery.exec()) {
        qDebug() << "无法加载 CSV 文件: " + localQuery.lastError().text();
        return false;
    }
    qDebug() << "CSV 文件加载成功: " << csvFilePath;
    return true;
}
// 对 Image 表的增删改查函数
bool Database::insert_image(const QString& tableName, const Image& image, QString& ErrorMessage)
{
    query.prepare("INSERT INTO " + tableName + " (id, step_id, task_table_name, device, image_data, temp_data, temp_width, temp_height) VALUES (:id, :step_id, :task_table_name, :device, :image_data, :temp_data, :temp_width, :temp_height)");
    query.bindValue(":id", image.id);
    query.bindValue(":step_id", image.step_id);
    query.bindValue(":task_table_name", image.task_table_name);
    query.bindValue(":device", image.device);
    query.bindValue(":image_data", image.image_data);
    query.bindValue(":temp_data", image.temp_data);
    query.bindValue(":temp_width", image.temp_width);
    query.bindValue(":temp_height", image.temp_height);
    if (!query.exec()) {
        ErrorMessage = "无法插入 Image 数据: " + query.lastError().text();
        return false;
    }
    return true;
}

bool Database::get_image(const QString& tableName, const QString& condition, std::vector<Image>& images, QString& ErrorMessage)
{
    QString sqlQuery = "SELECT * FROM " + tableName;
    if (!condition.isEmpty()) {
        sqlQuery += " WHERE " + condition;
    }
    query.prepare(sqlQuery);
    if (!query.exec()) {
        ErrorMessage = "无法查询 Image 数据: " + query.lastError().text();
        return false;
    }
    while (query.next()) {
        Image image;
        image.id = query.value("id").toString();
        image.step_id = query.value("step_id").toString();
        image.task_table_name = query.value("task_table_name").toString();
        image.device = query.value("device").toString();
        image.image_data = query.value("image_data").toByteArray();
        image.temp_data = query.value("temp_data").toByteArray();
        image.temp_width = query.value("temp_width").toInt();
        image.temp_height = query.value("temp_height").toInt();
        images.push_back(image);
    }
    return true;
}

bool Database::update_image(const QString& tableName, const Image& image)
{
    query.prepare("UPDATE " + tableName + " SET step_id = :step_id, task_table_name = :task_table_name, device = :device, image_data = :image_data, temp_data = :temp_data, temp_width = :temp_width, temp_height = :temp_height WHERE id = :id");
    query.bindValue(":step_id", image.step_id);
    query.bindValue(":task_table_name", image.task_table_name);
    query.bindValue(":device", image.device);
    query.bindValue(":image_data", image.image_data);
    query.bindValue(":temp_data", image.temp_data);
    query.bindValue(":temp_width", image.temp_width);
    query.bindValue(":temp_height", image.temp_height);
    query.bindValue(":id", image.id);
    if (!query.exec()) {
        QMessageBox::critical(parent, "错误", "无法更新 Image 数据: " + query.lastError().text(), QMessageBox::Ok);
        return false;
    }
    return true;
}

bool Database::delete_image(const QString& tableName, const QString& condition)
{
    // 检查condition是否包含等号,如果包含则需要用引号包裹值
    QString finalCondition;
    if(condition.contains("=")) {
        QStringList parts = condition.split("=");
        if(parts.length() == 2) {
            QString field = parts[0].trimmed();
            QString value = parts[1].trimmed();
            // 如果值不是纯数字,需要加引号
            bool isNumber = false;
            value.toInt(&isNumber);
            if(!isNumber) {
                finalCondition = field + "= '" + value + "'";
            } else {
                finalCondition = condition;
            }
        }
    } else {
        finalCondition = condition;
    }

    query.prepare("DELETE FROM " + tableName + " WHERE " + finalCondition);
    qDebug() << "删除 Image 数据: " + tableName + " " + finalCondition;
    if (!query.exec()) {
        QMessageBox::critical(parent, "错误", "无法删除 Image 数据: " + query.lastError().text(), QMessageBox::Ok);
        return false;
    }
    return true;
}
