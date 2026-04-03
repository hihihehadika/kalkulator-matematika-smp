/*
 * ShapeIllustration.cpp — Implementasi Wireframe 3D
 * Padanan Python: class ShapeIllustration (baris 1571-1977 di main.py)
 * IDENTIK PYTHON — proyeksi, animasi, skala, geometri, shading, label, solid/dash
 */
#include "graphics/ShapeIllustration.h"
#include "core/Theme.h"

// ════════════════════════════════════════════════════════
// Constructor — identik Python __init__
// ════════════════════════════════════════════════════════
ShapeIllustration::ShapeIllustration(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(190, 190); // identik Python
    connect(&m_timer, &QTimer::timeout, this, &ShapeIllustration::onTick);
    m_timer.setTimerType(Qt::PreciseTimer); // identik Python
    m_timer.start(33); // identik Python (33ms)
}

void ShapeIllustration::setShape(const QString &shape)
{
    m_shape = shape;
    update();
}

// ════════════════════════════════════════════════════════
// Tick — identik Python _tick
// ════════════════════════════════════════════════════════
void ShapeIllustration::onTick()
{
    m_angle_y += 0.022; // identik Python
    m_angle_x = std::sin(m_angle_y * 0.38) * 0.38; // identik Python — sinusoidal "nod"
    update();
}

// ════════════════════════════════════════════════════════
// Proyeksi 3D — identik Python _project
// ════════════════════════════════════════════════════════
QVector<Pt3> ShapeIllustration::project(const QVector<Vec3> &pts3d,
                                        double cx, double cy, double scale,
                                        double fov)
{
    double ay = m_angle_y, ax = m_angle_x;
    double cay = std::cos(ay), say = std::sin(ay);
    double cax = std::cos(ax), sax = std::sin(ax);
    QVector<Pt3> out;
    out.reserve(pts3d.size());
    for (const auto &pt : pts3d) {
        double x = pt.x, y = pt.y, z = pt.z;
        // Rotasi Y — identik Python
        double x2 =  x * cay + z * say;
        double z2 = -x * say + z * cay;
        // Rotasi X — identik Python
        double y3 =  y * cax - z2 * sax;
        double z3 =  y * sax + z2 * cax;
        double x3 =  x2;
        // Perspektif — identik Python
        double d = fov / (fov + z3 + 4.0);
        out.append({cx + x3 * scale * d,
                    cy + y3 * scale * d,  // identik Python: Y positif (ke bawah)
                    z3});
    }
    return out;
}

// ════════════════════════════════════════════════════════
// Backface culling — identik Python _cross_z
// ════════════════════════════════════════════════════════
double ShapeIllustration::crossZ(const QVector<Pt3> &pts2d)
{
    if (pts2d.size() < 3) return 1.0;
    double ax = pts2d[1].x - pts2d[0].x;
    double ay = pts2d[1].y - pts2d[0].y;
    double bx = pts2d[2].x - pts2d[0].x;
    double by = pts2d[2].y - pts2d[0].y;
    return ax * by - ay * bx;
}

// ════════════════════════════════════════════════════════
// Average Z — identik Python _avg_z
// ════════════════════════════════════════════════════════
double ShapeIllustration::avgZ(const QVector<Pt3> &allPts, const QVector<int> &idx)
{
    double sum = 0;
    for (int i : idx) sum += allPts[i].z;
    return sum / idx.size();
}

// ════════════════════════════════════════════════════════
// Scale — identik Python _sc
// ════════════════════════════════════════════════════════
double ShapeIllustration::sc(double modelR) const
{
    return TARGET_R / modelR;
}

// ════════════════════════════════════════════════════════
// Pen helpers — identik Python sp() dan dp()
// ════════════════════════════════════════════════════════
QPen ShapeIllustration::solidPen(double w)
{
    return QPen(QColor(0xC9, 0xA8, 0x4C, 210), w, Qt::SolidLine,
                Qt::RoundCap, Qt::RoundJoin);
}

