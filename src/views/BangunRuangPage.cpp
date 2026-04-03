/*
 * BangunRuangPage.cpp — Halaman Bangun Ruang
 * Padanan Python: class BangunRuangPage (baris 1979-2441 di main.py)
 * IDENTIK PYTHON — 8 bangun, dynamic fields, Volume reverse solving, 3D illustration
 */
#include "views/BangunRuangPage.h"
#include "MainWindow.h"
#include "core/Theme.h"
#include "core/MathUtils.h"
#include "helpers/PageHelpers.h"
#include "helpers/ResultRenderer.h"
#include "graphics/ShapeIllustration.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QFrame>
#include <QShortcut>
#include <cmath>
#include <QMap>

static const QStringList SHAPES = {
    "Kubus", "Balok", "Prisma Segitiga", "Tabung", "Kerucut", "Bola",
    "Limas Segitiga", "Limas Segi Empat"
};

struct FieldDef { QString label; QString ph; QString key; };
static QVector<FieldDef> fieldSchema(const QString &shape) {
    if (shape == "Kubus")           return {{"Panjang Sisi (s)", "contoh: 5", "s"}};
    if (shape == "Balok")           return {{"Panjang (p)", "contoh: 8", "p"}, {"Lebar (l)", "contoh: 4", "l"}, {"Tinggi (t)", "contoh: 3", "t"}};
    if (shape == "Prisma Segitiga") return {{"Luas Alas (A)", "contoh: 12", "A"}, {"Tinggi (t)", "contoh: 6", "t"}};
    if (shape == "Tabung")          return {{"Jari-jari (r)", "contoh: 7", "r"}, {"Tinggi (t)", "contoh: 10", "t"}};
    if (shape == "Kerucut")         return {{"Jari-jari (r)", "contoh: 3", "r"}, {"Tinggi (t)", "contoh: 4", "t"}};
    if (shape == "Bola")            return {{"Jari-jari (r)", "contoh: 6", "r"}};
    if (shape == "Limas Segitiga")  return {{"Luas Alas (A)", "contoh: 9", "A"}, {"Tinggi (t)", "contoh: 8", "t"}};
    if (shape == "Limas Segi Empat")return {{"Luas Alas (A)", "contoh: 16", "A"}, {"Tinggi (t)", "contoh: 9", "t"}};
    return {};
}

static QString hintText(const QString &shape) {
    if (shape == "Kubus")           return "Isi s untuk menghitung V dan L  |  Isi Volume untuk mencari s";
    if (shape == "Balok")           return "Isi p, l, t untuk menghitung V dan L  |  Isi 2 dimensi + Volume untuk mencari yang kosong";
    if (shape == "Prisma Segitiga") return "Isi A dan t untuk menghitung V  |  Isi 1 + Volume untuk mencari yang kosong";
    if (shape == "Tabung")          return "Isi r dan t untuk menghitung V dan L  |  Isi 1 + Volume untuk mencari yang kosong";
    if (shape == "Kerucut")         return "Isi r dan t untuk menghitung V dan L  |  Isi 1 + Volume untuk mencari yang kosong";
    if (shape == "Bola")            return "Isi r untuk menghitung V dan L  |  Isi Volume untuk mencari r";
    if (shape == "Limas Segitiga")  return "Isi A dan t untuk menghitung V  |  Isi 1 + Volume untuk mencari yang kosong";
    if (shape == "Limas Segi Empat")return "Isi A dan t untuk menghitung V  |  Isi 1 + Volume untuk mencari yang kosong";
    return "";
}

