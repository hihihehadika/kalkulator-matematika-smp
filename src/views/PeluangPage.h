/*
 * PeluangPage.h — Halaman Peluang Kejadian
 * Padanan Python: class PeluangPage (baris 2519-2636 di main.py)
 * IDENTIK PYTHON — 3 input (n(A), n(S), P(A)) + reverse solving
 */
#ifndef PELUANGPAGE_H
#define PELUANGPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>

class MainWindow;
class ProbabilityPieChart;
class ProbabilityBar;

class PeluangPage : public QWidget
{
    Q_OBJECT
public:
    explicit PeluangPage(MainWindow *parent);
    void updateStyles();
private slots:
    void calculate();
private:
    void reset();
    static QString interpret(double pct);
    MainWindow *m_mw;
    // 3 input identik Python: n(A), n(S), P(A)
    QLineEdit *m_na, *m_ns, *m_pa;
    QLabel *m_result;
    ProbabilityPieChart *m_pie;
    ProbabilityBar *m_bar;
};

#endif // PELUANGPAGE_H
