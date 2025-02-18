#include "FolderCheck.h"
#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <QDir>
#include "database.h"

FolderCheck::FolderCheck() {
    // 启动工作线程
    worker = std::thread(&FolderCheck::WorkerThread, this);
}

FolderCheck::~FolderCheck() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        running = false;
    }
    cv.notify_one();
    if(worker.joinable()) {
        worker.join();
    }
}

void FolderCheck::Check_Folder(QString device_id)
{
    FolderTask task;
    task.device_id = device_id;
    
    {
        std::lock_guard<std::mutex> lock(mutex);
        tasks.push(std::move(task));
    }
    
    cv.notify_one();
}

void FolderCheck::WorkerThread()
{
    while(running) {
        FolderTask task;
        
        {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this]() { return !tasks.empty() || !running; });
            
            if(!running && tasks.empty()) {
                return;
            }
            
            task = std::move(tasks.front());
            tasks.pop();
        }
        
        ProcessTask(task);
    }
}

void FolderCheck::ProcessTask(const FolderTask& task)
{
    try {
        // 只检查并创建 IMAGE 文件夹
        if (!QDir("./IMAGE").exists()) {
            QDir().mkpath("./IMAGE");
        }

        Database db("FolderCheck_" + task.device_id, nullptr);

        // 获取设备
        std::vector<Device> devices;
        if(task.device_id != "") {
            Device device;
            device.id = task.device_id;
            devices.push_back(device);
        } else {
            if(!db.get_device("", devices, false)) {
                std::cout << "设备获取失败！" << std::endl;
                return;
            }
        }

        // 一次性创建所有需要的目录
        for(const auto& device : devices) {
            QString table_name = device.id + "$$testtask";
            std::vector<TestTask> testtasks;
            QString ErrorMessage;
            
            if(!db.get_testtask(table_name, "", testtasks, ErrorMessage)) {
                std::cout << "测试任务获取失败！" << std::endl;
                return;
            }

            QStringList paths;
            paths << "./CollectData/" + device.id;
            for(const auto& task : testtasks) {
                paths << "./CollectData/" + device.id + "/" + task.id;
            }

            for(const auto& path : paths) {
                if(!QDir(path).exists()) {
                    QDir().mkpath(path);
                }
            }
        }
        
        db.disconnect();

    } catch (const std::exception& e) {
        std::cout << "文件夹创建异常：" << e.what() << std::endl;
    }
}

bool FolderCheck::Delete_Folder(QString path)
{
    if(QDir(path).exists()) {
        QDir(path).removeRecursively();
        return true;
    }
    return false;
}

bool FolderCheck::Create_Folder(QString path)
{
    if(!QDir(path).exists()) {
        QDir().mkpath(path);
        return true;
    }
    return false;
}

std::vector<QString> FolderCheck::Get_Folder_list(QString path)
{
    std::vector<QString> folder_list;
    if(!QDir(path).exists()) {
        std::cout << "文件夹不存在！" << std::endl;
        return folder_list;
    }
    QDir dir(path);
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name);
    QStringList entries = dir.entryList();
    folder_list.assign(entries.begin(), entries.end());
    return folder_list;
}
