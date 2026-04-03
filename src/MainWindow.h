/*
 * MainWindow.h — Jendela Utama (Versi Lengkap)
 * Padanan Python: class MainWindow (baris 4013-4082 di main.py)
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

class DashboardPage;
class FloatBtn;
class ShortcutsPopup;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    bool isDark() const { return m_isDark; }

    // Dipanggil halaman fitur untuk kembali + fokus kartu
    void backToDashboard(int pageIndex);

    // Pindah ke halaman fitur (0=dashboard, 1-7=fitur)
    void switchToFeature(int idx);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void build();
    void applyGlobalTheme();
    void repositionBtns();
    void toggleTheme();
    void toggleShortcuts();

    bool m_isDark = true;

    QStackedWidget *m_pages = nullptr;
    DashboardPage *m_dashboard = nullptr;

    // Floating buttons
    FloatBtn *m_scBtn = nullptr;
    FloatBtn *m_themeBtn = nullptr;
    ShortcutsPopup *m_scPopup = nullptr;
};

#endif // MAINWINDOW_H
