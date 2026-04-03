/*
 * FeatureCard.cpp — Implementasi Kartu Fitur
 * Padanan Python: class FeatureCardWidget (baris 653-698 di main.py)
 *
 * PENTING: Warna kartu SELALU krem (#FAF8EE) di semua mode tema!
 * Teks judul SELALU hitam #2A2A2A, deskripsi SELALU #777.
 * Ini identik persis dengan versi Python.
 */
#include "widgets/FeatureCard.h"
#include "widgets/IconPainters.h"

#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>
#include <QLinearGradient>

FeatureCardWidget::FeatureCardWidget(const QString &iconKey,
                                     const QString &title,
                                     const QString &desc,
                                     QWidget *parent)
    : QFrame(parent), m_iconKey(iconKey)
{
    setFixedSize(CW, CH);
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);

    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(16, 20, 16, 18);
    lay->setSpacing(8);

    // Spacer untuk area ikon
    auto *spacer = new QWidget();
    spacer->setFixedHeight(88);
    spacer->setAttribute(Qt::WA_TransparentForMouseEvents);
    spacer->setStyleSheet("background:transparent;");
    lay->addWidget(spacer);
    lay->addStretch(1);

    // Judul — SELALU hitam (identik Python)
    m_titleLbl = new QLabel(title);
    m_titleLbl->setStyleSheet(
        "font-size:14px;font-weight:700;background:transparent;"
        "color:#2A2A2A;border:none;padding:0;font-family:'Segoe UI',Arial;");
    m_titleLbl->setAlignment(Qt::AlignCenter);
    m_titleLbl->setWordWrap(true);
    lay->addWidget(m_titleLbl);

    // Deskripsi — SELALU abu (identik Python)
    m_descLbl = new QLabel(desc);
    m_descLbl->setStyleSheet(
        "font-size:10px;background:transparent;"
        "color:#777;border:none;padding:0;font-family:'Segoe UI',Arial;");
    m_descLbl->setAlignment(Qt::AlignCenter);
    m_descLbl->setWordWrap(true);
    lay->addWidget(m_descLbl);

    setStyleSheet("QFrame{background:transparent;border:none;}");
}

void FeatureCardWidget::setHoverStyle(bool hovered)
{
    m_hovered = hovered;
    setStyleSheet("QFrame{background:transparent;border:none;}");
    update();
}

void FeatureCardWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int W = CW, H = CH, R = RADIUS;

    // Clip rounded rect
    QPainterPath clip;
    clip.addRoundedRect(0, 0, W, H, R, R);
    p.setClipPath(clip);

    // Background — SELALU KREM (identik Python, tidak ikut theme)
    QColor bgCol = m_hovered ? QColor("#FDFBF2") : QColor("#FAF8EE");
    p.fillRect(0, 0, W, H, bgCol);

    // Inner gradient overlay
    QLinearGradient inner(0, 0, 0, H);
    inner.setColorAt(0.0, QColor(255, 255, 245, 90));
    inner.setColorAt(0.40, QColor(255, 255, 240, 0));
    p.setBrush(inner);
    p.setPen(Qt::NoPen);
    p.drawRect(0, 0, W, H);

    // Icon
    p.setClipping(false);
    p.setClipPath(clip);
    drawIcon(&p, m_iconKey, W / 2, 20 + 48, 72);

    // Border
    double bw = m_hovered ? 3.5 : 2.5;
    QColor borderCol = m_hovered
        ? QColor(0xC9, 0xA8, 0x4C, 210)
        : QColor(0xC9, 0xA8, 0x4C, 165);
    p.setClipping(false);
    QPen pen(borderCol, bw);
    pen.setJoinStyle(Qt::RoundJoin);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    double inset = bw / 2;
    p.drawRoundedRect(QRectF(inset, inset, W - bw, H - bw), R, R);
}
