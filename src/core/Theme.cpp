/*
 * Theme.cpp — Implementasi Design Tokens
 * Padanan Python: class T (baris 44-87 di main.py)
 */
#include "core/Theme.h"

// ╔══════════════════════════════════════════════════════╗
// ║           Definisi Warna QColor Statis              ║
// ╚══════════════════════════════════════════════════════╝

const QColor T::GOLD       = QColor(0xC9, 0xA8, 0x4C);
const QColor T::GOLD_PALE  = QColor(0xD4, 0xB9, 0x6A);
const QColor T::AMBER      = QColor(0xFF, 0xA7, 0x26);
const QColor T::TEAL       = QColor(0x49, 0x5E, 0x57);
const QColor T::TEAL_LIGHT = QColor(0x6B, 0x8F, 0x85);
const QColor T::BG_DEEP    = QColor(0x0D, 0x0D, 0x0F);
const QColor T::BG_PANEL   = QColor(0x16, 0x16, 0x1A);
const QColor T::BG_CARD    = QColor(0x1E, 0x1E, 0x24);
const QColor T::BG_INPUT   = QColor(0x25, 0x25, 0x2D);
const QColor T::BORDER     = QColor(0x30, 0x30, 0x3C);
const QColor T::BORDER_LIT = QColor(0xC9, 0xA8, 0x4C, 120);
const QColor T::TEXT_HI    = QColor(0xF2, 0xF2, 0xF5);
const QColor T::TEXT_MID   = QColor(0xA0, 0xA0, 0xB8);
const QColor T::TEXT_DIM   = QColor(0x60, 0x60, 0x78);
const QColor T::SUCCESS    = QColor(0x22, 0xC5, 0x5E);
const QColor T::SUCCESS_BG = QColor(0x0F, 0x2A, 0x1C);
const QColor T::ERROR      = QColor(0xEF, 0x44, 0x44);
const QColor T::ERROR_BG   = QColor(0x2A, 0x0F, 0x0F);

// ╔══════════════════════════════════════════════════════╗
// ║        Fungsi css() — Light / Dark mode             ║
// ╚══════════════════════════════════════════════════════╝

ThemeColors T::css(bool isDark)
{
    ThemeColors t;
    if (isDark) {
        t.bg         = QStringLiteral("#0D0D0F");
        t.panel      = QStringLiteral("#16161A");
        t.card       = QStringLiteral("#1E1E24");
        t.input      = QStringLiteral("#25252D");
        t.border     = QStringLiteral("#30303C");
        t.border_lit = QStringLiteral("rgba(244,206,20,0.47)");
        t.text       = QStringLiteral("#F2F2F5");
        t.text_mid   = QStringLiteral("#A0A0B8");
        t.text_dim   = QStringLiteral("#606078");
        t.primary    = QStringLiteral("#C9A84C");
        t.primary_h  = QStringLiteral("#D4B96A");
        t.success    = QStringLiteral("#22C55E");
        t.success_bg = QStringLiteral("#0F2A1C");
        t.error      = QStringLiteral("#EF4444");
        t.error_bg   = QStringLiteral("#2A0F0F");
        t.accent     = QStringLiteral("#6B8F85");
        t.gold_glow  = QStringLiteral("rgba(244,206,20,0.18)");
    } else {
        t.bg         = QStringLiteral("#F0F2F5");
        t.panel      = QStringLiteral("#FFFFFF");
        t.card       = QStringLiteral("#FFFFFF");
        t.input      = QStringLiteral("#F5F7FA");
        t.border     = QStringLiteral("#DDE1EA");
        t.border_lit = QStringLiteral("rgba(244,206,20,0.70)");
        t.text       = QStringLiteral("#1A1A2E");
        t.text_mid   = QStringLiteral("#4A4A6A");
        t.text_dim   = QStringLiteral("#8888AA");
        t.primary    = QStringLiteral("#C9A84C");
        t.primary_h  = QStringLiteral("#D4B96A");
        t.success    = QStringLiteral("#16A34A");
        t.success_bg = QStringLiteral("#D4F4E2");
        t.error      = QStringLiteral("#DC2626");
        t.error_bg   = QStringLiteral("#FAD7D7");
        t.accent     = QStringLiteral("#495E57");
        t.gold_glow  = QStringLiteral("rgba(244,206,20,0.12)");
    }
    return t;
}
