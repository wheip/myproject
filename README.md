# FaultDetect 项目

## 项目简介
FaultDetect 是一套基于 Qt 的故障检测系统，集成了图像处理、设备管理、任务管理以及数据库操作等多项功能。项目主要包含以下模块：
- **图像匹配与识别**：利用 OpenCV（支持 GPU 加速）实现 SIFT 特征提取与匹配，构建描述符数据库以进行 PCB 模型与元件的识别。
- **设备管理**：提供设备的增删改查接口，通过 MySQL 数据库存储设备信息，并支持图像数据的存储和展示。
- **任务管理**：支持多任务流程管理，用户可通过流程图界面创建、编辑及删除任务步骤。
- **摄像头与视频处理**：集成摄像头捕捉与处理模块，支持实时视频采集及故障检测。

## 系统需求
- 操作系统：Windows 64 位（其他平台亦可，但部分硬件相关模块可能需要适配）
- 编译器：支持 C++17 的编译器（MSVC）
- Qt 版本：推荐 Qt 5.x 或 Qt 6.x
- OpenCV：版本与项目配置相符（例如 OpenCV 4.1.1 或兼容版本）
- 数据库：MySQL 数据库（需安装相应的数据库驱动，例如 QMYSQL）
- 其他依赖：ONNXRuntime，PXIe5711、PXIe5320、PXIe8902 等设备驱动库（请确保库的路径和版本与项目一致）

## 依赖库
- **Qt 模块**：core, gui, widgets, sql, charts, concurrent, printsupport, serialport 等。
- **OpenCV**：用于图像处理、特征检测与匹配，支持 CUDA 加速（如果设备支持）。
- **ONNXRuntime**：用于深度学习模型推理（YOLO 模型）。
- **设备驱动库**：PXIe 系列驱动库以及其他硬件相关库。
- **MySQL**：用于设备与任务信息的存储。

## 构建与运行

### 构建步骤
1. **安装依赖：**
   - 安装 Qt SDK，并确保编译器（例如 MSVC 或 MinGW）支持 C++17。
   - 安装并配置 OpenCV（库文件及头文件路径需在项目的 `.pro` 文件中正确设置）。
   - 安装 ONNXRuntime 和其他设备相关驱动库，并确保库文件路径正确配置于项目中（例如 `$$PWD/lib/...`）。
   - 配置 MySQL 数据库，并确保系统安装了 MySQL 客户端以及对应的 Qt 插件（QMYSQL）。

2. **配置项目：**
   - 打开项目根目录下的 `FaultDetect.pro` 文件，可使用 Qt Creator 导入项目。
   - 根据你的环境修改 MySQL 数据库连接参数（如主机、用户名、密码及数据库名称），保证文件 [mysql/database.h/cpp] 中的配置满足当前数据库状态。

3. **编译项目：**
   - 在 Qt Creator 内选择合适的构建工具链进行编译。
   - 或使用命令行工具执行 `qmake FaultDetect.pro` 后再执行 `make`（或 `nmake`、`jom` 等）。

### 运行说明
- 编译完成后，运行生成的可执行文件。程序主界面将显示故障检测、设备管理、任务流程管理等功能模块。
- 系统集成了多线程及并发机制，支持实时摄像头采集、特征匹配和图像分析。
- 如遇运行错误，请检查依赖库（如 OpenCV、ONNXRuntime）是否正确加载，以及项目中的路径配置是否与实际环境匹配。

## 项目结构
- **mysql/**  
  包含数据库操作相关文件（`database.h`、`database.cpp`），封装了 MySQL 的连接、查询、更新等操作。

- **PCB_Model_Identification/**  
  主要实现 PCB 模型识别功能，包括描述符的提取、匹配以及数据库的创建和更新（例如 `siftmatcher.h/cpp`、`previewdialog.cpp`、`imageflowdialog.*` 等）。

- **PCB_Components_Detect/**  
  实现 PCB 组件检测和标签编辑功能，包含 `labelediting`、`pcbcomponentsdetect`、`yolomodel` 等模块。

- **TestTask/**  
  涉及任务管理、流程组织等功能模块，包括 `FlowTaskManager`、`StepEditDialog`、`executetask` 等。

- **Detect_Devices/**  
  实现设备搜索、添加与修改等功能（例如 `createdevicedialog.*`、`devicemanager.*`）。

- **Camera/** 与 **InfraredCamera/**  
  提供摄像头及红外摄像头的视频采集与图像处理支持。

- **ClassList.h**  
  定义了项目常用的核心数据结构与类，如 `Device`、`Label`、`Waveform`、`TestTask` 等。

- **FaultDetect.pro**  
  Qt 项目文件，配置了所有源文件、头文件、UI 文件、资源文件及外部库链接。

- **TaskManager/**  
  包含任务流程图管理、步骤编辑、输出/采集端口管理等相关代码。

## 注意事项
- **依赖库版本**：请确保 OpenCV、ONNXRuntime 以及各设备驱动库版本匹配，避免因版本不一致导致运行错误。
- **数据库配置**：在使用前请根据实际环境修改数据库连接参数，确保可正常连接 MySQL 数据库。
- **多线程调试**：项目使用了多线程和并发处理，调试时请注意线程安全及资源释放问题。
- **路径配置**：部分文件与库的路径在 `.pro` 文件中配置，请根据实际文件位置进行调整。

## 开发者
- **开发团队/作者**：whp
- **更新日期**：2025-02-21 