#include "region.h"
#include "QPolygon"
#include <QDebug>

namespace RegionToken {

const char* OPEN_REGION = "{";
const char* CLOSE_REGION = "}";
const char* SEP = ",";

} // namespace RegionToken

Region::Region(t_contour const& contour)
{
    //if(parseRegion(contour))
    m_vecContours.push_back(contour);
}

QVector<Region> Region::readFromFile(QFile& f)
{
    f.open(QIODevice::ReadOnly);
    bool temp = false;

    QVector<QPoint> currContour;
    QVector<Region> vecRegions;
    Region currentRegion;
    int k = 0;
    for (QString const& i :
         QString(f.readAll()).split(QRegExp("[(\r\n)]"), QString::SkipEmptyParts))
    {
        QPoint pos;
        if (i == RegionToken::OPEN_REGION)
        {
            if (k < 0)
                k = 0;
            k++;

            if (k >= 3)
            {
                k = 2;
                currContour.clear();
            }
            continue;
        }
        else if (i == RegionToken::CLOSE_REGION)
        {
            k--;
            if (k == 0)
            {
                if (temp)
                    temp = false;
                else
                    vecRegions.push_back(currentRegion);

                currentRegion.clear();
            }
            if (k <= 0)
                continue;

            if (currContour.size() < 4 && k == 1)
            {
                currContour.clear();
                continue;
            }
        }

        pos.setX(i.section(RegionToken::SEP, 0, 0).toInt());
        pos.setY(i.section(RegionToken::SEP, 1, 1).toInt());

        if (k == 2 && temp == false)
            currContour.push_back(pos);

        if (k == 1)
        {
            //temp=ParseRegions(vecRegions, currContour);
            if (temp)
            {
                currContour.clear();
                continue;
            }

            currentRegion.addContour(currContour);
            currContour.clear();
        }
    }
    return vecRegions;
}

void Region::addContour(t_contour const& contour)
{
    if (parseContour(contour))
        m_vecContours.push_back(contour);
}

void Region::clear()
{
    m_vecContours.clear();
}

int Region::size()
{
    return m_vecContours.size();
}

QVector<QVector<QPoint>> Region::GetRegion()
{
    return m_vecContours;
}

QVector<QPoint> const& Region::GetContour(const int& index) const
{
    return m_vecContours[index];
}

QPoint Region::GetPoint(const int& indexContour, const int& indexPoint) const
{
    return m_vecContours[indexContour][indexPoint];
}


int Region::ContourMinX(const int& index)
{
    int min = m_vecContours[index][0].x();
    for (int i = 1; i < m_vecContours[index].size(); i++)
        if (m_vecContours[index][i].x() < min)
            min = m_vecContours[index][i].x();
    return min;
}

int Region::ContourMinY(const int& index)
{
    int min = m_vecContours[index][0].y();
    for (int i = 1; i < m_vecContours[index].size(); i++)
        if (m_vecContours[index][i].y() < min)
            min = m_vecContours[index][i].y();
    return min;
}

int Region::ContourMaxX(const int& index)
{
    int max = m_vecContours[index][0].x();
    for (int i = 1; i < m_vecContours[index].size(); i++)
        if (m_vecContours[index][i].x() > max)
            max = m_vecContours[index][i].x();
    return max;
}

int Region::ContourMaxY(const int& index)
{
    int max = m_vecContours[index][0].y();
    for (int i = 1; i < m_vecContours[index].size(); i++)
        if (m_vecContours[index][i].y() > max)
            max = m_vecContours[index][i].y();
    return max;
}

int Region::RegionMinX()
{
    return ContourMinX(0);
}

int Region::RegionMinY()
{
    return ContourMinY(0);
}

int Region::RegionMaxX()
{
    return ContourMaxX(0);
}

int Region::RegionMaxY()
{
    return ContourMaxX(0);
}


int Region::range(Region& R)
{
    QPoint p;
    p.setX(abs(GetPoint(0, 0).x() - R.GetPoint(0, 0).x()));
    p.setY(abs(GetPoint(0, 0).y() - R.GetPoint(0, 0).y()));
    for (int i = 0; i < GetContour(0).size(); i++)
        for (int j = 0; j < R.GetContour(0).size(); j++)
        {
            if (p.x() > abs(GetPoint(0, i).x() - R.GetPoint(0, j).x()))
                p.setX(abs(GetPoint(0, i).x() - R.GetPoint(0, j).x()));
            if (p.y() > abs(GetPoint(0, i).y() - R.GetPoint(0, j).y()))
                p.setY(abs(GetPoint(0, i).y() - R.GetPoint(0, j).y()));
        }
    if (p.x() > p.y())
        return p.x();
    else
        return p.y();
}