BangunRuangPage::BangunRuangPage(MainWindow *parent)
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
    connect(back, &QPushButton::clicked, this, [this]() { m_mw->backToDashboard(2); });
    lay->addWidget(back, 0, Qt::AlignLeft);
    lay->addSpacing(18);
    lay->addWidget(sectionHeader("Kalkulator Bangun Ruang", "Volume dan Luas Permukaan"));
    lay->addSpacing(20);

    // Panel
    auto *panel = new QFrame();
    panel->setObjectName("panel");
    auto *play = new QVBoxLayout(panel);
    play->setContentsMargins(24, 22, 24, 22);
    play->setSpacing(16);

    // Shape selector row — identik Python
    auto *selRow = new QWidget();
    auto *selLay = new QHBoxLayout(selRow);
    selLay->setContentsMargins(0, 0, 0, 0);
    selLay->setSpacing(10);
    auto *selLbl = new QLabel("PILIH BANGUN:");
    selLbl->setObjectName("inputLabel");
    m_combo = new QComboBox();
    m_combo->addItems(SHAPES);
    selLay->addWidget(selLbl);
    selLay->addWidget(m_combo);
    selLay->addStretch();
    play->addWidget(selRow);

    // Hint label
    m_hintLbl = new QLabel();
    m_hintLbl->setObjectName("hintLabel");
    m_hintLbl->setWordWrap(true);
    play->addWidget(m_hintLbl);

    // Dynamic container for inputs
    m_dynamicContainer = new QWidget();
    m_dynamicLayout = new QVBoxLayout(m_dynamicContainer);
    m_dynamicLayout->setContentsMargins(0, 0, 0, 0);
    m_dynamicLayout->setSpacing(14);
    play->addWidget(m_dynamicContainer);

    // Calc button
    auto *btn = calcBtn("Hitung");
    connect(btn, &QPushButton::clicked, this, &BangunRuangPage::calculate);
    play->addWidget(btn);

    lay->addWidget(panel);
    lay->addSpacing(20);

    // Result
    m_result = new QLabel("Pilih bangun ruang dan masukkan dimensinya.");
    m_result->setObjectName("resultLabel");
    m_result->setWordWrap(true);
    m_result->setTextFormat(Qt::RichText);
    lay->addWidget(m_result);
    lay->addStretch();

    scroll->setWidget(content);
    main->addWidget(scroll);

    // Store ShapeIllustration pointer (will be added in updateForm)
    m_sim = new ShapeIllustration();

    connect(m_combo, &QComboBox::currentTextChanged, this, &BangunRuangPage::updateForm);

    connect(new QShortcut(QKeySequence("Ctrl+R"), this), &QShortcut::activated,
            this, &BangunRuangPage::reset);
    connect(new QShortcut(QKeySequence(Qt::Key_Escape), this), &QShortcut::activated,
            this, [this]() { m_mw->backToDashboard(2); });

    updateForm("Kubus");
    updateStyles();
}

void BangunRuangPage::clearDynamic()
{
    while (m_dynamicLayout->count()) {
        auto *item = m_dynamicLayout->takeAt(0);
        if (auto *w = item->widget()) {
            w->setParent(nullptr);
            w->deleteLater();
        }
        delete item;
    }
    m_dimBoxes.clear();
    m_volInput = nullptr;
    // Re-parent illustration so it doesn't get deleted
    m_sim->setParent(nullptr);
}

void BangunRuangPage::updateForm(const QString &shape)
{
    clearDynamic();
    m_hintLbl->setText(hintText(shape));

    auto schema = fieldSchema(shape);

    // Main grid: form (left) + illustration (right) — identik Python
    auto *mainGrid = new QWidget();
    auto *mgLay = new QHBoxLayout(mainGrid);
    mgLay->setContentsMargins(0, 0, 0, 0);
    mgLay->setSpacing(20);

    auto *formW = new QWidget();
    auto *formLay = new QVBoxLayout(formW);
    formLay->setContentsMargins(0, 0, 0, 0);
    formLay->setSpacing(14);

    // Grid for dimension inputs
    auto *gridW = new QWidget();
    auto *gridLay = new QGridLayout(gridW);
    gridLay->setSpacing(14);
    int cols = qMin(3, schema.size());
    if (cols < 1) cols = 1;

    for (int i = 0; i < schema.size(); ++i) {
        auto [box, inp] = inputBox(schema[i].label, schema[i].ph);
        connect(inp, &QLineEdit::returnPressed, this, &BangunRuangPage::calculate);
        int r = i / cols, c = i % cols;
        gridLay->addWidget(box, r, c);
        m_dimBoxes.append({box, inp, schema[i].key});
    }
    formLay->addWidget(gridW);

    // Separator — identik Python
    auto *sepW = new QWidget();
    auto *sepLay = new QHBoxLayout(sepW);
    sepLay->setContentsMargins(0, 4, 0, 4);
    auto *sepLbl = new QLabel("  ATAU isi Volume untuk Reverse Solving  ");
    sepLbl->setStyleSheet("font-size:11px;color:#606078;background:transparent;");
    sepLay->addWidget(sepLbl);
    formLay->addWidget(sepW);

    // Volume input — identik Python
    auto [volBox, volInp] = inputBox("Volume (V)  —  kosongkan jika ingin dihitung", "contoh: 300");
    connect(volInp, &QLineEdit::returnPressed, this, &BangunRuangPage::calculate);
    formLay->addWidget(volBox);
    m_volInput = volInp;

    mgLay->addWidget(formW, 3);

    // 3D illustration
    m_sim->setShape(shape);
    m_sim->setParent(mainGrid);
    mgLay->addWidget(m_sim, 1);

    m_dynamicLayout->addWidget(mainGrid);
    updateStyles();
}

