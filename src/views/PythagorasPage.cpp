/*
 * PythagorasPage.cpp — Halaman Teorema Pythagoras
 * Padanan Python: class PythagorasPage (baris 3503-3647 di main.py)
 * IDENTIK PYTHON — 3 input kosongkan 1, badges "AKAN DIHITUNG", triangle di panel
 */
#include "views/PythagorasPage.h"
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
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <cmath>

// ══════════════════ TriangleDiagram ══════════════════
// Identik Python TriangleDiagram (baris 3452-3500)

PythagorasPage::TriangleDiagram::TriangleDiagram(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(220, 220);
}

void PythagorasPage::TriangleDiagram::setSides(double a, double b, double c, const QString &missing)
{
    m_a = a; m_b = b; m_c = c; m_missing = missing;
    update();
}

void PythagorasPage::TriangleDiagram::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    int W = width(), H = height();
    QColor col(0xC9, 0xA8, 0x4C, 180);
    QColor gold(0xC9, 0xA8, 0x4C);
    QColor dimCol = isDark ? QColor(0xA0, 0xA0, 0xB8) : QColor(0x4A, 0x4A, 0x6A);
    QColor fillCol(0xC9, 0xA8, 0x4C, 20);

    int margin = 30;
    int bx = margin, by = H - margin;
    int tx = margin, ty = margin + 30;
    int rx = W - margin, ry = H - margin;

    // Fill triangle
    QPainterPath path;
    path.moveTo(bx, by); path.lineTo(rx, ry); path.lineTo(tx, ty); path.closeSubpath();
    p.setPen(Qt::NoPen); p.setBrush(fillCol); p.drawPath(path);

    // Draw sides
    struct Side { QString name; int x1, y1, x2, y2; };
    Side sides[] = {
        {"a", tx, ty, bx, by},
        {"b", bx, by, rx, ry},
        {"c", tx, ty, rx, ry}
    };
    for (auto &s : sides) {
        QPen pen;
        if (s.name == m_missing) {
            pen = QPen(gold, 2.5, Qt::DashLine);
        } else {
            pen = QPen(col, 2);
        }
        p.setPen(pen);
        p.drawLine(s.x1, s.y1, s.x2, s.y2);
    }

    // Right angle square
    p.setPen(QPen(col, 1.2)); p.setBrush(Qt::NoBrush);
    p.drawRect(bx, by - 12, 12, 12);

    // Labels
    p.setFont(QFont("Segoe UI", 11, QFont::Bold));
    struct Pos { QString name; int lx, ly; };
    Pos positions[] = {
        {"a", tx - 20, (ty + by) / 2},
        {"b", (bx + rx) / 2, by + 18},
        {"c", (tx + rx) / 2 + 10, (ty + ry) / 2 - 10}
    };
    double vals[] = {m_a, m_b, m_c};
    for (int i = 0; i < 3; ++i) {
        if (positions[i].name == m_missing) {
            p.setPen(gold);
            p.drawText(positions[i].lx, positions[i].ly,
                       QString("%1 = ?").arg(positions[i].name));
        } else if (vals[i] > 0) {
            p.setPen(dimCol);
            p.drawText(positions[i].lx, positions[i].ly,
                       QString("%1=%2").arg(positions[i].name, MathUtils::disp(vals[i])));
        } else {
            p.setPen(dimCol);
            p.drawText(positions[i].lx, positions[i].ly, positions[i].name);
        }
    }
}

// ══════════════════ PythagorasPage ══════════════════

