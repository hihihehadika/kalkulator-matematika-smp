/*
 * CardProxy.cpp — Implementasi Animasi Proxy Kartu
 * Padanan Python: class CardProxy (baris 800-989 di main.py)
 * IDENTIK PYTHON — termasuk animasi klik (lift → bounce → fade → navigate)
 */
#include "widgets/CardProxy.h"
#include "widgets/FeatureCard.h"
#include <QGraphicsScene>
#include <QGraphicsDropShadowEffect>
#include <QEasingCurve>
#include <QTransform>
#include <cmath>

CardProxy::CardProxy(int index, FeatureCardWidget *card)
    : QGraphicsProxyWidget(), m_index(index), m_card(card)
{
    setWidget(card);
    setAcceptHoverEvents(true);
    setTransformOriginPoint(FeatureCardWidget::CW / 2.0,
                            FeatureCardWidget::CH / 2.0);

    // Glow shadow (identik Python: blur=28, color=GOLD, offset=0,6)
    m_glow = new QGraphicsDropShadowEffect();
    m_glow->setBlurRadius(28);
    m_glow->setColor(QColor(0xC9, 0xA8, 0x4C));
    m_glow->setOffset(0, 6);
    setGraphicsEffect(m_glow);

    // ── ANIMASI KLIK — identik Python ────────────────────────────
    // Lift naik (Y) — 100ms OutQuad
    m_clickLiftAnim = new QPropertyAnimation(this, "y", this);
    m_clickLiftAnim->setDuration(100);
    m_clickLiftAnim->setEasingCurve(QEasingCurve::OutQuad);
    connect(m_clickLiftAnim, &QPropertyAnimation::finished,
            this, &CardProxy::onLiftDone);

    // Spring bounce turun (Y) — 80ms InQuad
    m_clickBounceAnim = new QPropertyAnimation(this, "y", this);
    m_clickBounceAnim->setDuration(80);
    m_clickBounceAnim->setEasingCurve(QEasingCurve::InQuad);
    connect(m_clickBounceAnim, &QPropertyAnimation::finished,
            this, &CardProxy::onBounceDone);

    // Scale pulse keluar — 100ms OutQuad
    m_clickScaleOut = new QPropertyAnimation(this, "scale", this);
    m_clickScaleOut->setDuration(100);
    m_clickScaleOut->setEasingCurve(QEasingCurve::OutQuad);

    // Scale pulse kembali — 80ms InQuad
    m_clickScaleIn = new QPropertyAnimation(this, "scale", this);
    m_clickScaleIn->setDuration(80);
    m_clickScaleIn->setEasingCurve(QEasingCurve::InQuad);

    // Fade out — 220ms InCubic
    m_clickFadeAnim = new QPropertyAnimation(this, "opacity", this);
    m_clickFadeAnim->setDuration(220);
    m_clickFadeAnim->setEasingCurve(QEasingCurve::InCubic);
    connect(m_clickFadeAnim, &QPropertyAnimation::finished,
            this, &CardProxy::onFadeDone);
}

void CardProxy::setHoverLift(qreal v)
{
    m_hoverLift = v;
    setPos(m_targetX, m_targetY - m_hoverLift);
}

void CardProxy::setEntranceScale(qreal v)
{
    m_entranceScale = v;
    setScale(v * m_targetScaleVal);
    setOpacity(v * m_targetOpacityVal);
}

void CardProxy::setTargetPos(qreal x, qreal y) { m_targetX = x; m_targetY = y; }
void CardProxy::setTargetScale(qreal s) { m_targetScaleVal = s; }
void CardProxy::setTargetOpacity(qreal o) { m_targetOpacityVal = o; }
void CardProxy::setTargetZ(qreal z) { m_targetZ = z; setZValue(z); }
void CardProxy::setTargetRotation(qreal rot) { m_targetRotVal = rot; }

void CardProxy::stopAllAnimations()
{
    if (m_liftAnim && m_liftAnim->state() == QAbstractAnimation::Running)
        m_liftAnim->stop();
    if (m_posAnim && m_posAnim->state() == QAbstractAnimation::Running)
        m_posAnim->stop();
    if (m_scaleAnim && m_scaleAnim->state() == QAbstractAnimation::Running)
        m_scaleAnim->stop();
    if (m_opacityAnim && m_opacityAnim->state() == QAbstractAnimation::Running)
        m_opacityAnim->stop();
    if (m_rotAnim && m_rotAnim->state() == QAbstractAnimation::Running)
        m_rotAnim->stop();
}