QPen ShapeIllustration::dashPen(double w)
{
    return QPen(QColor(0xC9, 0xA8, 0x4C, 75), w, Qt::DashLine,
                Qt::RoundCap);
}

// ════════════════════════════════════════════════════════
// draw_face — identik Python draw_face
// ════════════════════════════════════════════════════════
void ShapeIllustration::drawFace(QPainter &p, const QVector<Pt3> &verts2d, int alpha)
{
    QPainterPath path;
    path.moveTo(verts2d[0].x, verts2d[0].y);
    for (int i = 1; i < verts2d.size(); ++i)
        path.lineTo(verts2d[i].x, verts2d[i].y);
    path.closeSubpath();
    p.setBrush(QColor(0xC9, 0xA8, 0x4C, alpha));
    p.setPen(solidPen());
    p.drawPath(path);
}

// ════════════════════════════════════════════════════════
// draw_label_3d — identik Python draw_label_3d
// ════════════════════════════════════════════════════════
void ShapeIllustration::drawLabel3D(QPainter &p, double lx, double ly, double lz, const QString &text)
{
    // DEPAN kamera = z3 NEGATIF → tampil jika z < +0.5
    if (lz > 0.5) return;
    double fade = std::min(1.0, std::max(0.0, (0.5 - lz) / 1.5));
    int alpha = int(235 * fade);
    if (alpha < 30) return;
    p.save();
    QColor c(0xC9, 0xA8, 0x4C, alpha);
    p.setPen(c);
    p.setFont(QFont("Arial", 9, QFont::Bold));
    p.drawText(int(lx) + 3, int(ly) + 3, text);
    p.restore();
}

// ════════════════════════════════════════════════════════
// paintEvent — identik Python
// ════════════════════════════════════════════════════════
void ShapeIllustration::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);
    int W = width(), H = height();

    // Tidak fill background — biarkan transparan seperti Python
    // (Python tidak fill background di paintEvent)

    if      (m_shape == "Kubus")           drawKubus(p, W, H);
    else if (m_shape == "Balok")           drawBalok(p, W, H);
    else if (m_shape == "Tabung")          drawTabung(p, W, H);
    else if (m_shape == "Kerucut")         drawKerucut(p, W, H);
    else if (m_shape == "Bola")            drawBola(p, W, H);
    else if (m_shape == "Prisma Segitiga") drawPrismaSegitiga(p, W, H);
    else if (m_shape == "Limas Segitiga")  drawLimasSegitiga(p, W, H);
    else if (m_shape == "Limas Segi Empat")drawLimasSegiEmpat(p, W, H);
    else                                   drawKubus(p, W, H); // fallback
}

// ════════════════════════════════════════════════════════
// KUBUS — identik Python
// ════════════════════════════════════════════════════════
void ShapeIllustration::drawKubus(QPainter &p, int W, int H)
{
    double cx = W / 2.0, cy = H / 2.0;
    double scale = sc(std::sqrt(3.0)); // identik Python

    QVector<Vec3> raw = {
        {-1,-1,-1}, { 1,-1,-1}, { 1, 1,-1}, {-1, 1,-1},
        {-1,-1, 1}, { 1,-1, 1}, { 1, 1, 1}, {-1, 1, 1}
    };
    QVector<Pt3> pts = project(raw, cx, cy, scale);

    // Faces with alpha — identik Python
    using Face = std::pair<QVector<int>, int>;
    QVector<Face> faces = {
        {{4,5,6,7}, 36}, {{0,1,2,3}, 18},
        {{0,4,7,3}, 26}, {{1,5,6,2}, 26},
        {{3,2,6,7}, 32}, {{0,1,5,4}, 14}
    };

    // Sort by avg z descending — identik Python
    std::sort(faces.begin(), faces.end(), [&](const Face &a, const Face &b) {
        return avgZ(pts, a.first) > avgZ(pts, b.first);
    });

    // Draw faces with backface culling
    for (const auto &[idx, alpha] : faces) {
        QVector<Pt3> v;
        for (int i : idx) v.append(pts[i]);
        if (crossZ(v) >= 0)
            drawFace(p, v, alpha);
    }

    // Edges — identik Python
    p.setBrush(Qt::NoBrush);
    int edges[][2] = {
        {4,5},{5,6},{6,7},{7,4},
        {0,1},{1,2},{2,3},{3,0},
        {4,0},{5,1},{6,2},{7,3}
    };
    for (auto &e : edges) {
        p.setPen(solidPen());
        p.drawLine(int(pts[e[0]].x), int(pts[e[0]].y),
                   int(pts[e[1]].x), int(pts[e[1]].y));
    }

    // Label "s" — identik Python
    double lx = (pts[4].x + pts[5].x) / 2;
    double ly = (pts[4].y + pts[5].y) / 2 + 10;
    double lz = (pts[4].z + pts[5].z) / 2;
    drawLabel3D(p, lx, ly, lz, "s");
}

