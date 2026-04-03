/*
 * ProbabilityWidgets.h — Pie Chart & Bar Peluang
 * Padanan Python: ProbabilityPieChart + ProbabilityBar (baris 2447-2517)
 */
#ifndef PROBABILITYWIDGETS_H
#define PROBABILITYWIDGETS_H

#include <QWidget>

class ProbabilityPieChart : public QWidget
{
    Q_OBJECT
public:
    explicit ProbabilityPieChart(QWidget *parent = nullptr);
    void setValues(double favorable, double total);
    bool isDark = true;
protected:
    void paintEvent(QPaintEvent *e) override;
private:
    double m_favorable = 0, m_total = 1;
};

class ProbabilityBar : public QWidget
{
    Q_OBJECT
public:
    explicit ProbabilityBar(QWidget *parent = nullptr);
    void setValue(double ratio);
    bool isDark = true;
protected:
    void paintEvent(QPaintEvent *e) override;
private:
    double m_ratio = 0;
};

#endif // PROBABILITYWIDGETS_H
