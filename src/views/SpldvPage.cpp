/*
 * SpldvPage.cpp — Halaman Sistem Persamaan Linear
 * Padanan Python: class SPLDVPage (baris 2643-3445 di main.py)
 * IDENTIK PYTHON — Mode Koefisien + Mode Soal Harga + Reverse Solving
 *                  + Step-by-step Gaussian Elimination + Verifikasi
 */
#include "views/SpldvPage.h"
#include "MainWindow.h"
#include "core/Theme.h"
#include "core/MathUtils.h"
#include "helpers/PageHelpers.h"
#include "helpers/ResultRenderer.h"

#include <QScrollArea>
#include <QFrame>
#include <QShortcut>
#include <QGridLayout>
#include <cmath>
#include <numeric>

// ═══════════════ Nama variabel identik Python ═══════════════
QStringList SpldvPage::varNames() const {
    QStringList names = {"x","y","z","w","p","q","r","s"};
    return names.mid(0, m_nKoef);
}

// ═══════════════ KONSTRUKTOR ═══════════════
SpldvPage::SpldvPage(MainWindow *parent)
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
    lay->setContentsMargins(48, 32, 48, 40);  // identik Python
    lay->setSpacing(0);

    auto *back = makeBackBtn();
    connect(back, &QPushButton::clicked, this, [this]() { m_mw->backToDashboard(4); });
    lay->addWidget(back, 0, Qt::AlignLeft);
    lay->addSpacing(18);
    lay->addWidget(sectionHeader("Sistem Persamaan Linear N Variabel",
        "Mendukung 2 s/d 3 variabel  |  Mode Koefisien & Soal Harga  |  Reverse Solving"));
    lay->addSpacing(20);

    // ── Mode selector row — identik Python ──
    auto *modeRow = new QWidget();
    auto *mlay = new QHBoxLayout(modeRow);
    mlay->setContentsMargins(0, 0, 0, 0);
    mlay->setSpacing(8);

    m_btnKoef = new QPushButton("Mode Koefisien");
    m_btnHarga = new QPushButton("Mode Soal Harga");
    for (auto *b : {m_btnKoef, m_btnHarga}) {
        b->setFocusPolicy(Qt::NoFocus);
        b->setCheckable(true);
    }
    m_btnKoef->setChecked(true);
    connect(m_btnKoef, &QPushButton::clicked, this, [this]() { switchMode("koef"); });
    connect(m_btnHarga, &QPushButton::clicked, this, [this]() { switchMode("harga"); });
    mlay->addWidget(m_btnKoef);
    mlay->addWidget(m_btnHarga);
    mlay->addStretch();
    lay->addWidget(modeRow);
    lay->addSpacing(12);

    // ══════════════ Panel Koefisien ══════════════
    m_panelKoef = new QFrame();
    m_panelKoef->setObjectName("panel");
    auto *pklay = new QVBoxLayout(m_panelKoef);
    pklay->setContentsMargins(24, 22, 24, 22);
    pklay->setSpacing(14);

    auto *hintKoef = new QLabel(
        "Masukkan koefisien persamaan. Mendukung desimal dan negatif.\n"
        "Kosongkan kolom 'Nilai Diketahui' untuk variabel yang ingin dicari (Reverse Solving).");
    hintKoef->setObjectName("hintLabel");
    hintKoef->setWordWrap(true);
    pklay->addWidget(hintKoef);

    // Var count control
    auto *varCtrl = new QWidget();
    auto *vclay = new QHBoxLayout(varCtrl);
    vclay->setContentsMargins(0, 0, 0, 0);
    vclay->setSpacing(8);
    m_lblNKoef = new QLabel(QString("Jumlah variabel: %1").arg(m_nKoef));
    m_lblNKoef->setObjectName("hintLabel");
    auto *btnAddK = new QPushButton("+ Tambah Variabel");
    auto *btnDelK = new QPushButton("- Kurangi Variabel");
    for (auto *b : {btnAddK, btnDelK}) {
        b->setFocusPolicy(Qt::NoFocus);
        b->setFixedHeight(34);
    }
    connect(btnAddK, &QPushButton::clicked, this, [this]() { changeVars(1, "koef"); });
    connect(btnDelK, &QPushButton::clicked, this, [this]() { changeVars(-1, "koef"); });
    vclay->addWidget(m_lblNKoef);
    vclay->addSpacing(12);
    vclay->addWidget(btnAddK);
    vclay->addWidget(btnDelK);
    vclay->addStretch();
    pklay->addWidget(varCtrl);

    m_koefGridContainer = new QWidget();
    pklay->addWidget(m_koefGridContainer);

    m_calcBtnKoef = calcBtn("Selesaikan Sistem Persamaan");
    connect(m_calcBtnKoef, &QPushButton::clicked, this, &SpldvPage::solveKoef);
    pklay->addWidget(m_calcBtnKoef);

    // ══════════════ Panel Harga ══════════════
    m_panelHarga = new QFrame();
    m_panelHarga->setObjectName("panel");
    m_panelHarga->hide();
    auto *phlay = new QVBoxLayout(m_panelHarga);
    phlay->setContentsMargins(24, 22, 24, 22);
    phlay->setSpacing(14);

    auto *hintHarga = new QLabel(
        "Masukkan nama barang, koefisien persamaan, dan total harga.\n"
        "Kosongkan 'Harga Diketahui' suatu barang untuk Reverse Solving.\n"
        "Format mata uang bebas: Rp, USD, EUR, atau kosong (tanpa satuan).");
    hintHarga->setObjectName("hintLabel");
    hintHarga->setWordWrap(true);
    phlay->addWidget(hintHarga);

    // Barang count control
    auto *barangCtrl = new QWidget();
    auto *bclay = new QHBoxLayout(barangCtrl);
    bclay->setContentsMargins(0, 0, 0, 0);
    bclay->setSpacing(8);
    m_lblNHarga = new QLabel(QString("Jumlah barang: %1").arg(m_nHarga));
    m_lblNHarga->setObjectName("hintLabel");
    auto *btnAddH = new QPushButton("+ Tambah Barang");
    auto *btnDelH = new QPushButton("- Kurangi Barang");
    for (auto *b : {btnAddH, btnDelH}) {
        b->setFocusPolicy(Qt::NoFocus);
        b->setFixedHeight(34);
    }
    connect(btnAddH, &QPushButton::clicked, this, [this]() { changeVars(1, "harga"); });
    connect(btnDelH, &QPushButton::clicked, this, [this]() { changeVars(-1, "harga"); });
    bclay->addWidget(m_lblNHarga);
    bclay->addSpacing(12);
    bclay->addWidget(btnAddH);
    bclay->addWidget(btnDelH);
    bclay->addStretch();
    phlay->addWidget(barangCtrl);

    // Currency row
    auto *currRow = new QWidget();
    auto *crlay = new QHBoxLayout(currRow);
    crlay->setContentsMargins(0, 0, 0, 0);
    crlay->setSpacing(8);
    auto *lblCurr = new QLabel("Format mata uang:");
    lblCurr->setObjectName("hintLabel");
    m_currCombo = new QComboBox();
    m_currCombo->addItems({"Rp (Rupiah)", "Tanpa satuan", "USD ($)", "EUR (€)", "Kustom..."});
    m_currCombo->setFixedWidth(160);
    m_currCombo->setFixedHeight(36);
    m_currCustom = new QLineEdit();
    m_currCustom->setPlaceholderText("Ketik satuan kustom");
    m_currCustom->setFixedWidth(140);
    m_currCustom->setObjectName("calcInput");
    m_currCustom->hide();
    connect(m_currCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SpldvPage::onCurrChanged);
    crlay->addWidget(lblCurr);
    crlay->addWidget(m_currCombo);
    crlay->addWidget(m_currCustom);
    crlay->addStretch();
    phlay->addWidget(currRow);

    m_hargaGridContainer = new QWidget();
    phlay->addWidget(m_hargaGridContainer);

    m_calcBtnHarga = calcBtn("Selesaikan Soal Harga");
    connect(m_calcBtnHarga, &QPushButton::clicked, this, &SpldvPage::solveHarga);
    phlay->addWidget(m_calcBtnHarga);

    lay->addWidget(m_panelKoef);
    lay->addWidget(m_panelHarga);
    lay->addSpacing(16);

    m_result = new QLabel("Masukkan data dan tekan Selesaikan.");
    m_result->setObjectName("resultLabel");
    m_result->setWordWrap(true);
    m_result->setTextFormat(Qt::RichText);
    lay->addWidget(m_result);
    lay->addStretch();

    scroll->setWidget(content);
    main->addWidget(scroll);

    connect(new QShortcut(QKeySequence("Ctrl+R"), this), &QShortcut::activated,
            this, &SpldvPage::reset);
    connect(new QShortcut(QKeySequence(Qt::Key_Escape), this), &QShortcut::activated,
            this, [this]() { m_mw->backToDashboard(4); });

    rebuildKoefGrid();
    rebuildHargaGrid();
    updateStyles();
}

