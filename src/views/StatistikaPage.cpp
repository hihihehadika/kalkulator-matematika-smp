/*
 * StatistikaPage.cpp — Halaman Statistika Dasar
 * Padanan Python: class StatistikaPage (baris 3654-3768 di main.py)
 * IDENTIK PYTHON — count label, known_mean, ?, variansi, simpangan baku, colored cards
 */
#include "views/StatistikaPage.h"
#include "MainWindow.h"
#include "core/Theme.h"
#include "core/MathUtils.h"
#include "helpers/PageHelpers.h"
#include "helpers/ResultRenderer.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QShortcut>
#include <QStringList>
#include <QVector>
#include <QMap>
#include <algorithm>
#include <cmath>

StatistikaPage::StatistikaPage(MainWindow *parent)
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
    connect(back, &QPushButton::clicked, this, [this]() { m_mw->backToDashboard(6); });
    lay->addWidget(back, 0, Qt::AlignLeft);
    lay->addSpacing(18);
    lay->addWidget(sectionHeader("Statistika Dasar", "Rata-rata, Median, Modus, Jangkauan, Variansi"));
    lay->addSpacing(20);

    auto *panel = new QFrame();
    panel->setObjectName("panel");
    auto *play = new QVBoxLayout(panel);
    play->setContentsMargins(24, 22, 24, 22);
    play->setSpacing(16);

    // Hint — identik Python
    auto *hint = new QLabel("Masukkan data dipisahkan koma. Gunakan ? untuk data yang hilang.");
    hint->setObjectName("hintLabel");
    play->addWidget(hint);

    // Data input — identik Python (minimumHeight 52)
    auto [bd, dataIn] = inputBox("DATA", "5, 7, 3, 9, 5");
    m_data = dataIn;
    m_data->setMinimumHeight(52);
    play->addWidget(bd);

    // Count label — identik Python
    m_countLbl = new QLabel("Jumlah data terdeteksi: 0");
    m_countLbl->setStyleSheet("font-size:12px;color:#A0A0B8;background:transparent;");
    play->addWidget(m_countLbl);
    connect(m_data, &QLineEdit::textChanged, this, &StatistikaPage::updateCount);

    // Known mean input — identik Python
    auto [bm, meanIn] = inputBox("RATA-RATA YANG DIKETAHUI (OPSIONAL)", "isi jika ada data yang perlu dicari");
    m_knownMean = meanIn;
    play->addWidget(bm);

    // Button — identik Python
    auto *btn = calcBtn("Hitung Statistik");
    connect(btn, &QPushButton::clicked, this, &StatistikaPage::calculate);
    play->addWidget(btn);

    lay->addWidget(panel);
    lay->addSpacing(20);

    // Result — identik Python
    m_result = new QLabel("Masukkan data.");
    m_result->setObjectName("resultLabel");
    m_result->setWordWrap(true);
    m_result->setTextFormat(Qt::RichText);
    lay->addWidget(m_result);
    lay->addStretch();

    scroll->setWidget(content);
    main->addWidget(scroll);

    connect(m_data, &QLineEdit::returnPressed, this, &StatistikaPage::calculate);
    connect(m_knownMean, &QLineEdit::returnPressed, this, &StatistikaPage::calculate);
    connect(new QShortcut(QKeySequence("Ctrl+R"), this), &QShortcut::activated,
            this, &StatistikaPage::reset);
    connect(new QShortcut(QKeySequence(Qt::Key_Escape), this), &QShortcut::activated,
            this, [this]() { m_mw->backToDashboard(6); });

    updateStyles();
}

void StatistikaPage::updateCount()
{
    QString raw = m_data->text().trimmed();
    QStringList parts = raw.split(',', Qt::SkipEmptyParts);
    int count = 0;
    for (const auto &p : parts)
        if (!p.trimmed().isEmpty()) count++;
    m_countLbl->setText(QString("Jumlah data terdeteksi: %1").arg(count));
}

