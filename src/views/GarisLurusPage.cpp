/*
 * GarisLurusPage.cpp — Halaman Persamaan Garis Lurus
 * Padanan Python: class GarisLurusPage (baris 1429-1556 di main.py)
 * IDENTIK PYTHON — 4 input (m, c, x, y) + reverse solving + preview + grafik
 */
#include "views/GarisLurusPage.h"
#include "MainWindow.h"
#include "core/Theme.h"
#include "core/MathUtils.h"
#include "helpers/PageHelpers.h"
#include "helpers/ResultRenderer.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QFrame>
#include <QShortcut>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QScatterSeries>
#include <cmath>
#include <algorithm>

GarisLurusPage::GarisLurusPage(MainWindow *parent)
    : QWidget(parent), m_mw(parent)
{
    auto *main = new QVBoxLayout(this);
    main->setContentsMargins(0, 0, 0, 0);

    auto *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto *content = new QWidget();
    auto *lay = new QVBoxLayout(content);
    lay->setContentsMargins(64, 36, 64, 48);
    lay->setSpacing(0);

    auto *back = makeBackBtn();
    connect(back, &QPushButton::clicked, this, [this]() { m_mw->backToDashboard(1); });
    lay->addWidget(back, 0, Qt::AlignLeft);
    lay->addSpacing(18);
    lay->addWidget(sectionHeader("Persamaan Garis Lurus", "y = m.x + c"));
    lay->addSpacing(20);

    // Panel — identik Python
    auto *panel = new QFrame();
    panel->setObjectName("panel");
    auto *play = new QVBoxLayout(panel);
    play->setContentsMargins(24, 22, 24, 22);
    play->setSpacing(16);

    // Hint — identik Python
    auto *hint = new QLabel("Masukkan nilai m, c, dan x/y. Kosongkan tepat 1 variabel untuk reverse solving.");
    hint->setObjectName("hintLabel");
    play->addWidget(hint);

    // Grid 4 input — identik Python
    auto *grid = new QWidget();
    auto *gridLay = new QGridLayout(grid);
    gridLay->setSpacing(14);

    auto [bm, mIn] = inputBox("M  —  GRADIEN", "contoh: 2");
    auto [bc, cIn] = inputBox("C  —  KONSTANTA", "contoh: 1");
    auto [bx, xIn] = inputBox("X  —  TITIK EVALUASI", "contoh: 3");
    auto [by, yIn] = inputBox("Y  —  HASIL (OPSIONAL)", "kosongkan untuk dihitung");

    m_mIn = mIn; m_cIn = cIn; m_xIn = xIn; m_yIn = yIn;

    gridLay->addWidget(bm, 0, 0); gridLay->addWidget(bc, 0, 1);
    gridLay->addWidget(bx, 0, 2); gridLay->addWidget(by, 0, 3);
    gridLay->setColumnStretch(0, 1); gridLay->setColumnStretch(1, 1);
    gridLay->setColumnStretch(2, 1); gridLay->setColumnStretch(3, 1);
    play->addWidget(grid);

    // Preview label — identik Python
    m_previewLbl = new QLabel("y = ?");
    m_previewLbl->setAlignment(Qt::AlignCenter);
    m_previewLbl->setStyleSheet(
        "font-size:20px;font-weight:700;color:#C9A84C;background:transparent;padding:8px 0;");
    play->addWidget(m_previewLbl);

    connect(m_mIn, &QLineEdit::textChanged, this, &GarisLurusPage::updatePreview);
    connect(m_cIn, &QLineEdit::textChanged, this, &GarisLurusPage::updatePreview);

    // Calc button — identik Python
    auto *btn = calcBtn("Hitung dan Tampilkan Grafik");
    connect(btn, &QPushButton::clicked, this, &GarisLurusPage::calculate);
    play->addWidget(btn);

    lay->addWidget(panel);
    lay->addSpacing(20);

    // Result label — identik Python
    m_result = new QLabel("Masukkan nilai m, c, dan x (atau y).");
    m_result->setObjectName("resultLabel");
    m_result->setWordWrap(true);
    m_result->setTextFormat(Qt::RichText);
    lay->addWidget(m_result);
    lay->addSpacing(20);

    // Chart
    auto *chart = new QChart();
    chart->setAnimationOptions(QChart::SeriesAnimations);
    m_chartView = new QChartView(chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setMinimumHeight(380);
    lay->addWidget(m_chartView);

    lay->addStretch();
    scroll->setWidget(content);
    main->addWidget(scroll);

    // Shortcuts — identik Python
    for (auto *inp : {m_mIn, m_cIn, m_xIn, m_yIn})
        connect(inp, &QLineEdit::returnPressed, this, &GarisLurusPage::calculate);
    connect(new QShortcut(QKeySequence("Ctrl+R"), this), &QShortcut::activated,
            this, &GarisLurusPage::reset);
    connect(new QShortcut(QKeySequence(Qt::Key_Escape), this), &QShortcut::activated,
            this, [this]() { m_mw->backToDashboard(1); });

    updateStyles();
}

// ── Preview identik Python _update_preview() ──
void GarisLurusPage::updatePreview()
{
    QString mText = m_mIn->text().trimmed();
    QString cText = m_cIn->text().trimmed();
    QString mDisp = mText.isEmpty() ? "m" : mText;
    QString cDisp = cText.isEmpty() ? "c" : cText;

    bool ok = false;
    double cVal = cDisp.toDouble(&ok);
    QString sign, cShow;
    if (ok) {
        sign = (cVal >= 0) ? "+ " : "- ";
        cShow = QString::number(std::abs(cVal));
    } else {
        sign = "+ ";
        cShow = cDisp;
    }
    m_previewLbl->setText(QString("y = %1x %2%3").arg(mDisp, sign, cShow));
}

void GarisLurusPage::reset()
{
    for (auto *inp : {m_mIn, m_cIn, m_xIn, m_yIn}) inp->clear();
    m_result->setText("Masukkan nilai m, c, dan x (atau y).");
    m_result->setStyleSheet("");
    m_chartView->chart()->removeAllSeries();
    m_previewLbl->setText("y = ?");
}

void GarisLurusPage::updateStyles()
{
    if (m_mw) {
        ThemeColors t = T::css(m_mw->isDark());
        setStyleSheet(pageStylesheet(t));
        m_previewLbl->setStyleSheet(
            QString("font-size:20px;font-weight:700;color:%1;background:transparent;padding:8px 0;")
                .arg(t.primary));
        auto *chart = m_chartView->chart();
        chart->setBackgroundBrush(QColor(t.card));
        chart->setPlotAreaBackgroundBrush(QColor(t.card));
    }
}

// ── Reverse solving identik Python plot() ──
void GarisLurusPage::calculate()
{
    ThemeColors t = T::css(m_mw ? m_mw->isDark() : true);
    auto sf = [](const QString &s) -> std::optional<double> {
        return MathUtils::parseFloat(s);
    };
    auto ok = [](const std::optional<double> &v) { return v.has_value(); };

    try {
        auto mOpt = sf(m_mIn->text());
        auto cOpt = sf(m_cIn->text());
        auto xOpt = sf(m_xIn->text());
        auto yOpt = sf(m_yIn->text());

        QStringList empty, filled;
        if (!ok(mOpt)) empty << "m"; else filled << "m";
        if (!ok(cOpt)) empty << "c"; else filled << "c";
        if (!ok(xOpt)) empty << "x"; else filled << "x";
        if (!ok(yOpt)) empty << "y"; else filled << "y";

        double m = 0, c = 0, xv = 0, yv = 0;
        QString target;

        if (empty.size() == 0) {
            // Semua terisi → verifikasi
            m = *mOpt; c = *cOpt; xv = *xOpt; yv = *yOpt;
            double expected = m * xv + c;
            if (std::abs(expected - yv) > 1e-6) {
                resultErr(m_result,
                    QString("Semua terisi tapi tidak konsisten: y = %1*%2+%3 = %4, bukan %5.")
                        .arg(MathUtils::disp(m), MathUtils::disp(xv), MathUtils::disp(c),
                             MathUtils::disp(expected), MathUtils::disp(yv)), t);
                return;
            }
            target = "verify";
        } else if (empty.size() == 1) {
            target = empty[0];
            if (target == "y") {
                m = *mOpt; c = *cOpt; xv = *xOpt;
                yv = m * xv + c;
            } else if (target == "x") {
                m = *mOpt; c = *cOpt; yv = *yOpt;
                if (std::abs(m) < 1e-12) {
                    resultErr(m_result, "m = 0, sehingga x tidak bisa dihitung (garis horizontal).", t);
                    return;
                }
                xv = (yv - c) / m;
            } else if (target == "m") {
                c = *cOpt; xv = *xOpt; yv = *yOpt;
                if (std::abs(xv) < 1e-12) {
                    resultErr(m_result, "x = 0, sehingga m tidak bisa dihitung (pembagian dengan nol).", t);
                    return;
                }
                m = (yv - c) / xv;
            } else if (target == "c") {
                m = *mOpt; xv = *xOpt; yv = *yOpt;
                c = yv - m * xv;
            }
        } else {
            // Lebih dari 1 kosong
            resultErr(m_result,
                QString("Tepat 1 variabel harus dikosongkan. Saat ini kosong: %1.")
                    .arg(empty.join(", ")), t);
            return;
        }

        // ── Build HTML identik Python ──
        double xIntercept = (std::abs(m) > 1e-12) ? -c / m : 0;
        QString xInterceptStr = (std::abs(m) > 1e-12)
            ? QString("(%1, 0)").arg(MathUtils::disp(xIntercept))
            : "Tidak ada (garis horizontal)";
        QString yInterceptStr = QString("(0, %1)").arg(MathUtils::disp(c));

        QString html = htmlTitle("Langkah Perhitungan", t);
        html += htmlRow("<b>Rumus:</b>", "y = m.x + c", t);
        html += htmlRow("<b>Diketahui:</b>",
            QString("m=%1, c=%2, x=%3, y=%4")
                .arg(MathUtils::disp(m), MathUtils::disp(c),
                     MathUtils::disp(xv), MathUtils::disp(yv)), t);
        html += htmlRow("<b>Substitusi:</b>",
            QString("y = %1 * %2 + %3 = %4")
                .arg(MathUtils::disp(m), MathUtils::disp(xv),
                     MathUtils::disp(c), MathUtils::disp(yv)), t);
        html += htmlDiv(t);

        if (target != "verify") {
            double answer = 0;
            if (target == "y") answer = yv;
            else if (target == "x") answer = xv;
            else if (target == "m") answer = m;
            else if (target == "c") answer = c;
            html += htmlBig(QString("%1 = %2").arg(target, MathUtils::disp(answer)), t);
        } else {
            html += htmlBig("Verifikasi: konsisten!", t);
        }

        html += htmlDiv(t);
        html += htmlRow("<b>Titik potong sumbu X:</b>", xInterceptStr, t);
        html += htmlRow("<b>Titik potong sumbu Y:</b>", yInterceptStr, t);

        resultOk(m_result, html, t);

        // ── Chart identik Python ──
        auto *chart = m_chartView->chart();
        chart->removeAllSeries();

        double margin = std::max({std::abs(xv) * 1.5, std::abs(yv) * 1.5, 10.0});
        double xLo = -margin, xHi = margin;

        auto *line = new QLineSeries();
        for (int i = 0; i <= 200; ++i) {
            double x = xLo + i * (xHi - xLo) / 200.0;
            line->append(x, m * x + c);
        }
        line->setName(QString("y = %1x + %2").arg(MathUtils::disp(m), MathUtils::disp(c)));
        line->setColor(QColor(t.accent));
        line->setPen(QPen(QColor(t.accent), 2.2));
        chart->addSeries(line);

        auto *dot = new QScatterSeries();
        dot->setName(QString("(%1, %2)").arg(MathUtils::disp(xv), MathUtils::disp(yv)));
        dot->setMarkerSize(12);
        dot->setColor(QColor(t.primary));
        dot->setBorderColor(QColor(t.card));
        dot->append(xv, yv);
        chart->addSeries(dot);

        chart->createDefaultAxes();
        chart->setBackgroundBrush(QColor(t.card));

        auto axes = chart->axes();
        for (auto *ax : axes) {
            if (auto *va = qobject_cast<QValueAxis*>(ax)) {
                va->setLabelsColor(QColor(t.text_mid));
                va->setGridLineColor(QColor(t.border));
            }
        }

    } catch (const std::runtime_error &e) {
        resultErr(m_result, QString::fromStdString(e.what()), t);
    } catch (...) {
        resultErr(m_result, "Terjadi kesalahan.", t);
    }
}
