/*
 * FloatBtn.h — Tombol Melayang (Tema, Shortcut)
 * Padanan Python: class FloatBtn (baris 3968-4006 di main.py)
 */
#ifndef FLOATBTN_H
#define FLOATBTN_H

#include <QPushButton>

class FloatBtn : public QPushButton
{
    Q_OBJECT
public:
    // btnType: "shortcuts" atau "theme"
    explicit FloatBtn(const QString &btnType, QWidget *parent = nullptr);

    void setDark(bool dark);

protected:
    void enterEvent(QEnterEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void refresh();
    QString m_btnType;
    bool m_isDark = true;
    bool m_hovered = false;
};

#endif // FLOATBTN_H