void StatistikaPage::reset()
{
    m_data->clear();
    m_knownMean->clear();
    m_result->setText("Masukkan data.");
    m_result->setStyleSheet("");
    m_countLbl->setText("Jumlah data terdeteksi: 0");
}

void StatistikaPage::updateStyles()
{
    if (m_mw) setStyleSheet(pageStylesheet(T::css(m_mw->isDark())));
}

void StatistikaPage::calculate()
{
    ThemeColors t = T::css(m_mw ? m_mw->isDark() : true);
    try {
        QString raw = m_data->text().trimmed();
        if (raw.isEmpty()) throw std::runtime_error("Data kosong.");

        QStringList parts = raw.split(',', Qt::SkipEmptyParts);
        // Cari index ?
        QVector<int> qIndices;
        for (int i = 0; i < parts.size(); ++i)
            if (parts[i].trimmed() == "?") qIndices.append(i);

        QString kmStr = m_knownMean->text().trimmed();
        bool foundMissing = false;
        double foundVal = 0;

        // Reverse solving dengan ? — identik Python
        if (qIndices.size() == 1 && !kmStr.isEmpty()) {
            auto kmOpt = MathUtils::parseFloat(kmStr, "Rata-rata", false);
            double knownMean = *kmOpt;
            int n = parts.size();
            double sumKnown = 0;
            for (int i = 0; i < parts.size(); ++i) {
                if (qIndices.contains(i)) continue;
                auto v = MathUtils::parseFloat(parts[i].trimmed(), "Data", false);
                sumKnown += *v;
            }
            double missingVal = knownMean * n - sumKnown;
            parts[qIndices[0]] = QString::number(missingVal);
            foundMissing = true;
            foundVal = missingVal;
        } else if (qIndices.size() > 1) {
            throw std::runtime_error("Hanya boleh ada 1 tanda ? dalam data.");
        } else if (qIndices.size() == 1 && kmStr.isEmpty()) {
            throw std::runtime_error("Ada data ? tapi rata-rata yang diketahui belum diisi.");
        }

        // Parse all data
        QVector<double> data;
        for (const auto &p : parts) {
            auto v = MathUtils::parseFloat(p.trimmed(), "Data", false);
            data.append(*v);
        }
        if (data.size() < 2) throw std::runtime_error("Masukkan minimal 2 data.");

        int n = data.size();
        double sum = 0;
        for (double v : data) sum += v;
        double meanVal = sum / n;

        QVector<double> sd = data;
        std::sort(sd.begin(), sd.end());

        // Median
        double medianVal;
        if (n % 2 == 0) medianVal = (sd[n/2 - 1] + sd[n/2]) / 2.0;
        else medianVal = sd[n/2];

        // Modus
        QMap<double, int> counts;
        for (double v : data) counts[v]++;
        int mx = 0;
        for (auto it = counts.begin(); it != counts.end(); ++it)
            if (it.value() > mx) mx = it.value();
        QVector<double> mv;
        for (auto it = counts.begin(); it != counts.end(); ++it)
            if (it.value() == mx) mv.append(it.key());
        QString modusText;
        // Jika semua nilai unik memiliki frekuensi teratas yang sama, maka tidak ada modus.
        if (mv.size() == counts.size() && counts.size() > 1) modusText = "Tidak ada modus";
        else {
            std::sort(mv.begin(), mv.end());
            QStringList ml;
            for (double m : mv) ml.append(MathUtils::disp(m));
            modusText = ml.join(", ");
        }

        // Jangkauan, Variansi, Simpangan Baku
        double jangkauan = sd.last() - sd.first();
        double variansi = 0;
        for (double x : data) variansi += (x - meanVal) * (x - meanVal);
        variansi /= n;
        double stdDev = std::sqrt(variansi);

        // ── HTML identik Python ──
        QString html = htmlTitle("Hasil Statistik Lengkap", t);

        if (foundMissing) {
            html += htmlRow("<b>Data hilang ditemukan:</b>",
                QString("? = %1 (dari rata-rata = %2)").arg(MathUtils::fmt2(foundVal), kmStr),
                t, true, t.primary);
        }

        QStringList sortedStr;
        for (double v : sd) sortedStr.append(MathUtils::disp(v));
        html += htmlRow("<b>Data (terurut):</b>", sortedStr.join(", "), t);
        html += htmlRow("<b>Jumlah data (n):</b>", QString::number(n), t);
        html += htmlDiv(t);

        // Colored stat cards — identik Python
        bool isDk = m_mw ? m_mw->isDark() : true;
        struct StatColor { QString txt, bg, bdr; };
        QVector<StatColor> statColors;
        QString lblColor;

        if (isDk) {
            statColors = {
                {"#60A5FA", "rgba(96,165,250,0.12)", "rgba(96,165,250,0.35)"},
                {"#A78BFA", "rgba(167,139,250,0.12)", "rgba(167,139,250,0.35)"},
                {"#FBBF24", "rgba(251,191,36,0.12)", "rgba(251,191,36,0.35)"},
                {"#34D399", "rgba(52,211,153,0.12)", "rgba(52,211,153,0.35)"},
                {"#FB7185", "rgba(251,113,133,0.12)", "rgba(251,113,133,0.35)"},
                {"#22D3EE", "rgba(34,211,238,0.12)", "rgba(34,211,238,0.35)"},
            };
            lblColor = "#D1D5DB";
        } else {
            statColors = {
                {"#2563EB", "rgba(37,99,235,0.08)", "rgba(37,99,235,0.25)"},
                {"#7C3AED", "rgba(124,58,237,0.08)", "rgba(124,58,237,0.25)"},
                {"#D97706", "rgba(217,119,6,0.08)", "rgba(217,119,6,0.25)"},
                {"#059669", "rgba(5,150,105,0.08)", "rgba(5,150,105,0.25)"},
                {"#E11D48", "rgba(225,29,72,0.08)", "rgba(225,29,72,0.25)"},
                {"#0891B2", "rgba(8,145,178,0.08)", "rgba(8,145,178,0.25)"},
            };
            lblColor = "#4B5563";
        }

        struct StatItem { QString label, value; int colorIdx; };
        QVector<StatItem> stats = {
            {"Rata-rata", MathUtils::fmt2(meanVal), 0},
            {"Median", MathUtils::fmt2(medianVal), 1},
            {"Modus", modusText, 2},
            {"Jangkauan", MathUtils::fmt2(jangkauan), 3},
            {"Variansi", MathUtils::fmt2(variansi), 4},
            {"Simpangan Baku", MathUtils::fmt2(stdDev), 5},
        };

        QString cardsHtml = "<table style='border-collapse:separate;border-spacing:10px 8px;margin:8px 0'><tr>";
        for (int i = 0; i < stats.size(); ++i) {
            auto &sc = statColors[stats[i].colorIdx];
            cardsHtml += QString(
                "<td style='min-width:140px;border:1.5px solid %1;border-radius:12px;"
                "padding:12px 16px;text-align:center;background:%2'>"
                "<div style='font-size:11px;font-weight:700;color:%3;text-transform:uppercase;"
                "letter-spacing:1px'>%4</div>"
                "<div style='font-size:18px;font-weight:700;color:%5;margin-top:4px'>%6</div></td>"
            ).arg(sc.bdr, sc.bg, lblColor, stats[i].label, sc.txt, stats[i].value);
            if (i == 2) cardsHtml += "</tr><tr>";
        }
        cardsHtml += "</tr></table>";
        html += cardsHtml;

        resultOk(m_result, html, t);

    } catch (const std::runtime_error &e) {
        resultErr(m_result, QString::fromStdString(e.what()), t);
    } catch (...) {
        resultErr(m_result, "Terjadi kesalahan.", t);
    }
}
