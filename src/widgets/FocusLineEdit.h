/*
 * FocusLineEdit.h — LineEdit dengan Efek Fokus
 * Padanan Python: class FocusLineEdit (baris 1206-1231 di main.py)
 */
#ifndef FOCUSLINEEDIT_H
#define FOCUSLINEEDIT_H

#include <QLineEdit>

class FocusLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit FocusLineEdit(QWidget *parent = nullptr);

protected:
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;
};

#endif // FOCUSLINEEDIT_H
