/*
 * ResultRenderer.h — Renderer Hasil HTML
 * Padanan Python: _html_title, _html_row, _html_div, _html_big,
 *   _result_ok, _result_err, _result_history (baris 1345-1422)
 */
#ifndef RESULTRENDERER_H
#define RESULTRENDERER_H

#include <QLabel>
#include <QString>
#include <QVector>
#include "core/Theme.h"

// HTML building blocks
QString htmlTitle(const QString &text, const ThemeColors &t);
QString htmlRow(const QString &label, const QString &value,
                const ThemeColors &t, bool big = false,
                const QString &color = "");
QString htmlDiv(const ThemeColors &t);
QString htmlBig(const QString &text, const ThemeColors &t);

// Apply result to QLabel
void resultOk(QLabel *label, const QString &html, const ThemeColors &t);
void resultErr(QLabel *label, const QString &msg, const ThemeColors &t);
void resultHistory(QLabel *label, const QVector<QString> &history,
                   const ThemeColors &t);

#endif // RESULTRENDERER_H