// ═══════════════ Mode switching identik Python ═══════════════
void SpldvPage::switchMode(const QString &mode) {
    m_mode = mode;
    m_panelKoef->setVisible(mode == "koef");
    m_panelHarga->setVisible(mode == "harga");
    m_btnKoef->setChecked(mode == "koef");
    m_btnHarga->setChecked(mode == "harga");
    m_result->setText("Masukkan data dan tekan Selesaikan.");
    m_result->setStyleSheet("");
    updateTabStyles();
}

void SpldvPage::changeVars(int delta, const QString &panel) {
    if (panel == "koef") {
        int newN = qBound(MIN_VARS, m_nKoef + delta, MAX_VARS);
        if (newN == m_nKoef) return;
        m_nKoef = newN;
        rebuildKoefGrid();
    } else {
        int newN = qBound(MIN_VARS, m_nHarga + delta, MAX_VARS);
        if (newN == m_nHarga) return;
        m_nHarga = newN;
        rebuildHargaGrid();
    }
    m_result->setText("Masukkan data dan tekan Selesaikan.");
    m_result->setStyleSheet("");
    updateStyles();
}

void SpldvPage::onCurrChanged(int idx) {
    m_currCustom->setVisible(idx == 4);
}

QString SpldvPage::getCurrencyPrefix() const {
    int idx = m_currCombo->currentIndex();
    if (idx == 0) return "Rp";
    if (idx == 1) return "";
    if (idx == 2) return "$";
    if (idx == 3) return QStringLiteral("€");
    return m_currCustom->text().trimmed();
}