// ════════════════════════════════════════════════════════
// BALOK — identik Python
// ════════════════════════════════════════════════════════
void ShapeIllustration::drawBalok(QPainter &p, int W, int H)
{
    double cx = W / 2.0, cy = H / 2.0;
    double scale = sc(std::sqrt(1.4*1.4 + 1.0*1.0 + 0.85*0.85)); // identik Python

    QVector<Vec3> raw = {
        {-1.4,-1.0,-0.85}, { 1.4,-1.0,-0.85},
        { 1.4, 1.0,-0.85}, {-1.4, 1.0,-0.85},
        {-1.4,-1.0, 0.85}, { 1.4,-1.0, 0.85},
        { 1.4, 1.0, 0.85}, {-1.4, 1.0, 0.85}
    };
    QVector<Pt3> pts = project(raw, cx, cy, scale);

    // Faces — identik Python
    using Face = std::pair<QVector<int>, int>;
    QVector<Face> faces = {
        {{4,5,6,7}, 36}, {{0,1,2,3}, 18},
        {{0,4,7,3}, 26}, {{1,5,6,2}, 26},
        {{3,2,6,7}, 32}, {{0,1,5,4}, 14}
    };
    std::sort(faces.begin(), faces.end(), [&](const Face &a, const Face &b) {
        return avgZ(pts, a.first) > avgZ(pts, b.first);
    });
    for (const auto &[idx, alpha] : faces) {
        QVector<Pt3> v;
        for (int i : idx) v.append(pts[i]);
        if (crossZ(v) >= 0)
            drawFace(p, v, alpha);
    }

    // Edges
    p.setBrush(Qt::NoBrush);
    int edges[][2] = {
        {4,5},{5,6},{6,7},{7,4},
        {0,1},{1,2},{2,3},{3,0},
        {4,0},{5,1},{6,2},{7,3}
    };
    for (auto &e : edges) {
        p.setPen(solidPen());
        p.drawLine(int(pts[e[0]].x), int(pts[e[0]].y),
                   int(pts[e[1]].x), int(pts[e[1]].y));
    }

    // Labels — identik Python
    double mpx = (pts[4].x + pts[5].x) / 2;
    double mpy = (pts[4].y + pts[5].y) / 2 + 10;
    double mpz = (pts[4].z + pts[5].z) / 2;
    drawLabel3D(p, mpx, mpy, mpz, "p");

    double mlx = (pts[5].x + pts[6].x) / 2 + 4;
    double mly = (pts[5].y + pts[6].y) / 2;
    double mlz = (pts[5].z + pts[6].z) / 2;
    drawLabel3D(p, mlx, mly, mlz, "l");

    double mtx = (pts[4].x + pts[7].x) / 2 - 14;
    double mty = (pts[4].y + pts[7].y) / 2;
    double mtz = (pts[4].z + pts[7].z) / 2;
    drawLabel3D(p, mtx, mty, mtz, "t");
}

