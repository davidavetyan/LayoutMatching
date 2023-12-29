#ifndef REGION_H
#define REGION_H

#include <QFile>
#include <QPainterPath>
#include <QPoint>
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
    QVector<QPoint> const& GetContour(const int& index) const;
    QPoint GetPoint(const int& indexContour, const int& indexPoint) const;

    int ContourMinX(const int& index);
    int ContourMinY(const int& index);
    int ContourMaxX(const int& index);
    int ContourMaxY(const int& index);

    int RegionMinX();
    int RegionMinY();
    int RegionMaxX();
    int RegionMaxY();

    int range(Region&);
    int perimeter();
    int surface();
    bool contains(QPoint point);
    bool intersects(Region& Regionn);
    bool lineIntersects(const int& x, const int& y, const int& x1, const int& y1);

    t_contour intersected(t_contour contour);
    t_contour intersected(Region region);
    int intersects_surface(Region region);
    int intersects_surface(t_contour contour);
    QVector<int> DeterminationOfRowsAndCoiumns(int, int, bool& temp);

    QPainterPath getPainterPath() const;
    static qreal getPainterPathArea(QPainterPath const& painterPath);

private:
    bool parseContour(const t_contour& currContour);
    bool parseRegion(const t_contour& currContour);
    static bool selfIntersects(Region::t_contour currContour);
    static bool ParseRegions(QVector<Region> const& vecRegions,
                             Region::t_contour const& currContour);

    QVector<t_contour> m_vecContours;
};

#endif // REGION_H
