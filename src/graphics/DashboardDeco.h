/*
 * DashboardDeco.h — Dekorasi Simbol & Lingkaran Dashboard
 * Padanan Python: class DashboardDecoItem (baris 705-793 di main.py)
 */
#ifndef DASHBOARDDECO_H
#define DASHBOARDDECO_H

#include <QGraphicsItem>
#include <QPainter>
#include <QFont>
#include <QVector>
#include <QMap>
#include <random>
#include <cmath>

// ╔══════════════════════════════════════════════════════╗
// ║       DashboardDecoItem — Dekorasi Dashboard        ║
// ╚══════════════════════════════════════════════════════╝

class DashboardDecoItem : public QGraphicsItem
{
public:
    DashboardDecoItem();

    void setLayout(double W, double H, double cardCx, double cardCy);
    void tick(double t);

    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

private:
    QFont getFont(int size);

    double m_t = 0.0;
    double m_W = 1.0, m_H = 1.0;
    double m_cardCx = 0.0, m_cardCy = 0.0;

    // Data simbol
    struct SymbolData {
        QString sym;
        double xf, yf, phase, spd, amp;
        int alpha, size;
    };
    QVector<SymbolData> m_symData;

    // Data partikel (titik kecil)
    struct ParticleData {
        double xf, yf, phase, spd, ampX, ampY;
        int alpha;
        double size;
    };
    QVector<ParticleData> m_particles;

    QMap<int, QFont> m_fontCache;
};

#endif // DASHBOARDDECO_H
