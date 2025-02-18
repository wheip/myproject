#ifndef CONNECTIONLOCATIONDIALOG_H
#define CONNECTIONLOCATIONDIALOG_H

#include <QDialog>
#include "PCB_Components_Detect/labelediting.h"
#include <memory>

class ConnectionLocationDialog : public QDialog {
    Q_OBJECT
public:
    ConnectionLocationDialog(const QImage& image, const std::vector<Label>& labels, QWidget* parent = nullptr);
    int getSelectedLabelId() const { return selectedLabelId; }

private:
    std::shared_ptr<LabelEditingWindow> labelEditor;
    int selectedLabelId = -1;

private slots:
    void onWindowClosed();
};

#endif // CONNECTIONLOCATIONDIALOG_H 