QT       += core gui sql charts core5compat concurrent printsupport serialport

QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO += -Od

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# CONFIG += moc

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    FolderCheck.cpp \
    ch340.cpp \
    InfraredCamera/irimagedisplay.cpp \
    PCB_Components_Detect/labelediting.cpp \
    PCB_Components_Detect/labelrectitem.cpp \
    PCB_Components_Detect/pcbcomponentsdetect.cpp \
    PCB_Components_Detect/yolomodel.cpp \
    PCB_Components_Detect/labelimageviewdialog.cpp \
    PCB_Components_Detect/labelmanagerdialog.cpp \
    TestTask/datamanager.cpp \
    TestTask/executetask.cpp \
    TestTask/managetask.cpp \
    TestTask/taskconnectwire.cpp \
    TestTask/uestcqcustomplot.cpp \
    camera/detectcamera.cpp \
    InfraredCamera/camera.cpp \
    include/qcustomplot.cpp \
    imageviewer.cpp \
    led.cpp \
    main.cpp \
    mainwindow.cpp \
    PXIe5711/pxie5711.cpp \
    PXIe5320/pxie5320.cpp \
    mysql/database.cpp \
    pxie8902/pxie8902.cpp \
    Detect_Devices/createdevicedialog.cpp \
    Detect_Devices/devicemanager.cpp \
    TaskManager/FlowTaskManager.cpp \
    TaskManager/FlowNode.cpp \
    TaskManager/AcquisitionManageDialog.cpp \
    TaskManager/StepEditDialog.cpp \
    TaskManager/VisioView.cpp \
    TaskManager/WaveformManageDialog.cpp \
    TaskManager/ConnectionLocationDialog.cpp \
    PCB_Model_Identification/previewdialog.cpp \
    PCB_Model_Identification/siftmatcher.cpp \
    PCB_Model_Identification/ImageDialog/imageflowdialog.cpp \


HEADERS += \
    FolderCheck.h \
    ch340.h \
    IconImage.h \
    InfraredCamera/irimagedisplay.h \
    PCB_Components_Detect/labelediting.h \
    PCB_Components_Detect/labelrectitem.h \
    PCB_Components_Detect/pcbcomponentsdetect.h \
    PCB_Components_Detect/yolomodel.h \
    PCB_Components_Detect/labelimageviewdialog.h \
    PCB_Components_Detect/labelmanagerdialog.h \
    TestTask/ThreadPool.hpp \
    TestTask/datamanager.h \
    TestTask/executetask.h \
    TestTask/managetask.h \
    TestTask/taskconnectwire.h \
    TestTask/uestcqcustomplot.h \
    camera/detectcamera.h \
    InfraredCamera/camera.h \
    include/qcustomplot.h \
    imageviewer.h \
    led.h \
    mainwindow.h \
    ClassList.h \
    PXIe5711/pxie5711.h \
    PXIe5320/pxie5320.h \
    mysql/database.h \
    pxie8902/pxie8902.h \
    Detect_Devices/createdevicedialog.h \
    Detect_Devices/devicemanager.h \
    deviceid.h \
    TaskManager/FlowTaskManager.h \
    TaskManager/FlowNode.h \
    TaskManager/AcquisitionManageDialog.h \
    TaskManager/StepEditDialog.h \
    TaskManager/VisioView.h \
    TaskManager/WaveformManageDialog.h \
    TaskManager/ConnectionLocationDialog.h \
    PCB_Model_Identification/previewdialog.h \
    PCB_Model_Identification/siftmatcher.h \
    PCB_Model_Identification/ImageDialog/imagedialog.h \
    PCB_Model_Identification/ImageDialog/imageflowdialog.h \


FORMS += \
    PCB_Components_Detect/pcbcomponentsdetect.ui \
    TestTask/managetask.ui \
    TestTask/taskconnectwire.ui \
    camera/detectcamera.ui \
    InfraredCamera/camera.ui \
    mainwindow.ui \
    PXIe5711/PXIe5711.ui \
 \#    PXIe5320/PXIe5320.ui 
    Detect_Devices/createdevicedialog.ui \
    Detect_Devices/devicemanager.ui

INCLUDEPATH += \
    $$PWD/camera \
    $$PWD/TestTask \
    $$PWD/InfraredCamera \
    $$PWD/include \
    $$PWD/PXIe5711 \
    $$PWD/PXIe5320 \
    $$PWD/PXIe8902 \
    $$PWD/mysql \
    $$PWD/PCB_Components_Detect \
    $$PWD/Detect_Devices \
    $$PWD/PCB_Model_Identification \
    $$PWD/TaskManager \
    $$PWD/PCB_Model_Identification/ImageDialog

LIBS += -L"$$PWD/lib/windows/x64" -lRtNet \
        -L"$$PWD/lib/opencv_release" -lopencv_world4110 \
        -L"$$PWD/lib/onnxruntime" -lonnxruntime -lonnxruntime_providers_cuda -lonnxruntime_providers_shared \
        -L"$$PWD/lib/pxie5711" -lJY5710Core \
        -L"$$PWD/lib/pxie5320" -lJY5320Core \
        -L"$$PWD/lib/pxie8902" -lJY8902Core

win32 {
    CONFIG(release, debug|release) {
        # 创建目标目录（如果不存在）
        QMAKE_POST_LINK += $$quote(cmd /c if not exist \"$$OUT_PWD/release\" mkdir \"$$OUT_PWD/release\" $$escape_expand(\n\t))

        # 复制OpenCV DLL文件
        QMAKE_POST_LINK += $$quote(cmd /c copy /y \"$$PWD/bin\\*.dll\" \"$$OUT_PWD/release\" $$escape_expand(\n\t))

        # 如果需要复制其他文件，可以继续添加copy命令
        # QMAKE_POST_LINK += $$quote(cmd /c copy /y \"$$PWD/other_files\\*.*\" \"$$OUT_PWD/release\" $$escape_expand(\n\t))
    }
}
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