// ════════════════════════════════════════════════════════
// BOLA — identik Python (wireframe sphere)
// ════════════════════════════════════════════════════════
void ShapeIllustration::drawBola(QPainter &p, int W, int H)
{
    double cx = W / 2.0, cy = H / 2.0;
    double scVal = TARGET_R; // identik Python: sc = TARGET_R
    double r3 = 1.0;
    double ay = m_angle_y, ax = m_angle_x;
    double cay_ = std::cos(ay), say_ = std::sin(ay);
    double cax_ = std::cos(ax), sax_ = std::sin(ax);
    double fov = 5.0;
    int segs = 48; // identik Python

    // rot3 — identik Python (inline lambda)
    auto rot3 = [&](double x, double y, double z) -> Pt3 {
        double x2 =  x * cay_ + z * say_;
        double z2 = -x * say_ + z * cay_;
        double y3 =  y * cax_ - z2 * sax_;
        double z3 =  y * sax_ + z2 * cax_;
        double d = fov / (fov + z3 + 4.0);
        return {cx + x2 * scVal * d, cy + y3 * scVal * d, z3};
    };

    p.setBrush(Qt::NoBrush);

    // Outline luar — identik Python
    p.setPen(solidPen(1.9));
    Pt3 prev = {0, 0, 0};
    bool hasPrev = false;
    for (int i = 0; i <= segs; ++i) {
        double a = 2.0 * M_PI * i / segs;
        Pt3 pt = rot3(r3 * std::cos(a), r3 * std::sin(a), 0);
        if (hasPrev) {
            p.drawLine(int(prev.x), int(prev.y), int(pt.x), int(pt.y));
        }
        prev = pt;
        hasPrev = true;
    }

    // 7 garis lintang — identik Python
    for (int li = 1; li < 8; ++li) {
        double lat = M_PI * li / 8.0 - M_PI / 2.0;
        double rl = r3 * std::cos(lat);
        double yl = r3 * std::sin(lat);
        QVector<Pt3> lpts;
        lpts.reserve(segs + 1);
        for (int i = 0; i <= segs; ++i) {
            double a = 2.0 * M_PI * i / segs;
            lpts.append(rot3(rl * std::cos(a), yl, rl * std::sin(a)));
        }
        for (int i = 0; i < segs; ++i) {
            double z_avg = (lpts[i].z + lpts[i + 1].z) / 2.0;
            p.setPen(z_avg < 0 ? solidPen(1.1) : dashPen(0.7));
            p.drawLine(int(lpts[i].x), int(lpts[i].y),
                       int(lpts[i + 1].x), int(lpts[i + 1].y));
        }
    }

    // 8 garis bujur — identik Python
    for (int li = 0; li < 8; ++li) {
        double lon = 2.0 * M_PI * li / 8.0;
        QVector<Pt3> lpts;
        lpts.reserve(segs + 1);
        for (int i = 0; i <= segs; ++i) {
            double phi = M_PI * i / segs - M_PI / 2.0;
            lpts.append(rot3(
                r3 * std::cos(phi) * std::cos(lon),
                r3 * std::sin(phi),
                r3 * std::cos(phi) * std::sin(lon)
            ));
        }
        for (int i = 0; i < segs; ++i) {
            double z_avg = (lpts[i].z + lpts[i + 1].z) / 2.0;
            p.setPen(z_avg < 0 ? solidPen(1.1) : dashPen(0.7));
            p.drawLine(int(lpts[i].x), int(lpts[i].y),
                       int(lpts[i + 1].x), int(lpts[i + 1].y));
        }
    }

    // Garis radius — identik Python
    Pt3 rPt = rot3(r3, 0, 0);
    p.setPen(solidPen(1.2));
    p.drawLine(int(cx), int(cy), int(rPt.x), int(rPt.y));
    drawLabel3D(p, rPt.x, rPt.y - 4, rPt.z, "r");
}

