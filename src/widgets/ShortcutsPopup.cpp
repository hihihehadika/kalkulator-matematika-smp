/*
 * ShortcutsPopup.cpp — Implementasi Popup Pintasan
 * Padanan Python: class ShortcutsPopup (baris 3870-3965 di main.py)
 */
#include "widgets/ShortcutsPopup.h"
#include <QVBoxLayout>

ShortcutsPopup::ShortcutsPopup(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(8, 8, 8, 8);

    m_card = new QFrame();
    m_card->setObjectName("spCard");
    auto *clay = new QVBoxLayout(m_card);
    clay->setContentsMargins(18, 14, 18, 14);
    clay->setSpacing(8);

    m_title = new QLabel("Pintasan Keyboard");
    m_body = new QLabel();
    m_body->setTextFormat(Qt::RichText);
    m_body->setWordWrap(true);
    m_body->setMinimumWidth(280);

    clay->addWidget(m_title);
    clay->addWidget(m_body);
    lay->addWidget(m_card);

    setFixedWidth(340);
    hide();
    updateTheme(true);
}

void ShortcutsPopup::updateTheme(bool isDark)
{
    QString bg, bo, tc, tx, hc;
    if (isDark) {
        bg = "rgba(20,20,26,0.97)";
        bo = "#C9A84C"; tc = "#C9A84C";
        tx = "#D0D0E0"; hc = "#FFC107";
    } else {
        bg = "rgba(255,255,255,0.97)";
        bo = "#C9A84C"; tc = "#1A1A2E";
        tx = "#4A4A6A"; hc = "#B8960A";
    }

    m_card->setStyleSheet(QString(
        "QFrame#spCard{"
        "background:%1;"
        "border:2px solid %2;"
        "border-radius:12px;"
        "}").arg(bg, bo));

    m_title->setStyleSheet(QString(
        "font-size:14px;font-weight:700;color:%1;").arg(tc));

    m_body->setStyleSheet(QString(
        "font-size:11.5px;color:%1;line-height:1.6;").arg(tx));

    m_body->setText(QString(
        "<b style='color:%1'>Navigasi Carousel:</b><br>"
        "Kiri/Kanan, A/D, Scroll Mouse, Tombol panah<br>"
        "Spasi atau Enter untuk masuk kalkulator<br><br>"
        "<b style='color:%1'>Halaman:</b><br>"
        "Ctrl+1 Dasbor  |  Ctrl+2 s/d Ctrl+8 Fitur<br>"
        "Esc Kembali  |  Ctrl+R Atur ulang semua input<br><br>"
        "<b style='color:%1'>Lainnya:</b><br>"
        "F11 Layar penuh  |  Enter di kolom input untuk menghitung"
    ).arg(hc));

    adjustSize();
}

void ShortcutsPopup::showNear(QPushButton *btn)
{
    adjustSize();
    QPoint gp = btn->mapToGlobal(QPoint(0, 0));
    int px = gp.x() - (width() - btn->width()) / 2;
    int py = gp.y() - height() - 12;
    if (py < 10) py = gp.y() + btn->height() + 12;
    if (px < 10) px = 10;
    move(px, py);
    show();
    raise();
}
