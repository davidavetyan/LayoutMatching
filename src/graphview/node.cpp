#include <QtWidgets>

#include "edge.h"
#include "mgraphview.h"
#include "mscene.h"
#include "node.h"

Node::Node(MScene* graphWidget, QString val) : scene(graphWidget), main_color(Qt::white), value(val)
{
    setFlags(ItemIsMovable | ItemSendsGeometryChanges | ItemIsSelectable);
    setCacheMode(DeviceCoordinateCache);
    setAcceptHoverEvents(true);
    shape_type = scene->nodeShape();
    m_diameter = scene->diameter();
    setToolTip(QString::number(m_diameter));
}

QPainterPath Node::shape() const
{
    QPainterPath path;
    //int m_diameter = scene->m_diameter();
    if (shape_type == Rectangle)
        path.addRect(-m_diameter / 2, -m_diameter / 2, m_diameter, m_diameter);
    else if (shape_type == Circle)
        path.addEllipse(-m_diameter / 2, -m_diameter / 2, m_diameter, m_diameter);
    else
        path.addPolygon(QPolygonF() << QPointF(-m_diameter / 2, 0) << QPointF(0, -m_diameter / 2)
                                    << QPointF(m_diameter / 2, 0) << QPointF(0, m_diameter / 2));

    return path;
}

int Node::nodeShape() const
{
    return shape_type;
}
QRectF Node::boundingRect() const
{
    qreal adjust = 20;
    //int m_diameter = scene->m_diameter();
    return QRectF(-m_diameter / 2 - adjust, -m_diameter / 2 - adjust, m_diameter + adjust * 2,
                  m_diameter + adjust * 2);
}

void Node::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*)
{

    //int m_diameter = scene->m_diameter();
    painter->setClipRect(option->exposedRect);
    if (childrenList.contains(this))
    {
        painter->setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->drawEllipse(-m_diameter * 2 / 3, -m_diameter * 2 / 3, m_diameter / 2,
                             m_diameter / 2);
        painter->setBrush(Qt::black);
        painter->drawPolygon(QPolygonF() << QPointF(-17 * m_diameter / 30, -m_diameter / 3)
                                         << QPointF(-7 * m_diameter / 15, -m_diameter / 6)
                                         << QPointF(-19 * m_diameter / 30, -m_diameter / 6));
    }

    QRadialGradient gradient(-10, -10, m_diameter / 2);
    if (isSelected())
        painter->setPen(QPen(Qt::black, 4));
    else
        painter->setPen(QPen(Qt::black, 2));

    if (option->state.testFlag(QStyle::State_MouseOver))
    {
        gradient.setColorAt(0, main_color.lighter());
        gradient.setColorAt(1, main_color);
    }
    else
    {
        gradient.setColorAt(0, main_color);
        gradient.setColorAt(1, main_color.darker(130));
    }
    painter->setBrush(gradient);
    if (shape_type == Rectangle)
        painter->drawRect(-m_diameter / 2, -m_diameter / 2, m_diameter, m_diameter);
    else if (shape_type == Circle)
        painter->drawEllipse(-m_diameter / 2, -m_diameter / 2, m_diameter, m_diameter);
    else
        painter->drawPolygon(QPolygonF()
                             << QPointF(-m_diameter / 2, 0) << QPointF(0, -m_diameter / 2)
                             << QPointF(m_diameter / 2, 0) << QPointF(0, m_diameter / 2));


    QTextOption opt(Qt::AlignCenter);
    opt.setWrapMode(QTextOption::WrapAnywhere);
    painter->drawText(QRectF(-m_diameter / 2, -m_diameter / 2, m_diameter, m_diameter), value, opt);
    update();
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionHasChanged)
    {
        foreach(Edge* edge, edgeList)
            edge->adjust();
    }

    return QGraphicsItem::itemChange(change, value);
}

bool Node::are_nodes_connected(Node* src, Node* dst)
{
    if (src->childrenList.contains(dst))
        return true;
    return false;
}

