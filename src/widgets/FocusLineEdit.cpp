/*
 * FocusLineEdit.cpp — Implementasi LineEdit Fokus
 * Padanan Python: class FocusLineEdit (baris 1206-1231 di main.py)
 */
#include "widgets/FocusLineEdit.h"
#include <QFocusEvent>

FocusLineEdit::FocusLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
}

void FocusLineEdit::focusInEvent(QFocusEvent *e)
{
    QLineEdit::focusInEvent(e);
    // Efek fokus ditangani oleh QSS :focus di pageStylesheet
}

void FocusLineEdit::focusOutEvent(QFocusEvent *e)
{
    QLineEdit::focusOutEvent(e);
}
