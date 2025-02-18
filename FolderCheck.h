#ifndef FOLDERCHECK_H
#define FOLDERCHECK_H
#include "ClassList.h"
#include "database.h"
#include <mutex>
#include <condition_variable>
#include <queue>
#include <QThread>
#include <atomic>

// 定义全局访问宏
#define g_FolderCheck FolderCheck::GetInstance()

// 定义任务结构体
struct FolderTask {
    QString device_id;
    std::promise<std::pair<bool, std::string>> promise;
};

class FolderCheck
{
public:
    static FolderCheck& GetInstance() {
        static FolderCheck instance;
        return instance;
    }

    FolderCheck(const FolderCheck&) = delete;
    FolderCheck& operator=(const FolderCheck&) = delete;

    ~FolderCheck();

    void Check_Folder(QString device_id = "");

    bool Delete_Folder(QString path);

    bool Create_Folder(QString path);

    std::vector<QString> Get_Folder_list(QString path);

private:
    FolderCheck();
    void WorkerThread();
    void ProcessTask(const FolderTask& task);

    std::mutex mutex;
    std::condition_variable cv;
    std::queue<FolderTask> tasks;
    std::atomic<bool> running{true};
    std::thread worker;
};

#endif // FOLDERCHECK_H