// ════════════════════════════════════════════════════════
// TABUNG — identik Python
// ════════════════════════════════════════════════════════
void ShapeIllustration::drawTabung(QPainter &p, int W, int H)
{
    double cx = W / 2.0, cy = H / 2.0;
    double r3 = 1.1, h3 = 0.95; // identik Python
    double scale = sc(std::sqrt(r3 * r3 + h3 * h3)); // identik Python
    int segs = 32; // identik Python

    QVector<Vec3> topRaw, botRaw;
    topRaw.reserve(segs);
    botRaw.reserve(segs);
    for (int i = 0; i < segs; ++i) {
        double a = 2.0 * M_PI * i / segs;
        topRaw.append({r3 * std::cos(a), -h3, r3 * std::sin(a)});
        botRaw.append({r3 * std::cos(a),  h3, r3 * std::sin(a)});
    }
    QVector<Pt3> tpts = project(topRaw, cx, cy, scale);
    QVector<Pt3> bpts = project(botRaw, cx, cy, scale);

    p.setBrush(Qt::NoBrush);

    // Garis vertikal — setiap 4 segmen (8 garis) — identik Python
    for (int i = 0; i < segs; i += 4) {
        double z_avg = (tpts[i].z + bpts[i].z) / 2.0;
        p.setPen(z_avg < 0 ? solidPen(1.3) : dashPen(0.8));
        p.drawLine(int(tpts[i].x), int(tpts[i].y),
                   int(bpts[i].x), int(bpts[i].y));
    }

    // Lingkaran atas & bawah — identik Python
    for (const auto &ring : {tpts, bpts}) {
        for (int i = 0; i < segs; ++i) {
            int j = (i + 1) % segs;
            double z_avg = (ring[i].z + ring[j].z) / 2.0;
            p.setPen(z_avg < 0 ? solidPen(1.8) : dashPen(1.0));
            p.drawLine(int(ring[i].x), int(ring[i].y),
                       int(ring[j].x), int(ring[j].y));
        }
    }

    // Labels — identik Python
    QVector<Pt3> rPt = project({{r3, -h3, 0}}, cx, cy, scale);
    drawLabel3D(p, rPt[0].x + 2, rPt[0].y - 7, rPt[0].z, "r");
    QVector<Pt3> tPt = project({{r3, 0, 0}}, cx, cy, scale);
    drawLabel3D(p, tPt[0].x + 4, tPt[0].y, tPt[0].z, "t");
}

// ════════════════════════════════════════════════════════
// KERUCUT — identik Python
// ════════════════════════════════════════════════════════
void ShapeIllustration::drawKerucut(QPainter &p, int W, int H)
{
    double cx = W / 2.0, cy = H / 2.0;
    double r3 = 1.1, h3 = 1.3; // identik Python
    double scale = sc(std::sqrt(r3 * r3 + h3 * h3)); // identik Python
    int segs = 32; // identik Python

    QVector<Vec3> apexRaw = {{0, -h3, 0}};
    QVector<Vec3> baseRaw;
    baseRaw.reserve(segs);
    for (int i = 0; i < segs; ++i) {
        double a = 2.0 * M_PI * i / segs;
        baseRaw.append({r3 * std::cos(a), h3, r3 * std::sin(a)});
    }
    QVector<Pt3> apexPt = project(apexRaw, cx, cy, scale);
    QVector<Pt3> bpts = project(baseRaw, cx, cy, scale);

    p.setBrush(Qt::NoBrush);

    // 8 garis sisi — setiap 4 segmen — identik Python
    for (int i = 0; i < segs; i += 4) {
        double z_avg = (apexPt[0].z + bpts[i].z) / 2.0;
        p.setPen(z_avg < 0 ? solidPen(1.5) : dashPen(0.8));
        p.drawLine(int(apexPt[0].x), int(apexPt[0].y),
                   int(bpts[i].x), int(bpts[i].y));
    }

    // Lingkaran alas — identik Python
    for (int i = 0; i < segs; ++i) {
        int j = (i + 1) % segs;
        double z_avg = (bpts[i].z + bpts[j].z) / 2.0;
        p.setPen(z_avg < 0 ? solidPen(1.8) : dashPen(1.0));
        p.drawLine(int(bpts[i].x), int(bpts[i].y),
                   int(bpts[j].x), int(bpts[j].y));
    }

    // Labels — identik Python
    drawLabel3D(p, apexPt[0].x + 4, apexPt[0].y + 2, apexPt[0].z, "t");
    QVector<Pt3> rPt = project({{r3, h3, 0}}, cx, cy, scale);
    drawLabel3D(p, rPt[0].x + 4, rPt[0].y + 8, rPt[0].z, "r");
}

