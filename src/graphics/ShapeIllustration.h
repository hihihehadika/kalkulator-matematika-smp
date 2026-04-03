/*
 * ShapeIllustration.h — Wireframe 3D Bangun Ruang
 * Padanan Python: class ShapeIllustration (baris 1571-1977 di main.py)
 * IDENTIK PYTHON — proyeksi, skala, geometri, shading, label
 */
#ifndef SHAPEILLUSTRATION_H
#define SHAPEILLUSTRATION_H

#include <QWidget>
#include <QTimer>
#include <QString>
#include <QVector>
#include <QPainterPath>
#include <QPainter>
#include <QPen>
#include <QFont>
#include <cmath>
#include <algorithm>
#include <tuple>

struct Vec3 { double x, y, z; };

// Projected point: screen x, screen y, depth z
struct Pt3 { double x, y, z; };

class ShapeIllustration : public QWidget
{
    Q_OBJECT
public:
    explicit ShapeIllustration(QWidget *parent = nullptr);
    void setShape(const QString &shape);
    bool isDark = true;
protected:
    void paintEvent(QPaintEvent *e) override;
private slots:
    void onTick();
private:
    // Identik Python _project — batch projection
    QVector<Pt3> project(const QVector<Vec3> &pts3d, double cx, double cy, double scale, double fov = 5.0);

    // Identik Python _cross_z — backface culling
    static double crossZ(const QVector<Pt3> &pts2d);
    // Identik Python _avg_z — depth sorting
    static double avgZ(const QVector<Pt3> &allPts, const QVector<int> &idx);
    // Identik Python _sc — scale calculation
    double sc(double modelR) const;

    // Draw helpers
    void drawFace(QPainter &p, const QVector<Pt3> &verts2d, int alpha);
    void drawLabel3D(QPainter &p, double lx, double ly, double lz, const QString &text);
    QPen solidPen(double w = 1.7);
    QPen dashPen(double w = 1.0);

    // Shape drawers — identik Python
    void drawKubus(QPainter &p, int W, int H);
    void drawBalok(QPainter &p, int W, int H);
    void drawBola(QPainter &p, int W, int H);
    void drawTabung(QPainter &p, int W, int H);
    void drawKerucut(QPainter &p, int W, int H);
    void drawPrismaSegitiga(QPainter &p, int W, int H);
    void drawLimasSegitiga(QPainter &p, int W, int H);
    void drawLimasSegiEmpat(QPainter &p, int W, int H);

    QString m_shape = "Kubus";
    QTimer m_timer;
    double m_angle_y = 0;
    double m_angle_x = 0;

    static constexpr double TARGET_R = 72.0; // identik Python _TARGET_R
};

#endif // SHAPEILLUSTRATION_H
