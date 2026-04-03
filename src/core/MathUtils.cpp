/*
 * MathUtils.cpp — Implementasi Utilitas Matematika
 * Padanan Python: class MathUtils (baris 94-191 di main.py)
 */
#include "core/MathUtils.h"
#include <cmath>
#include <numeric>

// ╔══════════════════════════════════════════════════════╗
// ║              SimpleFraction                         ║
// ╚══════════════════════════════════════════════════════╝

SimpleFraction::SimpleFraction(long long n, long long d)
    : num(n), den(d)
{
    simplify();
}

void SimpleFraction::simplify()
{
    if (den == 0) { num = 0; den = 1; return; }
    if (den < 0) { num = -num; den = -den; }
    long long g = std::gcd(std::abs(num), std::abs(den));
    if (g > 0) { num /= g; den /= g; }
}

SimpleFraction SimpleFraction::fromDouble(double val, long long maxDen)
{
    // Algoritma Stern-Brocot tree (setara Python Fraction.limit_denominator)
    if (std::isnan(val) || std::isinf(val))
        return SimpleFraction(0, 1);

    bool negative = val < 0;
    val = std::abs(val);

    long long p0 = 0, q0 = 1;
    long long p1 = 1, q1 = 0;
    double x = val;

    for (int i = 0; i < 64; ++i) {
        long long a = static_cast<long long>(x);
        long long p2 = a * p1 + p0;
        long long q2 = a * q1 + q0;
        if (q2 > maxDen) break;

        p0 = p1; q0 = q1;
        p1 = p2; q1 = q2;

        double rem = x - static_cast<double>(a);
        if (std::abs(rem) < 1e-12) break;
        x = 1.0 / rem;
    }

    return SimpleFraction(negative ? -p1 : p1, q1);
}

QString SimpleFraction::toString() const
{
    if (den == 1) return QString::number(num);
    return QString("%1/%2").arg(num).arg(den);
}

// ╔══════════════════════════════════════════════════════╗
// ║              MathUtils                              ║
// ╚══════════════════════════════════════════════════════╝

SqrtResult MathUtils::simplifySqrt(double n)
{
    if (n < 0) return {0, 0, false};
    if (n == 0) return {0, 0, true};

    double sqrtN = std::sqrt(n);
    if (std::abs(sqrtN - std::round(sqrtN)) < 1e-9) {
        return {static_cast<int>(std::round(sqrtN)), 1, true};
    }

    if (std::abs(n - std::round(n)) < 1e-9) {
        int nInt = static_cast<int>(std::round(n));
        int k = 1, m = nInt, i = 2;
        while (i * i <= nInt) {
            while (m % (i * i) == 0) {
                k *= i;
                m /= (i * i);
            }
            i++;
        }
        return {k, static_cast<double>(m), false};
    }

    return {1, n, false};
}

QString MathUtils::sqrtHtml(double n, const QString &varName,
                            const QString &color, const QString &textMid)
{
    auto result = simplifySqrt(n);
    int k = result.factor;
    double m = result.rem;
    bool isPerfect = result.isPerfect;
    double dec = std::sqrt(n);
    QString nD = disp(n);

    if (isPerfect) {
        return QString(
            "<p style='font-size:13px;color:%1;margin:2px 0'>"
            "<b>Bentuk akar:</b> akar(%2) = %3 (akar sempurna)</p>"
            "<p style='font-size:18px;font-weight:700;color:%4;margin:8px 0 4px'>"
            "%5 = %3</p>"
        ).arg(textMid, nD, QString::number(k), color, varName);
    }
    else if (k > 1) {
        int nInt = static_cast<int>(std::round(n));
        int factorSq = k * k;
        int mInt = static_cast<int>(std::round(m));
        return QString(
            "<p style='font-size:13px;color:%1;margin:4px 0'>"
            "<b>Bentuk akar awal:</b> akar(%2)</p>"
            "<p style='font-size:13px;color:%1;margin:2px 0'>"
            "<b>Penyederhanaan:</b></p>"
            "<p style='font-size:13px;color:%1;margin:1px 0 1px 20px'>"
            "akar(%3) = akar(%4 x %5)</p>"
            "<p style='font-size:13px;color:%1;margin:1px 0 1px 20px'>"
            "akar(%3) = akar(%4) x akar(%5)</p>"
            "<p style='font-size:13px;color:%1;margin:1px 0 1px 20px'>"
            "akar(%3) = %6 akar(%5)</p>"
            "<p style='font-size:18px;font-weight:700;color:%7;margin:8px 0 4px'>"
            "%8 = %6 akar(%5) &nbsp;=&nbsp; %9</p>"
        ).arg(textMid, nD, QString::number(nInt),
              QString::number(factorSq), QString::number(mInt),
              QString::number(k), color, varName,
              QString::number(dec, 'f', 2));
    }
    else {
        return QString(
            "<p style='font-size:13px;color:%1;margin:4px 0'>"
            "<b>Bentuk akar:</b> akar(%2) (tidak dapat disederhanakan)</p>"
            "<p style='font-size:18px;font-weight:700;color:%3;margin:8px 0 4px'>"
            "%4 = akar(%2) &nbsp;=&nbsp; %5</p>"
        ).arg(textMid, nD, color, varName, QString::number(dec, 'f', 2));
    }
}

QString MathUtils::disp(double v)
{
    if (std::abs(v - std::round(v)) < 1e-9) {
        return QString::number(static_cast<long long>(std::round(v)));
    }
    return QString::number(v, 'f', 2);
}

QString MathUtils::fmt2(double v)
{
    return QString::number(v, 'f', 2);
}

QString MathUtils::fracStr(double numerator, double denominator)
{
    if (std::abs(denominator) < 1e-12) return QStringLiteral("?");
    try {
        long long n = static_cast<long long>(std::round(numerator));
        long long d = static_cast<long long>(std::round(denominator));
        SimpleFraction f(n, d);
        return f.toString();
    } catch (...) {
        return QString::number(numerator / denominator, 'f', 6);
    }
}

QString MathUtils::toFrac(double val)
{
    try {
        SimpleFraction f = SimpleFraction::fromDouble(val, 10000);
        if (f.den == 1) return QString::number(f.num);
        return f.toString();
    } catch (...) {
        return QString::number(val, 'f', 6);
    }
}

std::optional<double> MathUtils::parseFloat(const QString &text,
                                            const QString &fieldName,
                                            bool allowEmpty)
{
    QString s = text.trimmed().replace(',', '.');
    if (s.isEmpty()) {
        if (allowEmpty) return std::nullopt;
        if (!fieldName.isEmpty())
            throw std::runtime_error(
                QString("Kolom '%1' tidak boleh kosong.").arg(fieldName).toStdString());
        else
            throw std::runtime_error("Kolom tidak boleh kosong.");
    }

    bool ok = false;
    double val = s.toDouble(&ok);
    if (!ok) {
        QString msg = QStringLiteral("bukan angka yang valid");
        if (!fieldName.isEmpty())
            throw std::runtime_error(
                QString("Input '%1' pada kolom '%2' %3. Masukkan angka saja.")
                    .arg(text, fieldName, msg).toStdString());
        else
            throw std::runtime_error(
                QString("Input '%1' %2. Masukkan angka saja.")
                    .arg(text, msg).toStdString());
    }
    return val;
}
