/*
 * Matematika SMP — By: Dika (C++ Version)
 * main.cpp — Titik Masuk Aplikasi
 *
 * Padanan Python: def main() (baris 4089-4097 di main.py)
 */
#include "MainWindow.h"
#include "widgets/SplashScreen.h"
#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setStyle("Fusion");

  // Splash screen 1.8 detik (identik Python)
  SplashScreen splash;
  splash.show();
  app.processEvents();

  MainWindow win;
  QTimer::singleShot(1800, &splash, &QSplashScreen::close);
  QTimer::singleShot(1800, &win, &QMainWindow::show);

  return app.exec();
}
