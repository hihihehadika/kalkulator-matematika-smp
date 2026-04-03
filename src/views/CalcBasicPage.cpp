/*
 * CalcBasicPage.cpp — Halaman Kalkulator Basic + Fraksi
 * Padanan Python: class KalkulatorBasicPage (baris 3774-3866 di main.py)
 * IDENTIK PYTHON — margins 48,32,48,40, operator buttons, vertical layout
 */
#include "views/CalcBasicPage.h"
#include "MainWindow.h"
#include "core/Theme.h"
#include "core/MathUtils.h"
#include "helpers/PageHelpers.h"
#include "helpers/ResultRenderer.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QPushButton>
#include <QShortcut>
#include <QJSEngine>
#include <QJSValue>
#include <QRegularExpression>
#include <cmath>

CalcBasicPage::CalcBasicPage(MainWindow *parent)
    : QWidget(parent), m_mw(parent)
{
    m_js = new QJSEngine(this);

    auto *main = new QVBoxLayout(this);
    main->setContentsMargins(0, 0, 0, 0);

    auto *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto *content = new QWidget();
    auto *lay = new QVBoxLayout(content);
    // Margins identik Python: 48, 32, 48, 40
    lay->setContentsMargins(48, 32, 48, 40);
    lay->setSpacing(0);

    auto *back = makeBackBtn();
    connect(back, &QPushButton::clicked, this, [this]() { m_mw->backToDashboard(7); });
    lay->addWidget(back, 0, Qt::AlignLeft);
    lay->addSpacing(18);
    lay->addWidget(sectionHeader("Kalkulator Basic", "Penjumlahan, Pengurangan, Perkalian, Pembagian, Pecahan"));
    lay->addSpacing(20);

    // Panel — identik Python
    auto *panel = new QFrame();
    panel->setObjectName("panel");
    auto *play = new QVBoxLayout(panel);
    play->setContentsMargins(24, 22, 24, 22);
    play->setSpacing(14);

    // Hint — identik Python
    auto *hint = new QLabel("Ketik ekspresi matematis. Gunakan titik (.) untuk desimal, garis miring (/) untuk pecahan.");
    hint->setObjectName("hintLabel");
    hint->setWordWrap(true);
    play->addWidget(hint);

    // Input — identik Python
    auto [bexpr, exprIn] = inputBox("Ekspresi", "contoh: 1/2 + 3/4  atau  2.5 * 3 - 0.5");
    m_exprIn = exprIn;
    play->addWidget(bexpr);

    // Operator buttons row — identik Python
    auto *opsRow = new QWidget();
    auto *opsLay = new QHBoxLayout(opsRow);
    opsLay->setContentsMargins(0, 0, 0, 0);
    opsLay->setSpacing(8);

    QStringList symbols = {"+", "-", "x", ":", ".", "(", ")", "/"};
    for (const auto &sym : symbols) {
        auto *opBtn = new QPushButton(sym);
        opBtn->setFixedSize(44, 36);
        opBtn->setFocusPolicy(Qt::NoFocus);
        opBtn->setStyleSheet("font-size:16px;font-weight:700;border-radius:8px;");
        // Map display symbol to actual character
        QString actual = sym;
        if (sym == "x") actual = "*";
        else if (sym == ":") actual = "/";
        connect(opBtn, &QPushButton::clicked, this, [this, actual]() { insertOp(actual); });
        opsLay->addWidget(opBtn);
    }
    opsLay->addStretch();
    play->addWidget(opsRow);

    // Calc button — identik Python
    auto *btn = calcBtn("Hitung");
    connect(btn, &QPushButton::clicked, this, &CalcBasicPage::calculate);
    play->addWidget(btn);

    lay->addWidget(panel);
    lay->addSpacing(16);

    // Result — identik Python (vertical, bukan side-by-side)
    m_result = new QLabel("Masukkan ekspresi untuk dihitung.");
    m_result->setObjectName("resultLabel");
    m_result->setWordWrap(true);
    m_result->setTextFormat(Qt::RichText);
    lay->addWidget(m_result);

    // History label — identik Python
    auto *histLabel = new QLabel("Riwayat terakhir:");
    histLabel->setObjectName("hintLabel");
    lay->addSpacing(16);
    lay->addWidget(histLabel);

    // History result — identik Python
    m_histResult = new QLabel("Belum ada riwayat.");
    m_histResult->setObjectName("resultLabel");
    m_histResult->setWordWrap(true);
    m_histResult->setTextFormat(Qt::RichText);
    lay->addWidget(m_histResult);
    lay->addStretch();

    scroll->setWidget(content);
    main->addWidget(scroll);

    connect(m_exprIn, &QLineEdit::returnPressed, this, &CalcBasicPage::calculate);
    connect(new QShortcut(QKeySequence("Ctrl+R"), this), &QShortcut::activated,
            this, &CalcBasicPage::reset);
    connect(new QShortcut(QKeySequence(Qt::Key_Escape), this), &QShortcut::activated,
            this, [this]() { m_mw->backToDashboard(7); });

    updateStyles();
}

