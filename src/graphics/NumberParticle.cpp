/*
 * NumberParticle.cpp — Implementasi Sistem Partikel Angka
 * Padanan Python: class NumberParticle + NumberParticleSystem
 *                 (baris 257-394 di main.py)
 */
#include "graphics/NumberParticle.h"
#include <QFontMetrics>
#include <cmath>

// ══════════════════════════════════════════════════════
//   Helper random distributions
// ══════════════════════════════════════════════════════

static int randInt(std::mt19937 &rng, int a, int b) {
    return std::uniform_int_distribution<int>(a, b)(rng);
}
static double randUniform(std::mt19937 &rng, double a, double b) {
    return std::uniform_real_distribution<double>(a, b)(rng);
}
static double randReal01(std::mt19937 &rng) {
    return std::uniform_real_distribution<double>(0.0, 1.0)(rng);
}

template<typename T>
static T randChoice(std::mt19937 &rng, std::initializer_list<T> items) {
    int idx = randInt(rng, 0, static_cast<int>(items.size()) - 1);
    return *(items.begin() + idx);
}

// ╔══════════════════════════════════════════════════════╗
// ║              NumberParticle::init                    ║
// ╚══════════════════════════════════════════════════════╝

void NumberParticle::init(std::mt19937 &rng, double cw, double ch)
{
    canvasW = cw;
    canvasH = ch;

    digit      = QString::number(randInt(rng, 1, 9));
    fontSize   = randChoice<int>(rng, {16, 20, 24, 28, 34, 40});
    baseAlpha  = randInt(rng, 25, 65);
    x          = randUniform(rng, 0.06, 0.94) * cw;
    y          = randUniform(rng, 0.12, 0.88) * ch;
    motion     = randChoice<int>(rng, {0, 1, 2, 3}); // float, orbit, drift, bounce

    phaseX     = randUniform(rng, 0, M_PI * 2);
    phaseY     = randUniform(rng, 0, M_PI * 2);
    phaseAlpha = randUniform(rng, 0, M_PI * 2);
    phaseScale = randUniform(rng, 0, M_PI * 2);

    spdX       = randUniform(rng, 0.35, 1.0);
    spdY       = randUniform(rng, 0.25, 0.75);
    spdAlpha   = randUniform(rng, 0.50, 1.60);
    spdScale   = randUniform(rng, 0.40, 1.20);

    ampX       = randUniform(rng, 10, 40);
    ampY       = randUniform(rng, 8, 30);

    orbitR     = randUniform(rng, 18, 50);
    orbitSpd   = randUniform(rng, 0.25, 0.80) * randChoice<int>(rng, {-1, 1});
    orbitCx    = x;
    orbitCy    = y;

    vy         = randUniform(rng, -1.1, -0.35);
    vx         = randUniform(rng, -0.45, 0.45);
    gravity    = randUniform(rng, 0.012, 0.035);
    bounceY    = randUniform(rng, 0.5, 0.92) * ch;
    floor      = randUniform(rng, 0.58, 0.90) * ch;

    rot        = randUniform(rng, 0, M_PI * 2);
    rotSpd     = randUniform(rng, -0.006, 0.006);
    colorVar   = randChoice<int>(rng, {0, 1, 2}); // gold, gold_pale, warm

    drawX      = x;
    drawY      = y;
    drawAlpha  = baseAlpha;
    drawScale  = 1.0;
}

// ╔══════════════════════════════════════════════════════╗
// ║            NumberParticle::update                    ║
// ╚══════════════════════════════════════════════════════╝

