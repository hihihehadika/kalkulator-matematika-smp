/*
 * PeluangPage.cpp — Halaman Peluang Kejadian
 * Padanan Python: class PeluangPage (baris 2519-2636 di main.py)
 * IDENTIK PYTHON — 3 input (n(A), n(S), P(A)) + reverse solving
 */
#include "views/PeluangPage.h"
#include "MainWindow.h"
#include "core/Theme.h"
#include "core/MathUtils.h"
#include "helpers/PageHelpers.h"
#include "helpers/ResultRenderer.h"
#include "graphics/ProbabilityWidgets.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QFrame>
#include <QShortcut>
#include <cmath>

PeluangPage::PeluangPage(MainWindow *parent)
    : QWidget(parent), m_mw(parent)
{
    auto *main = new QVBoxLayout(this);
    main->setContentsMargins(0,0,0,0);

    auto *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto *content = new QWidget();
    auto *lay = new QVBoxLayout(content);
    lay->setContentsMargins(64,36,64,48);
    lay->setSpacing(0);

    auto *back = makeBackBtn();
    connect(back, &QPushButton::clicked, this, [this](){ m_mw->backToDashboard(3); });
    lay->addWidget(back, 0, Qt::AlignLeft);
    lay->addSpacing(18);
    lay->addWidget(sectionHeader("Kalkulator Peluang","P(A) = n(A) dibagi n(S)"));
    lay->addSpacing(20);

    // Panel — identik Python
    auto *panel = new QFrame();
    panel->setObjectName("panel");
    auto *play = new QVBoxLayout(panel);
    play->setContentsMargins(24,22,24,22);
    play->setSpacing(16);

    // Hint — identik Python
    auto *hint = new QLabel("Masukkan 2 dari 3 nilai: n(A), n(S), P(A). Kosongkan yang ingin dicari.");
    hint->setObjectName("hintLabel");
    play->addWidget(hint);

    // Context label — identik Python
    auto *ctx = new QLabel("Contoh: melempar dadu, mengambil kartu, dll.");
    ctx->setStyleSheet("font-size:12px;font-style:italic;color:#606078;background:transparent;");
    play->addWidget(ctx);

    // 3 input dalam grid — identik Python
    auto *rowW = new QWidget();
    auto *rLay = new QGridLayout(rowW);
    rLay->setSpacing(14);

    auto [bna, na] = inputBox("N(A)  —  JUMLAH KEJADIAN", "contoh: 2");
    auto [bns, ns] = inputBox("N(S)  —  TOTAL RUANG SAMPEL", "contoh: 6");
    auto [bpa, pa] = inputBox("P(A)  —  PELUANG (0-1 atau %)", "kosongkan untuk dihitung");

    m_na = na; m_ns = ns; m_pa = pa;
    rLay->addWidget(bna, 0, 0);
    rLay->addWidget(bns, 0, 1);
    rLay->addWidget(bpa, 0, 2);
    play->addWidget(rowW);

    // Calc button — identik Python
    auto *btn = calcBtn("Hitung Peluang");
    connect(btn, &QPushButton::clicked, this, &PeluangPage::calculate);
    play->addWidget(btn);

    lay->addWidget(panel);
    lay->addSpacing(20);

    // Result + PieChart row — identik Python layout
    auto *resRow = new QWidget();
    auto *resLay = new QHBoxLayout(resRow);
    resLay->setContentsMargins(0,0,0,0);
    resLay->setSpacing(20);

    m_result = new QLabel("Masukkan n(A) dan n(S), atau gunakan reverse solving.");
    m_result->setObjectName("resultLabel");
    m_result->setWordWrap(true);
    m_result->setTextFormat(Qt::RichText);

    m_pie = new ProbabilityPieChart();

    resLay->addWidget(m_result, 3);
    resLay->addWidget(m_pie, 0);
    lay->addWidget(resRow);
    lay->addSpacing(20);

    // ProbabilityBar — identik Python
    m_bar = new ProbabilityBar();
    m_bar->setFixedHeight(80);
    lay->addWidget(m_bar);
    lay->addStretch();

    scroll->setWidget(content);
    main->addWidget(scroll);

    // Shortcuts — identik Python
    for (auto *inp : {m_na, m_ns, m_pa})
        connect(inp, &QLineEdit::returnPressed, this, &PeluangPage::calculate);
    connect(new QShortcut(QKeySequence("Ctrl+R"), this),
            &QShortcut::activated, this, &PeluangPage::reset);
    connect(new QShortcut(QKeySequence(Qt::Key_Escape), this),
            &QShortcut::activated, this, [this](){ m_mw->backToDashboard(3); });

    updateStyles();
}

void PeluangPage::reset()
{
    m_na->clear(); m_ns->clear(); m_pa->clear();
    m_result->setText("Masukkan n(A) dan n(S), atau gunakan reverse solving.");
    m_result->setStyleSheet("");
    m_pie->setValues(0, 1);
    m_bar->setValue(0);
}

void PeluangPage::updateStyles()
{
    if (m_mw) {
        setStyleSheet(pageStylesheet(T::css(m_mw->isDark())));
        m_pie->isDark = m_mw->isDark(); m_pie->update();
        m_bar->isDark = m_mw->isDark(); m_bar->update();
    }
}

// ── Interpretasi identik Python _interpret() ──
QString PeluangPage::interpret(double pct)
{
    if (pct == 0) return "Kejadian ini mustahil terjadi.";
    if (pct <= 20) return "Kejadian ini sangat jarang terjadi.";
    if (pct <= 40) return "Kejadian ini kurang sering terjadi.";
    if (pct <= 60) return "Kejadian ini memiliki kemungkinan seimbang.";
    if (pct <= 80) return "Kejadian ini cukup sering terjadi.";
    if (pct < 100) return "Kejadian ini sangat sering terjadi.";
    return "Kejadian ini pasti selalu terjadi.";
}

