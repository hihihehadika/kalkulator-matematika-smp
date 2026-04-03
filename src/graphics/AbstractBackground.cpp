/*
 * AbstractBackground.cpp — Implementasi Latar Belakang Animasi
 * Padanan Python: class AbstractBackground (baris 401-514 di main.py)
 */
#include "graphics/AbstractBackground.h"
#include "core/Theme.h"

#include <QPainter>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QPen>
#include <cmath>
#include <algorithm>
#include <random>

AbstractBackground::AbstractBackground(QWidget *parent)
    : QWidget(parent),
      m_warmWhite(0xFF, 0xF0, 0xB0)
{
    isDarkMode = true;
    t = 0.0;

    // ── Inisialisasi bintang (seed = 42, identik Python) ──
    std::mt19937 rng(42);
    auto rf = [&](double a, double b) {
        return std::uniform_real_distribution<double>(a, b)(rng);
    };

    m_stars.reserve(STAR_COUNT);
    for (int i = 0; i < STAR_COUNT; ++i) {
        double xf  = rf(0.0, 1.0);
        double yf  = rf(0.0, 1.0);
        double sz;
        int choice = std::uniform_int_distribution<int>(0, 4)(rng);
        const double sizes[] = {0.8, 0.9, 1.1, 1.4, 1.8};
        sz = sizes[choice];
        double phase = rf(0.0, M_PI * 2);
        double spd   = rf(0.08, 0.18);
        m_stars.push_back({xf, yf, sz, phase, spd});
    }

    // ── Inisialisasi streak (garis diagonal) ──
    auto sinD = [](double deg) { return std::sin(deg * M_PI / 180.0); };
    auto cosD = [](double deg) { return std::cos(deg * M_PI / 180.0); };

    m_streaks = {
        {32.0, 0.00, sinD(-20), cosD(-20), 1.2, T::GOLD,      55},
        {48.0, 0.33, sinD(-15), cosD(-15), 0.7, T::GOLD_PALE,  32},
        {64.0, 0.67, sinD(-24), cosD(-24), 0.5, T::TEAL,       24},
    };

    m_streakPens.resize(m_streaks.size());
    for (int i = 0; i < m_streaks.size(); ++i) {
        m_streakPens[i] = QPen(QColor(Qt::white), m_streaks[i].thick);
    }

    m_cacheSize = QSize(0, 0);

    // ── Timer animasi 60fps ──
    connect(&m_timer, &QTimer::timeout, this, &AbstractBackground::onTick);
    m_timer.setTimerType(Qt::PreciseTimer);
    m_timer.start(16);
}

void AbstractBackground::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    if (!m_timer.isActive()) m_timer.start(16);
}

void AbstractBackground::hideEvent(QHideEvent *e)
{
    QWidget::hideEvent(e);
    m_timer.stop();
}

void AbstractBackground::onTick()
{
    t = std::fmod(t + 0.004, 1000.0);
    update();
}

void AbstractBackground::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    m_cache = QPixmap(); // invalidate cache
}

// ╔══════════════════════════════════════════════════════╗
// ║           Rebuild Static Cache                      ║
// ╚══════════════════════════════════════════════════════╝

void AbstractBackground::rebuildCache(int W, int H)
{
    QPixmap pix(W, H);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    // Background
    p.fillRect(0, 0, W, H, QColor(0x0D, 0x0D, 0x0F));

    // Grid
    QColor gc = T::GOLD;
    gc.setAlpha(GRID_ALPHA);
    p.setPen(QPen(gc, 0.5));
    for (int x = 0; x <= W + GRID_SPACING; x += GRID_SPACING)
        p.drawLine(x, 0, x, H);
    for (int y = 0; y <= H + GRID_SPACING; y += GRID_SPACING)
        p.drawLine(0, y, W, y);

    // Edge vignette
    int d = static_cast<int>(std::min(W, H) * 0.28);
    p.setPen(Qt::NoPen);

    // Top vignette
    QLinearGradient gTop(0, 0, 0, d);
    gTop.setColorAt(0, QColor(0, 0, 0, 160));
    gTop.setColorAt(1, QColor(0, 0, 0, 0));
    p.setBrush(gTop);
    p.drawRect(0, 0, W, d);

    // Bottom vignette
    QLinearGradient gBot(0, H, 0, H - d);
    gBot.setColorAt(0, QColor(0, 0, 0, 160));
    gBot.setColorAt(1, QColor(0, 0, 0, 0));
    p.setBrush(gBot);
    p.drawRect(0, H - d, W, d);

    // Left vignette
    QLinearGradient gLeft(0, 0, d, 0);
    gLeft.setColorAt(0, QColor(0, 0, 0, 160));
    gLeft.setColorAt(1, QColor(0, 0, 0, 0));
    p.setBrush(gLeft);
    p.drawRect(0, 0, d, H);

    // Right vignette
    QLinearGradient gRight(W, 0, W - d, 0);
    gRight.setColorAt(0, QColor(0, 0, 0, 160));
    gRight.setColorAt(1, QColor(0, 0, 0, 0));
    p.setBrush(gRight);
    p.drawRect(W - d, 0, d, H);

    p.end();
    m_cache = pix;
    m_cacheSize = QSize(W, H);
}

