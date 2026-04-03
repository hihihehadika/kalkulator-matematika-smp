/*
 * PageHelpers.h — Fungsi Pembuat UI Bersama
 * Padanan Python: _make_back_btn, _section_header, _input_box,
 *   _calc_btn, _page_stylesheet (baris 1234-1342 di main.py)
 */
#ifndef PAGEHELPERS_H
#define PAGEHELPERS_H

#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QFrame>
#include <QWidget>
#include <QString>
#include <QPair>
#include "core/Theme.h"

// Tombol "← Kembali ke Dashboard"
QPushButton* makeBackBtn();

// Header halaman (judul + subjudul)
QWidget* sectionHeader(const QString &title, const QString &subtitle);

// Kotak input (label + QLineEdit), return {container, lineEdit}
QPair<QWidget*, QLineEdit*> inputBox(const QString &label,
                                      const QString &placeholder = "");

// Tombol "Hitung ..."
QPushButton* calcBtn(const QString &text = "Hitung");

// Stylesheet untuk seluruh halaman (panel, input, button, dll)
QString pageStylesheet(const ThemeColors &t);

#endif // PAGEHELPERS_H
