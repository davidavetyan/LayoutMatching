#pragma once
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QWidget>
#include <iostream>

#include "region.h"

class Canvas : public QWidget
{

public:
    Canvas(QWidget* p = nullptr);

    void CalculateIntersections();

    void AddRegion(QPainterPath const& region);
    QVector<QPainterPath> const& GetRegions() const;

    void SetShowIndexes(bool bSet);

    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseDoubleClickEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;

    void DrawRegions(QPainter* painter);

    void SetRegions(QVector<Region> const& regions);


private:
    QVector<QPainterPath> m_vecIntersections;
    QVector<QPainterPath> m_vecPaths;
    QPainterPath* m_pCurrentPath;

    QPointF m_point1;
    QPointF m_point2;
    bool m_bFirstPress;
    bool m_bSecondPress;

    bool m_bDrawing;

    QVector<Region> m_regions;

    bool m_bShowIndexes;
};
