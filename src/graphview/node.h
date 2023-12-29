#ifndef NODE_H
#define NODE_H

#include <QGraphicsItem>
#include <QList>


class Edge;
class MGraphView;
class MScene;

//Класс для определения вершин графа
class Node : public QGraphicsItem
{
    friend class Edge;
    friend class MScene;
    friend QString& operator<<(QString& str, const Node& node);
    //оператор вывода вершины в строку
public:
    Node(MScene* graphWidget, QString val = "");
    //конструктор вершины с заданным значением

    void setColor(QColor col);  //установить цвет
    void setValue(QString val); //установить значение
    void removeNode();          //удалить данную вершину
    void setLoop();             //установить петлю
    void removeLoop();          //удалить петлю
    void setShape(int shape);
    void setDiameter(int diam);

    QString get_val() const; //извлечение значения
    QColor get_color() const;

    QList<Edge*>& edges(); //списки ребер и детей
    QList<Node*>& children();

    static bool is_two_nodes_connected(Node* src, Node* dst);
    //соединены ли две вершины

    enum
    {
        Type = UserType + 1
    }; //Тип "вершина"
    int nodeShape() const;
    int diameter() const;
    int type() const override;
    enum
    {
        Circle = 0,
        Rhomb,
        Rectangle
    };

protected:
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    QPainterPath shape() const override;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    //переопределенные методы

private:
    void addChild(Node* child);
    void addEdge(Edge* edge);


    QList<Edge*> edgeList;     //Список ребер
    QList<Node*> childrenList; //Смежные вершины

    MScene* scene;             //сцена в которых находится вершина

    QColor main_color;         //цвет
    QString value;             //значение
    int shape_type;
    int m_diameter;
};

#endif // NODE_H