void BangunRuangPage::reset()
{
    for (auto &db : m_dimBoxes) db.input->clear();
    if (m_volInput) m_volInput->clear();
    m_result->setText("Pilih bangun ruang dan masukkan dimensinya.");
    m_result->setStyleSheet("");
}

void BangunRuangPage::updateStyles()
{
    if (m_mw) setStyleSheet(pageStylesheet(T::css(m_mw->isDark())));
}

void BangunRuangPage::calculate()
{
    ThemeColors t = T::css(m_mw ? m_mw->isDark() : true);
    const double PI = 3.14; // Nilai SMP standar
    auto f = MathUtils::fmt2;
    auto sf = [](const QString &s) { return MathUtils::parseFloat(s); };
    auto ok = [](const std::optional<double> &v) { return v.has_value(); };
    auto err = [&](const QString &msg) { resultErr(m_result, msg, t); };
    auto show = [&](const QString &body) { resultOk(m_result, body, t); };
    auto row = [&](const QString &lbl, const QString &val, bool big = false, const QString &color = "") -> QString {
        QString c = color.isEmpty() ? t.text_mid : color;
        QString fs = big ? "17px" : "14px";
        QString fw = big ? "700" : "400";
        return QString("<p style='font-size:%1;font-weight:%2;color:%3;margin:4px 0'>%4 %5</p>")
            .arg(fs, fw, c, lbl, val);
    };
    auto title = [&](const QString &txt) { return htmlTitle(txt, t); };
    auto div = [&]() { return htmlDiv(t); };

    QString shape = m_combo->currentText();
    try {
        QMap<QString, std::optional<double>> dims;
        for (auto &db : m_dimBoxes)
            dims[db.key] = sf(db.input->text());
        auto vIn = m_volInput ? sf(m_volInput->text()) : std::optional<double>{};

        if (shape == "Kubus") {
            auto s = dims["s"];
            if (ok(s) && !ok(vIn)) {
                if (*s <= 0) throw std::runtime_error("Nilai s harus lebih dari 0.");
                double V = std::pow(*s, 3), L = 6 * std::pow(*s, 2);
                show(title("Kubus  -  Hitung V dan L") +
                     row("<b>Diketahui:</b>", "s = " + f(*s)) +
                     row("<b>Ditanya:</b>", "Volume (V), Luas Permukaan (L)") + div() +
                     row("<b>Rumus V:</b>", "s pangkat 3") +
                     row("<b>Substitusi:</b>", QString("V = %1 x %1 x %1").arg(f(*s))) +
                     row("", QString("<b>V = %1</b>").arg(f(V)), true, t.success) +
                     row("<b>Rumus L:</b>", "6 x s pangkat 2") +
                     row("<b>Substitusi:</b>", QString("L = 6 x %1 x %1").arg(f(*s))) +
                     row("", QString("<b>L = %1</b>").arg(f(L)), true, t.success));
            } else if (ok(vIn) && !ok(s)) {
                if (*vIn <= 0) throw std::runtime_error("Volume harus lebih dari 0.");
                double sAns = std::cbrt(*vIn), L = 6 * sAns * sAns;
                show(title("Kubus  -  Cari s dari V") +
                     row("<b>Diketahui:</b>", "V = " + f(*vIn)) +
                     row("<b>Ditanya:</b>", "Panjang sisi (s)") + div() +
                     row("<b>Rumus:</b>", "s = akar pangkat 3 dari V") +
                     row("<b>Substitusi:</b>", "s = akar3(" + f(*vIn) + ")") +
                     row("", QString("<b>s = %1</b>").arg(f(sAns)), true, t.success) +
                     row("<b>Bonus L:</b>", QString("6 x %1 x %1 = %2").arg(f(sAns), f(L))));
            } else if (ok(s) && ok(vIn)) {
                err("Isi <b>s</b> saja atau <b>Volume</b> saja, tidak keduanya.");
            } else {
                err("Masukkan nilai <b>s</b> atau nilai <b>Volume</b>.");
            }

        } else if (shape == "Balok") {
            auto pv = dims["p"], lv = dims["l"], tv = dims["t"];
            int dimCount = (ok(pv)?1:0) + (ok(lv)?1:0) + (ok(tv)?1:0);
            QStringList emptyKeys;
            if (!ok(pv)) emptyKeys << "p"; if (!ok(lv)) emptyKeys << "l"; if (!ok(tv)) emptyKeys << "t";

            if (dimCount == 3 && !ok(vIn)) {
                if (*pv <= 0 || *lv <= 0 || *tv <= 0) throw std::runtime_error("Semua dimensi harus lebih dari 0.");
                double V = *pv * *lv * *tv;
                double L = 2 * (*pv * *lv + *pv * *tv + *lv * *tv);
                show(title("Balok  -  Hitung V dan L") +
                     row("<b>Diketahui:</b>", QString("p=%1, l=%2, t=%3").arg(f(*pv), f(*lv), f(*tv))) +
                     row("<b>Ditanya:</b>", "Volume (V), Luas Permukaan (L)") + div() +
                     row("<b>Rumus V:</b>", "p x l x t") +
                     row("<b>Substitusi:</b>", QString("V = %1 x %2 x %3").arg(f(*pv), f(*lv), f(*tv))) +
                     row("", QString("<b>V = %1</b>").arg(f(V)), true, t.success) +
                     row("<b>Rumus L:</b>", "2(pl + pt + lt)") +
                     row("<b>Substitusi:</b>", QString("L = 2(%1 + %2 + %3)").arg(f(*pv * *lv), f(*pv * *tv), f(*lv * *tv))) +
                     row("", QString("<b>L = %1</b>").arg(f(L)), true, t.success));
            } else if (dimCount == 3 && ok(vIn)) {
                err("Semua dimensi dan Volume terisi. Kosongkan 1 dimensi untuk perhitungan balik.");
            } else if (dimCount == 2 && ok(vIn) && emptyKeys.size() == 1) {
                if (*vIn <= 0) throw std::runtime_error("Volume harus lebih dari 0.");
                QString target = emptyKeys[0];
                double ans, divisor;
                QString knownStr;
                QMap<QString, QString> labels = {{"p", "Panjang (p)"}, {"l", "Lebar (l)"}, {"t", "Tinggi (t)"}};
                if (target == "p") { ans = *vIn / (*lv * *tv); divisor = *lv * *tv; knownStr = QString("V=%1, l=%2, t=%3").arg(f(*vIn), f(*lv), f(*tv)); }
                else if (target == "l") { ans = *vIn / (*pv * *tv); divisor = *pv * *tv; knownStr = QString("V=%1, p=%2, t=%3").arg(f(*vIn), f(*pv), f(*tv)); }
                else { ans = *vIn / (*pv * *lv); divisor = *pv * *lv; knownStr = QString("V=%1, p=%2, l=%3").arg(f(*vIn), f(*pv), f(*lv)); }
                show(title(QString("Balok  -  Cari %1 dari V").arg(target)) +
                     row("<b>Diketahui:</b>", knownStr) +
                     row("<b>Ditanya:</b>", labels[target]) + div() +
                     row("<b>Rumus:</b>", target + " = V dibagi dimensi lain") +
                     row("<b>Substitusi:</b>", QString("%1 = %2 : %3").arg(target, f(*vIn), f(divisor))) +
                     row("", QString("<b>%1 = %2</b>").arg(target, f(ans)), true, t.success));
            } else {
                err("Balok: isi 3 dimensi untuk V dan L  |  isi 2 + Volume untuk mencari 1 dimensi");
            }

        } else if (shape == "Tabung") {
            auto rv = dims["r"], tv = dims["t"];
            if (ok(rv) && ok(tv) && !ok(vIn)) {
                if (*rv <= 0 || *tv <= 0) throw std::runtime_error("r dan t harus lebih dari 0.");
                double V = PI * *rv * *rv * *tv;
                double L = 2 * PI * *rv * (*rv + *tv);
                show(title("Tabung  -  Hitung V dan L") +
                     row("<b>Diketahui:</b>", QString("r=%1, t=%2").arg(f(*rv), f(*tv))) +
                     row("<b>Ditanya:</b>", "Volume (V), Luas Permukaan (L)") + div() +
                     row("<b>Rumus V:</b>", "pi x r pangkat 2 x t") +
                     row("<b>Substitusi:</b>", QString("V = 3,14 x %1 x %1 x %2").arg(f(*rv), f(*tv))) +
                     row("", QString("<b>V = %1</b>").arg(f(V)), true, t.success) +
                     row("<b>Rumus L:</b>", "2 x pi x r x (r + t)") +
                     row("<b>Substitusi:</b>", QString("L = 2 x 3,14 x %1 x (%1 + %2)").arg(f(*rv), f(*tv))) +
                     row("", QString("<b>L = %1</b>").arg(f(L)), true, t.success));
            } else if (ok(rv) && !ok(tv) && ok(vIn)) {
                if (*rv <= 0) throw std::runtime_error("r harus lebih dari 0.");
                if (*vIn <= 0) throw std::runtime_error("Volume harus lebih dari 0.");
                double tAns = *vIn / (PI * *rv * *rv);
                show(title("Tabung  -  Cari t dari V") +
                     row("<b>Diketahui:</b>", QString("V=%1, r=%2").arg(f(*vIn), f(*rv))) +
                     row("<b>Ditanya:</b>", "Tinggi (t)") + div() +
                     row("<b>Rumus:</b>", "t = V dibagi (pi x r pangkat 2)") +
                     row("<b>Substitusi:</b>", QString("t = %1 : (3,14 x %2 x %2) = %1 : %3").arg(f(*vIn), f(*rv), f(PI * *rv * *rv))) +
                     row("", QString("<b>t = %1</b>").arg(f(tAns)), true, t.success));
            } else if (!ok(rv) && ok(tv) && ok(vIn)) {
                if (*tv <= 0) throw std::runtime_error("t harus lebih dari 0.");
                if (*vIn <= 0) throw std::runtime_error("Volume harus lebih dari 0.");
                double rAns = std::sqrt(*vIn / (PI * *tv));
                show(title("Tabung  -  Cari r dari V") +
                     row("<b>Diketahui:</b>", QString("V=%1, t=%2").arg(f(*vIn), f(*tv))) +
                     row("<b>Ditanya:</b>", "Jari-jari (r)") + div() +
                     row("<b>Rumus:</b>", "r = akar(V dibagi (pi x t))") +
                     row("<b>Substitusi:</b>", QString("r = akar(%1 : (3,14 x %2)) = akar(%3)").arg(f(*vIn), f(*tv), f(*vIn / (PI * *tv)))) +
                     row("", QString("<b>r = %1</b>").arg(f(rAns)), true, t.success));
            } else {
                err("Tabung: isi r dan t untuk V dan L  |  r + Volume untuk cari t  |  t + Volume untuk cari r");
            }

        } else if (shape == "Kerucut") {
            auto rv = dims["r"], tv = dims["t"];
            if (ok(rv) && ok(tv) && !ok(vIn)) {
                if (*rv <= 0 || *tv <= 0) throw std::runtime_error("r dan t harus lebih dari 0.");
                double sSlant = std::sqrt(*rv * *rv + *tv * *tv);
                double V = (1.0/3) * PI * *rv * *rv * *tv;
                double L = PI * *rv * (*rv + sSlant);
                show(title("Kerucut  -  Hitung V dan L") +
                     row("<b>Diketahui:</b>", QString("r=%1, t=%2").arg(f(*rv), f(*tv))) +
                     row("<b>Ditanya:</b>", "Volume (V), Luas Permukaan (L)") + div() +
                     row("<b>Garis pelukis:</b>", QString("s = akar(r² + t²) = %1").arg(f(sSlant))) +
                     row("<b>Rumus V:</b>", "1/3 x pi x r pangkat 2 x t") +
                     row("<b>Substitusi:</b>", QString("V = 1/3 x 3,14 x %1 x %1 x %2").arg(f(*rv), f(*tv))) +
                     row("", QString("<b>V = %1</b>").arg(f(V)), true, t.success) +
                     row("<b>Rumus L:</b>", "pi x r x (r + s)") +
                     row("<b>Substitusi:</b>", QString("L = 3,14 x %1 x (%1 + %2)").arg(f(*rv), f(sSlant))) +
                     row("", QString("<b>L = %1</b>").arg(f(L)), true, t.success));
            } else if (ok(rv) && !ok(tv) && ok(vIn)) {
                if (*rv <= 0) throw std::runtime_error("r harus lebih dari 0.");
                if (*vIn <= 0) throw std::runtime_error("Volume harus lebih dari 0.");
                double tAns = (3 * *vIn) / (PI * *rv * *rv);
                show(title("Kerucut  -  Cari t dari V") +
                     row("<b>Diketahui:</b>", QString("V=%1, r=%2").arg(f(*vIn), f(*rv))) +
                     row("<b>Ditanya:</b>", "Tinggi (t)") + div() +
                     row("<b>Rumus:</b>", "t = 3V dibagi (pi x r pangkat 2)") +
                     row("<b>Substitusi:</b>", QString("t = 3 x %1 : (3,14 x %2 x %2) = %3 : %4").arg(f(*vIn), f(*rv), f(3 * *vIn), f(PI * *rv * *rv))) +
                     row("", QString("<b>t = %1</b>").arg(f(tAns)), true, t.success));
            } else if (!ok(rv) && ok(tv) && ok(vIn)) {
                if (*tv <= 0) throw std::runtime_error("t harus lebih dari 0.");
                if (*vIn <= 0) throw std::runtime_error("Volume harus lebih dari 0.");
                double rSq = (3 * *vIn) / (PI * *tv);
                double rAns = std::sqrt(rSq);
                show(title("Kerucut  -  Cari r dari V") +
                     row("<b>Diketahui:</b>", QString("V=%1, t=%2").arg(f(*vIn), f(*tv))) +
                     row("<b>Ditanya:</b>", "Jari-jari (r)") + div() +
                     row("<b>Rumus:</b>", "r = akar(3V dibagi (pi x t))") +
                     row("<b>Substitusi:</b>", QString("r² = 3 x %1 : (3,14 x %2) = %3").arg(f(*vIn), f(*tv), f(rSq))) +
                     row("<b>Aljabar:</b>", QString("r = akar(%1)").arg(f(rSq))) +
                     row("", QString("<b>r = %1</b>").arg(f(rAns)), true, t.success));
            } else {
                err("Kerucut: isi r dan t untuk V dan L  |  r + Volume untuk cari t  |  t + Volume untuk cari r");
            }

        } else if (shape == "Bola") {
            auto rv = dims["r"];
            if (ok(rv) && !ok(vIn)) {
                if (*rv <= 0) throw std::runtime_error("r harus lebih dari 0.");
                double V = (4.0/3) * PI * std::pow(*rv, 3);
                double L = 4 * PI * *rv * *rv;
                show(title("Bola  -  Hitung V dan L") +
                     row("<b>Diketahui:</b>", "r=" + f(*rv)) +
                     row("<b>Ditanya:</b>", "Volume (V), Luas Permukaan (L)") + div() +
                     row("<b>Rumus V:</b>", "4/3 x pi x r pangkat 3") +
                     row("<b>Substitusi:</b>", QString("V = 4/3 x 3,14 x %1 x %1 x %1").arg(f(*rv))) +
                     row("", QString("<b>V = %1</b>").arg(f(V)), true, t.success) +
                     row("<b>Rumus L:</b>", "4 x pi x r pangkat 2") +
                     row("<b>Substitusi:</b>", QString("L = 4 x 3,14 x %1 x %1").arg(f(*rv))) +
                     row("", QString("<b>L = %1</b>").arg(f(L)), true, t.success));
            } else if (ok(vIn) && !ok(rv)) {
                if (*vIn <= 0) throw std::runtime_error("Volume harus lebih dari 0.");
                double rAns = std::cbrt(*vIn / ((4.0/3) * PI));
                show(title("Bola  -  Cari r dari V") +
                     row("<b>Diketahui:</b>", "V=" + f(*vIn)) +
                     row("<b>Ditanya:</b>", "Jari-jari (r)") + div() +
                     row("<b>Rumus:</b>", "r = akar pangkat 3 dari (3V dibagi 4pi)") +
                     row("<b>Substitusi:</b>", QString("r³ = 3 x %1 : (4 x 3,14) = %2").arg(f(*vIn), f(3 * *vIn / (4 * PI)))) +
                     row("<b>Aljabar:</b>", QString("r = akar3(%1)").arg(f(3 * *vIn / (4 * PI)))) +
                     row("", QString("<b>r = %1</b>").arg(f(rAns)), true, t.success));
            } else if (ok(rv) && ok(vIn)) {
                err("Isi <b>r</b> saja atau <b>Volume</b> saja, tidak keduanya.");
            } else {
                err("Masukkan nilai <b>r</b> atau nilai <b>Volume</b>.");
            }

        } else if (shape == "Prisma Segitiga" || shape == "Limas Segitiga" || shape == "Limas Segi Empat") {
            auto av = dims["A"], tv = dims["t"];
            bool isLimas = shape.contains("Limas");
            QString coef = isLimas ? "1/3 x " : "";
            double coefNum = isLimas ? 1.0/3 : 1.0;

            if (ok(av) && ok(tv) && !ok(vIn)) {
                if (*av <= 0 || *tv <= 0) throw std::runtime_error("A dan t harus lebih dari 0.");
                double V = coefNum * *av * *tv;
                show(title(shape + "  -  Hitung V") +
                     row("<b>Diketahui:</b>", QString("A=%1, t=%2").arg(f(*av), f(*tv))) +
                     row("<b>Ditanya:</b>", "Volume (V)") + div() +
                     row("<b>Rumus:</b>", "V = " + coef + "Luas Alas x Tinggi") +
                     row("<b>Substitusi:</b>", QString("V = %1%2 x %3").arg(coef, f(*av), f(*tv))) +
                     row("", QString("<b>V = %1</b>").arg(f(V)), true, t.success));
            } else if (ok(av) && !ok(tv) && ok(vIn)) {
                if (*av <= 0) throw std::runtime_error("A harus lebih dari 0.");
                if (*vIn <= 0) throw std::runtime_error("Volume harus lebih dari 0.");
                double tAns = *vIn / (coefNum * *av);
                show(title(shape + "  -  Cari t dari V") +
                     row("<b>Diketahui:</b>", QString("V=%1, A=%2").arg(f(*vIn), f(*av))) +
                     row("<b>Ditanya:</b>", "Tinggi (t)") + div() +
                     row("<b>Rumus:</b>", "t = V dibagi (" + coef + "A)") +
                     row("<b>Substitusi:</b>", QString("t = %1 : (%2%3) = %1 : %4").arg(f(*vIn), coef, f(*av), f(coefNum * *av))) +
                     row("", QString("<b>t = %1</b>").arg(f(tAns)), true, t.success));
            } else if (!ok(av) && ok(tv) && ok(vIn)) {
                if (*tv <= 0) throw std::runtime_error("t harus lebih dari 0.");
                if (*vIn <= 0) throw std::runtime_error("Volume harus lebih dari 0.");
                double aAns = *vIn / (coefNum * *tv);
                show(title(shape + "  -  Cari A dari V") +
                     row("<b>Diketahui:</b>", QString("V=%1, t=%2").arg(f(*vIn), f(*tv))) +
                     row("<b>Ditanya:</b>", "Luas Alas (A)") + div() +
                     row("<b>Rumus:</b>", "A = V dibagi (" + coef + "t)") +
                     row("<b>Substitusi:</b>", QString("A = %1 : (%2%3) = %1 : %4").arg(f(*vIn), coef, f(*tv), f(coefNum * *tv))) +
                     row("", QString("<b>A = %1</b>").arg(f(aAns)), true, t.success));
            } else {
                err(shape + ": isi A dan t untuk V  |  A + Volume untuk cari t  |  t + Volume untuk cari A");
            }
        } else {
            err("Bangun ruang tidak dikenali.");
        }

        // Update 3D illustration
        m_sim->setShape(shape);

    } catch (const std::runtime_error &e) {
        err(QString::fromStdString(e.what()));
    } catch (...) {
        err("Terjadi kesalahan.");
    }
}