// ════════════════════════════════════════════════════════
// PRISMA SEGITIGA — identik Python
// ════════════════════════════════════════════════════════
void ShapeIllustration::drawPrismaSegitiga(QPainter &p, int W, int H)
{
    double cx = W / 2.0, cy = H / 2.0;
    double h3 = std::sqrt(3.0);
    double hp = 1.2;
    QVector<Vec3> raw = {
        { 0.0, -2*h3/3, -hp},
        {-1.0,    h3/3, -hp},
        { 1.0,    h3/3, -hp},
        { 0.0, -2*h3/3,  hp},
        {-1.0,    h3/3,  hp},
        { 1.0,    h3/3,  hp}
    };
    double scale = sc(std::sqrt(1.0*1.0 + (h3*2/3)*(h3*2/3) + hp*hp)); // identik Python
    QVector<Pt3> pts = project(raw, cx, cy, scale);

    // Faces — identik Python
    using Face = std::pair<QVector<int>, int>;
    QVector<Face> faces = {
        {{0,1,2}, 28}, {{3,4,5}, 28},
        {{0,3,4,1}, 22}, {{1,4,5,2}, 18}, {{0,2,5,3}, 32}
    };
    std::sort(faces.begin(), faces.end(), [&](const Face &a, const Face &b) {
        return avgZ(pts, a.first) > avgZ(pts, b.first);
    });
    for (const auto &[idx, alpha] : faces) {
        QVector<Pt3> v;
        for (int i : idx) v.append(pts[i]);
        if (crossZ(v) >= 0)
            drawFace(p, v, alpha);
    }

    // Edges
    p.setBrush(Qt::NoBrush);
    int edges[][2] = {{0,1},{1,2},{2,0},{3,4},{4,5},{5,3},{0,3},{1,4},{2,5}};
    for (auto &e : edges) {
        p.setPen(solidPen());
        p.drawLine(int(pts[e[0]].x), int(pts[e[0]].y),
                   int(pts[e[1]].x), int(pts[e[1]].y));
    }

    // Labels — identik Python
    double cAx = (pts[3].x + pts[4].x + pts[5].x) / 3.0;
    double cAy = (pts[3].y + pts[4].y + pts[5].y) / 3.0;
    double cAz = (pts[3].z + pts[4].z + pts[5].z) / 3.0;
    drawLabel3D(p, cAx - 4, cAy + 14, cAz, "A");
    double mTx = (pts[2].x + pts[5].x) / 2.0 + 4;
    double mTy = (pts[2].y + pts[5].y) / 2.0;
    double mTz = (pts[2].z + pts[5].z) / 2.0;
    drawLabel3D(p, mTx, mTy, mTz, "t");
}

