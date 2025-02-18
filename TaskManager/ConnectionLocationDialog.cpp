#include "ConnectionLocationDialog.h"
#include <QVBoxLayout>

ConnectionLocationDialog::ConnectionLocationDialog(const QImage& image, const std::vector<Label>& labels, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("选择接线位置");
    setModal(true);
    resize(1000, 600);

    QVBoxLayout* layout = new QVBoxLayout(this);
    
    // 创建标签编辑窗口
    std::vector<Label> labelsAdd;  // 空的新增标签列表
    std::vector<int> deleteIds;    // 空的删除ID列表
    labelEditor = std::make_shared<LabelEditingWindow>(this, image, labels, labelsAdd, deleteIds);
    labelEditor->setSelectModel();  // 设置为选择模式
    
    layout->addWidget(labelEditor.get());
    
    // 连接关闭信号
    connect(labelEditor.get(), &LabelEditingWindow::window_close, this, &ConnectionLocationDialog::onWindowClosed);
}

void ConnectionLocationDialog::onWindowClosed()
{
    selectedLabelId = labelEditor->getSelectedLabelId();
    accept();
} 