QString SpldvPage::fmtCurrency(double v, const QString &prefix) const {
    QString numStr;
    if (std::abs(v - std::round(v)) < 1e-9) {
        numStr = QString("%L1").arg(static_cast<long long>(std::round(v)));
    } else {
        numStr = QString::number(v, 'f', 2);
    }
    if (!prefix.isEmpty())
        numStr = prefix + " " + numStr;
    return numStr;
}

// ═══════════════ Build Koefisien Grid — identik Python ═══════════════
void SpldvPage::rebuildKoefGrid() {
    if (m_koefGridContainer->layout()) {
        QLayoutItem *child;
        while ((child = m_koefGridContainer->layout()->takeAt(0)) != nullptr) {
            if (child->widget()) child->widget()->deleteLater();
            delete child;
        }
        delete m_koefGridContainer->layout();
    }
    m_koefInputs.clear();
    m_koefKnown.clear();

    int n = m_nKoef;
    QStringList vn = varNames();

    auto *outer = new QVBoxLayout(m_koefGridContainer);
    outer->setContentsMargins(0, 4, 0, 0);
    outer->setSpacing(12);

    auto *step1 = new QLabel("LANGKAH 1  —  ISI PERSAMAAN");
    step1->setObjectName("spldvStep");
    outer->addWidget(step1);

    auto *hint1 = new QLabel(QString("Masukkan koefisien setiap variabel dan nilai hasil (ruas kanan) untuk %1 persamaan.").arg(n));
    hint1->setObjectName("spldvHint");
    hint1->setWordWrap(true);
    outer->addWidget(hint1);
    outer->addSpacing(4);

    for (int i = 0; i < n; ++i) {
        auto *rowW = new QWidget();
        auto *rl = new QHBoxLayout(rowW);
        rl->setContentsMargins(0, 0, 0, 0);
        rl->setSpacing(8);

        auto *num = new QLabel(QString("P%1").arg(i + 1));
        num->setObjectName("spldvEqNum");
        num->setFixedWidth(30);
        num->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        rl->addWidget(num);

        QVector<QLineEdit*> rowInps;
        for (int j = 0; j < n; ++j) {
            auto *inp = new QLineEdit();
            inp->setPlaceholderText("0");
            inp->setObjectName("spldvInput");
            inp->setFixedWidth(155);
            rl->addWidget(inp);
            rowInps.append(inp);

            auto *vl = new QLabel(vn[j]);
            vl->setObjectName("spldvVar");
            vl->setFixedWidth(16);
            rl->addWidget(vl);

            if (j < n - 1) {
                auto *op = new QLabel("+");
                op->setObjectName("spldvOp");
                op->setFixedWidth(24);
                op->setAlignment(Qt::AlignCenter);
                rl->addWidget(op);
            }
        }

        auto *eq = new QLabel("=");
        eq->setObjectName("spldvOp");
        eq->setFixedWidth(24);
        eq->setAlignment(Qt::AlignCenter);
        rl->addWidget(eq);

        auto *inpR = new QLineEdit();
        inpR->setPlaceholderText("0");
        inpR->setObjectName("spldvInput");
        inpR->setFixedWidth(165);
        rl->addWidget(inpR);
        rowInps.append(inpR);
        rl->addStretch();

        outer->addWidget(rowW);
        m_koefInputs.append(rowInps);
        for (auto *inp : rowInps)
            connect(inp, &QLineEdit::returnPressed, this, &SpldvPage::solveKoef);
    }

    outer->addSpacing(20);
    auto *step2 = new QLabel("LANGKAH 2  —  REVERSE SOLVING (OPSIONAL)");
    step2->setObjectName("spldvStep");
    outer->addWidget(step2);

    auto *hint2 = new QLabel(
        "Jika nilai salah satu variabel sudah diketahui, isi di sini. "
        "Biarkan kosong jika ingin sistem yang mencarikannya.");
    hint2->setObjectName("spldvHint");
    hint2->setWordWrap(true);
    outer->addWidget(hint2);
    outer->addSpacing(4);

    auto *revRow = new QWidget();
    auto *rrl = new QHBoxLayout(revRow);
    rrl->setContentsMargins(0, 0, 0, 0);
    rrl->setSpacing(16);
    rrl->setAlignment(Qt::AlignVCenter);
    for (int j = 0; j < n; ++j) {
        auto *cell = new QWidget();
        auto *cl = new QVBoxLayout(cell);
        cl->setContentsMargins(0, 0, 0, 0);
        cl->setSpacing(5);
        cl->setAlignment(Qt::AlignHCenter);
        auto *lv = new QLabel(QString("Nilai  %1  (jika diketahui)").arg(vn[j]));
        lv->setObjectName("spldvHint");
        lv->setAlignment(Qt::AlignCenter);
        auto *ik = new QLineEdit();
        ik->setPlaceholderText("kosong = sistem yang cari");
        ik->setObjectName("spldvRevInput");
        ik->setFixedWidth(232);
        ik->setFixedHeight(36);
        cl->addWidget(lv, 0, Qt::AlignHCenter);
        cl->addWidget(ik, 0, Qt::AlignHCenter);
        rrl->addWidget(cell);
        m_koefKnown.append(ik);
        connect(ik, &QLineEdit::returnPressed, this, &SpldvPage::solveKoef);
    }
    rrl->addStretch();
    outer->addWidget(revRow);
    m_lblNKoef->setText(QString("Jumlah variabel: %1").arg(n));
}