void Node::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    QGraphicsItem::contextMenuEvent(event);
    setSelected(true);

    QMenu menu;
    QAction* rename_action = menu.addAction("Переименовать");
    QAction* color_change_action = menu.addAction("Изменить цвет");
    QMenu sub_menu("Изменить форму");
    QAction* circle_is_choosed = sub_menu.addAction("Круг");
    QAction* rect_is_choosed = sub_menu.addAction("Квадрат");
    QAction* rhomb_is_choosed = sub_menu.addAction("Ромб");
    menu.addMenu(&sub_menu);
    QAction* change_diametr = menu.addAction("Изменить размер");
    menu.addSeparator();

    QAction* self_in_action = Q_NULLPTR;
    QAction* remove_self = Q_NULLPTR;

    if (childrenList.contains(this))
        remove_self = menu.addAction("Удалить петлю");
    else
        self_in_action = menu.addAction("Создать петлю");

    QAction* remove_action = menu.addAction("Удалить вершину");

    QAction* selected_action = menu.exec(event->screenPos());

    if (selected_action == color_change_action)
        scene->setColorOfSelectedNodes(main_color);
    else if (selected_action == rename_action)
    {
        bool ok;
        QString text = QInputDialog::getText(scene->getMGraph(), "Переименовать",
                                             "Имя вершины:", QLineEdit::Normal, value, &ok);
        if (ok && !text.isEmpty())
            setValue(text);
    }
    else if (selected_action == remove_action)
        scene->removeSelectedNodes();
    else if (selected_action == self_in_action)
        setLoop();
    else if (selected_action == remove_self)
        removeLoop();
    else if (selected_action == change_diametr)
    {
        scene->setSizeOfSelectedNodes(m_diameter);
    }
    else if (selected_action == circle_is_choosed)
        scene->setShapeOfSelectedNodes(Node::Circle);
    else if (selected_action == rect_is_choosed)
        scene->setShapeOfSelectedNodes(Node::Rectangle);
    else if (selected_action == rhomb_is_choosed)
        scene->setShapeOfSelectedNodes(Node::Rhomb);
    //setSelected(false);
}

void Node::setShape(int shape)
{
    shape_type = shape;
    if (edgeList.isEmpty())
        update();
    else
        foreach(Edge* e, edgeList)
            e->adjust();
}
int Node::diameter() const
{
    return m_diameter;
}
void Node::setDiameter(int diam)
{
    m_diameter = diam;
    if (edgeList.isEmpty())
        update();
    else
        foreach(Edge* e, edgeList)
            e->adjust();
}

void Node::removeNode()
{
    foreach(Edge* edge, edgeList)
    {
        edge->removeEdge();
        edge->sourceNode()->update();
        edge->destNode()->update();
    }

    scene->nodesList().removeAll(this);
    scene->removeItem(this);
    scene->deleted << this;
}

QString& operator<<(QString& str, const Node& node)
{
    str.append(node.value + " : ");
    if (node.childrenList.isEmpty())
        return str;

    foreach(Node* n, node.childrenList)
    {
        if (n == node.childrenList.last())
            str.append(n->value);
        else
            str.append(n->value + ", ");
    }
    return str;
}

void Node::setLoop()
{
    if (!childrenList.contains(this))
        childrenList << this;
    update();
}

void Node::addEdge(Edge* edge)
{
    edgeList << edge;
    edge->adjust();
}

void Node::addChild(Node* child)
{
    childrenList << child;
}

QList<Edge*>& Node::edges()
{
    return edgeList;
}

QList<Node*>& Node::children()
{
    return childrenList;
}

void Node::setColor(QColor col)
{
    if (col == Qt::black)
        return;

    main_color = col;
    update();
}

int Node::type() const
{
    return Type;
}

QString Node::get_val() const
{
    return value;
}

QColor Node::get_color() const
{
    return main_color;
}

void Node::setValue(QString val)
{
    value = val;
    update();
}

void Node::removeLoop()
{
    if (childrenList.contains(this))
        childrenList.removeAll(this);
    update();
}

void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{

    QGraphicsItem::mouseDoubleClickEvent(event);
    if (event->button() != Qt::LeftButton)
        return;
    bool ok;
    QString text = QInputDialog::getText(scene->getMGraph(), "Переименовать",
                                         "Имя вершины:", QLineEdit::Normal, value, &ok);
    if (ok && !text.isEmpty())
        setValue(text);
}
