/*
 * DashboardView.cpp — Implementasi Dashboard Carousel
 * Padanan Python: class DashboardView (baris 996-1199 di main.py)
 */
#include "views/DashboardView.h"
#include "MainWindow.h"
#include "graphics/AbstractBackground.h"
#include "graphics/NumberParticle.h"
#include "graphics/DashboardDeco.h"
#include "widgets/FeatureCard.h"
#include "widgets/CardProxy.h"
#include "widgets/IconPainters.h"
#include "core/Theme.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsProxyWidget>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QShortcut>
#include <QPushButton>
#include <QMap>
#include <cmath>

// ── Slot parameters identik Python (xFactor, yOffset, rotation, scale, opacity) ──
struct SlotParam { double xf; double dy; double rot; double sc; double op; };
static const QMap<int, SlotParam> SLOT_PARAMS = {
    {-3, {-2.5,  80, -22, 0.55, 0.0}},
    {-2, {-1.7,  50, -14, 0.70, 0.0}},
    {-1, {-1.05, 30, -10, 0.84, 0.88}},
    { 0, { 0.00,  0,   0, 1.00, 1.00}},
    { 1, { 1.05, 30,  10, 0.84, 0.88}},
    { 2, { 1.7,  50,  14, 0.70, 0.0}},
    { 3, { 2.5,  80,  22, 0.55, 0.0}},
};

DashboardView::DashboardView(MainWindow *parent)
    : QWidget(parent), m_mw(parent)
{
    setFocusPolicy(Qt::StrongFocus);

    // ── Definisi 7 fitur (IDENTIK Python) ──
    m_features = {
        {ICON_LINECHART, "Persamaan\nGaris Lurus",      "Buat grafik dari persamaan linear"},
        {ICON_BOX3D,     "Kalkulator\nBangun Ruang",    "Volume dan luas permukaan geometri"},
        {ICON_DICE,      "Probabilitas\ndan Peluang",   "Kalkulator peluang interaktif"},
        {ICON_EQUATION,  "Sistem Persamaan\nLinear",    "Selesaikan SPLDV dengan eliminasi"},
        {ICON_TRIANGLE,  "Teorema\nPythagoras",         "Hitung sisi segitiga siku-siku"},
        {ICON_CHART,     "Statistika\nDasar",           "Rata-rata, median, modus dari data"},
        {ICON_CALC,      "Kalkulator\nBasic",           "Hitung + - x : pecahan desimal"},
    };
    m_cardCount = m_features.size();

    buildScene();

    // ── Timer animasi 60fps ──
    connect(&m_animTimer, &QTimer::timeout, this, &DashboardView::onTick);
    m_animTimer.setTimerType(Qt::PreciseTimer);
    m_animTimer.start(16);

    // ── Scroll cooldown ──
    m_scrollTimer.setSingleShot(true);
    m_scrollTimer.setInterval(440);
    connect(&m_scrollTimer, &QTimer::timeout, this, [this]() { m_scrollCooldown = false; });
}

void DashboardView::buildScene()
{
    m_scene = new QGraphicsScene(this);
    m_view = new QGraphicsView(m_scene, this);
    m_view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform
                           | QPainter::TextAntialiasing);
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setStyleSheet("background:transparent;border:none;");
    m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    m_view->setFocusPolicy(Qt::NoFocus);

    // Background
    m_bg = new AbstractBackground();
    auto *bgProxy = m_scene->addWidget(m_bg);
    bgProxy->setZValue(-10);

    // Particles
    m_particles = new NumberParticleSystem();
    m_scene->addItem(m_particles);

    // Deco
    m_deco = new DashboardDecoItem();
    m_scene->addItem(m_deco);

    // ── Header text items (identik Python — scene items, bukan QLabel) ──
    QFont tf("Segoe UI", 26, QFont::Bold);
    tf.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
    m_titleItem = m_scene->addText("Matematika SMP");
    m_titleItem->setFont(tf);
    m_titleItem->setDefaultTextColor(QColor(T::GOLD));
    m_titleItem->setZValue(2);

    QFont sf("Arial", 11);
    sf.setLetterSpacing(QFont::AbsoluteSpacing, 2.0);
    m_subtitleItem = m_scene->addText("Belajar matematika jadi lebih mudah");
    m_subtitleItem->setFont(sf);
    m_subtitleItem->setDefaultTextColor(QColor(255, 255, 255, 110));
    m_subtitleItem->setZValue(2);

    QFont cf("Arial", 9);
    cf.setLetterSpacing(QFont::AbsoluteSpacing, 4.0);
    m_creditItem = m_scene->addText("B Y   D I K A");
    m_creditItem->setFont(cf);
    m_creditItem->setDefaultTextColor(QColor(0xC9, 0xA8, 0x4C, 150));
    m_creditItem->setZValue(2);

    // Gold separator line
    QPen rulePen(QColor(0xC9, 0xA8, 0x4C, 50), 0.8);
    m_ruleLine = m_scene->addLine(0, 0, 1, 1, rulePen);
    m_ruleLine->setZValue(2);

    // ── Nav buttons ‹ dan › (identik Python) ──
    m_navLeftBtn = makeNavBtn(QString::fromUtf8("\u2039"));
    m_navRightBtn = makeNavBtn(QString::fromUtf8("\u203A"));
    connect(static_cast<QPushButton*>(m_navLeftBtn->widget()), &QPushButton::clicked,
            this, [this]() { scrollLeft(); });
    connect(static_cast<QPushButton*>(m_navRightBtn->widget()), &QPushButton::clicked,
            this, [this]() { scrollRight(); });

    // ── Dot indicators ──
    for (int i = 0; i < m_cardCount; ++i) {
        auto *dot = m_scene->addEllipse(0, 0, 10, 10,
            QPen(Qt::NoPen),
            i == 0 ? QColor(T::GOLD) : QColor(0x40, 0x40, 0x55));
        dot->setZValue(10);
        m_dots.append(dot);
    }

    // ── Buat kartu ──
    for (int i = 0; i < m_cardCount; ++i) {
        auto &f = m_features[i];
        auto *card = new FeatureCardWidget(f.iconKey, f.title, f.desc);
        auto *proxy = new CardProxy(i, card);
        m_scene->addItem(proxy);
        connect(proxy, &CardProxy::clicked, this, &DashboardView::onCardClicked);
        // Identik Python: navigasi SETELAH animasi klik selesai
        connect(proxy, &CardProxy::clickAnimFinished, this, [this](int cardIndex) {
            emit featureSelected(cardIndex + 1);
        });
        m_proxies.append(proxy);
    }

    // Layout: view fills the widget
    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->addWidget(m_view);
}

