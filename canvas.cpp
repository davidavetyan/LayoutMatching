#include "Canvas.h"
#include <cmath>
#include <QPainterPath>

Canvas::Canvas(QWidget* p)
    : m_bShowIndexes(false),
      m_bDrawing(false),
	  m_pCurrentPath(nullptr)
{
    m_bFirstPress = false;
    m_bSecondPress = false;
    
    setMouseTracking(true);

    setMinimumSize(600,700);
}

void Canvas::paintEvent(QPaintEvent* e) 
{
    QPainter painter(this);

    DrawRegions(&painter);

    if (m_pCurrentPath)
    	painter.drawPath(*m_pCurrentPath);

    if (m_bFirstPress && !m_bSecondPress)
    {
    	painter.setPen(QPen(Qt::white, 1, Qt::DotLine, Qt::FlatCap));
    	painter.drawLine(m_point1, m_point2);
    }
    if (m_bFirstPress && m_bSecondPress)
    {
    	painter.setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::FlatCap));
    	painter.drawLine(m_point1, m_point2);
    	m_bSecondPress = false;
    }
}

void Canvas::mousePressEvent(QMouseEvent* e)
{
    m_bDrawing = true;

    if (!m_pCurrentPath)
        m_pCurrentPath = new QPainterPath(e->pos());
    else
        m_pCurrentPath->lineTo(m_point2);


    if (!m_bFirstPress && !m_bSecondPress)
    {
        m_point1 = m_pCurrentPath->currentPosition();
        m_point2 = m_point1;
        m_bFirstPress = true;
    }
    else if (m_bFirstPress && !m_bSecondPress) 
    {
        m_point1 = m_pCurrentPath->currentPosition();
        m_bSecondPress = true;
    }

    QPointF lastPoint = m_pCurrentPath->currentPosition();
    repaint();
}

void Canvas::mouseDoubleClickEvent(QMouseEvent* e)
{
    m_bFirstPress = false;
    m_bSecondPress = false;
	
    m_point1 = e->pos();
    m_point2 = e->pos();
	
    if (m_pCurrentPath)
    {
        QPointF firstPoint = m_pCurrentPath->elementAt(0);
        QPointF currentPoint = m_pCurrentPath->currentPosition();

    	if (firstPoint.x() != currentPoint.x() && firstPoint.y() != currentPoint.y())
        {
            QPointF intermediatePoint(firstPoint.x(), currentPoint.y());
    		if (m_pCurrentPath->elementCount() > 2)
    		{
                QPointF lastPoint = m_pCurrentPath->elementAt(m_pCurrentPath->elementCount() - 2);
                if (currentPoint.y() == lastPoint.y())
                {
                    if (abs(currentPoint.x() - lastPoint.x()) > abs(firstPoint.x() - lastPoint.x()))
                    {
                        intermediatePoint.setX(currentPoint.x());
                        intermediatePoint.setY(firstPoint.y());
                    }
                }
                else
                {
                    if (abs(currentPoint.y() - lastPoint.y()) < abs(firstPoint.y() - lastPoint.y()))
                    {
                        intermediatePoint.setX(currentPoint.x());
                        intermediatePoint.setY(firstPoint.y());
                    }
                }
    		}

            m_pCurrentPath->lineTo(intermediatePoint);
        }

        m_pCurrentPath->lineTo(firstPoint);
    	
        AddRegion(QPainterPath(*m_pCurrentPath));
        m_pCurrentPath = nullptr;
    }

    repaint();
}

void Canvas::mouseMoveEvent(QMouseEvent* e)
{
    if (m_pCurrentPath)
        QPointF lastPoint = m_pCurrentPath->currentPosition();

    if (m_bDrawing)
    {
        m_point2 = m_point1;

        if (abs(e->pos().x() - m_point1.x()) <= abs(e->pos().y() - m_point1.y()))
            m_point2.setY(e->pos().y());
        else
            m_point2.setX(e->pos().x());

        repaint();
    }
}

void Canvas::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape)
    {
        delete m_pCurrentPath;
        m_pCurrentPath = nullptr;
        m_bDrawing = false;
        m_bFirstPress = false;
    	m_bSecondPress = false;

        repaint();
    }
}

void Canvas::DrawRegions(QPainter* painter)
{
    painter->save();
	
	painter->setPen(QPen(QBrush(Qt::blue), 3));
    for (int i = 0; i < m_vecPaths.size(); i++)
    {
		auto const& path = m_vecPaths[i];
        painter->drawPath(path);
        painter->fillPath(path, QBrush(Qt::blue, Qt::BDiagPattern));
    }

    painter->setPen(QPen(QBrush(Qt::red), 3));
    for (auto const& oPath : m_vecIntersections)
    {
        painter->drawPath(oPath);
        painter->fillPath(oPath, QBrush(Qt::red, Qt::FDiagPattern));
    }

    if (m_bShowIndexes)
    {
        QFont font = painter->font();
        font.setPixelSize(16);
        painter->setFont(font);
        painter->setPen(QPen(QBrush(Qt::white), 3));
        for (int i = 0; i < m_vecPaths.size(); i++)
        {
            auto const& path = m_vecPaths[i];
            painter->drawText(path.boundingRect(), Qt::AlignCenter, QString::number(i));
        }
    }

    painter->restore();
}

void Canvas::SetRegions(QVector<Region> const& regions)
{
	for (Region const& region : regions)
        m_vecPaths.push_back(region.getPainterPath());

    CalculateIntersections();

    repaint();
}

QVector<QPainterPath> const& Canvas::GetRegions() const
{
    return m_vecPaths;
}

void Canvas::SetShowIndexes(bool bSet)
{
    m_bShowIndexes = bSet;
    repaint();
}

void Canvas::CalculateIntersections()
{
    m_vecIntersections.clear();

    for (int i = 0; i < m_vecPaths.size(); ++i)
    {
        for (int j = i + 1; j < m_vecPaths.size(); ++j)
        {
            QPainterPath pathIntersect = (m_vecPaths[i]).intersected((m_vecPaths[j]));
            if (pathIntersect.isEmpty())
                continue;

            m_vecIntersections.push_back(pathIntersect);
        }
    }
}

void Canvas::AddRegion(QPainterPath const& region)
{
    m_vecPaths.push_back(region);
	for (int i = 0; i < m_vecPaths.size() - 1; i++)
	{
        QPainterPath pathIntersect = (m_vecPaths[i]).intersected(region);
        if (pathIntersect.isEmpty())
            continue;

        m_vecIntersections.push_back(pathIntersect);
	}
}