// ── Animasi identik Python: InOutCubic untuk pos/scale/rot, InOutQuad untuk opacity ──
void CardProxy::applyTarget(bool animate)
{
    setZValue(m_targetZ);

    if (!animate) {
        setPos(m_targetX, m_targetY - m_hoverLift);
        setScale(m_targetScaleVal);
        setOpacity(m_targetOpacityVal);
        setRotation(m_targetRotVal);
        return;
    }

    stopAllAnimations();

    // Durasi identik Python: pos=400, scale=400, rot=400, opacity=280
    const int MOVE_DUR = 400;
    const int SCALE_DUR = 400;
    const int ROT_DUR = 400;
    const int OP_DUR = 280;

    // ── Animasi X (identik Python: terpisah x dan y, tapi kita pakai "pos") ──
    if (!m_posAnim) m_posAnim = new QPropertyAnimation(this, "pos", this);
    m_posAnim->stop();
    m_posAnim->setDuration(MOVE_DUR);
    m_posAnim->setStartValue(pos());
    m_posAnim->setEndValue(QPointF(m_targetX, m_targetY - m_hoverLift));
    m_posAnim->setEasingCurve(QEasingCurve::InOutCubic);
    m_posAnim->start();

    // ── Animasi Scale ──
    if (!m_scaleAnim) m_scaleAnim = new QPropertyAnimation(this, "scale", this);
    m_scaleAnim->stop();
    m_scaleAnim->setDuration(SCALE_DUR);
    m_scaleAnim->setStartValue(scale());
    m_scaleAnim->setEndValue(m_targetScaleVal);
    m_scaleAnim->setEasingCurve(QEasingCurve::InOutCubic);
    m_scaleAnim->start();

    // ── Animasi Rotation ──
    if (!m_rotAnim) m_rotAnim = new QPropertyAnimation(this, "rotation", this);
    m_rotAnim->stop();
    m_rotAnim->setDuration(ROT_DUR);
    m_rotAnim->setStartValue(rotation());
    m_rotAnim->setEndValue(m_targetRotVal);
    m_rotAnim->setEasingCurve(QEasingCurve::InOutCubic);
    m_rotAnim->start();

    // ── Animasi Opacity ──
    if (!m_opacityAnim) m_opacityAnim = new QPropertyAnimation(this, "opacity", this);
    m_opacityAnim->stop();
    m_opacityAnim->setDuration(OP_DUR);
    m_opacityAnim->setStartValue(opacity());
    m_opacityAnim->setEndValue(m_targetOpacityVal);
    m_opacityAnim->setEasingCurve(QEasingCurve::InOutQuad);
    m_opacityAnim->start();
}

void CardProxy::playEntrance(int delayMs)
{
    setScale(0);
    setOpacity(0);

    QTimer::singleShot(delayMs, this, [this]() {
        auto *anim = new QPropertyAnimation(this, "entranceScale", this);
        anim->setDuration(700);
        anim->setStartValue(0.0);
        anim->setEndValue(1.0);
        anim->setEasingCurve(QEasingCurve::OutBack);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    });
}

void CardProxy::playBounce()
{
    stopAllAnimations();

    auto *seq = new QSequentialAnimationGroup(this);

    auto *up = new QPropertyAnimation(this, "hoverLift", this);
    up->setDuration(200);
    up->setStartValue(m_hoverLift);
    up->setEndValue(16.0);
    up->setEasingCurve(QEasingCurve::OutCubic);
    seq->addAnimation(up);

    auto *down = new QPropertyAnimation(this, "hoverLift", this);
    down->setDuration(350);
    down->setStartValue(16.0);
    down->setEndValue(0.0);
    down->setEasingCurve(QEasingCurve::OutBounce);
    seq->addAnimation(down);

    seq->start(QAbstractAnimation::DeleteWhenStopped);
}

void CardProxy::setGlow(bool strong)
{
    if (!m_glow) return;
    if (strong) {
        m_glow->setBlurRadius(70);
        m_glow->setOffset(0, 18);
    } else {
        m_glow->setBlurRadius(28);
        m_glow->setOffset(0, 6);
    }
    m_glow->setColor(QColor(0xC9, 0xA8, 0x4C));
}

void CardProxy::hoverEnterEvent(QGraphicsSceneHoverEvent *e)
{
    // Identik Python: jangan hover jika locked (sedang animasi klik)
    if (m_locked) { QGraphicsProxyWidget::hoverEnterEvent(e); return; }

    // Identik Python: hanya aktifkan hover jika kartu di slot tengah (0)
    if (m_currentSlot != 0) {
        QGraphicsProxyWidget::hoverEnterEvent(e);
        return;
    }
    m_hovered = true;
    m_card->setHoverStyle(true);
    setGlow(true);

    // Hover scale up 2.5% (identik Python)
    auto *sc = new QPropertyAnimation(this, "scale", this);
    sc->setDuration(180);
    sc->setStartValue(scale());
    sc->setEndValue(scale() * 1.025);
    sc->setEasingCurve(QEasingCurve::OutCubic);
    sc->start(QAbstractAnimation::DeleteWhenStopped);

    if (!m_liftAnim) m_liftAnim = new QPropertyAnimation(this, "hoverLift", this);
    m_liftAnim->stop();
    m_liftAnim->setDuration(250);
    m_liftAnim->setStartValue(m_hoverLift);
    m_liftAnim->setEndValue(12.0);
    m_liftAnim->setEasingCurve(QEasingCurve::OutCubic);
    m_liftAnim->start();

    QGraphicsProxyWidget::hoverEnterEvent(e);
}