// ═══════════════ Build Harga Grid — identik Python ═══════════════
void SpldvPage::rebuildHargaGrid() {
    if (m_hargaGridContainer->layout()) {
        QLayoutItem *child;
        while ((child = m_hargaGridContainer->layout()->takeAt(0)) != nullptr) {
            if (child->widget()) child->widget()->deleteLater();
            delete child;
        }
        delete m_hargaGridContainer->layout();
    }
    m_namaInputs.clear();
    m_hargaKoefInputs.clear();
    m_hargaKnownInputs.clear();

    int n = m_nHarga;
    QStringList defaults = {"pensil","buku","penghapus","spidol","pena","crayon","kuas","tas"};

    auto *outer = new QVBoxLayout(m_hargaGridContainer);
    outer->setContentsMargins(0, 4, 0, 0);
    outer->setSpacing(12);

    auto *step1 = new QLabel("LANGKAH 1  —  NAMA BARANG");
    step1->setObjectName("spldvStep");
    outer->addWidget(step1);
    auto *hint1 = new QLabel(QString("Ketik nama %1 barang yang ada dalam soal.").arg(n));
    hint1->setObjectName("spldvHint");
    outer->addWidget(hint1);
    outer->addSpacing(4);

    auto *namaRow = new QWidget();
    auto *nrl = new QHBoxLayout(namaRow);
    nrl->setContentsMargins(0, 0, 0, 0);
    nrl->setSpacing(12);
    nrl->setAlignment(Qt::AlignVCenter);
    for (int j = 0; j < n; ++j) {
        auto *cell = new QWidget();
        auto *cl = new QVBoxLayout(cell);
        cl->setContentsMargins(0, 0, 0, 0);
        cl->setSpacing(5);
        cl->setAlignment(Qt::AlignHCenter);
        auto *lbl = new QLabel(QString("Barang %1").arg(j + 1));
        lbl->setObjectName("spldvHint");
        lbl->setAlignment(Qt::AlignCenter);
        auto *inp = new QLineEdit();
        inp->setPlaceholderText(j < defaults.size() ? defaults[j] : QString("barang %1").arg(j + 1));
        inp->setObjectName("spldvNameInput");
        inp->setFixedWidth(195);
        inp->setFixedHeight(36);
        cl->addWidget(lbl, 0, Qt::AlignHCenter);
        cl->addWidget(inp, 0, Qt::AlignHCenter);
        nrl->addWidget(cell);
        m_namaInputs.append(inp);
    }
    nrl->addStretch();
    outer->addWidget(namaRow);

    outer->addSpacing(20);
    auto *step2 = new QLabel("LANGKAH 2  —  PERSAMAAN HARGA");
    step2->setObjectName("spldvStep");
    outer->addWidget(step2);
    auto *hint2 = new QLabel("Masukkan jumlah masing-masing barang di setiap persamaan dan total harganya.");
    hint2->setObjectName("spldvHint");
    hint2->setWordWrap(true);
    outer->addWidget(hint2);
    outer->addSpacing(4);

    for (int i = 0; i < n; ++i) {
        auto *rowW = new QWidget();
        auto *rl = new QHBoxLayout(rowW);
        rl->setContentsMargins(0, 0, 0, 0);
        rl->setSpacing(8);

        auto *num = new QLabel(QString("P%1").arg(i + 1));
        num->setObjectName("spldvEqNum");
        num->setFixedWidth(30);
        num->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        rl->addWidget(num);

        QVector<QLineEdit*> rowInps;
        for (int j = 0; j < n; ++j) {
            auto *inp = new QLineEdit();
            inp->setPlaceholderText("0");
            inp->setObjectName("spldvInput");
            inp->setFixedWidth(148);
            rl->addWidget(inp);
            rowInps.append(inp);

            auto *nmLbl = new QLabel(j < defaults.size() ? defaults[j] : QString("B%1").arg(j + 1));
            nmLbl->setObjectName("spldvVar");
            nmLbl->setFixedWidth(58);
            rl->addWidget(nmLbl);

            if (j < n - 1) {
                auto *op = new QLabel("+");
                op->setObjectName("spldvOp");
                op->setFixedWidth(20);
                op->setAlignment(Qt::AlignCenter);
                rl->addWidget(op);
            }
        }

        auto *eq = new QLabel("=");
        eq->setObjectName("spldvOp");
        eq->setFixedWidth(24);
        eq->setAlignment(Qt::AlignCenter);
        rl->addWidget(eq);

        auto *inpT = new QLineEdit();
        inpT->setPlaceholderText("total harga");
        inpT->setObjectName("spldvInput");
        inpT->setFixedWidth(175);
        rl->addWidget(inpT);
        rowInps.append(inpT);
        rl->addStretch();

        outer->addWidget(rowW);
        m_hargaKoefInputs.append(rowInps);
        for (auto *inp : rowInps)
            connect(inp, &QLineEdit::returnPressed, this, &SpldvPage::solveHarga);
    }

    outer->addSpacing(20);
    auto *step3 = new QLabel("LANGKAH 3  —  REVERSE SOLVING (OPSIONAL)");
    step3->setObjectName("spldvStep");
    outer->addWidget(step3);
    auto *hint3 = new QLabel(
        "Jika harga salah satu barang sudah diketahui, isi di sini. "
        "Biarkan kosong jika ingin sistem yang mencarikannya.");
    hint3->setObjectName("spldvHint");
    hint3->setWordWrap(true);
    outer->addWidget(hint3);
    outer->addSpacing(4);

    auto *rev2Row = new QWidget();
    auto *r2l = new QHBoxLayout(rev2Row);
    r2l->setContentsMargins(0, 0, 0, 0);
    r2l->setSpacing(16);
    r2l->setAlignment(Qt::AlignVCenter);
    for (int j = 0; j < n; ++j) {
        QString nm = j < defaults.size() ? defaults[j] : QString("B%1").arg(j + 1);
        auto *cell2 = new QWidget();
        auto *cl3 = new QVBoxLayout(cell2);
        cl3->setContentsMargins(0, 0, 0, 0);
        cl3->setSpacing(5);
        cl3->setAlignment(Qt::AlignHCenter);
        auto *lbj = new QLabel(QString("Harga %1 (jika diketahui)").arg(nm));
        lbj->setObjectName("spldvHint");
        lbj->setAlignment(Qt::AlignCenter);
        auto *ik2 = new QLineEdit();
        ik2->setPlaceholderText("kosong = sistem yang cari");
        ik2->setObjectName("spldvRevInput");
        ik2->setFixedWidth(242);
        ik2->setFixedHeight(36);
        cl3->addWidget(lbj, 0, Qt::AlignHCenter);
        cl3->addWidget(ik2, 0, Qt::AlignHCenter);
        r2l->addWidget(cell2);
        m_hargaKnownInputs.append(ik2);
        connect(ik2, &QLineEdit::returnPressed, this, &SpldvPage::solveHarga);
    }
    r2l->addStretch();
    outer->addWidget(rev2Row);
    m_lblNHarga->setText(QString("Jumlah barang: %1").arg(n));
}

