/*
 * GarisLurusPage.h — Halaman Persamaan Garis Lurus
 * Padanan Python: class GarisLurusPage (baris 1429-1556 di main.py)
 * IDENTIK PYTHON — 4 input (m, c, x, y) + reverse solving + preview
 */
#ifndef GARISLURUSPAGE_H
#define GARISLURUSPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QtCharts/QChartView>

class MainWindow;

class GarisLurusPage : public QWidget
{
    Q_OBJECT
public:
    explicit GarisLurusPage(MainWindow *parent);
    void updateStyles();

private slots:
    void calculate();

private:
    void reset();
    void updatePreview();
    MainWindow *m_mw;

    // 4 input identik Python: m, c, x, y
    QLineEdit *m_mIn, *m_cIn, *m_xIn, *m_yIn;

    QLabel *m_previewLbl;
    QLabel *m_result;
    QChartView *m_chartView;
};

#endif // GARISLURUSPAGE_H