void CardProxy::hoverLeaveEvent(QGraphicsSceneHoverEvent *e)
{
    // Identik Python: jangan hover jika locked
    if (m_locked) { QGraphicsProxyWidget::hoverLeaveEvent(e); return; }

    if (!m_hovered) {
        QGraphicsProxyWidget::hoverLeaveEvent(e);
        return;
    }
    m_hovered = false;
    m_card->setHoverStyle(false);
    setGlow(false);

    // Scale kembali normal (identik Python)
    auto *sc = new QPropertyAnimation(this, "scale", this);
    sc->setDuration(200);
    sc->setStartValue(scale());
    sc->setEndValue(m_targetScaleVal);
    sc->setEasingCurve(QEasingCurve::OutCubic);
    sc->start(QAbstractAnimation::DeleteWhenStopped);

    if (!m_liftAnim) m_liftAnim = new QPropertyAnimation(this, "hoverLift", this);
    m_liftAnim->stop();
    m_liftAnim->setDuration(300);
    m_liftAnim->setStartValue(m_hoverLift);
    m_liftAnim->setEndValue(0.0);
    m_liftAnim->setEasingCurve(QEasingCurve::OutCubic);
    m_liftAnim->start();

    QGraphicsProxyWidget::hoverLeaveEvent(e);
}

// ════════════════════════════════════════════════════════
// MOUSE PRESS — identik Python mousePressEvent
// ════════════════════════════════════════════════════════
void CardProxy::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    if (!m_locked) {
        if (m_currentSlot == 0) {
            // ── Kartu tengah diklik: mulai animasi klik ──
            m_locked = true;
            m_liftYStart = y();

            // Lift naik 32px — identik Python
            m_clickLiftAnim->stop();
            m_clickLiftAnim->setStartValue(y());
            m_clickLiftAnim->setEndValue(y() - 32);
            m_clickLiftAnim->start();

            // Scale pulse keluar ke 1.06 — identik Python
            m_clickScaleOut->stop();
            m_clickScaleOut->setStartValue(scale());
            m_clickScaleOut->setEndValue(1.06);
            m_clickScaleOut->start();

        } else if (m_currentSlot == -1) {
            // Klik kartu kiri → scroll ke kiri
            emit clicked(m_index);
        } else if (m_currentSlot == 1) {
            // Klik kartu kanan → scroll ke kanan
            emit clicked(m_index);
        }
    }
    QGraphicsProxyWidget::mousePressEvent(e);
}

// ════════════════════════════════════════════════════════
// Identik Python: _spring_bounce
// Dipanggil setelah lift selesai
// ════════════════════════════════════════════════════════
void CardProxy::onLiftDone()
{
    // Turun sedikit — efek spring/bounce, tetap di atas tapi lebih rendah
    m_clickBounceAnim->stop();
    m_clickBounceAnim->setStartValue(y());
    m_clickBounceAnim->setEndValue(m_liftYStart - 20); // identik Python
    m_clickBounceAnim->start();

    // Scale kembali ke 1.02 — identik Python
    m_clickScaleIn->stop();
    m_clickScaleIn->setStartValue(scale());
    m_clickScaleIn->setEndValue(1.02);
    m_clickScaleIn->start();
}

// ════════════════════════════════════════════════════════
// Identik Python: _start_fade
// Dipanggil setelah bounce selesai
// ════════════════════════════════════════════════════════
void CardProxy::onBounceDone()
{
    m_clickFadeAnim->stop();
    m_clickFadeAnim->setStartValue(1.0);
    m_clickFadeAnim->setEndValue(0.0);
    m_clickFadeAnim->start();
}

// ════════════════════════════════════════════════════════
// Identik Python: _on_fade_done
// Reset state + navigate ke halaman fitur
// ════════════════════════════════════════════════════════
void CardProxy::onFadeDone()
{
    // Reset — identik Python
    setOpacity(1.0);
    m_card->setHoverStyle(false);
    setGlow(false);
    m_locked = false;

    // Emit signal untuk navigate — identik Python w.switch_to_feature(self.page_index)
    emit clickAnimFinished(m_index);
}