// ═══════════════ Reset & Styles ═══════════════
void SpldvPage::reset() {
    rebuildKoefGrid();
    rebuildHargaGrid();
    m_result->setText("Masukkan data dan tekan Selesaikan.");
    m_result->setStyleSheet("");
    updateStyles();
}

void SpldvPage::updateStyles() {
    if (m_mw) {
        ThemeColors t = T::css(m_mw->isDark());
        setStyleSheet(pageStylesheet(t));
        updateTabStyles();
    }
}

void SpldvPage::updateTabStyles() {
    if (!m_mw) return;
    ThemeColors t = T::css(m_mw->isDark());
    QString act = QString("background:%1;color:#1A1A1A;border:none;font-weight:700;border-radius:8px;padding:8px 18px;")
                      .arg(t.primary);
    QString inact = QString("background:%1;color:%2;border:1px solid %3;border-radius:8px;padding:8px 18px;")
                        .arg(t.card, t.text, t.border);
    m_btnKoef->setStyleSheet(m_mode == "koef" ? act : inact);
    m_btnHarga->setStyleSheet(m_mode == "harga" ? act : inact);
}

// ═══════════════ Gaussian Elimination — identik Python ═══════════════
QString SpldvPage::matrixToHtml(const QVector<QVector<double>> &M) {
    QString rowsHtml;
    for (const auto &row : M) {
        QString cells;
        for (double v : row)
            cells += QString("<td style='padding:2px 8px;border:1px solid #888;text-align:right'>%1</td>")
                         .arg(MathUtils::disp(v));
        rowsHtml += QString("<tr>%1</tr>").arg(cells);
    }
    return QString("<table style='border-collapse:collapse;font-size:12px;margin:4px 0'>%1</table>").arg(rowsHtml);
}

