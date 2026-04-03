/*
 * StatistikaPage.h — Halaman Statistika Dasar
 * Padanan Python: class StatistikaPage (baris 3654-3768 di main.py)
 * IDENTIK PYTHON — count label, known_mean, ? reverse solving
 */
#ifndef STATISTIKAPAGE_H
#define STATISTIKAPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>

class MainWindow;

class StatistikaPage : public QWidget
{
    Q_OBJECT
public:
    explicit StatistikaPage(MainWindow *parent);
    void updateStyles();

private slots:
    void calculate();

private:
    void reset();
    void updateCount();
    MainWindow *m_mw;

    QLineEdit *m_data;
    QLineEdit *m_knownMean;
    QLabel *m_countLbl;
    QLabel *m_result;
};

#endif // STATISTIKAPAGE_H