// ════════════════════════════════════════════════════════
// LIMAS SEGITIGA — identik Python
// ════════════════════════════════════════════════════════
void ShapeIllustration::drawLimasSegitiga(QPainter &p, int W, int H)
{
    double cx = W / 2.0, cy = H / 2.0;
    double h3 = std::sqrt(3.0);
    double hp = 1.5;
    QVector<Vec3> raw = {
        {-1.0,  0.4,  h3/3},
        { 1.0,  0.4,  h3/3},
        { 0.0,  0.4, -2*h3/3},
        { 0.0, -hp,   0.0}
    };
    double scale = sc(1.60); // identik Python
    QVector<Pt3> pts = project(raw, cx, cy, scale);

    // Faces — identik Python
    using Face = std::pair<QVector<int>, int>;
    QVector<Face> faces = {
        {{0,1,3}, 34}, {{1,2,3}, 24},
        {{2,0,3}, 18}, {{0,1,2}, 22}
    };
    std::sort(faces.begin(), faces.end(), [&](const Face &a, const Face &b) {
        return avgZ(pts, a.first) > avgZ(pts, b.first);
    });
    for (const auto &[idx, alpha] : faces) {
        QVector<Pt3> v;
        for (int i : idx) v.append(pts[i]);
        if (crossZ(v) >= 0)
            drawFace(p, v, alpha);
    }

    // Edges
    p.setBrush(Qt::NoBrush);
    int edges[][2] = {{0,1},{1,2},{2,0},{0,3},{1,3},{2,3}};
    for (auto &e : edges) {
        p.setPen(solidPen());
        p.drawLine(int(pts[e[0]].x), int(pts[e[0]].y),
                   int(pts[e[1]].x), int(pts[e[1]].y));
    }

    // Labels — identik Python
    double cAx = (pts[0].x + pts[1].x + pts[2].x) / 3.0;
    double cAy = (pts[0].y + pts[1].y + pts[2].y) / 3.0;
    double cAz = (pts[0].z + pts[1].z + pts[2].z) / 3.0;
    drawLabel3D(p, cAx - 4, cAy + 12, cAz, "A");
    double mTx = (cAx + pts[3].x) / 2.0 + 4;
    double mTy = (cAy + pts[3].y) / 2.0;
    double mTz = (cAz + pts[3].z) / 2.0;
    drawLabel3D(p, mTx, mTy, mTz, "t");
}

// ════════════════════════════════════════════════════════
// LIMAS SEGI EMPAT — identik Python
// ════════════════════════════════════════════════════════
void ShapeIllustration::drawLimasSegiEmpat(QPainter &p, int W, int H)
{
    double cx = W / 2.0, cy = H / 2.0;
    double hp = 1.4;
    QVector<Vec3> raw = {
        {-1.0,  0.3, -1.0}, { 1.0,  0.3, -1.0},
        { 1.0,  0.3,  1.0}, {-1.0,  0.3,  1.0},
        { 0.0, -hp,   0.0}
    };
    double scale = sc(1.58); // identik Python
    QVector<Pt3> pts = project(raw, cx, cy, scale);

    // Faces — identik Python
    using Face = std::pair<QVector<int>, int>;
    QVector<Face> faces = {
        {{0,1,4}, 32}, {{1,2,4}, 26},
        {{2,3,4}, 20}, {{3,0,4}, 24},
        {{0,1,2,3}, 22}
    };
    std::sort(faces.begin(), faces.end(), [&](const Face &a, const Face &b) {
        return avgZ(pts, a.first) > avgZ(pts, b.first);
    });
    for (const auto &[idx, alpha] : faces) {
        QVector<Pt3> v;
        for (int i : idx) v.append(pts[i]);
        if (crossZ(v) >= 0)
            drawFace(p, v, alpha);
    }

    // Edges
    p.setBrush(Qt::NoBrush);
    int edges[][2] = {{0,1},{1,2},{2,3},{3,0},{0,4},{1,4},{2,4},{3,4}};
    for (auto &e : edges) {
        p.setPen(solidPen());
        p.drawLine(int(pts[e[0]].x), int(pts[e[0]].y),
                   int(pts[e[1]].x), int(pts[e[1]].y));
    }

    // Labels — identik Python
    double cAx = (pts[0].x + pts[1].x + pts[2].x + pts[3].x) / 4.0;
    double cAy = (pts[0].y + pts[1].y + pts[2].y + pts[3].y) / 4.0;
    double cAz = (pts[0].z + pts[1].z + pts[2].z + pts[3].z) / 4.0;
    drawLabel3D(p, cAx - 4, cAy + 12, cAz, "A");
    double mTx = (cAx + pts[4].x) / 2.0 + 4;
    double mTy = (cAy + pts[4].y) / 2.0;
    double mTz = (cAz + pts[4].z) / 2.0;
    drawLabel3D(p, mTx, mTy, mTz, "t");
}
