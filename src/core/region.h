#ifndef REGION_H
#define REGION_H

#include <QFile>
#include <QPainterPath>
#include <QVector>

namespace RegionToken {
extern const char* OPEN_REGION;
extern const char* CLOSE_REGION;
extern const char* SEP;
} // namespace RegionToken

class Region
{
public:
    using t_contour = QVector<QPoint>;

    Region() = default;
    Region(t_contour const& contour);
    void addContour(t_contour const& contour);

    static QVector<Region> readFromFile(QFile&);

    void clear();
    int size();
    QVector<QVector<QPoint>> GetRegion();
    QVector<QPoint> const& GetContur(const int& index) const;
    QPoint GetPoint(const int& indexContur, const int& indexPoint) const;

    int ConturMinX(const int& index);
    int ConturMinY(const int& index);
    int ConturMaxX(const int& index);
    int ConturMaxY(const int& index);

    int RegionMinX();
    int RegionMinY();
    int RegionMaxX();
    int RegionMaxY();


    int range(Region&);
    int perimeter();
    int surface();
    bool ketpatk(const int& x, const int& y);
    bool ketgit(const int& x, const int& y);
    bool intersects(Region& Regionn);
    bool LineIntersects(const int& x, const int& y, const int& x1, const int& y1);

    t_contour intersected(t_contour contour);
    t_contour intersected(Region Regionn);
    int intersects_surface(Region Regionn);
    int intersects_surface(t_contour contour);
    QVector<int> DeterminationOfRowsAndCoiumns(int, int, bool& temp);

    QPainterPath getPainterPath() const;
    static qreal getPainterPathArea(QPainterPath const& painterPath);

private:
    bool parsContur(const t_contour& currContour);
    bool parsRegion(const t_contour& currContour);
    static bool selfintersects(Region::t_contour currContur);
    static bool ParsRegionn(QVector<Region> const& vecRegions,
                            Region::t_contour const& currContour);

    QVector<t_contour> m_vecContours;
};

#endif // REGION_H