SpldvPage::GaussResult SpldvPage::gaussianElimination(QVector<QVector<double>> A, QVector<double> b) {
    int n = b.size();
    // Build augmented matrix
    QVector<QVector<double>> M(n);
    for (int i = 0; i < n; ++i) {
        M[i] = A[i];
        M[i].append(b[i]);
    }

    QStringList steps;
    steps.append("<b>Matriks augmented awal [A|b]:</b>");
    steps.append(matrixToHtml(M));

    for (int col = 0; col < n; ++col) {
        // Partial pivoting
        int maxRow = col;
        for (int row = col + 1; row < n; ++row)
            if (std::abs(M[row][col]) > std::abs(M[maxRow][col]))
                maxRow = row;
        if (maxRow != col) {
            std::swap(M[col], M[maxRow]);
            steps.append(QString("<b>Tukar baris %1 dengan baris %2:</b>").arg(col + 1).arg(maxRow + 1));
            steps.append(matrixToHtml(M));
        }

        double pivot = M[col][col];
        if (std::abs(pivot) < 1e-12) {
            throw std::runtime_error(
                QString("Sistem tidak memiliki solusi tunggal (matriks singular di kolom %1). "
                        "Periksa persamaan — mungkin ada persamaan yang sejajar atau bergantung linear.")
                    .arg(col + 1).toStdString());
        }

        QStringList elimDesc;
        for (int row = col + 1; row < n; ++row) {
            if (std::abs(M[row][col]) < 1e-14) continue;
            double factor = M[row][col] / pivot;
            for (int j = 0; j <= n; ++j)
                M[row][j] -= factor * M[col][j];
            elimDesc.append(QString("B%1 = B%1 - (%2) x B%3").arg(row + 1).arg(MathUtils::disp(factor)).arg(col + 1));
        }
        if (!elimDesc.isEmpty()) {
            steps.append("<b>Eliminasi maju:</b> " + elimDesc.join(" | "));
            steps.append(matrixToHtml(M));
        }
    }

    steps.append("<b>Matriks setelah eliminasi maju (row echelon):</b>");
    steps.append(matrixToHtml(M));

    // Back substitution
    QVector<double> x(n, 0);
    for (int i = n - 1; i >= 0; --i) {
        x[i] = M[i][n];
        for (int j = i + 1; j < n; ++j)
            x[i] -= M[i][j] * x[j];
        x[i] /= M[i][i];
    }

    steps.append("<b>Substitusi balik:</b>");
    for (int i = n - 1; i >= 0; --i)
        steps.append(QString("Variabel ke-%1 = %2").arg(i + 1).arg(MathUtils::disp(x[i])));

    return {x, steps};
}

SpldvPage::ApplyResult SpldvPage::applyKnownValues(
    const QVector<QVector<double>> &A, const QVector<double> &b,
    const QMap<int, double> &known)
{
    int n = b.size();
    QVector<int> unknownIdx;
    for (int i = 0; i < n; ++i)
        if (!known.contains(i)) unknownIdx.append(i);

    QVector<double> bNew = b;
    for (auto it = known.begin(); it != known.end(); ++it) {
        for (int row = 0; row < n; ++row)
            bNew[row] -= A[row][it.key()] * it.value();
    }

    int m = unknownIdx.size();
    QVector<QVector<double>> aNew(n, QVector<double>(m));
    for (int row = 0; row < n; ++row)
        for (int j = 0; j < m; ++j)
            aNew[row][j] = A[row][unknownIdx[j]];

    // Select valid rows
    QVector<int> selectedRows;
    for (int row = 0; row < n; ++row) {
        bool hasNonZero = false;
        for (int j = 0; j < m; ++j)
            if (std::abs(aNew[row][j]) > 1e-12) { hasNonZero = true; break; }
        if (hasNonZero) selectedRows.append(row);
    }
    if (selectedRows.size() < m)
        throw std::runtime_error("Tidak cukup persamaan yang valid untuk mencari variabel yang tidak diketahui.");

    QVector<QVector<double>> aRed(m);
    QVector<double> bRed(m);
    for (int i = 0; i < m; ++i) {
        int r = selectedRows[i];
        aRed[i].resize(m);
        for (int j = 0; j < m; ++j)
            aRed[i][j] = aNew[r][j];
        bRed[i] = bNew[r];
    }

    return {aRed, bRed, unknownIdx};
}

