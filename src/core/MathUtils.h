/*
 * Matematika SMP — By: Dika (C++ Version)
 * MathUtils.h — Utilitas Matematika Bersama
 *
 * Padanan Python: class MathUtils (baris 94-191 di main.py)
 */
#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <QString>
#include <cmath>
#include <stdexcept>
#include <numeric>   // std::gcd
#include <optional>

// ╔══════════════════════════════════════════════════════╗
// ║     SqrtResult — hasil simplify_sqrt()              ║
// ╚══════════════════════════════════════════════════════╝

struct SqrtResult {
    int factor;       // koefisien luar akar (sebelumnya 'k')
    double rem;       // angka di dalam akar (sebelumnya 'm')
    bool isPerfect;   // apakah akar sempurna
};

// ╔══════════════════════════════════════════════════════╗
// ║   SimpleFraction — pengganti fractions.Fraction     ║
// ╚══════════════════════════════════════════════════════╝

struct SimpleFraction {
    long long num;   // pembilang
    long long den;   // penyebut

    SimpleFraction(long long n = 0, long long d = 1);
    void simplify();
    static SimpleFraction fromDouble(double val, long long maxDen = 10000);
    QString toString() const;
};

// ╔══════════════════════════════════════════════════════╗
// ║           MathUtils — Fungsi Utilitas               ║
// ╚══════════════════════════════════════════════════════╝

class MathUtils {
public:
    // Sederhanakan akar(n) menjadi k*akar(m)
    static SqrtResult simplifySqrt(double n);

    // Generate HTML untuk hasil akar kuadrat
    static QString sqrtHtml(double n, const QString &varName,
                            const QString &color, const QString &textMid);

    // Format angka: integer jika bulat, 2 desimal jika tidak
    static QString disp(double v);

    // Format angka selalu 2 desimal
    static QString fmt2(double v);

    // Format pecahan dari pembilang/penyebut
    static QString fracStr(double numerator, double denominator);

    // Konversi double ke bentuk pecahan string
    static QString toFrac(double val);

    // Parse string ke double dengan validasi
    // allowEmpty=true → string kosong return std::nullopt
    // Input invalid → throw std::runtime_error (pesan Indonesia)
    static std::optional<double> parseFloat(const QString &text,
                                            const QString &fieldName = "",
                                            bool allowEmpty = true);
};

#endif // MATHUTILS_H
