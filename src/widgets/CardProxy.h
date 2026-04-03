/*
 * CardProxy.h — Animasi Proxy Kartu Carousel
 * Padanan Python: class CardProxy (baris 800-989 di main.py)
 * IDENTIK PYTHON — termasuk animasi klik (lift → bounce → fade → navigate)
 */
#ifndef CARDPROXY_H
#define CARDPROXY_H

#include <QGraphicsProxyWidget>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QTimer>
#include <QVector>

class QGraphicsDropShadowEffect;
class FeatureCardWidget;

class CardProxy : public QGraphicsProxyWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal hoverLift READ hoverLift WRITE setHoverLift)
    Q_PROPERTY(qreal entranceScale READ entranceScale WRITE setEntranceScale)

public:
    CardProxy(int index, FeatureCardWidget *card);

    int index() const { return m_index; }

    qreal hoverLift() const { return m_hoverLift; }
    void setHoverLift(qreal v);

    qreal entranceScale() const { return m_entranceScale; }
    void setEntranceScale(qreal v);

    void setTargetPos(qreal x, qreal y);
    void setTargetScale(qreal s);
    void setTargetOpacity(qreal o);
    void setTargetZ(qreal z);
    void setTargetRotation(qreal rot);
    void setCurrentSlot(int slot) { m_currentSlot = slot; }
    int currentSlot() const { return m_currentSlot; }
    qreal targetX() const { return m_targetX; }
    qreal targetY() const { return m_targetY; }

    // Animasi entrance 7 bintang
    void playEntrance(int delayMs);

    // Animasi bounce saat fokus
    void playBounce();

    // Terapkan posisi target
    void applyTarget(bool animate = true);

signals:
    void clicked(int index);
    // Signal baru: dikirim SETELAH animasi klik selesai (identik Python _on_fade_done)
    void clickAnimFinished(int index);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *e) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *e) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *e) override;

private slots:
    // Identik Python: _spring_bounce → dipanggil setelah lift selesai
    void onLiftDone();
    // Identik Python: _start_fade → dipanggil setelah bounce selesai
    void onBounceDone();
    // Identik Python: _on_fade_done → reset + navigate
    void onFadeDone();

private:
    void stopAllAnimations();
    void setGlow(bool strong);

    int m_index;
    FeatureCardWidget *m_card;
    QGraphicsDropShadowEffect *m_glow = nullptr;

    qreal m_hoverLift = 0.0;
    qreal m_entranceScale = 0.0;
    qreal m_targetX = 0, m_targetY = 0;
    qreal m_targetScaleVal = 1.0;
    qreal m_targetOpacityVal = 1.0;
    qreal m_targetZ = 0;
    qreal m_targetRotVal = 0.0;
    int m_currentSlot = 0;
    bool m_hovered = false;
    bool m_locked = false;           // identik Python: _locked
    qreal m_liftYStart = 0.0;       // identik Python: _lift_y_start

    // Animasi carousel (posisi/scale/rot/opacity)
    QPropertyAnimation *m_liftAnim = nullptr;
    QPropertyAnimation *m_posAnim = nullptr;
    QPropertyAnimation *m_scaleAnim = nullptr;
    QPropertyAnimation *m_opacityAnim = nullptr;
    QPropertyAnimation *m_rotAnim = nullptr;

    // Animasi klik — identik Python
    QPropertyAnimation *m_clickLiftAnim = nullptr;     // lift naik (Y)
    QPropertyAnimation *m_clickBounceAnim = nullptr;   // spring bounce turun (Y)
    QPropertyAnimation *m_clickScaleOut = nullptr;     // scale pulse keluar
    QPropertyAnimation *m_clickScaleIn = nullptr;      // scale pulse kembali
    QPropertyAnimation *m_clickFadeAnim = nullptr;     // fade out
};

#endif // CARDPROXY_H
