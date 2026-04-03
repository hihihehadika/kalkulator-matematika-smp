/*
 * FeatureCard.h — Kartu Fitur Dashboard
 * Padanan Python: class FeatureCardWidget (baris 653-698 di main.py)
 */
#ifndef FEATURECARD_H
#define FEATURECARD_H

#include <QFrame>
#include <QLabel>
#include <QString>

class FeatureCardWidget : public QFrame
{
    Q_OBJECT
public:
    static constexpr int CW = 205;
    static constexpr int CH = 295;
    static constexpr int RADIUS = 16;

    FeatureCardWidget(const QString &iconKey, const QString &title,
                      const QString &desc, QWidget *parent = nullptr);

    void setHoverStyle(bool hovered);

    QString iconKey() const { return m_iconKey; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_iconKey;
    bool m_hovered = false;
    QLabel *m_titleLbl;
    QLabel *m_descLbl;
};

#endif // FEATURECARD_H
