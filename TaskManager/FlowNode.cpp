#include "FlowNode.h"
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include "StepEditDialog.h"
#include <QMessageBox>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QDebug>
#include <QLinearGradient>

const qreal FlowNode::NODE_WIDTH = 180;
const qreal FlowNode::NODE_HEIGHT = 60;
const qreal FlowNode::CONNECTOR_SIZE = 12;

FlowNode::FlowNode(const QString &text, bool isRoot, const QString &taskId, QGraphicsItem *parent)
    : QObject()
    , QGraphicsItem(parent)
    , nodeText(text)
    , rect(0, 0, NODE_WIDTH, NODE_HEIGHT)
    , isEditing(false)
    , textEditor(nullptr)
    , next(nullptr)
    , isRootNode(isRoot)
    , m_connector(nullptr)
    , m_blockSymbol(nullptr)
    , taskId(taskId)
    , backgroundColor(QColor("#2ecc71"))  // 所有节点使用相同的绿色
    , borderColor(QColor("#27ae60"))
    , connectorColor(QColor("#27ae60"))
    , borderWidth(isRoot ? 2 : 1)
    , cornerRadius(8)
    , isSelected(false)
    , prevNode(nullptr)
    , m_taskIdRect(nullptr)
    , m_taskIdText(nullptr)
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
    stepData.continue_step = false;  // 初始化为阻塞状态
    createConnector();
    
    // 连接 continueStepChanged 信号到 updateConnectorStyle 槽
    connect(this, &FlowNode::continueStepChanged, this, &FlowNode::updateConnectorStyle);
    
    // 如果是第一个节点，创建任务ID矩形框
    if (isRoot) {
        // 创建临时文本项来计算文本大小
        QGraphicsTextItem* tempText = new QGraphicsTextItem();
        tempText->setPlainText("任务ID:\n" + taskId);
        QFont font = tempText->font();
        font.setBold(true);
        tempText->setFont(font);
        QRectF textRect = tempText->boundingRect();
        delete tempText;
        
        // 计算矩形框大小，添加一些内边距
        qreal padding = 20;  // 文本周围的内边距
        qreal rectWidth = std::max(100.0, textRect.width() + padding * 2);  // 最小宽度为100
        qreal rectHeight = textRect.height() + padding * 2;
        
        // 创建矩形框，使用不同的样式
        m_taskIdRect = new QGraphicsRectItem(this);
        // 设置矩形框位置，与节点保持一定距离
        qreal spacing = 50;  // 矩形框与节点之间的距离
        m_taskIdRect->setRect(-rectWidth - spacing, 
                             (NODE_HEIGHT - rectHeight) / 2,  // 垂直居中
                             rectWidth, 
                             rectHeight);
        
        // 矩形框使用蓝色系
        QColor rectColor = QColor("#3498db");
        QColor rectBorderColor = QColor("#2980b9");
        m_taskIdRect->setPen(QPen(rectBorderColor, 2));
        
        // 创建渐变背景
        QLinearGradient gradient(0, 0, 0, rectHeight);
        gradient.setColorAt(0, rectColor.lighter(110));
        gradient.setColorAt(1, rectColor);
        m_taskIdRect->setBrush(gradient);
        
        // 创建文本项并设置为矩形框的子项
        m_taskIdText = new QGraphicsTextItem(m_taskIdRect);
        m_taskIdText->setDefaultTextColor(Qt::white);
        m_taskIdText->setPlainText("任务ID:\n" + taskId);
        m_taskIdText->setFont(font);
        
        // 在矩形框内居中显示文本
        QRectF actualTextRect = m_taskIdText->boundingRect();
        m_taskIdText->setPos(-rectWidth - spacing + (rectWidth - actualTextRect.width()) / 2,
                            (NODE_HEIGHT - rectHeight) / 2 + (rectHeight - actualTextRect.height()) / 2);
    }
}

FlowNode::~FlowNode()
{
    delete m_connector;
    delete m_blockSymbol;
    delete m_taskIdRect;  // m_taskIdText 会随 m_taskIdRect 自动删除
}

void FlowNode::createConnector()
{
    // 创建连接线（初始为阻塞样式）
    m_connector = new QGraphicsLineItem(this);
    m_connector->setLine(0, NODE_HEIGHT/2, -50, NODE_HEIGHT/2);  // 普通节点的连接线
    QPen pen(Qt::red);  // 默认使用红色
    pen.setWidth(2);
    m_connector->setPen(pen);
    // 禁用缓存
    m_connector->setCacheMode(QGraphicsItem::NoCache);
    
    // 创建阻塞符号（初始显示）
    m_blockSymbol = new QGraphicsPathItem(this);
    QPainterPath path;
    // 绘制一个 X 形状
    path.moveTo(-30, NODE_HEIGHT/2 - 10);
    path.lineTo(-20, NODE_HEIGHT/2);
    path.lineTo(-30, NODE_HEIGHT/2 + 10);
    path.moveTo(-20, NODE_HEIGHT/2 - 10);
    path.lineTo(-30, NODE_HEIGHT/2);
    path.lineTo(-20, NODE_HEIGHT/2 + 10);
    m_blockSymbol->setPath(path);
    m_blockSymbol->setPen(QPen(Qt::red, 2));
    // 禁用缓存
    m_blockSymbol->setCacheMode(QGraphicsItem::NoCache);
    m_blockSymbol->show();  // 默认显示阻塞符号
}