CalcBasicPage::~CalcBasicPage() {}

void CalcBasicPage::insertOp(const QString &op)
{
    m_exprIn->insert(op);
    m_exprIn->setFocus();
}

void CalcBasicPage::reset()
{
    m_exprIn->clear();
    m_result->setText("Masukkan ekspresi untuk dihitung.");
    m_result->setStyleSheet("");
    m_histList.clear();
    m_histResult->setText("Belum ada riwayat.");
    m_histResult->setStyleSheet("");
}

void CalcBasicPage::updateStyles()
{
    if (m_mw) {
        ThemeColors t = T::css(m_mw->isDark());
        setStyleSheet(pageStylesheet(t));
        resultHistory(m_histResult, m_histList, t);
    }
}

QString CalcBasicPage::solveDirectEval(const QString &expr)
{
    // ── Validasi regex identik Python ──
    QRegularExpression re("^[\\d\\s\\+\\-\\*/\\.\\(\\)]+$");
    if (!re.match(expr).hasMatch() || expr.contains("**"))
        throw std::runtime_error("Karakter tidak valid dalam ekspresi.");

    QJSValue val = m_js->evaluate(expr);
    if (val.isError())
        throw std::runtime_error("Ekspresi tidak valid (Syntax Error).");
    if (!val.isNumber())
        throw std::runtime_error("Hasil bukan berupa angka matematis.");
    double num = val.toNumber();
    if (std::isinf(num) || std::isnan(num))
        throw std::runtime_error("Pembagian dengan nol tidak diperbolehkan.");
    return MathUtils::disp(num);
}

QString CalcBasicPage::solveFractionFallback(const QString &expr)
{
    // Validasi keamanan — sama dengan solveDirectEval
    QRegularExpression re("^[\\d\\s\\+\\-\\*/\\.\\(\\)]+$");
    if (!re.match(expr).hasMatch())
        throw std::runtime_error("Karakter tidak valid dalam ekspresi.");

    QStringList parts = expr.split('+');
    double total = 0;
    for (const auto &p : parts) {
        if (p.contains('/')) {
            QStringList p2 = p.split('/');
            if (p2.size() == 2) {
                double den = p2[1].trimmed().toDouble();
                if (std::abs(den) < 1e-14)
                    throw std::runtime_error("Pembagian dengan nol tidak diperbolehkan.");
                total += p2[0].trimmed().toDouble() / den;
            }
        } else {
            total += p.trimmed().toDouble();
        }
    }
    if (std::isinf(total) || std::isnan(total))
        throw std::runtime_error("Pembagian dengan nol tidak diperbolehkan.");
    auto frac = SimpleFraction::fromDouble(total, 5000);
    if (frac.den != 1) {
        QString fr = QString("%1/%2").arg(frac.num).arg(frac.den);
        return QString("%1<br><span style='font-size:12px;color:#A0A0B0'>(≈ %2)</span>")
               .arg(fr, MathUtils::disp(total));
    }
    return MathUtils::disp(total);
}

void CalcBasicPage::calculate()
{
    ThemeColors t = T::css(m_mw ? m_mw->isDark() : true);
    QString expr = m_exprIn->text().trimmed();
    if (expr.isEmpty()) return;

    try {
        QString finalStr;
        if (expr.contains('/') && !expr.contains('*') && !expr.contains('('))
            finalStr = solveFractionFallback(expr);
        else
            finalStr = solveDirectEval(expr);

        QString html = htmlTitle("Hasil Perhitungan", t);
        html += htmlRow("<b>Ekspresi:</b>", QString("<code>%1</code>").arg(expr), t);
        html += htmlDiv(t);
        html += htmlBig(QString("= %1").arg(finalStr), t);

        resultOk(m_result, html, t);

        // History — identik Python
        QString clearStr = finalStr.split("<br>").first();
        QString he = QString("%1 = %2").arg(expr, clearStr);
        if (m_histList.isEmpty() || m_histList.last() != he) {
            m_histList.append(he);
            if (m_histList.size() > 5) m_histList.removeFirst();
            resultHistory(m_histResult, m_histList, t);
        }

    } catch (const std::runtime_error &e) {
        resultErr(m_result, QString::fromStdString(e.what()), t);
    } catch (...) {
        resultErr(m_result, "Terjadi kesalahan pada parser matematika.", t);
    }
}