// ╔══════════════════════════════════════════════════════╗
// ║                 Paint Event                         ║
// ╚══════════════════════════════════════════════════════╝

void AbstractBackground::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    int W = width(), H = height();
    if (W == 0 || H == 0) return;

    if (m_cache.isNull() || m_cacheSize != QSize(W, H)) {
        rebuildCache(W, H);
    }
    p.drawPixmap(0, 0, m_cache);
    paintOrbs(p, W, H);
    paintStreaks(p, W, H, t);
    paintStars(p, W, H, t);
}

void AbstractBackground::paintOrbs(QPainter &p, int W, int H)
{
    p.setPen(Qt::NoPen);
    double scx = W * 0.50, scy = H * 0.28;
    double sr = std::max(W, H) * 0.55;

    QRadialGradient grad(scx, scy, sr);
    QColor cCore(0xFF, 0xF5, 0xD0); cCore.setAlpha(35);
    QColor cMid(0xC9, 0xA8, 0x4C);  cMid.setAlpha(18);
    QColor cEdge(0xC9, 0xA8, 0x4C); cEdge.setAlpha(0);
    grad.setColorAt(0.0, cCore);
    grad.setColorAt(0.35, cMid);
    grad.setColorAt(1.0, cEdge);
    p.setBrush(grad);
    p.drawEllipse(QRectF(scx - sr, scy - sr, sr * 2, sr * 2));
}

void AbstractBackground::paintStreaks(QPainter &p, int W, int H, double t)
{
    p.setBrush(Qt::NoBrush);
    double halfH = H * 0.5;

    for (int idx = 0; idx < m_streaks.size(); ++idx) {
        const auto &cfg = m_streaks[idx];
        double prog = std::fmod(t / cfg.period + cfg.phase, 1.0);
        double fade = std::sin(prog * M_PI);
        int a = static_cast<int>(cfg.maxAlpha * fade);
        if (a < 3) continue;

        QColor sc = cfg.color;
        sc.setAlpha(a);
        QPen pen = m_streakPens[idx];
        pen.setColor(sc);
        pen.setWidthF(cfg.thick);
        pen.setCapStyle(Qt::RoundCap);
        p.setPen(pen);

        double cx = (-0.15 + prog * 1.30) * W;
        double L = H * 2.5;
        double hL = L * 0.5;
        p.drawLine(
            static_cast<int>(cx - cfg.cosA * hL),
            static_cast<int>(halfH - cfg.sinA * hL),
            static_cast<int>(cx + cfg.cosA * hL),
            static_cast<int>(halfH + cfg.sinA * hL)
        );
    }
}

void AbstractBackground::paintStars(QPainter &p, int W, int H, double t)
{
    p.setPen(Qt::NoPen);
    for (const auto &star : m_stars) {
        double bright = 0.35 + 0.65 * (0.5 + 0.5 * std::sin(t * star.spd * 8 + star.phase));
        int baseA = star.sz < 1.0 ? 50 : (star.sz < 1.4 ? 80 : 110);
        m_warmWhite.setAlpha(static_cast<int>(baseA * bright));
        p.setBrush(m_warmWhite);
        double hs = star.sz * 0.5;
        p.drawEllipse(QRectF(
            star.xf * W - hs, star.yf * H - hs,
            star.sz, star.sz
        ));
    }
}
