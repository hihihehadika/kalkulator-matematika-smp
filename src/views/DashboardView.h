/*
 * DashboardView.h — Carousel Dashboard Utama
 * Padanan Python: class DashboardView + DashboardPage (baris 996-1199)
 */
#ifndef DASHBOARDVIEW_H
#define DASHBOARDVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsProxyWidget>
#include <QLabel>
#include <QTimer>
#include <QVector>
#include <utility>

class AbstractBackground;
class NumberParticleSystem;
class DashboardDecoItem;
class CardProxy;
class FeatureCardWidget;
class MainWindow;

class DashboardView : public QWidget
{
    Q_OBJECT
public:
    explicit DashboardView(MainWindow *parent);

    void positionCards();
    void focusCard(int pageIndex);
    void updateTheme(bool isDark);

signals:
    void featureSelected(int index); // 1-based page index

protected:
    void resizeEvent(QResizeEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;

private slots:
    void onTick();
    void onCardClicked(int cardIndex);

private:
    void buildScene();
    void relayout();
    void animateAll(bool instant);
    void scrollLeft();
    void scrollRight();
    void updateDots();
    int  slotOfProxy(int cardIndex);
    int  headerHeight();
    std::pair<double,double> cardCenter(int W, int H);
    void layoutHeader(int W, int H);
    void layoutDots(int W, int H);
    void layoutNavBtns(int W, int H);
    QGraphicsProxyWidget* makeNavBtn(const QString &symbol);

    MainWindow *m_mw;

    // Scene & View
    QGraphicsView *m_view;
    QGraphicsScene *m_scene;
    AbstractBackground *m_bg;
    NumberParticleSystem *m_particles;
    DashboardDecoItem *m_deco;

    // Kartu
    struct FeatureInfo {
        QString iconKey;
        QString title;
        QString desc;
    };
    QVector<FeatureInfo> m_features;
    QVector<CardProxy*> m_proxies;

    // Header scene items (identik Python)
    QGraphicsTextItem *m_titleItem = nullptr;
    QGraphicsTextItem *m_subtitleItem = nullptr;
    QGraphicsTextItem *m_creditItem = nullptr;
    QGraphicsLineItem *m_ruleLine = nullptr;

    // Nav buttons
    QGraphicsProxyWidget *m_navLeftBtn = nullptr;
    QGraphicsProxyWidget *m_navRightBtn = nullptr;

    // Dot indicators
    QVector<QGraphicsEllipseItem*> m_dots;

    // Nav hint overlay
    QLabel *m_navHint = nullptr;

    // State
    int m_focusIdx = 0;
    int m_cardCount = 7;

    // Scroll cooldown
    bool m_scrollCooldown = false;
    QTimer m_scrollTimer;

    QTimer m_animTimer;
    double m_t = 0.0;
};

// ╔══════════════════════════════════════════════════════╗
// ║   DashboardPage — wrapper (compat dengan Python)    ║
// ╚══════════════════════════════════════════════════════╝

class DashboardPage : public DashboardView
{
    Q_OBJECT
public:
    explicit DashboardPage(MainWindow *parent) : DashboardView(parent) {}
};

#endif // DASHBOARDVIEW_H
