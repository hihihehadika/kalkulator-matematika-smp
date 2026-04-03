/*
 * CalcBasicPage.h — Halaman Kalkulator Basic + Fraksi
 * Padanan Python: class KalkulatorBasicPage (baris 3774-3866 di main.py)
 * IDENTIK PYTHON — operator buttons, vertical layout, history terpisah
 */
#ifndef CALCBASICPAGE_H
#define CALCBASICPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QString>
#include <QVector>

class MainWindow;
class QJSEngine;

class CalcBasicPage : public QWidget
{
    Q_OBJECT
public:
    explicit CalcBasicPage(MainWindow *parent);
    ~CalcBasicPage();
    void updateStyles();

private slots:
    void calculate();

private:
    void reset();
    void insertOp(const QString &op);
    QString solveDirectEval(const QString &expr);
    QString solveFractionFallback(const QString &expr);
    
    MainWindow *m_mw;

    QLineEdit *m_exprIn;
    QLabel *m_result;
    QLabel *m_histResult;
    
    QVector<QString> m_histList;
    QJSEngine *m_js;
};

#endif // CALCBASICPAGE_H
