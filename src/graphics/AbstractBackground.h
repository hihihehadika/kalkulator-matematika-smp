/*
 * AbstractBackground.h — Latar Belakang Animasi
 * Padanan Python: class AbstractBackground (baris 401-514 di main.py)
 */
#ifndef ABSTRACTBACKGROUND_H
#define ABSTRACTBACKGROUND_H

#include <QWidget>
#include <QTimer>
#include <QPixmap>
#include <QPen>
#include <QColor>
#include <QVector>
#include <cmath>
#include <random>

// ╔══════════════════════════════════════════════════════╗
// ║       AbstractBackground — Animasi Latar            ║
// ╚══════════════════════════════════════════════════════╝

class AbstractBackground : public QWidget
{
    Q_OBJECT

public:
    explicit AbstractBackground(QWidget *parent = nullptr);

    bool isDarkMode = true;
    double t = 0.0;

protected:
    void showEvent(QShowEvent *e) override;
    void hideEvent(QHideEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void paintEvent(QPaintEvent *e) override;

private slots:
    void onTick();

private:
    void rebuildCache(int W, int H);
    void paintOrbs(QPainter &p, int W, int H);
    void paintStreaks(QPainter &p, int W, int H, double t);
    void paintStars(QPainter &p, int W, int H, double t);

    // Konstanta
    static constexpr int STAR_COUNT = 52;
    static constexpr int GRID_SPACING = 62;
    static constexpr int GRID_ALPHA = 16;

    // Data bintang: {xFrac, yFrac, size, phase, speed}
    struct StarData {
        double xf, yf, sz, phase, spd;
    };
    QVector<StarData> m_stars;

    // Data streak: {period, phase, sinA, cosA, thick, color, maxAlpha}
    struct StreakData {
        double period, phase, sinA, cosA, thick;
        QColor color;
        int maxAlpha;
    };
    QVector<StreakData> m_streaks;
    QVector<QPen> m_streakPens;

    QColor m_warmWhite;
    QPixmap m_cache;
    QSize m_cacheSize;
    QTimer m_timer;
};

#endif // ABSTRACTBACKGROUND_H
