/*
 * SplashScreen.h — Splash Screen Startup
 * Padanan Python: class SplashScreen (baris 198-250 di main.py)
 */
#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QSplashScreen>
#include <QPixmap>

class SplashScreen : public QSplashScreen
{
    Q_OBJECT
public:
    explicit SplashScreen();
};

#endif // SPLASHSCREEN_H