void FlowNode::updateConnectorStyle()
{
    if (!m_connector || !m_blockSymbol) return;
    
    qDebug() << "updateConnectorStyle triggered, continue_step:" << stepData.continue_step;
    
    // 通知项几何形状准备更新
    prepareGeometryChange();
    
    // 根据 continue_step 更新连接线样式
    if (stepData.continue_step) {
        m_connector->setPen(QPen(Qt::black, 2));
        m_blockSymbol->hide();
    } else {
        m_connector->setPen(QPen(Qt::red, 2));
        m_blockSymbol->show();
    }
    // 触发当前项及子项的重新绘制
    m_connector->update();
    m_blockSymbol->update();
    update(); // 更新当前 FlowNode 绘制

    // 强制刷新整个场景区域
    if (scene())
        scene()->update(scene()->sceneRect());
}

QRectF FlowNode::boundingRect() const
{
    // 如果是根节点，包含左侧矩形框的范围
    if (isRootNode) {
        return QRectF(-150, -NODE_HEIGHT/2, NODE_WIDTH + 150, NODE_HEIGHT * 2);
    } else {
        return QRectF(-50, 0, NODE_WIDTH + 50, NODE_HEIGHT);
    }
}

void FlowNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    painter->setRenderHint(QPainter::Antialiasing);
    
    // 绘制节点背景
    QLinearGradient gradient(0, 0, 0, NODE_HEIGHT);
    gradient.setColorAt(0, backgroundColor.lighter(110));
    gradient.setColorAt(1, backgroundColor);
    
    QPainterPath path;
    path.addRoundedRect(rect, cornerRadius, cornerRadius);
    
    // 如果节点被选中，使用更粗的边框
    if (isSelected) {
        painter->setPen(QPen(borderColor, borderWidth + 2));
    } else {
        painter->setPen(QPen(borderColor, borderWidth));
    }
    painter->setBrush(gradient);
    painter->drawPath(path);
    
    // 绘制文本
    painter->setPen(Qt::white);
    QFont font = painter->font();
    font.setBold(true);
    painter->setFont(font);
    
    // 所有节点都显示步骤文本
    painter->drawText(rect, Qt::AlignCenter, nodeText);
    
    // 绘制右侧连接点
    painter->setPen(Qt::NoPen);
    painter->setBrush(connectorColor);
    painter->drawEllipse(QRectF(NODE_WIDTH - CONNECTOR_SIZE/2, NODE_HEIGHT/2 - CONNECTOR_SIZE/2, CONNECTOR_SIZE, CONNECTOR_SIZE));
}

void FlowNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // 处理选中状态
    isSelected = true;
    update();  // 重绘节点以显示选中效果
    
    // 调用基类的鼠标按下事件处理
    QGraphicsItem::mousePressEvent(event);
}

void FlowNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    // 如果是根节点，不处理双击事件
    
    emit nodeRunRequested(this);
    
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void FlowNode::createTextEditor()
{
    if (!isEditing) {
        isEditing = true;
        textEditor = new QLineEdit;
        textEditor->setText(nodeText);
        textEditor->setAlignment(Qt::AlignCenter);
        
        QGraphicsProxyWidget *proxy = scene()->addWidget(textEditor);
        proxy->setPos(pos().x(), pos().y() + (NODE_HEIGHT - textEditor->height()) / 2);
        proxy->setZValue(zValue() + 1);
        
        connect(textEditor, &QLineEdit::editingFinished, this, &FlowNode::finishEditing);
        textEditor->setFocus();
        textEditor->selectAll();
    }
}

void FlowNode::finishEditing()
{
    if (isEditing && textEditor) {
        nodeText = textEditor->text();
        scene()->removeItem(textEditor->graphicsProxyWidget());
        delete textEditor;
        textEditor = nullptr;
        isEditing = false;
        update();
    }
}

void FlowNode::setText(const QString &text)
{
    nodeText = text;
    update();
}

QString FlowNode::getText() const
{
    return nodeText;
}

void FlowNode::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    showContextMenu(event->screenPos());
    event->accept();
}

void FlowNode::showContextMenu(const QPointF& pos)
{
    QMenu menu;
    
    // 添加删除选项
    QAction *deleteAction = menu.addAction("删除");
    connect(deleteAction, &QAction::triggered, [this]() {
        QMessageBox::StandardButton reply = QMessageBox::question(
            nullptr,
            "删除节点",
            "确定删除步骤节点吗？",
            QMessageBox::Yes | QMessageBox::No
        );
        if(reply == QMessageBox::Yes) {
            emit nodeDeleted(this);
        }
    });
    
    // 预留菜单项
    QAction *runAction = menu.addAction("运行");
    
    menu.exec(pos.toPoint());
}

void FlowNode::setContinueStep(bool value)
{
    if (stepData.continue_step != value) {
        stepData.continue_step = value;
        emit continueStepChanged(value);
    }
}

void FlowNode::updateConnectorPosition()
{
    if (!m_connector) return;
    
    if (prevNode) {
        // 计算连接线的起点和终点
        QPointF startPoint = mapFromScene(prevNode->scenePos() + QPointF(NODE_WIDTH, NODE_HEIGHT/2));
        QPointF endPoint = QPointF(0, NODE_HEIGHT/2);
        m_connector->setLine(QLineF(endPoint, startPoint));
    }
}

void FlowNode::setPrevNode(FlowNode* node)
{
    prevNode = node;
    updateConnectorPosition();
} 