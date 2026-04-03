/*
 * PythagorasPage.h — Halaman Teorema Pythagoras
 * Padanan Python: class PythagorasPage (baris 3503-3647 di main.py)
 * IDENTIK PYTHON — 3 input (a,b,c) kosongkan 1, badges, triangle di panel
 */
#ifndef PYTHAGORASPAGE_H
#define PYTHAGORASPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QTimer>

class MainWindow;

class PythagorasPage : public QWidget
{
    Q_OBJECT
public:
    explicit PythagorasPage(MainWindow *parent);
    void updateStyles();

private slots:
    void calculate();

private:
    void reset();
    void updateBadges();
    MainWindow *m_mw;

    QLineEdit *m_sisiA, *m_sisiB, *m_sisiC;
    QLabel *m_badgeA, *m_badgeB, *m_badgeC;
    QLabel *m_result;

    // Triangle diagram (identik Python TriangleDiagram)
    class TriangleDiagram : public QWidget {
    public:
        explicit TriangleDiagram(QWidget *parent = nullptr);
        void setSides(double a = -1, double b = -1, double c = -1,
                      const QString &missing = "");
        bool isDark = true;
    protected:
        void paintEvent(QPaintEvent *e) override;
    private:
        double m_a = -1, m_b = -1, m_c = -1;
        QString m_missing;
    };

    TriangleDiagram *m_triangle;
};

#endif // PYTHAGORASPAGE_H
