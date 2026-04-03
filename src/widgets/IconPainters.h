/*
 * IconPainters.h — Fungsi Painter Ikon Dashboard
 * Padanan Python: _draw_icon_* functions (baris 521-646 di main.py)
 */
#ifndef ICONPAINTERS_H
#define ICONPAINTERS_H

#include <QPainter>
#include <QString>

// Konstanta key ikon
inline const QString ICON_LINECHART = "linechart";
inline const QString ICON_BOX3D     = "box3d";
inline const QString ICON_DICE      = "dice";
inline const QString ICON_EQUATION  = "equation";
inline const QString ICON_TRIANGLE  = "triangle";
inline const QString ICON_CHART     = "chart";
inline const QString ICON_CALC      = "calc";

// Fungsi gambar ikon
void drawIconLinechart(QPainter *p, int cx, int cy, int size);
void drawIconBox3d(QPainter *p, int cx, int cy, int size);
void drawIconDice(QPainter *p, int cx, int cy, int size);
void drawIconEquation(QPainter *p, int cx, int cy, int size);
void drawIconTriangle(QPainter *p, int cx, int cy, int size);
void drawIconChart(QPainter *p, int cx, int cy, int size);
void drawIconCalc(QPainter *p, int cx, int cy, int size);

// Dispatch: gambar ikon berdasarkan key
void drawIcon(QPainter *p, const QString &key, int cx, int cy, int size);

#endif // ICONPAINTERS_H