// ═══════════════ Solve Koef — identik Python ═══════════════
void SpldvPage::solveKoef() {
    ThemeColors t = T::css(m_mw ? m_mw->isDark() : true);
    int n = m_nKoef;
    QStringList vn = varNames();

    try {
        QVector<QVector<double>> A(n, QVector<double>(n));
        QVector<double> b(n);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                auto v = MathUtils::parseFloat(m_koefInputs[i][j]->text(),
                    QString("baris %1 kolom %2").arg(i + 1).arg(vn[j]), false);
                A[i][j] = *v;
            }
            auto r = MathUtils::parseFloat(m_koefInputs[i][n]->text(),
                QString("hasil baris %1").arg(i + 1), false);
            b[i] = *r;
        }

        QMap<int, double> known;
        for (int j = 0; j < n; ++j) {
            QString txt = m_koefKnown[j]->text().trimmed();
            if (!txt.isEmpty()) {
                auto v = MathUtils::parseFloat(txt, QString("nilai diketahui %1").arg(vn[j]), false);
                known[j] = *v;
            }
        }

        QString html = htmlTitle(QString("Solusi SPLDV %1 Variabel  —  Metode Gaussian Elimination").arg(n), t);
        html += htmlRow("<b>Diketahui:</b>", "", t);
        for (int i = 0; i < n; ++i) {
            QStringList terms;
            for (int j = 0; j < n; ++j) terms.append(MathUtils::disp(A[i][j]) + vn[j]);
            html += htmlRow("", QString("Persamaan %1: %2 = %3").arg(i + 1).arg(terms.join(" + "), MathUtils::disp(b[i])), t);
        }

        QMap<QString, double> results;
        QStringList steps;

        if (!known.isEmpty()) {
            html += htmlDiv(t);
            html += htmlRow("<b>Nilai diketahui (Reverse Solving):</b>", "", t);
            for (auto it = known.begin(); it != known.end(); ++it)
                html += htmlRow("", QString("%1 = %2").arg(vn[it.key()], MathUtils::disp(it.value())), t);

            auto ar = applyKnownValues(A, b, known);
            QStringList unkNames;
            for (int idx : ar.unknownIdx) unkNames.append(vn[idx]);

            html += htmlDiv(t);
            html += htmlRow("<b>Setelah substitusi nilai diketahui:</b>", "", t);
            for (int i = 0; i < ar.A_red.size(); ++i) {
                QStringList terms;
                for (int j = 0; j < unkNames.size(); ++j)
                    terms.append(MathUtils::disp(ar.A_red[i][j]) + unkNames[j]);
                html += htmlRow("", QString("Persamaan %1: %2 = %3").arg(i + 1).arg(terms.join(" + "), MathUtils::disp(ar.b_red[i])), t);
            }

            auto gr = gaussianElimination(ar.A_red, ar.b_red);
            steps = gr.steps;

            for (auto it = known.begin(); it != known.end(); ++it)
                results[vn[it.key()]] = it.value();
            for (int i = 0; i < ar.unknownIdx.size(); ++i)
                results[vn[ar.unknownIdx[i]]] = gr.x[i];
        } else {
            auto gr = gaussianElimination(A, b);
            steps = gr.steps;
            for (int j = 0; j < n; ++j)
                results[vn[j]] = gr.x[j];
        }

        html += htmlDiv(t);
        html += htmlRow("<b>Langkah Penyelesaian (Gaussian Elimination):</b>", "", t);
        for (const auto &step : steps)
            html += QString("<p style='font-size:12px;color:%1;margin:2px 0'>%2</p>").arg(t.text_mid, step);

        html += htmlDiv(t);
        html += htmlRow("<b>Hasil Akhir:</b>", "", t);
        for (auto it = results.begin(); it != results.end(); ++it)
            html += htmlBig(QString("%1 = %2").arg(it.key(), MathUtils::disp(it.value())), t);

        // Verifikasi
        html += htmlDiv(t);
        html += htmlRow("<b>Verifikasi:</b>", "", t);
        for (int i = 0; i < n; ++i) {
            double computed = 0;
            for (int j = 0; j < n; ++j)
                computed += A[i][j] * results[vn[j]];
            bool ok = std::abs(computed - b[i]) < 1e-6;
            QString status = ok ? "sesuai" : QString("TIDAK SESUAI (%1)").arg(MathUtils::disp(computed));
            QStringList terms;
            for (int j = 0; j < n; ++j)
                terms.append(QString("%1*%2").arg(MathUtils::disp(A[i][j]), MathUtils::disp(results[vn[j]])));
            html += htmlRow("", QString("Pers. %1: %2 = %3 (%4)").arg(i + 1).arg(terms.join(" + "), MathUtils::disp(computed), status), t);
        }

        resultOk(m_result, html, t);

    } catch (const std::runtime_error &e) {
        resultErr(m_result, QString::fromStdString(e.what()), t);
    } catch (...) {
        resultErr(m_result, "Terjadi kesalahan.", t);
    }
}

