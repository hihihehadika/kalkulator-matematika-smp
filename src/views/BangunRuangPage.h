/*
 * BangunRuangPage.h — Halaman Bangun Ruang
 * Padanan Python: class BangunRuangPage (baris 1979-2441 di main.py)
 * IDENTIK PYTHON — 8 bangun, dynamic fields, Volume reverse solving
 */
#ifndef BANGUNRUANGPAGE_H
#define BANGUNRUANGPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>
#include <QVector>
#include <tuple>

class MainWindow;
class ShapeIllustration;

class BangunRuangPage : public QWidget
{
    Q_OBJECT
public:
    explicit BangunRuangPage(MainWindow *parent);
    void updateStyles();

private slots:
    void calculate();
    void updateForm(const QString &shape);

private:
    void reset();
    void clearDynamic();
    MainWindow *m_mw;

    QComboBox *m_combo;
    QLabel *m_hintLbl;
    QWidget *m_dynamicContainer;
    QVBoxLayout *m_dynamicLayout;
    QLabel *m_result;
    ShapeIllustration *m_sim;

    // Dynamic field storage: (widget, lineEdit, key)
    struct DimBox { QWidget *widget; QLineEdit *input; QString key; };
    QVector<DimBox> m_dimBoxes;
    // Volume reverse-solving box
    QLineEdit *m_volInput = nullptr;
};

#endif // BANGUNRUANGPAGE_H
