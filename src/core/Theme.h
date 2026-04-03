/*
 * Matematika SMP — By: Dika (C++ Version)
 * Theme.h — Design Tokens & Warna Aplikasi
 *
 * Padanan Python: class T (baris 44-87 di main.py)
 */
#ifndef THEME_H
#define THEME_H

#include <QColor>
#include <QString>

// ╔══════════════════════════════════════════════════════╗
// ║       ThemeColors — CSS-like string values           ║
// ╚══════════════════════════════════════════════════════╝

struct ThemeColors {
    QString bg;
    QString panel;
    QString card;
    QString input;
    QString border;
    QString border_lit;
    QString text;
    QString text_mid;
    QString text_dim;
    QString primary;
    QString primary_h;
    QString success;
    QString success_bg;
    QString error;
    QString error_bg;
    QString accent;
    QString gold_glow;
};

// ╔══════════════════════════════════════════════════════╗
// ║           T — Design Token utama                    ║
// ╚══════════════════════════════════════════════════════╝

class T {
public:
    // Warna QColor statis (digunakan untuk QPainter langsung)
    static const QColor GOLD;
    static const QColor GOLD_PALE;
    static const QColor AMBER;
    static const QColor TEAL;
    static const QColor TEAL_LIGHT;
    static const QColor BG_DEEP;
    static const QColor BG_PANEL;
    static const QColor BG_CARD;
    static const QColor BG_INPUT;
    static const QColor BORDER;
    static const QColor BORDER_LIT;
    static const QColor TEXT_HI;
    static const QColor TEXT_MID;
    static const QColor TEXT_DIM;
    static const QColor SUCCESS;
    static const QColor SUCCESS_BG;
    static const QColor ERROR;
    static const QColor ERROR_BG;

    // Ambil tema CSS berdasarkan mode dark/light
    static ThemeColors css(bool isDark = true);
};

#endif // THEME_H