QGraphicsProxyWidget* DashboardView::makeNavBtn(const QString &symbol)
{
    auto *btn = new QPushButton(symbol);
    btn->setFixedSize(44, 90);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFocusPolicy(Qt::NoFocus);
    btn->setStyleSheet(
        "QPushButton{background:rgba(13,13,15,0.60);border:1.5px solid rgba(201,168,76,0.35);"
        "border-radius:10px;color:rgba(201,168,76,0.80);font-size:30px;font-weight:bold;padding-bottom:3px;}"
        "QPushButton:hover{background:rgba(201,168,76,0.14);border-color:rgba(201,168,76,0.75);color:#D4B96A;}"
        "QPushButton:pressed{background:rgba(201,168,76,0.22);}");
    auto *item = m_scene->addWidget(btn);
    item->setZValue(10);
    return item;
}

void DashboardView::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    relayout();
}

void DashboardView::relayout()
{
    int W = width(), H = height();
    if (W < 100) return;
    m_scene->setSceneRect(0, 0, W, H);
    m_bg->setFixedSize(W, H);
    m_particles->setSize(W, H);

    layoutHeader(W, H);
    animateAll(true);
    layoutDots(W, H);
    layoutNavBtns(W, H);

    auto cc = cardCenter(W, H);
    m_deco->setLayout(W, H, cc.first, cc.second);
}

int DashboardView::headerHeight()
{
    int PAD = 42;
    double th = m_titleItem ? m_titleItem->boundingRect().height() : 40;
    double sh = m_subtitleItem ? m_subtitleItem->boundingRect().height() : 20;
    return int(PAD + th + 8 + sh + 18);
}

std::pair<double,double> DashboardView::cardCenter(int W, int H)
{
    int hh = headerHeight();
    double available = H - hh - 60;
    return {W / 2.0, hh + available * 0.54};
}

void DashboardView::layoutHeader(int W, int H)
{
    int PAD = 42;
    double tw = m_titleItem->boundingRect().width();
    double th = m_titleItem->boundingRect().height();
    m_titleItem->setPos((W - tw) / 2.0, PAD);

    double ry = PAD + th + 5;
    double rlen = std::min(400.0, W * 0.36);
    m_ruleLine->setLine((W - rlen) / 2.0, ry, (W + rlen) / 2.0, ry);

    double sw = m_subtitleItem->boundingRect().width();
    double sh = m_subtitleItem->boundingRect().height();
    m_subtitleItem->setPos((W - sw) / 2.0, ry + 9);

    double cw = m_creditItem->boundingRect().width();
    m_creditItem->setPos((W - cw) / 2.0, ry + 9 + sh + 1);
}

void DashboardView::layoutDots(int W, int H)
{
    int n = m_cardCount;
    int dotSize = 10, gap = 10;
    int total = n * dotSize + (n - 1) * gap;
    double sx = (W - total) / 2.0;
    double sy = H - 38;

    for (int i = 0; i < n; ++i) {
        int sz = (i == m_focusIdx) ? 12 : 8;
        double x = sx + i * (dotSize + gap) + (dotSize - sz) / 2.0;
        m_dots[i]->setRect(x, sy, sz, sz);
    }
}

void DashboardView::layoutNavBtns(int W, int H)
{
    auto cc = cardCenter(W, H);
    int bw = 44, bh = 90;
    int margin = std::max(16, int(W * 0.04));
    m_navLeftBtn->setPos(margin, cc.second - bh / 2.0);
    m_navRightBtn->setPos(W - margin - bw, cc.second - bh / 2.0);
}