// ── Reverse solving identik Python calculate() ──
void PeluangPage::calculate()
{
    ThemeColors t = T::css(m_mw ? m_mw->isDark() : true);
    try {
        QString naStr = m_na->text().trimmed();
        QString nsStr = m_ns->text().trimmed();
        QString paStr = m_pa->text().trimmed();

        // Parse P(A) — bisa persen atau desimal
        std::optional<double> paVal;
        if (!paStr.isEmpty()) {
            if (paStr.endsWith('%')) {
                auto pv = MathUtils::parseFloat(paStr.left(paStr.length() - 1));
                if (pv.has_value()) paVal = *pv / 100.0;
            } else {
                paVal = MathUtils::parseFloat(paStr);
                if (paVal.has_value() && *paVal > 1.0 && *paVal <= 100.0)
                    paVal = *paVal / 100.0;
            }
            if (paVal.has_value() && (*paVal < 0 || *paVal > 1))
                throw std::runtime_error("P(A) harus antara 0 dan 1 (atau 0% - 100%).");
        }

        auto naOpt = MathUtils::parseFloat(naStr);
        auto nsOpt = MathUtils::parseFloat(nsStr);

        // Hitung berapa yang kosong
        QStringList emptyList;
        if (!naOpt.has_value()) emptyList << "n(A)";
        if (!nsOpt.has_value()) emptyList << "n(S)";
        if (!paVal.has_value()) emptyList << "P(A)";

        if (emptyList.size() > 1) {
            resultErr(m_result,
                QString("Isi minimal 2 dari 3 nilai. Saat ini kosong: %1.").arg(emptyList.join(", ")), t);
            return;
        }

        double pResult, naFinal, nsFinal;
        QString fracStr;

        if (emptyList.size() == 0) {
            // Semua terisi → verifikasi
            double expected = (std::abs(*nsOpt) > 1e-12) ? *naOpt / *nsOpt : 0;
            if (std::abs(expected - *paVal) > 1e-6) {
                resultErr(m_result, "Semua terisi tapi tidak konsisten.", t);
                return;
            }
            pResult = *paVal;
            naFinal = *naOpt;
            nsFinal = *nsOpt;
        } else if (emptyList.contains("P(A)")) {
            // Cari P(A) dari n(A) dan n(S)
            if (*nsOpt <= 0) throw std::runtime_error("n(S) harus lebih dari 0.");
            if (*naOpt < 0) throw std::runtime_error("n(A) tidak boleh negatif.");
            if (*naOpt > *nsOpt) throw std::runtime_error("n(A) tidak boleh lebih besar dari n(S).");
            pResult = *naOpt / *nsOpt;
            naFinal = *naOpt;
            nsFinal = *nsOpt;
        } else if (emptyList.contains("n(A)")) {
            // Cari n(A) dari n(S) dan P(A)
            if (*nsOpt <= 0) throw std::runtime_error("n(S) harus lebih dari 0.");
            naFinal = *paVal * *nsOpt;
            nsFinal = *nsOpt;
            pResult = *paVal;
        } else {
            // Cari n(S) dari n(A) dan P(A)
            if (!paVal.has_value() || std::abs(*paVal) < 1e-12)
                throw std::runtime_error("P(A) harus lebih dari 0 untuk mencari n(S).");
            if (*naOpt < 0) throw std::runtime_error("n(A) tidak boleh negatif.");
            nsFinal = *naOpt / *paVal;
            naFinal = *naOpt;
            pResult = *paVal;
        }

        int naInt = static_cast<int>(std::round(naFinal));
        int nsInt = static_cast<int>(std::round(nsFinal));
        fracStr = MathUtils::toFrac(pResult);
        double pct = pResult * 100.0;
        QString interpretation = interpret(pct);

        // Mode string — identik Python
        QString modeStr;
        if (emptyList.contains("n(A)")) modeStr = " (Reverse: mencari n(A))";
        else if (emptyList.contains("n(S)")) modeStr = " (Reverse: mencari n(S))";

        // HTML identik Python
        QString html = htmlTitle(QString("Hasil Peluang%1").arg(modeStr), t);
        html += htmlRow("<b>Diketahui:</b>",
            QString("n(A) = %1,  n(S) = %2,  P(A) = %3")
                .arg(naInt).arg(nsInt).arg(fracStr), t);
        html += htmlRow("<b>Rumus:</b>", "P(A) = n(A) dibagi n(S)", t);
        html += htmlDiv(t);
        html += htmlRow("<b>Substitusi:</b>",
            QString("P(A) = %1 dibagi %2 = %3").arg(naInt).arg(nsInt).arg(fracStr), t);
        html += htmlRow("<b>Desimal:</b>",
            QString("P(A) = %1").arg(QString::number(pResult, 'f', 6)), t);
        html += htmlDiv(t);
        html += htmlBig(QString("P(A) = %1%").arg(QString::number(pct, 'f', 2)), t);
        html += htmlRow("<b>Interpretasi:</b>",
            QString("<i>%1</i>").arg(interpretation), t);

        resultOk(m_result, html, t);
        m_pie->setValues(naFinal, nsFinal);
        m_bar->setValue(pResult);

    } catch (const std::runtime_error &e) {
        resultErr(m_result, QString::fromStdString(e.what()), t);
    } catch (...) {
        resultErr(m_result, "Terjadi kesalahan.", t);
    }
}
