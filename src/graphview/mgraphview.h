#ifndef MGRAPHVIEW_H
#define MGRAPHVIEW_H
#include <QtWidgets>

class MScene;

/**
    \brief Main graphics view for the graph
*/
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

private:
    void scaleView(qreal scaleFactor);

    MScene* scene;
};

#endif // MGRAPHVIEW_H