int DashboardView::slotOfProxy(int cardIndex)
{
    int raw = (cardIndex - m_focusIdx) % m_cardCount;
    if (raw > m_cardCount / 2) raw -= m_cardCount;
    if (raw < -(m_cardCount / 2)) raw += m_cardCount;
    return std::max(-3, std::min(3, raw));
}

void DashboardView::positionCards()
{
    animateAll(false);
}

void DashboardView::animateAll(bool instant)
{
    int W = width(), H = height();
    if (W < 100 || m_proxies.isEmpty()) return;

    auto cc = cardCenter(W, H);
    double cx = cc.first, cy = cc.second;
    int cw = FeatureCardWidget::CW;
    int ch = FeatureCardWidget::CH;
    double spacing = cw * 1.18;

    m_deco->setLayout(W, H, cx, cy);

    for (int i = 0; i < m_cardCount; ++i) {
        int slot = slotOfProxy(i);
        SlotParam sp = SLOT_PARAMS.value(slot, (slot > 0) ? SLOT_PARAMS[3] : SLOT_PARAMS[-3]);

        double tx = cx + sp.xf * spacing - cw / 2.0;
        double ty = cy + sp.dy - ch / 2.0;

        double z = 0;
        if (slot == 0) z = 10;
        else if (std::abs(slot) == 1) z = 5;
        else if (std::abs(slot) == 2) z = -1;
        else z = -2;

        m_proxies[i]->setCurrentSlot(slot);
        m_proxies[i]->setTargetPos(tx, ty);
        m_proxies[i]->setTargetScale(sp.sc);
        m_proxies[i]->setTargetOpacity(sp.op);
        m_proxies[i]->setTargetZ(z);
        m_proxies[i]->setTargetRotation(sp.rot);
        m_proxies[i]->applyTarget(!instant);
        m_proxies[i]->setVisible(true);
    }
}

void DashboardView::scrollLeft()
{
    if (m_scrollCooldown) return;
    m_focusIdx = (m_focusIdx - 1 + m_cardCount) % m_cardCount;
    animateAll(false);
    updateDots();
    m_scrollCooldown = true;
    m_scrollTimer.start();
}

void DashboardView::scrollRight()
{
    if (m_scrollCooldown) return;
    m_focusIdx = (m_focusIdx + 1) % m_cardCount;
    animateAll(false);
    updateDots();
    m_scrollCooldown = true;
    m_scrollTimer.start();
}

void DashboardView::updateDots()
{
    for (int i = 0; i < m_dots.size(); ++i) {
        if (i == m_focusIdx) {
            m_dots[i]->setBrush(QColor(T::GOLD));
            m_dots[i]->setRect(m_dots[i]->rect().x(), m_dots[i]->rect().y(), 12, 12);
        } else {
            m_dots[i]->setBrush(QColor(0x40, 0x40, 0x55));
            m_dots[i]->setRect(m_dots[i]->rect().x(), m_dots[i]->rect().y(), 8, 8);
        }
    }
}

void DashboardView::focusCard(int pageIndex)
{
    int cardIdx = (pageIndex - 1) % m_cardCount;
    if (cardIdx < 0) cardIdx += m_cardCount;
    m_focusIdx = cardIdx;
    animateAll(false);
    updateDots();
    if (cardIdx >= 0 && cardIdx < m_proxies.size()) {
        m_proxies[cardIdx]->playBounce();
    }
}

void DashboardView::onTick()
{
    m_t += 0.0032;
    m_deco->tick(m_t);
    m_particles->tick(m_t);
}

void DashboardView::onCardClicked(int cardIndex)
{
    // Identik Python: clicked hanya untuk scroll kiri/kanan
    // Navigasi ke halaman fitur ditangani oleh clickAnimFinished
    int slot = slotOfProxy(cardIndex);
    if (slot == -1) {
        scrollLeft();
    } else if (slot == 1) {
        scrollRight();
    }
}

void DashboardView::updateTheme(bool isDark)
{
    m_bg->isDarkMode = isDark;
    // Title color stays gold in dark mode
    m_titleItem->setDefaultTextColor(isDark ? QColor(T::GOLD) : QColor("#C9A84C"));
    m_subtitleItem->setDefaultTextColor(isDark ? QColor(255,255,255,110) : QColor("#4A4A6A"));
    m_creditItem->setDefaultTextColor(isDark ? QColor(0xC9,0xA8,0x4C,150) : QColor("#C9A84C"));
}

void DashboardView::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Left: case Qt::Key_A:
        scrollLeft(); break;
    case Qt::Key_Right: case Qt::Key_D:
        scrollRight(); break;
    case Qt::Key_Return: case Qt::Key_Enter: case Qt::Key_Space:
        emit featureSelected(m_focusIdx + 1); break;
    default:
        QWidget::keyPressEvent(e);
    }
}

void DashboardView::wheelEvent(QWheelEvent *e)
{
    // Identik Python: cek horizontal dulu (trackpad), lalu vertical
    int delta = e->angleDelta().x();
    if (delta == 0) delta = e->angleDelta().y();
    if (delta > 20) scrollLeft();
    else if (delta < -20) scrollRight();
}