int Region::perimeter()
{
    int sum = 0;

    for (int i = 0; i < m_vecContours[0].size() - 1; i++)
    {
        if (m_vecContours[0][i].x() == m_vecContours[0][i + 1].x())
            sum += abs(m_vecContours[0][i].y() - m_vecContours[0][i + 1].y());
        if (m_vecContours[0][i].y() == m_vecContours[0][i + 1].y())
            sum += abs(m_vecContours[0][i].x() - m_vecContours[0][i + 1].x());
    }
    sum += m_vecContours[0].last().x() - m_vecContours[0].first().x();
    return sum;
}

int Region::surface()
{
    int sum = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    for (int k = 0; k < m_vecContours[0].size() - 1; k++)
    {
        sum1 += m_vecContours[0][k].x() * m_vecContours[0][k + 1].y();
        sum2 += m_vecContours[0][k].y() * m_vecContours[0][k + 1].x();
    }
    sum1 += m_vecContours[0][0].y() * m_vecContours[0][m_vecContours[0].size() - 1].x();
    sum2 += m_vecContours[0][0].x() * m_vecContours[0][m_vecContours[0].size() - 1].y();

    sum = abs(sum1 - sum2) / 2;

    for (int k = 1; k < m_vecContours.size(); k++)
    {
        sum1 = 0;
        sum2 = 0;
        for (int c = 0; c < m_vecContours[k].size() - 1; c++)
        {
            sum1 += m_vecContours[k][c].x() * m_vecContours[k][c + 1].y();
            sum2 += m_vecContours[k][c].y() * m_vecContours[k][c + 1].x();
        }

        sum1 += m_vecContours[k][0].y() * m_vecContours[k][m_vecContours[k].size() - 1].x();
        sum2 += m_vecContours[k][0].x() * m_vecContours[k][m_vecContours[k].size() - 1].y();

        sum3 += (abs(sum1 - sum2) / 2);
    }

    return sum - sum3;
}

bool Region::contains(QPoint point)
{
    int sum = 0;
    int x = point.x();
    int y = point.y();
    for (int i = 0; i < m_vecContours.size(); i++)
        for (int j = 0; j < m_vecContours[i].size(); j++)
            if (x < m_vecContours[i][j].x() && y < m_vecContours[i][j].y())
                sum++;
    if (sum % 2 == 0)
        return false;
    else
        true;
}

bool Region::intersects(Region& region)
{
    QPoint p, p1;
    int x, y, x1, y1;
    for (int i = 0; i < region.GetContour(0).size() - 1; i++)
    {
        p = region.GetPoint(0, i);
        p1 = region.GetPoint(0, i + 1);
        if (p.x() <= p1.x())
        {
            x = p.x();
            x1 = p1.x();
        }
        else
        {
            x = p1.x();
            x1 = p.x();
        }
        if (p.y() <= p1.y())
        {
            y = p.y();
            y1 = p1.y();
        }
        else
        {
            y = p1.y();
            y1 = p.y();
        }
        if (lineIntersects(x, y, x1, y1))
            return true;
    }
    return false;
}

bool Region::lineIntersects(const int& x, const int& y, const int& x1, const int& y1)
{
    QVector<QPoint> a;

    for (int k = 0; k < size(); k++)
    {
        a = GetContour(k);
        for (int c = 0; c < a.size() - 1; c++)
        {
            if (a[c].x() > x && a[c].x() < x1 && a[c + 1].x() > x && a[c + 1].x() < x1)
            {
                if (a[c].y() < y && a[c].y() < y1 && a[c + 1].y() > y && a[c + 1].y() > y1)
                    return true;


                if (a[c].y() > y && a[c].y() > y1 && a[c + 1].y() < y && a[c + 1].y() < y1)
                    return true;
            }

            if (a[c].x() < x && a[c].x() < x1 && a[c + 1].x() > x && a[c + 1].x() > x1)

                if (a[c].y() > y && a[c].y() < y1 && a[c + 1].y() > y && a[c + 1].y() < y1)
                    return true;
        }


        if (a.first().x() < x && a.first().x() < x1 && a.last().x() > x && a.last().x() > x1)
            if (a.first().y() > y && a.first().y() < y1 && a.last().y() > y && a.last().y() < y1)
                return true;
    }
    return false;
}

