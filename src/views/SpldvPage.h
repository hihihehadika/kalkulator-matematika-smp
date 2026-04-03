/*
 * SpldvPage.h — Halaman Sistem Persamaan Linear (N-Variabel)
 * Padanan Python: class SPLDVPage (baris 2643-3445 di main.py)
 * IDENTIK PYTHON — Mode Koefisien + Mode Soal Harga + Reverse Solving + Step-by-step
 */
#ifndef SPLDVPAGE_H
#define SPLDVPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVector>
#include <QString>
#include <QMap>

class MainWindow;

class SpldvPage : public QWidget
{
    Q_OBJECT
public:
    explicit SpldvPage(MainWindow *parent);
    void updateStyles();

    static constexpr int MIN_VARS = 2;
    static constexpr int MAX_VARS = 3;

private slots:
    void solveKoef();
    void solveHarga();

private:
    void reset();
    void switchMode(const QString &mode);
    void changeVars(int delta, const QString &panel);
    void rebuildKoefGrid();
    void rebuildHargaGrid();
    void updateTabStyles();
    void onCurrChanged(int idx);

    QStringList varNames() const;
    QString getCurrencyPrefix() const;
    QString fmtCurrency(double v, const QString &prefix) const;

    // Gaussian Elimination identik Python
    struct GaussResult {
        QVector<double> x;
        QStringList steps;
    };
    static GaussResult gaussianElimination(QVector<QVector<double>> A, QVector<double> b);
    static QString matrixToHtml(const QVector<QVector<double>> &M);

    struct ApplyResult {
        QVector<QVector<double>> A_red;
        QVector<double> b_red;
        QVector<int> unknownIdx;
    };
    static ApplyResult applyKnownValues(const QVector<QVector<double>> &A,
                                         const QVector<double> &b,
                                         const QMap<int, double> &known);

    MainWindow *m_mw;
    QString m_mode = "koef";

    // Mode Koefisien
    int m_nKoef = 2;
    QFrame *m_panelKoef = nullptr;
    QWidget *m_koefGridContainer = nullptr;
    QVector<QVector<QLineEdit*>> m_koefInputs;
    QVector<QLineEdit*> m_koefKnown;
    QLabel *m_lblNKoef = nullptr;
    QPushButton *m_btnKoef = nullptr;

    // Mode Soal Harga
    int m_nHarga = 2;
    QFrame *m_panelHarga = nullptr;
    QWidget *m_hargaGridContainer = nullptr;
    QVector<QLineEdit*> m_namaInputs;
    QVector<QVector<QLineEdit*>> m_hargaKoefInputs;
    QVector<QLineEdit*> m_hargaKnownInputs;
    QLabel *m_lblNHarga = nullptr;
    QPushButton *m_btnHarga = nullptr;
    QComboBox *m_currCombo = nullptr;
    QLineEdit *m_currCustom = nullptr;

    QPushButton *m_calcBtnKoef = nullptr;
    QPushButton *m_calcBtnHarga = nullptr;
    QLabel *m_result = nullptr;
};

#endif // SPLDVPAGE_H
