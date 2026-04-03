/*
 * MainWindow.cpp — Implementasi Jendela Utama (Versi Lengkap)
 * Padanan Python: class MainWindow (baris 4013-4082 di main.py)
 */
#include "MainWindow.h"
#include "core/Theme.h"
#include "views/DashboardView.h"
#include "widgets/FloatBtn.h"
#include "widgets/ShortcutsPopup.h"

#include "views/BangunRuangPage.h"
#include "views/CalcBasicPage.h"
#include "views/GarisLurusPage.h"
#include "views/PeluangPage.h"
#include "views/PythagorasPage.h"
#include "views/SpldvPage.h"
#include "views/StatistikaPage.h"

#include <QKeySequence>
#include <QShortcut>
#include <QTimer>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) { build(); }

void MainWindow::build() {
  setWindowTitle("Matematika SMP  -  By Dika");
  setMinimumSize(1080, 680);

  // ── Shortcut F11 = fullscreen ──
  auto *fsShortcut = new QShortcut(QKeySequence("F11"), this);
  connect(fsShortcut, &QShortcut::activated, this, [this]() {
    if (isFullScreen())
      showNormal();
    else
      showFullScreen();
  });

  // ── Central widget ──
  auto *central = new QWidget(this);
  setCentralWidget(central);
  auto *root = new QVBoxLayout(central);
  root->setContentsMargins(0, 0, 0, 0);

  // ── Stacked pages ──
  m_pages = new QStackedWidget();
  m_dashboard = new DashboardPage(this);

  // Index 0 = Dashboard
  m_pages->addWidget(m_dashboard);

  // Index 1-7 = Halaman fitur
  m_pages->addWidget(new GarisLurusPage(this));
  m_pages->addWidget(new BangunRuangPage(this));
  m_pages->addWidget(new PeluangPage(this));
  m_pages->addWidget(new SpldvPage(this));
  m_pages->addWidget(new PythagorasPage(this));
  m_pages->addWidget(new StatistikaPage(this));
  m_pages->addWidget(new CalcBasicPage(this));

  root->addWidget(m_pages);

  // ── Dashboard → featureSelected → switchToFeature ──
  connect(m_dashboard, &DashboardView::featureSelected, this,
          &MainWindow::switchToFeature);

  // ── Floating Buttons ──
  m_scBtn = new FloatBtn("shortcuts", central);
  m_themeBtn = new FloatBtn("theme", central);
  repositionBtns();

  m_scPopup = new ShortcutsPopup(this);

  connect(m_scBtn, &QPushButton::clicked, this, &MainWindow::toggleShortcuts);
  connect(m_themeBtn, &QPushButton::clicked, this, &MainWindow::toggleTheme);

  // ── Ctrl+1..Ctrl+8 shortcuts ──
  for (int i = 0; i < 8; ++i) {
    auto *sc = new QShortcut(QKeySequence(QString("Ctrl+%1").arg(i + 1)), this);
    connect(sc, &QShortcut::activated, this,
            [this, i]() { switchToFeature(i); });
  }

  applyGlobalTheme();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
  QMainWindow::resizeEvent(event);
  repositionBtns();
}

void MainWindow::repositionBtns() {
  if (!m_scBtn || !m_themeBtn)
    return;
  int W = width(), H = height();
  int bh = 42, bw = 42, margin = 32;
  m_scBtn->move(margin, H - bh - margin);
  m_themeBtn->move(W - bw - margin, H - bh - margin);
  m_scBtn->raise();
  m_themeBtn->raise();
}

void MainWindow::switchToFeature(int idx) {
  if (idx < 0 || idx >= m_pages->count()) {
    // Jika halaman belum ada, tetap di dashboard
    m_pages->setCurrentIndex(0);
    return;
  }
  m_pages->setCurrentIndex(idx);
  if (idx == 0) {
    QTimer::singleShot(40, m_dashboard, &DashboardView::positionCards);
    m_dashboard->setFocus();
  }
}

void MainWindow::backToDashboard(int pageIndex) {
  m_pages->setCurrentIndex(0);
  QTimer::singleShot(
      40, this, [this, pageIndex]() { m_dashboard->focusCard(pageIndex); });
  m_dashboard->setFocus();
}

void MainWindow::toggleShortcuts() {
  if (m_scPopup->isVisible())
    m_scPopup->hide();
  else
    m_scPopup->showNear(m_scBtn);
}

void MainWindow::toggleTheme() {
  m_isDark = !m_isDark;
  m_scPopup->updateTheme(m_isDark);
  m_dashboard->updateTheme(m_isDark);
  applyGlobalTheme();

  if (m_pages->count() > 1) {
    static_cast<GarisLurusPage *>(m_pages->widget(1))->updateStyles();
    static_cast<BangunRuangPage *>(m_pages->widget(2))->updateStyles();
    static_cast<PeluangPage *>(m_pages->widget(3))->updateStyles();
    static_cast<SpldvPage *>(m_pages->widget(4))->updateStyles();
    static_cast<PythagorasPage *>(m_pages->widget(5))->updateStyles();
    static_cast<StatistikaPage *>(m_pages->widget(6))->updateStyles();
    static_cast<CalcBasicPage *>(m_pages->widget(7))->updateStyles();
  }
}

void MainWindow::applyGlobalTheme() {
  ThemeColors t = T::css(m_isDark);
  setStyleSheet(QString("QMainWindow, #CentralWidget {"
                        "  background: %1;"
                        "}"
                        "QWidget {"
                        "  color: %2;"
                        "  font-family: 'Segoe UI', Arial;"
                        "}")
                    .arg(t.bg, t.text));
  if (m_scBtn)
    m_scBtn->setDark(m_isDark);
  if (m_themeBtn)
    m_themeBtn->setDark(m_isDark);
}
