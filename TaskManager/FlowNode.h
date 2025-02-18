#ifndef FLOWNODE_H
#define FLOWNODE_H

#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QPainter>
#include <QString>
#include <QGraphicsSceneMouseEvent>
#include <QLineEdit>
#include <QObject>
#include <QGraphicsSceneHoverEvent>
#include <QMenu>
#include "ClassList.h"

class FlowNode : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_PROPERTY(QGraphicsItem* graphicsItem READ graphicsItem)
    Q_PROPERTY(bool continueStep READ getContinueStep WRITE setContinueStep NOTIFY continueStepChanged)

signals:
    void nodeDeleted(FlowNode* node);  // 添加节点删除信号
    void nodeRunRequested(FlowNode* node);  // 添加运行信号
    void continueStepChanged(bool);    // 添加 continue_step 改变信号

public:
    static const qreal NODE_WIDTH;
    static const qreal NODE_HEIGHT;
    static const qreal CONNECTOR_SIZE;

    FlowNode(const QString &text, bool isRoot = false, const QString &taskId = "", QGraphicsItem *parent = nullptr);
    ~FlowNode() override;
    
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    
    void setText(const QString &text);
    QString getText() const;
    
    // 添加这个函数来满足 Q_PROPERTY 的需求
    QGraphicsItem* graphicsItem() { return this; }
    
    Step stepData;  // 步骤数据
    void updateConnectorStyle();  // 更新连接线样式

    // 指向链表中下一个步骤节点
    FlowNode *next;

    // 添加 continue_step 的 getter 和 setter
    bool getContinueStep() const { return stepData.continue_step; }
    void setContinueStep(bool value);

    // 设置前一个节点
    void setPrevNode(FlowNode* node);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private slots:
    void finishEditing();

private:
    QString nodeText;
    QRectF rect;
    bool isEditing;
    QLineEdit *textEditor;
    QString taskId;  // 存储任务ID
    bool isSelected;  // 添加选中状态
    bool isRootNode;  // 添加标识是否为根节点的标志
    

    QGraphicsLineItem* m_connector;     // 连接线
    QGraphicsPathItem* m_blockSymbol;   // 阻塞符号（当 continue_step 为 false 时显示）
    QGraphicsRectItem* m_taskIdRect;    // 任务ID矩形框
    QGraphicsTextItem* m_taskIdText;    // 任务ID文本
    FlowNode* prevNode;                 // 前一个节点的指针
    void createConnector();             // 创建连接线
    void updateBlockSymbol();           // 更新阻塞符号
    void updateConnectorPosition();      // 更新连接线位置

    void createTextEditor();
    void showContextMenu(const QPointF& pos);

    // 样式相关
    QColor backgroundColor;
    QColor borderColor;
    QColor connectorColor;
    int borderWidth;
    qreal cornerRadius;
};

#endif // FLOWNODE_H 
