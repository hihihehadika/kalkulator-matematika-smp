/*
 * ShortcutsPopup.h — Popup Pintasan Keyboard
 * Padanan Python: class ShortcutsPopup (baris 3870-3965 di main.py)
 */
#ifndef SHORTCUTSPOPUP_H
#define SHORTCUTSPOPUP_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QPoint>

class ShortcutsPopup : public QWidget
{
    Q_OBJECT
public:
    explicit ShortcutsPopup(QWidget *parent = nullptr);

    void updateTheme(bool isDark);
    void showNear(QPushButton *btn);

private:
    QFrame *m_card;
    QLabel *m_title;
    QLabel *m_body;
};

#endif // SHORTCUTSPOPUP_H
