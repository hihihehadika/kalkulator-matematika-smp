/*
 * ProbabilityWidgets.cpp — Implementasi Pie Chart & Bar
 * Padanan Python: baris 2447-2517 di main.py
 */
#include "graphics/ProbabilityWidgets.h"
#include "core/Theme.h"
#include <QPainter>
#include <QPen>
#include <cmath>

// ═══════════════════ PIE CHART ═══════════════════

ProbabilityPieChart::ProbabilityPieChart(QWidget *parent) : QWidget(parent) {
  setFixedSize(110, 110);
}

void ProbabilityPieChart::setValues(double favorable, double total) {
  m_favorable = favorable;
  m_total = (total > 0) ? total : 1;
  update();
}

void ProbabilityPieChart::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);
  int S = qMin(width(), height()) - 8;
  int ox = (width() - S) / 2, oy = (height() - S) / 2;

  // Background circle
  QColor bgCol = isDark ? QColor(0x30, 0x30, 0x3C) : QColor(0xDD, 0xE1, 0xEA);
  p.setPen(Qt::NoPen);
  p.setBrush(bgCol);
  p.drawEllipse(ox, oy, S, S);

  // Favorable arc
  double ratio = m_favorable / m_total;
  int span = static_cast<int>(ratio * 360 * 16);
  if (span > 0) {
    p.setBrush(T::GOLD);
    p.drawPie(ox, oy, S, S, 90 * 16, -span);
  }

  // Center circle (donut)
  int inner = S * 55 / 100;
  int ix = ox + (S - inner) / 2, iy = oy + (S - inner) / 2;
  QColor centerCol =
      isDark ? QColor(0x1E, 0x1E, 0x24) : QColor(0xFF, 0xFF, 0xFF);
  p.setBrush(centerCol);
  p.drawEllipse(ix, iy, inner, inner);

  // Percentage text
  QFont f("Segoe UI", 13, QFont::Bold);
  p.setFont(f);
  p.setPen(T::GOLD);
  QString pct = QString("%1%").arg(ratio * 100, 0, 'f', 1);
  p.drawText(QRect(ox, oy, S, S), Qt::AlignCenter, pct);
}

// ═══════════════════ BAR ═══════════════════

ProbabilityBar::ProbabilityBar(QWidget *parent) : QWidget(parent) {
  setFixedHeight(28);
  setMinimumWidth(200);
}

void ProbabilityBar::setValue(double ratio) {
  m_ratio = qBound(0.0, ratio, 1.0);
  update();
}

void ProbabilityBar::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);
  int W = width(), H = height();
  int R = H / 2;

  // Track
  QColor trackCol =
      isDark ? QColor(0x30, 0x30, 0x3C) : QColor(0xDD, 0xE1, 0xEA);
  p.setPen(Qt::NoPen);
  p.setBrush(trackCol);
  p.drawRoundedRect(0, 0, W, H, R, R);

  // Fill
  int fillW = static_cast<int>(W * m_ratio);
  if (fillW > 4) {
    QLinearGradient grad(0, 0, fillW, 0);
    grad.setColorAt(0, T::GOLD);
    grad.setColorAt(1, T::GOLD_PALE);
    p.setBrush(grad);
    p.drawRoundedRect(0, 0, fillW, H, R, R);
  }

  // Label
  QFont f("Segoe UI", 9, QFont::Bold);
  p.setFont(f);
  p.setPen(isDark ? QColor(0xF2, 0xF2, 0xF5) : QColor(0x1A, 0x1A, 0x2E));
  QString lbl = QString("%1%").arg(m_ratio * 100, 0, 'f', 1);
  p.drawText(QRect(0, 0, W, H), Qt::AlignCenter, lbl);
}