void NumberParticle::update(double t)
{
    switch (motion) {
    case 0: // float
        drawX = x + std::sin(t * spdX + phaseX) * ampX;
        drawY = y + std::cos(t * spdY + phaseY) * ampY;
        break;
    case 1: { // orbit
        double angle = t * orbitSpd + phaseX;
        drawX = orbitCx + std::cos(angle) * orbitR;
        drawY = orbitCy + std::sin(angle) * orbitR * 0.5;
        break;
    }
    case 2: // drift
        drawX = x + std::sin(t * spdX * 0.35 + phaseX) * ampX * 1.4;
        drawY = y + std::cos(t * spdY * 0.28 + phaseY) * ampY * 0.55;
        break;
    case 3: // bounce
        vy += gravity;
        bounceY += vy;
        if (bounceY > floor) bounceY = floor;
        if (bounceY >= floor) {
            vy = -std::abs(vy) * 0.70;
            if (std::abs(vy) < 0.12) {
                // Re-seed bounce (deterministic enough)
                vy = -(0.35 + std::abs(std::sin(t + phaseX)) * 0.6);
            }
        }
        x += vx;
        if (x < 0 || x > canvasW) vx *= -1;
        drawX = x;
        drawY = bounceY;
        break;
    }

    double alphaPulse = 0.42 + 0.58 * (0.5 + 0.5 * std::sin(t * spdAlpha + phaseAlpha));
    drawAlpha = static_cast<int>(baseAlpha * alphaPulse);
    drawScale = 0.86 + 0.22 * (0.5 + 0.5 * std::sin(t * spdScale + phaseScale));
    rot += rotSpd;
}

// ╔══════════════════════════════════════════════════════╗
// ║           NumberParticleSystem                      ║
// ╚══════════════════════════════════════════════════════╝

NumberParticleSystem::NumberParticleSystem()
    : m_rng(99) // Seed identik dengan Python: random.Random(99)
{
    setZValue(0.5);
}

void NumberParticleSystem::buildParticles()
{
    m_particles.clear();
    m_particles.resize(PARTICLE_COUNT);
    for (auto &p : m_particles) {
        p.init(m_rng, m_W, m_H);
    }
}

void NumberParticleSystem::setSize(double W, double H)
{
    if (std::abs(W - m_W) > 1 || std::abs(H - m_H) > 1) {
        m_W = W;
        m_H = H;
        buildParticles();
        prepareGeometryChange();
    }
}

void NumberParticleSystem::tick(double t)
{
    m_t = t;
    update();
}

QRectF NumberParticleSystem::boundingRect() const
{
    return QRectF(0, 0, m_W, m_H);
}

QFont NumberParticleSystem::getFont(int size)
{
    auto it = m_fontCache.find(size);
    if (it == m_fontCache.end()) {
        QFont f("Segoe UI", size, QFont::Bold);
        f.setHintingPreference(QFont::PreferVerticalHinting);
        m_fontCache.insert(size, f);
        return f;
    }
    return it.value();
}

void NumberParticleSystem::paint(QPainter *p,
                                const QStyleOptionGraphicsItem *,
                                QWidget *)
{
    p->setRenderHint(QPainter::Antialiasing, true);
    p->setRenderHint(QPainter::TextAntialiasing, true);
    p->setRenderHint(QPainter::SmoothPixmapTransform, true);

    for (auto &particle : m_particles) {
        particle.update(m_t);
        int a = particle.drawAlpha;
        if (a < 3) continue;

        QColor col;
        switch (particle.colorVar) {
        case 0:  col = QColor(0xC9, 0xA8, 0x4C, a); break; // gold
        case 1:  col = QColor(0xD4, 0xB9, 0x6A, a); break; // gold_pale
        default: col = QColor(0xFF, 0xF0, 0xD0, a); break; // warm
        }

        QFont font = getFont(particle.fontSize);
        QFontMetrics fm(font);
        int tw = fm.horizontalAdvance(particle.digit);
        int th = fm.height();

        p->save();
        p->translate(particle.drawX, particle.drawY);
        double sc = particle.drawScale;
        p->scale(sc, sc);
        if (std::abs(particle.rotSpd) > 0.001) {
            p->rotate(particle.rot * 180.0 / M_PI);
        }
        p->setFont(font);
        p->setPen(col);
        p->drawText(QRectF(-tw / 2.0, -th * 0.65, tw + 2.0, th + 2.0),
                    Qt::AlignCenter, particle.digit);
        p->restore();
    }
}
