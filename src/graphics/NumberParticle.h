/*
 * NumberParticle.h — Sistem Partikel Angka HD
 * Padanan Python: class NumberParticle + NumberParticleSystem
 *                 (baris 257-394 di main.py)
 */
#ifndef NUMBERPARTICLE_H
#define NUMBERPARTICLE_H

#include <QGraphicsItem>
#include <QPainter>
#include <QFont>
#include <QRectF>
#include <QString>
#include <QVector>
#include <QMap>
#include <cmath>
#include <random>

// ╔══════════════════════════════════════════════════════╗
// ║       NumberParticle — data 1 partikel angka        ║
// ╚══════════════════════════════════════════════════════╝

struct NumberParticle {
    // Identitas
    QString digit;
    int fontSize;
    int baseAlpha;

    // Posisi awal
    double x, y;

    // Tipe gerakan: 0=float, 1=orbit, 2=drift, 3=bounce
    int motion;

    // Parameter fase & kecepatan
    double phaseX, phaseY, phaseAlpha, phaseScale;
    double spdX, spdY, spdAlpha, spdScale;
    double ampX, ampY;

    // Orbit
    double orbitR, orbitSpd, orbitCx, orbitCy;

    // Bounce
    double vy, vx, gravity, bounceY, floor;

    // Rotasi
    double rot, rotSpd;

    // Variasi warna: 0=gold, 1=gold_pale, 2=warm
    int colorVar;

    // Hasil draw (diupdate per frame)
    double drawX, drawY;
    int drawAlpha;
    double drawScale;

    // Canvas size (untuk batas bounce)
    double canvasW, canvasH;

    // Inisialisasi dengan random generator
    void init(std::mt19937 &rng, double cw, double ch);

    // Update posisi frame
    void update(double t);
};

// ╔══════════════════════════════════════════════════════╗
// ║   NumberParticleSystem — QGraphicsItem renderer      ║
// ╚══════════════════════════════════════════════════════╝

class NumberParticleSystem : public QGraphicsItem
{
public:
    static constexpr int PARTICLE_COUNT = 18;

    NumberParticleSystem();

    void setSize(double W, double H);
    void tick(double t);

    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

private:
    void buildParticles();
    QFont getFont(int size);

    double m_t = 0.0;
    double m_W = 1.0;
    double m_H = 1.0;
    QVector<NumberParticle> m_particles;
    std::mt19937 m_rng;
    QMap<int, QFont> m_fontCache;
};

#endif // NUMBERPARTICLE_H
