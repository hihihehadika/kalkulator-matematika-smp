/*
 * PageHelpers.cpp — Implementasi Fungsi Pembuat UI Bersama
 * Padanan Python: baris 1234-1342 di main.py
 * IDENTIK PYTHON — menggunakan objectName dan stylesheet yang sama
 */
#include "helpers/PageHelpers.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QSizePolicy>

QPushButton* makeBackBtn()
{
    auto *btn = new QPushButton("Kembali");
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFixedHeight(36);
    btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btn->setFocusPolicy(Qt::NoFocus);
    return btn;
}

QWidget* sectionHeader(const QString &title, const QString &subtitle)
{
    auto *w = new QWidget();
    w->setAttribute(Qt::WA_TranslucentBackground);
    auto *lay = new QVBoxLayout(w);
    lay->setContentsMargins(0, 0, 0, 8);
    lay->setSpacing(4);

    auto *t = new QLabel(title);
    t->setObjectName("pageTitle");
    lay->addWidget(t);

    if (!subtitle.isEmpty()) {
        auto *s = new QLabel(subtitle);
        s->setObjectName("pageSubtitle");
        lay->addWidget(s);
    }

    return w;
}

QPair<QWidget*, QLineEdit*> inputBox(const QString &label,
                                      const QString &placeholder)
{
    auto *container = new QWidget();
    auto *lay = new QVBoxLayout(container);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(6);

    auto *lbl = new QLabel(label);
    lbl->setObjectName("inputLabel");
    lay->addWidget(lbl);

    auto *input = new QLineEdit();
    input->setObjectName("calcInput");
    input->setPlaceholderText(placeholder);
    input->setMaxLength(100);
    lay->addWidget(input);

    return {container, input};
}

QPushButton* calcBtn(const QString &text)
{
    auto *btn = new QPushButton(text);
    btn->setObjectName("calcBtn");
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFixedHeight(50);
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    btn->setFocusPolicy(Qt::NoFocus);

    // Gold shadow (identik Python)
    auto *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(18);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0xC9, 0xA8, 0x4C, 80));
    btn->setGraphicsEffect(shadow);

    return btn;
}

// ── Stylesheet identik Python _page_stylesheet() (baris 1266-1342) ──
QString pageStylesheet(const ThemeColors &t)
{
    return QString(
        // Base
        "QWidget{background:%1;color:%2;font-family:'Segoe UI',Arial;}"

        // Panel
        "QFrame#panel{background:%3;border:1px solid %4;border-radius:16px;}"

        // Page title
        "QLabel#pageTitle{font-size:28px;font-weight:800;color:%2;background:transparent;letter-spacing:0.3px;}"

        // Page subtitle
        "QLabel#pageSubtitle{font-size:13px;color:%5;letter-spacing:2px;background:transparent;text-transform:uppercase;}"

        // Hint label
        "QLabel#hintLabel{font-size:14px;color:%6;background:transparent;line-height:1.7;}"

        // Input label
        "QLabel#inputLabel{font-size:12px;font-weight:700;color:%6;background:transparent;text-transform:uppercase;letter-spacing:1px;}"

        // Result label
        "QLabel#resultLabel{padding:24px 28px;background:%3;border:1px solid %4;border-radius:12px;color:%6;font-size:14px;line-height:1.9;}"

        // Input field
        "QLineEdit#calcInput{"
        "  background:%7;"
        "  border:1.5px solid %4;"
        "  border-radius:8px;"
        "  padding:0px 14px;"
        "  color:%2;"
        "  font-size:15px;"
        "  font-weight:600;"
        "  min-height:36px;max-height:36px;height:36px;"
        "}"
        "QLineEdit#calcInput:focus{border:2px solid %5;background:%3;}"

        // ComboBox
        "QComboBox{background:%7;border:1.5px solid %4;border-radius:8px;"
        "padding:0px 14px;color:%2;font-size:14px;min-height:36px;max-height:36px;}"
        "QComboBox:focus{border:2px solid %5;}"
        "QComboBox::drop-down{border:none;width:28px;}"
        "QComboBox QAbstractItemView{background:%3;color:%2;selection-background-color:%5;outline:0;}"

        // Generic button
        "QPushButton{background:%3;border:1.5px solid %4;border-radius:10px;"
        "padding:0 16px;color:%6;font-weight:600;font-size:13px;height:36px;}"
        "QPushButton:hover{background:%7;color:%2;}"

        // Calc button (gold gradient)
        "QPushButton#calcBtn{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
        "stop:0 %5,stop:1 %8);border:none;color:#1A1A1A;"
        "font-size:16px;font-weight:800;border-radius:14px;min-height:50px;max-height:50px;}"
        "QPushButton#calcBtn:hover{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
        "stop:0 %8,stop:1 %5);}"

        // Scrollbar
        "QScrollArea{background:%1;border:none;}"
        "QScrollBar:vertical{background:%1;width:8px;border-radius:4px;}"
        "QScrollBar::handle:vertical{background:%4;border-radius:4px;min-height:20px;}"
        "QScrollBar::handle:vertical:hover{background:%5;}"
        "QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical{height:0;}"

        // SPLDV specific
        "QLabel#spldvStep{font-size:11px;font-weight:700;color:%5;letter-spacing:2px;background:transparent;padding:0;}"
        "QLabel#spldvHint{font-size:13px;color:%6;background:transparent;line-height:1.5;}"
        "QLabel#spldvEqNum{font-size:15px;font-weight:700;color:%5;background:transparent;min-width:52px;}"
        "QLabel#spldvVar{font-size:13px;color:%6;background:transparent;}"
        "QLabel#spldvOp{font-size:20px;font-weight:300;color:%6;background:transparent;padding:0 2px;}"
        "QLineEdit#spldvInput{"
        "  background:%7;border:1.5px solid %4;border-radius:8px;"
        "  padding:0px 14px;color:%2;font-size:15px;font-weight:600;"
        "  min-height:36px;max-height:36px;height:36px;"
        "}"
        "QLineEdit#spldvInput:focus{border:2px solid %5;background:%3;}"
        "QLineEdit#spldvRevInput{"
        "  background:transparent;border:1.5px dashed %4;border-radius:8px;"
        "  padding:0px 14px;color:%6;font-size:14px;"
        "  min-height:36px;max-height:36px;height:36px;"
        "}"
        "QLineEdit#spldvRevInput:focus{border:2px solid %5;color:%2;}"
        "QLineEdit#spldvNameInput{"
        "  background:%7;border:1.5px solid %4;border-radius:8px;"
        "  padding:0px 14px;color:%2;font-size:14px;"
        "  min-height:36px;max-height:36px;height:36px;"
        "}"
        "QLineEdit#spldvNameInput:focus{border:2px solid %5;}"
    ).arg(t.bg, t.text, t.card, t.border,
          t.primary, t.text_mid, t.input, t.primary_h);
}
