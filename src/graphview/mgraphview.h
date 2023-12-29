#ifndef MGRAPHVIEW_H
#define MGRAPHVIEW_H
#include <QtWidgets>

class MScene;
//Класс с виджетом
class MGraphView : public QGraphicsView
{
public:
    MGraphView(QWidget* parent = 0);
    MScene* getMscene() { return scene; }

    ~MGraphView();

protected:
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent* event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent* event) Q_DECL_OVERRIDE;
    void dragLeaveEvent(QDragLeaveEvent* event) Q_DECL_OVERRIDE;
    void dragMoveEvent(QDragMoveEvent* event) Q_DECL_OVERRIDE;
    //Переопределенные виртуальные методы для
    //взаимодействия с виджетом

private:
    void scaleView(qreal scaleFactor);
    //увеличение масштаба

    MScene* scene;
};

#endif // MGRAPHVIEW_H
