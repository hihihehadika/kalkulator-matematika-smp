/*
 * DashboardDeco.cpp — Implementasi Dekorasi Dashboard
 * Padanan Python: class DashboardDecoItem (baris 705-793 di main.py)
 */
#include "graphics/DashboardDeco.h"
#include "core/Theme.h"

#include <QPen>
#include <QColor>
#include <QPointF>
#include <QRectF>
#include <cmath>

// ╔══════════════════════════════════════════════════════╗
// ║              DashboardDecoItem                      ║
// ╚══════════════════════════════════════════════════════╝

DashboardDecoItem::DashboardDecoItem()
{
    setZValue(-0.5);

    std::mt19937 rng(7); // Seed identik Python: random.Random(7)
    auto rf = [&](double a, double b) {
        return std::uniform_real_distribution<double>(a, b)(rng);
    };
    auto ri = [&](int a, int b) {
        return std::uniform_int_distribution<int>(a, b)(rng);
    };

    // ── Simbol dekoratif ──
    const QStringList symbols = {
        "pi", "V", "S", "~", "d", "8", "+-", "x", "2", "3",
        ":", "a", "b", "t", "L", "f", "D", "n"
    };
    const int sizes[] = {14, 16, 18, 20, 22};

    m_symData.reserve(symbols.size());
    for (const auto &s : symbols) {
        SymbolData sd;
        sd.sym   = s;
        sd.xf    = rf(0.04, 0.96);
        sd.yf    = rf(0.15, 0.93);
        sd.phase = rf(0, M_PI * 2);
        sd.spd   = rf(0.07, 0.18);
        sd.amp   = rf(8, 18);
        sd.alpha = ri(40, 85);
        sd.size  = sizes[ri(0, 4)];
        m_symData.push_back(sd);
    }

    // ── Partikel titik kecil ──
    const double psizes[] = {1.5, 2.0, 2.5};
    m_particles.reserve(30);
    for (int i = 0; i < 30; ++i) {
        ParticleData pd;
        pd.xf    = rf(0.0, 1.0);
        pd.yf    = rf(0.0, 1.0);
        pd.phase = rf(0, M_PI * 2);
        pd.spd   = rf(0.08, 0.20);
        pd.ampX  = rf(3, 8);
        pd.ampY  = rf(2, 6);
        pd.alpha = ri(30, 70);
        pd.size  = psizes[ri(0, 2)];
        m_particles.push_back(pd);
    }
}

void DashboardDecoItem::setLayout(double W, double H, double cardCx, double cardCy)
{
    m_W = W; m_H = H; m_cardCx = cardCx; m_cardCy = cardCy;
    prepareGeometryChange();
}

void DashboardDecoItem::tick(double t)
{
    m_t = t;
    update();
}

QRectF DashboardDecoItem::boundingRect() const
{
    return QRectF(0, 0, m_W, m_H);
}

QFont DashboardDecoItem::getFont(int size)
{
    auto it = m_fontCache.find(size);
    if (it == m_fontCache.end()) {
        QFont f("Segoe UI", size, QFont::Bold);
        f.setHintingPreference(QFont::PreferVerticalHinting);
        m_fontCache.insert(size, f);
        return f;
    }
    return it.value();
}

void DashboardDecoItem::paint(QPainter *p,
                              const QStyleOptionGraphicsItem *,
                              QWidget *)
{
    p->setRenderHint(QPainter::Antialiasing, true);
    p->setRenderHint(QPainter::TextAntialiasing, true);

    double t = m_t;
    double W = m_W, H = m_H;
    double cx = m_cardCx, cy = m_cardCy;

    p->setBrush(Qt::NoBrush);

    // ══════════════════════════════════════════════
    //  Lingkaran konsentris beranimasi
    // ══════════════════════════════════════════════
    struct CircleConfig {
        double baseR; int maxA; double period; double offset; bool dotted;
    };
    const CircleConfig circles[] = {
        {185, 50, 4.0, 0.00, false},
        {265, 32, 5.5, 0.33, true},
        {345, 18, 7.0, 0.66, false},
    };

    for (const auto &cc : circles) {
        double phase = std::fmod(t / cc.period + cc.offset, 1.0);
        double r = cc.baseR + std::sin(phase * M_PI * 2) * 16.0;
        int alpha = static_cast<int>(cc.maxA * (0.5 + 0.5 * std::sin(phase * M_PI * 2)));
        if (alpha < 2) continue;

        QColor rc = T::GOLD;
        rc.setAlpha(alpha);
        QPen pen(rc, 1.5);
        pen.setCosmetic(false);
        pen.setStyle(cc.dotted ? Qt::DotLine : Qt::SolidLine);
        p->setPen(pen);
        p->drawEllipse(QRectF(cx - r, cy - r, r * 2, r * 2));
    }

    // ══════════════════════════════════════════════
    //  Tanda sudut di 4 pojok
    // ══════════════════════════════════════════════
    QColor cc2 = T::GOLD;
    cc2.setAlpha(65);
    p->setPen(QPen(cc2, 1.8));
    double LL = 42;
    struct Corner { double ox, oy; int sx, sy; };
    const Corner corners[] = {
        {22,    22,     1,  1},
        {W-22,  22,    -1,  1},
        {22,    H-22,   1, -1},
        {W-22,  H-22,  -1, -1},
    };
    for (const auto &co : corners) {
        p->drawLine(static_cast<int>(co.ox), static_cast<int>(co.oy),
                    static_cast<int>(co.ox + co.sx * LL), static_cast<int>(co.oy));
        p->drawLine(static_cast<int>(co.ox), static_cast<int>(co.oy),
                    static_cast<int>(co.ox), static_cast<int>(co.oy + co.sy * LL));
    }

    // ══════════════════════════════════════════════
    //  Partikel titik kecil
    // ══════════════════════════════════════════════
    p->setPen(Qt::NoPen);
    for (const auto &pd : m_particles) {
        double dx = std::sin(t * pd.spd * 0.8 + pd.phase) * pd.ampX;
        double dy = std::cos(t * pd.spd * 1.1 + pd.phase * 1.5) * pd.ampY;
        double px = pd.xf * W + dx;
        double py = pd.yf * H + dy;
        double pulse = 0.4 + 0.6 * std::sin(t * pd.spd * 4 + pd.phase);
        int a = static_cast<int>(pd.alpha * pulse);
        if (a < 5) continue;
        QColor pc = T::GOLD_PALE;
        pc.setAlpha(a);
        p->setBrush(pc);
        double sz = pd.size;
        p->drawEllipse(QRectF(px - sz / 2, py - sz / 2, sz, sz));
    }

    // ══════════════════════════════════════════════
    //  Simbol matematika mengambang
    // ══════════════════════════════════════════════
    for (const auto &sd : m_symData) {
        double dx = std::sin(t * sd.spd * 0.7 + sd.phase) * sd.amp;
        double dy = std::cos(t * sd.spd + sd.phase * 1.3) * sd.amp * 0.55;
        double sx = sd.xf * W + dx;
        double sy = sd.yf * H + dy;
        double pulse = 0.50 + 0.50 * std::sin(t * sd.spd * 3 + sd.phase);
        int a = static_cast<int>(sd.alpha * pulse);
        if (a < 5) continue;
        QColor sc = T::GOLD_PALE;
        sc.setAlpha(a);
        p->setPen(sc);
        p->setFont(getFont(sd.size));
        p->drawText(QPointF(sx, sy), sd.sym);
    }
}
