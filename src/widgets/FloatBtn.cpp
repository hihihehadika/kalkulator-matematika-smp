/*
 * FloatBtn.cpp — Implementasi Tombol Melayang
 * Padanan Python: class FloatBtn (baris 3968-4006 di main.py)
 */
#include "widgets/FloatBtn.h"
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QRectF>
#include <cmath>

FloatBtn::FloatBtn(const QString &btnType, QWidget *parent)
    : QPushButton(parent), m_btnType(btnType)
{
    setFixedSize(42, 42);
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::NoFocus);
    refresh();
}

void FloatBtn::setDark(bool dark)
{
    m_isDark = dark;
    refresh();
}

void FloatBtn::refresh()
{
    if (m_isDark) {
        setStyleSheet(
            "QPushButton{background:rgba(13,13,15,0.82);"
            "border:1.5px solid rgba(201,168,76,0.65);border-radius:21px;}"
            "QPushButton:hover{background:rgba(201,168,76,0.14);"
            "border:1.5px solid #C9A84C;}");
    } else {
        setStyleSheet(
            "QPushButton{background:rgba(255,255,255,0.88);"
            "border:1.5px solid rgba(180,140,10,0.55);border-radius:21px;}"
            "QPushButton:hover{background:rgba(244,206,20,0.18);"
            "border:1.5px solid #C4A800;}");
    }
    update();
}

void FloatBtn::enterEvent(QEnterEvent *e)
{
    m_hovered = true; update();
    QPushButton::enterEvent(e);
}

void FloatBtn::leaveEvent(QEvent *e)
{
    m_hovered = false; update();
    QPushButton::leaveEvent(e);
}

void FloatBtn::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QColor col;
    if (m_isDark && m_hovered)       col = QColor(0xD4, 0xB9, 0x6A);
    else if (m_isDark)               col = QColor(0xC9, 0xA8, 0x4C);
    else if (m_hovered)              col = QColor(0x5A, 0x40, 0x00);
    else                             col = QColor(0x8A, 0x6A, 0x00);

    int cx = 21, cy = 21;

    if (m_btnType == "shortcuts") {
        p.setPen(QPen(col, 1.8, Qt::SolidLine, Qt::RoundCap));
        p.setBrush(Qt::NoBrush);
        p.drawRoundedRect(cx - 8, cy - 5, 16, 11, 2, 2);
        for (int i = 0; i < 3; i++) {
            p.drawRoundedRect(int(cx - 5 + i * 3.7), cy - 3, 2, 2, 1, 1);
            p.drawRoundedRect(int(cx - 5 + i * 3.7), cy, 2, 2, 1, 1);
        }
        p.drawRoundedRect(cx - 6, cy + 3, 12, 2, 1, 1);
    }
    else if (m_btnType == "theme") {
        if (m_isDark) {
            // Ikon matahari
            p.setPen(Qt::NoPen); p.setBrush(col);
            p.drawEllipse(cx - 4, cy - 4, 8, 8);
            p.setPen(QPen(col, 2, Qt::SolidLine, Qt::RoundCap));
            for (int angle = 0; angle < 360; angle += 45) {
                double rad = angle * M_PI / 180.0;
                int x1 = cx + int(std::cos(rad) * 7);
                int y1 = cy + int(std::sin(rad) * 7);
                int x2 = cx + int(std::cos(rad) * 10);
                int y2 = cy + int(std::sin(rad) * 10);
                p.drawLine(x1, y1, x2, y2);
            }
        } else {
            // Ikon bulan sabit
            p.setPen(Qt::NoPen); p.setBrush(col);
            QPainterPath path;
            path.addEllipse(QRectF(cx - 7, cy - 7, 14, 14));
            QPainterPath path2;
            path2.addEllipse(QRectF(cx - 6 + 3, cy - 6, 12, 12));
            p.drawPath(path.subtracted(path2));
        }
    }
}
