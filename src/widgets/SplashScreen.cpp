/*
 * SplashScreen.cpp — Implementasi Splash Screen
 * Padanan Python: class SplashScreen (baris 198-250 di main.py)
 *
 * Seluruh piksel, gradient, dan posisi teks direplikasi persis
 * dari versi Python untuk menjamin tampilan identik.
 */
#include "widgets/SplashScreen.h"
#include "core/Theme.h"

#include <QPainter>
#include <QFont>
#include <QRect>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QPen>
#include <QColor>

SplashScreen::SplashScreen()
{
    const int W = 640, H = 380;
    QPixmap pix(W, H);
    pix.fill(Qt::transparent);

    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    // ── Background gradient ──
    QLinearGradient bg(0, 0, W, H);
    bg.setColorAt(0.0, QColor(0x0D, 0x0D, 0x0F));
    bg.setColorAt(0.6, QColor(0x16, 0x16, 0x1A));
    bg.setColorAt(1.0, QColor(0x1E, 0x1E, 0x24));
    p.fillRect(0, 0, W, H, bg);

    // ── Gold orb glow ──
    QRadialGradient orb(W * 0.75, H * 0.25, 180);
    QColor c = T::GOLD;
    c.setAlpha(45);
    orb.setColorAt(0, c);
    c.setAlpha(0);
    orb.setColorAt(1, c);
    p.fillRect(0, 0, W, H, orb);

    // ── Border halus ──
    p.setPen(QPen(QColor(0xC9, 0xA8, 0x4C, 40), 1));
    p.setBrush(Qt::NoBrush);
    p.drawRoundedRect(1, 1, W - 2, H - 2, 16, 16);

    // ── Garis dekoratif ──
    p.setPen(QPen(QColor(0xC9, 0xA8, 0x4C, 160), 2));
    p.drawLine(W / 2 - 80, 58, W / 2 + 80, 58);

    // ── Judul "Matematika SMP" ──
    QFont tf("Segoe UI", 34, QFont::Bold);
    tf.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
    p.setFont(tf);
    p.setPen(T::GOLD);
    p.drawText(QRect(0, 68, W, 60), Qt::AlignCenter, "Matematika SMP");

    // ── Subjudul ──
    QFont sf("Arial", 12);
    sf.setLetterSpacing(QFont::AbsoluteSpacing, 3.0);
    p.setFont(sf);
    p.setPen(QColor(0xFF, 0xFF, 0xFF, 120));
    p.drawText(QRect(0, 132, W, 30), Qt::AlignCenter,
               "K A L K U L A T O R   M A T E M A T I K A");

    // ── Pill "BY DIKA" ──
    int pill_w = 120, pill_h = 28;
    int pill_x = (W - pill_w) / 2, pill_y = 172;
    p.setPen(QPen(QColor(0xC9, 0xA8, 0x4C, 80), 1));
    p.setBrush(QColor(0xC9, 0xA8, 0x4C, 22));
    p.drawRoundedRect(pill_x, pill_y, pill_w, pill_h, 14, 14);

    QFont cf("Arial", 10);
    cf.setLetterSpacing(QFont::AbsoluteSpacing, 2.5);
    p.setFont(cf);
    p.setPen(QColor(0xC9, 0xA8, 0x4C, 200));
    p.drawText(QRect(pill_x, pill_y, pill_w, pill_h),
               Qt::AlignCenter, "B Y  D I K A");

    // ── Progress bar ──
    int bar_w = 280, bar_h = 4;
    int bar_x = (W - bar_w) / 2, bar_y = 230;
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0xFF, 0xFF, 0xFF, 18));
    p.drawRoundedRect(bar_x, bar_y, bar_w, bar_h, 2, 2);

    QLinearGradient grad_bar(bar_x, 0, bar_x + bar_w, 0);
    grad_bar.setColorAt(0, T::GOLD);
    grad_bar.setColorAt(1, T::GOLD_PALE);
    p.setBrush(grad_bar);
    p.drawRoundedRect(bar_x, bar_y, bar_w, bar_h, 2, 2);

    // ── Teks loading ──
    QFont lf("Arial", 10);
    p.setFont(lf);
    p.setPen(QColor(0xFF, 0xFF, 0xFF, 90));
    p.drawText(QRect(0, 244, W, 24), Qt::AlignCenter, "Memuat aplikasi...");

    p.end();

    // Set pixmap dan window flags
    setPixmap(pix);
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
}