Region::t_contour Region::intersected(Region::t_contour contour)
{
    QPolygon p1(GetContour(0));
    QPolygon p2(contour);
    QPolygon p3 = p1.intersected(p2);

    for (int i = 1; i < size(); i++)
    {
        QPolygon p4 = p3.subtracted(QPolygon(GetContour(i)));

        if (p4.isEmpty())
            continue;
        else
            p3 = p4;
    }
    t_contour c2;
    for (int i = 0; i < p3.size() - 1; i++)
        c2.push_back(p3[i]);
    return c2;
}

Region::t_contour Region::intersected(Region region)
{
    QPolygon p1 = intersected(region.GetContour(0));
    QPolygon p2 = region.intersected(GetContour(0));
    QPolygon p3 = p1.intersected(p2);
    t_contour c;
    for (int i = 0; i < p3.size(); i++)
        c.push_back(p3[i]);
    return c;
}

int Region::intersects_surface(Region region)
{
    Region r(intersected(region));
    return r.surface();
}

int Region::intersects_surface(Region::t_contour contour)
{
    int sum = 0;
    Region c3(intersected(contour));
    sum = c3.surface();
    return sum;
}

QVector<int> Region::DeterminationOfRowsAndCoiumns(int w, int h, bool& temp)
{
    QVector<int> aa;
    return aa;
}

QPainterPath Region::getPainterPath() const
{
    if (m_vecContours.isEmpty())
        return QPainterPath();

    QPainterPath painterPath(m_vecContours.first().first());
    for (int i = 1; i < m_vecContours.first().size(); i++)
    {
        QPoint const& point = m_vecContours.first().at(i);
        painterPath.lineTo(point);
    }
    painterPath.lineTo(m_vecContours.first().first());
    return painterPath;
}

qreal Region::getPainterPathArea(QPainterPath const& painterPath)
{
    QPointF a, b;
    qreal len;
    qreal area = 0;
    for (len = 0; len < painterPath.length(); len++)
    {
        a = painterPath.pointAtPercent(painterPath.percentAtLength(len));
        b = painterPath.pointAtPercent(painterPath.percentAtLength(
            len + 1 > painterPath.length() ? painterPath.length() : len + 1));
        area += (b.x() - a.x()) * (a.y() + b.y());
    }
    return (area + 1) / qreal(2);
}

bool Region::parseContour(t_contour const& currContour)
{
    bool bRes = true;
    if (selfIntersects(currContour))
        return false;
    if (!parseRegion(currContour))
        return false;
    for (int i = 0; i < currContour.size() - 1; i++)
        if (currContour[i].x() == currContour[i + 1].x() ||
            currContour[i].y() == currContour[i + 1].y())
            continue;
        else
        {
            bRes = false;
            break;
        };
    if (!currContour.empty() && currContour.last().y() != currContour.first().y() && bRes)
        return false;
    return true;
}

bool Region::parseRegion(t_contour const& currContour)
{
    if (GetRegion().size() > 0)
    {
        for (int i = 0; i < currContour.size(); i++)
            if (contains({ currContour[i].x(), currContour[i].y() }))
                continue;
            else
                return false;
    }
    return true;
}

bool Region::ParseRegions(QVector<Region> const& vecRegions, Region::t_contour const& currContour)
{
    if (vecRegions.size() > 1)
    {
        for (int i = 0; i < vecRegions.size(); i++)
        {
            if (vecRegions[i].GetContour(0).size() != currContour.size())
                return false;

            for (int j = 0; j < currContour.size(); j++)
                if (vecRegions[i].GetPoint(0, j) != currContour[j])
                    return false;

            return true;
        }
        return true;
    }

    return false;
}

bool Region::selfIntersects(Region::t_contour currContour)
{
    Region::t_contour v = currContour;
    for (int i = 0; i < v.size() - 4; i++)
    {
        if (v[i].x() < v[i + 3].x() && v[i].x() < v[i + 4].x() && v[i + 1].x() > v[i + 3].x() &&
            v[i + 1].x() > v[i + 4].x())
            if (v[i].y() < v[i + 3].y() && v[i].y() > v[i + 4].y() && v[i + 1].y() < v[i + 3].y() &&
                v[i + 1].y() > v[i + 4].y())
                return true;
    }
    return false;
}