PythagorasPage::PythagorasPage(MainWindow *parent)
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
    connect(back, &QPushButton::clicked, this, [this]() { m_mw->backToDashboard(5); });
    lay->addWidget(back, 0, Qt::AlignLeft);
    lay->addSpacing(18);
    lay->addWidget(sectionHeader("Teorema Pythagoras", "a pangkat 2 + b pangkat 2 = c pangkat 2"));
    lay->addSpacing(20);

    // Panel — identik Python
    auto *panel = new QFrame();
    panel->setObjectName("panel");
    auto *play = new QVBoxLayout(panel);
    play->setContentsMargins(24, 22, 24, 22);
    play->setSpacing(16);

    // Hint — identik Python
    auto *hint = new QLabel("Kosongkan 1 sisi yang ingin dicari — isi 2 sisi lainnya");
    hint->setObjectName("hintLabel");
    play->addWidget(hint);

    // Main row: inputs (left) + triangle (right) — identik Python
    auto *mainRow = new QWidget();
    auto *mrLay = new QHBoxLayout(mainRow);
    mrLay->setContentsMargins(0, 0, 0, 0);
    mrLay->setSpacing(20);

    // Input column
    auto *inpW = new QWidget();
    auto *inpLay = new QVBoxLayout(inpW);
    inpLay->setContentsMargins(0, 0, 0, 0);
    inpLay->setSpacing(14);

    // 3 inputs with badges — identik Python
    auto [ba, aIn] = inputBox("SISI a (TEGAK)", "kosongkan jika ingin dicari");
    auto [bb, bIn] = inputBox("SISI b (ALAS)", "kosongkan jika ingin dicari");
    auto [bc, cIn] = inputBox("SISI c (MIRING / HIPOTENUSA)", "kosongkan jika ingin dicari");
    m_sisiA = aIn; m_sisiB = bIn; m_sisiC = cIn;

    // Badge style — identik Python
    QString badgeStyle = "font-size:10px;font-weight:700;color:#C9A84C;"
                         "background:rgba(201,168,76,0.15);border-radius:4px;padding:2px 6px;";
    m_badgeA = new QLabel(""); m_badgeA->setStyleSheet(badgeStyle);
    m_badgeB = new QLabel(""); m_badgeB->setStyleSheet(badgeStyle);
    m_badgeC = new QLabel(""); m_badgeC->setStyleSheet(badgeStyle);

    struct { QWidget *box; QLabel *badge; } pairs[] = {{ba, m_badgeA}, {bb, m_badgeB}, {bc, m_badgeC}};
    for (auto &pr : pairs) {
        auto *row = new QWidget();
        auto *rl = new QHBoxLayout(row);
        rl->setContentsMargins(0, 0, 0, 0);
        rl->setSpacing(8);
        rl->addWidget(pr.box, 1);
        rl->addWidget(pr.badge, 0);
        inpLay->addWidget(row);
    }

    // Connect textChanged → updateBadges
    for (auto *inp : {m_sisiA, m_sisiB, m_sisiC})
        connect(inp, &QLineEdit::textChanged, this, &PythagorasPage::updateBadges);

    mrLay->addWidget(inpW, 3);

    // Triangle diagram — identik Python
    m_triangle = new TriangleDiagram();
    mrLay->addWidget(m_triangle, 0);

    play->addWidget(mainRow);

    // Calc button — identik Python
    auto *btn = calcBtn("Hitung Sisi yang Kosong");
    connect(btn, &QPushButton::clicked, this, &PythagorasPage::calculate);
    play->addWidget(btn);

    lay->addWidget(panel);
    lay->addSpacing(20);

    // Result — identik Python
    m_result = new QLabel("Kosongkan 1 sisi yang ingin dicari, isi 2 sisi lainnya.");
    m_result->setObjectName("resultLabel");
    m_result->setWordWrap(true);
    m_result->setTextFormat(Qt::RichText);
    lay->addWidget(m_result);
    lay->addStretch();

    scroll->setWidget(content);
    main->addWidget(scroll);

    // Shortcuts
    for (auto *inp : {m_sisiA, m_sisiB, m_sisiC})
        connect(inp, &QLineEdit::returnPressed, this, &PythagorasPage::calculate);
    connect(new QShortcut(QKeySequence("Ctrl+R"), this), &QShortcut::activated,
            this, &PythagorasPage::reset);
    connect(new QShortcut(QKeySequence(Qt::Key_Escape), this), &QShortcut::activated,
            this, [this]() { m_mw->backToDashboard(5); });

    updateStyles();
}

// ── Badges identik Python _update_badges() ──
void PythagorasPage::updateBadges()
{
    QString at = m_sisiA->text().trimmed();
    QString bt = m_sisiB->text().trimmed();
    QString ct = m_sisiC->text().trimmed();
    int filled = (!at.isEmpty() ? 1 : 0) + (!bt.isEmpty() ? 1 : 0) + (!ct.isEmpty() ? 1 : 0);

    m_badgeA->setText(at.isEmpty() && filled == 2 ? "AKAN DIHITUNG" : "");
    m_badgeB->setText(bt.isEmpty() && filled == 2 ? "AKAN DIHITUNG" : "");
    m_badgeC->setText(ct.isEmpty() && filled == 2 ? "AKAN DIHITUNG" : "");

    // Update triangle preview
    auto sf = [](const QString &s) -> double {
        if (s.isEmpty()) return -1;
        bool ok; double v = s.toDouble(&ok);
        return ok ? v : -1;
    };
    QString missing;
    if (at.isEmpty() && filled == 2) missing = "a";
    else if (bt.isEmpty() && filled == 2) missing = "b";
    else if (ct.isEmpty() && filled == 2) missing = "c";
    m_triangle->setSides(sf(at), sf(bt), sf(ct), missing);
}