// ═══════════════ Solve Harga — identik Python ═══════════════
void SpldvPage::solveHarga() {
    ThemeColors t = T::css(m_mw ? m_mw->isDark() : true);
    int n = m_nHarga;
    QString prefix = getCurrencyPrefix();

    try {
        QStringList names;
        for (int j = 0; j < n; ++j) {
            QString nm = m_namaInputs[j]->text().trimmed();
            names.append(nm.isEmpty() ? QString("barang %1").arg(j + 1) : nm);
        }

        QVector<QVector<double>> A(n, QVector<double>(n));
        QVector<double> b(n);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                auto v = MathUtils::parseFloat(m_hargaKoefInputs[i][j]->text(),
                    QString("jumlah %1 persamaan %2").arg(names[j]).arg(i + 1), false);
                A[i][j] = *v;
            }
            auto r = MathUtils::parseFloat(m_hargaKoefInputs[i][n]->text(),
                QString("total persamaan %1").arg(i + 1), false);
            b[i] = *r;
        }

        QMap<int, double> known;
        for (int j = 0; j < n; ++j) {
            QString txt = m_hargaKnownInputs[j]->text().trimmed();
            if (!txt.isEmpty()) {
                auto v = MathUtils::parseFloat(txt, QString("harga diketahui %1").arg(names[j]), false);
                known[j] = *v;
            }
        }

        QString html = htmlTitle(QString("Soal Harga  —  %1 Barang  |  Gaussian Elimination").arg(n), t);
        html += htmlRow("<b>Misalkan:</b>", "", t);
        for (int j = 0; j < n; ++j)
            html += htmlRow("", QString("x%1 = harga 1 %2").arg(j + 1).arg(names[j]), t);
        html += htmlDiv(t);

        html += htmlRow("<b>Persamaan:</b>", "", t);
        for (int i = 0; i < n; ++i) {
            QStringList terms;
            for (int j = 0; j < n; ++j) terms.append(MathUtils::disp(A[i][j]) + " " + names[j]);
            html += htmlRow("", QString("Persamaan %1: %2 = %3").arg(i + 1).arg(terms.join(" + "), fmtCurrency(b[i], prefix)), t);
        }

        QMap<int, double> results;
        QStringList steps;

        if (!known.isEmpty()) {
            html += htmlDiv(t);
            html += htmlRow("<b>Harga diketahui (Reverse Solving):</b>", "", t);
            for (auto it = known.begin(); it != known.end(); ++it)
                html += htmlRow("", QString("Harga %1 = %2").arg(names[it.key()], fmtCurrency(it.value(), prefix)), t);

            auto ar = applyKnownValues(A, b, known);
            auto gr = gaussianElimination(ar.A_red, ar.b_red);
            steps = gr.steps;

            for (auto it = known.begin(); it != known.end(); ++it)
                results[it.key()] = it.value();
            for (int i = 0; i < ar.unknownIdx.size(); ++i)
                results[ar.unknownIdx[i]] = gr.x[i];
        } else {
            auto gr = gaussianElimination(A, b);
            steps = gr.steps;
            for (int j = 0; j < n; ++j)
                results[j] = gr.x[j];
        }

        html += htmlDiv(t);
        html += htmlRow("<b>Langkah Penyelesaian:</b>", "", t);
        for (const auto &step : steps)
            html += QString("<p style='font-size:12px;color:%1;margin:2px 0'>%2</p>").arg(t.text_mid, step);

        html += htmlDiv(t);
        html += htmlRow("<b>Hasil Akhir:</b>", "", t);
        for (int j = 0; j < n; ++j)
            html += htmlBig(QString("Harga 1 %1 = %2").arg(names[j], fmtCurrency(results[j], prefix)), t);

        // Verifikasi
        html += htmlDiv(t);
        html += htmlRow("<b>Verifikasi:</b>", "", t);
        for (int i = 0; i < n; ++i) {
            double computed = 0;
            for (int j = 0; j < n; ++j)
                computed += A[i][j] * results[j];
            bool ok = std::abs(computed - b[i]) < 1e-6;
            QString status = ok ? "sesuai" : "TIDAK SESUAI";
            QStringList terms;
            for (int j = 0; j < n; ++j)
                terms.append(QString("%1 x %2").arg(MathUtils::disp(A[i][j]), fmtCurrency(results[j], prefix)));
            html += htmlRow("", QString("Pers. %1: %2 = %3 (%4)").arg(i + 1).arg(terms.join(" + "), fmtCurrency(computed, prefix), status), t);
        }

        resultOk(m_result, html, t);

    } catch (const std::runtime_error &e) {
        resultErr(m_result, QString::fromStdString(e.what()), t);
    } catch (...) {
        resultErr(m_result, "Terjadi kesalahan.", t);
    }
}