void PythagorasPage::reset()
{
    m_sisiA->clear(); m_sisiB->clear(); m_sisiC->clear();
    m_result->setText("Kosongkan 1 sisi yang ingin dicari, isi 2 sisi lainnya.");
    m_result->setStyleSheet("");
    m_triangle->setSides();
    updateBadges();
}

void PythagorasPage::updateStyles()
{
    if (m_mw) {
        setStyleSheet(pageStylesheet(T::css(m_mw->isDark())));
        m_triangle->isDark = m_mw->isDark();
        m_triangle->update();
    }
}

// ── Calculate identik Python ──
void PythagorasPage::calculate()
{
    ThemeColors t = T::css(m_mw ? m_mw->isDark() : true);
    auto sf = [](const QString &s) -> std::optional<double> {
        return MathUtils::parseFloat(s);
    };
    auto ok = [](const std::optional<double> &v) { return v.has_value(); };

    try {
        auto aOpt = sf(m_sisiA->text());
        auto bOpt = sf(m_sisiB->text());
        auto cOpt = sf(m_sisiC->text());

        int emptyCount = (!ok(aOpt) ? 1 : 0) + (!ok(bOpt) ? 1 : 0) + (!ok(cOpt) ? 1 : 0);

        if (emptyCount > 1) {
            resultErr(m_result,
                QString("Tepat <b>1 sisi</b> harus dikosongkan. Saat ini <b>%1 sisi</b> kosong.").arg(emptyCount), t);
            return;
        }
        if (emptyCount == 0) {
            resultErr(m_result, "Semua sisi terisi. <b>Kosongkan 1 sisi</b> yang ingin dicari.", t);
            return;
        }

        // Validasi nilai positif
        for (auto &[name, val] : std::initializer_list<std::pair<QString, std::optional<double>>>{
                {"a", aOpt}, {"b", bOpt}, {"c", cOpt}}) {
            if (ok(val) && *val <= 0) {
                resultErr(m_result, QString("Nilai <b>%1</b> harus lebih dari 0.").arg(name), t);
                return;
            }
        }

        auto row = [&](const QString &lbl, const QString &val) { return htmlRow(lbl, val, t); };
        auto title = [&](const QString &txt) { return htmlTitle(txt, t); };
        auto div = [&]() { return htmlDiv(t); };

        // Verifikasi dan Tripel helper
        auto verifyAndTriple = [&](double a, double b, double c) -> QString {
            QString html = div();
            html += row("<b>Verifikasi Pythagoras:</b>", "");
            html += row("", QString("a² + b² = %1² + %2² = %3 + %4 = %5")
                .arg(MathUtils::disp(a), MathUtils::disp(b),
                     MathUtils::disp(a*a), MathUtils::disp(b*b), MathUtils::disp(a*a + b*b)));
            html += row("", QString("c² = %1² = %2").arg(MathUtils::disp(c), MathUtils::disp(c*c)));
            if (std::abs(a*a + b*b - c*c) < 1e-6) {
                html += row("", "<span style='color:#22C55E;font-weight:700'>a² + b² = c²  ✓  Terbukti!</span>");
            }
            // Tripel check
            if (std::abs(a - std::round(a)) < 1e-9 && std::abs(b - std::round(b)) < 1e-9 &&
                std::abs(c - std::round(c)) < 1e-9) {
                int ai = static_cast<int>(std::round(a));
                int bi = static_cast<int>(std::round(b));
                int ci = static_cast<int>(std::round(c));
                if (ai*ai + bi*bi == ci*ci) {
                    html += QString("<p style='font-size:14px;font-weight:700;color:#C9A84C;"
                        "background:rgba(201,168,76,0.12);padding:8px 12px;border-radius:8px;"
                        "margin:8px 0'>Tripel Pythagoras! (%1-%2-%3)</p>")
                        .arg(ai).arg(bi).arg(ci);
                }
            }
            return html;
        };

        if (!ok(cOpt)) {
            // Cari c
            double a = *aOpt, b = *bOpt;
            double a2 = a*a, b2 = b*b, c2 = a2 + b2;
            double cVal = std::sqrt(c2);

            QString html = title("Pythagoras  -  Cari c (sisi miring)");
            html += row("<b>Diketahui:</b>", QString("a = %1,  b = %2").arg(MathUtils::disp(a), MathUtils::disp(b)));
            html += row("<b>Ditanya:</b>", "Sisi miring (c)");
            html += div();
            html += row("<b>Rumus:</b>", "c² = a² + b²");
            html += row("<b>Substitusi:</b>", QString("c² = %1² + %2²").arg(MathUtils::disp(a), MathUtils::disp(b)));
            html += row("<b>Hitung:</b>", QString("c² = %1 + %2 = %3").arg(MathUtils::disp(a2), MathUtils::disp(b2), MathUtils::disp(c2)));
            html += row("<b>Aljabar:</b>", QString("c = √(%1)").arg(MathUtils::disp(c2)));
            html += div();
            html += MathUtils::sqrtHtml(c2, "c", t.success, t.text_mid);
            html += verifyAndTriple(a, b, cVal);
            resultOk(m_result, html, t);
            m_triangle->setSides(a, b, cVal, "c");

        } else if (!ok(bOpt)) {
            // Cari b
            double a = *aOpt, c = *cOpt;
            if (c <= a) {
                resultErr(m_result, QString("Hipotenusa (c) harus lebih besar dari kedua sisi lainnya. c=%1 <= a=%2.")
                    .arg(MathUtils::disp(c), MathUtils::disp(a)), t);
                return;
            }
            double a2 = a*a, c2 = c*c, b2 = c2 - a2;
            if (b2 < 0) { resultErr(m_result, "b² kurang dari 0. Kombinasi tidak valid.", t); return; }
            double bVal = std::sqrt(b2);

            QString html = title("Pythagoras  -  Cari b");
            html += row("<b>Diketahui:</b>", QString("a = %1,  c = %2").arg(MathUtils::disp(a), MathUtils::disp(c)));
            html += row("<b>Ditanya:</b>", "Sisi alas (b)");
            html += div();
            html += row("<b>Rumus:</b>", "b² = c² - a²");
            html += row("<b>Substitusi:</b>", QString("b² = %1² - %2²").arg(MathUtils::disp(c), MathUtils::disp(a)));
            html += row("<b>Hitung:</b>", QString("b² = %1 - %2 = %3").arg(MathUtils::disp(c2), MathUtils::disp(a2), MathUtils::disp(b2)));
            html += row("<b>Aljabar:</b>", QString("b = √(%1)").arg(MathUtils::disp(b2)));
            html += div();
            html += MathUtils::sqrtHtml(b2, "b", t.success, t.text_mid);
            html += verifyAndTriple(a, bVal, c);
            resultOk(m_result, html, t);
            m_triangle->setSides(a, bVal, c, "b");

        } else if (!ok(aOpt)) {
            // Cari a
            double b = *bOpt, c = *cOpt;
            if (c <= b) {
                resultErr(m_result, QString("Hipotenusa (c) harus lebih besar dari kedua sisi lainnya. c=%1 <= b=%2.")
                    .arg(MathUtils::disp(c), MathUtils::disp(b)), t);
                return;
            }
            double b2 = b*b, c2 = c*c, a2 = c2 - b2;
            if (a2 < 0) { resultErr(m_result, "a² kurang dari 0. Kombinasi tidak valid.", t); return; }
            double aVal = std::sqrt(a2);

            QString html = title("Pythagoras  -  Cari a");
            html += row("<b>Diketahui:</b>", QString("b = %1,  c = %2").arg(MathUtils::disp(b), MathUtils::disp(c)));
            html += row("<b>Ditanya:</b>", "Sisi tegak (a)");
            html += div();
            html += row("<b>Rumus:</b>", "a² = c² - b²");
            html += row("<b>Substitusi:</b>", QString("a² = %1² - %2²").arg(MathUtils::disp(c), MathUtils::disp(b)));
            html += row("<b>Hitung:</b>", QString("a² = %1 - %2 = %3").arg(MathUtils::disp(c2), MathUtils::disp(b2), MathUtils::disp(a2)));
            html += row("<b>Aljabar:</b>", QString("a = √(%1)").arg(MathUtils::disp(a2)));
            html += div();
            html += MathUtils::sqrtHtml(a2, "a", t.success, t.text_mid);
            html += verifyAndTriple(aVal, b, c);
            resultOk(m_result, html, t);
            m_triangle->setSides(aVal, b, c, "a");
        }

    } catch (const std::runtime_error &e) {
        resultErr(m_result, QString::fromStdString(e.what()), t);
    } catch (...) {
        resultErr(m_result, "Terjadi kesalahan.", t);
    }
}
