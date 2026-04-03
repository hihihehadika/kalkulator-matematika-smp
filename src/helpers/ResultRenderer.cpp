/*
 * ResultRenderer.cpp — Implementasi Renderer Hasil HTML
 * Padanan Python: baris 1345-1422 di main.py
 * IDENTIK PYTHON — layout, warna, dan border yang sama
 */
#include "helpers/ResultRenderer.h"

// ── Identik Python _html_title() ──
QString htmlTitle(const QString &text, const ThemeColors &t)
{
    return QString(
        "<p style='font-size:16px;font-weight:700;color:%1;margin:0 0 12px'>%2</p>"
    ).arg(t.text, text);
}

// ── Identik Python _html_row() ──
QString htmlRow(const QString &label, const QString &value,
                const ThemeColors &t, bool big,
                const QString &color)
{
    QString c = color.isEmpty() ? t.text_mid : color;
    QString fs = big ? "17px" : "14px";
    QString fw = big ? "700" : "400";
    return QString(
        "<p style='font-size:%1;font-weight:%2;color:%3;margin:4px 0'>%4 %5</p>"
    ).arg(fs, fw, c, label, value);
}

// ── Identik Python _html_div() ──
QString htmlDiv(const ThemeColors &t)
{
    return QString(
        "<p style='color:%1;margin:8px 0'>————————————————————</p>"
    ).arg(t.border);
}

// ── Identik Python _html_big() ──
QString htmlBig(const QString &text, const ThemeColors &t)
{
    return QString(
        "<p style='font-size:20px;font-weight:700;color:%1;margin:10px 0 5px'>%2</p>"
    ).arg(t.success, text);
}

// ── Identik Python _result_ok() ──
void resultOk(QLabel *label, const QString &html, const ThemeColors &t)
{
    QString icon = QString(
        "<div style='display:inline-block;width:20px;height:20px;border-radius:10px;"
        "background:%1;text-align:center;line-height:20px;font-size:11px;"
        "font-weight:700;color:#fff;margin-bottom:8px'>OK</div>"
    ).arg(t.success);

    label->setText(QString("<div style='font-family:Segoe UI,Arial'>%1%2</div>").arg(icon, html));
    label->setStyleSheet(QString(
        "padding:24px 28px;background:%1;"
        "border-left:4px solid %2;border-top:none;border-right:none;border-bottom:none;"
        "border-radius:12px;color:%3;line-height:1.9;"
    ).arg(t.success_bg, t.success, t.text));
}

// ── Identik Python _result_err() ──
void resultErr(QLabel *label, const QString &msg, const ThemeColors &t)
{
    QString icon = QString(
        "<div style='display:inline-block;width:20px;height:20px;border-radius:10px;"
        "background:%1;text-align:center;line-height:20px;font-size:11px;"
        "font-weight:700;color:#fff;margin-bottom:8px'>!</div>"
    ).arg(t.error);

    label->setText(QString(
        "<div style='font-family:Segoe UI,Arial'>%1"
        "<p style='font-size:15px;font-weight:700;color:%2;margin:0 0 8px'>Perhatian</p>"
        "<p style='font-size:14px;color:%3;margin:0;line-height:1.9'>%4</p></div>"
    ).arg(icon, t.error, t.text_mid, msg));
    label->setStyleSheet(QString(
        "padding:24px 28px;background:%1;"
        "border-left:4px solid %2;border-top:none;border-right:none;border-bottom:none;"
        "border-radius:12px;color:%3;line-height:1.9;"
    ).arg(t.error_bg, t.error, t.text));
}

// ── Identik Python _result_history() ──
void resultHistory(QLabel *label, const QVector<QString> &history,
                   const ThemeColors &t)
{
    if (history.isEmpty()) {
        label->setText("Belum ada riwayat.");
        label->setStyleSheet("");
        return;
    }

    bool isDark = (t.bg == "#0D0D0F");
    QString panel_bg, panel_border, new_bg, new_border, new_text, new_icon_bg, old_bg, old_border, old_text;

    if (isDark) {
        panel_bg = "#141420";
        panel_border = "rgba(129,140,248,0.35)";
        new_bg = "rgba(99,102,241,0.18)";
        new_border = "rgba(129,140,248,0.55)";
        new_text = "#A5B4FC";
        new_icon_bg = "#6366F1";
        old_bg = "rgba(55,55,75,0.25)";
        old_border = "rgba(100,100,130,0.20)";
        old_text = "#8888A8";
    } else {
        panel_bg = "#F5F3FF";
        panel_border = "rgba(99,102,241,0.30)";
        new_bg = "rgba(99,102,241,0.10)";
        new_border = "rgba(99,102,241,0.40)";
        new_text = "#4338CA";
        new_icon_bg = "#6366F1";
        old_bg = "rgba(0,0,0,0.03)";
        old_border = "rgba(0,0,0,0.08)";
        old_text = "#6B7280";
    }

    QString items;
    for (int i = history.size() - 1; i >= 0; --i) {
        bool isNew = (i == history.size() - 1);
        if (isNew) {
            items += QString(
                "<div style='background:%1;border:1.5px solid %2;"
                "border-radius:6px;padding:6px 12px;margin:3px 0 8px 0'>"
                "<span style='font-size:11px;color:%3'>✦</span>"
                "&nbsp;&nbsp;"
                "<span style='font-size:12px;font-weight:600;color:%4'>%5</span>"
                "</div>"
            ).arg(new_bg, new_border, new_icon_bg, new_text, history[i]);
        } else {
            items += QString(
                "<div style='background:%1;border:1px solid %2;"
                "border-radius:6px;padding:7px 14px;margin:3px 0'>"
                "<span style='font-size:12px;color:%3'>%4</span>"
                "</div>"
            ).arg(old_bg, old_border, old_text, history[i]);
        }
    }

    label->setText(QString("<div style='font-family:Segoe UI,Arial'>%1</div>").arg(items));
    label->setStyleSheet(QString(
        "padding:16px 20px;background:%1;"
        "border-left:4px solid %2;border-top:none;border-right:none;border-bottom:none;"
        "border-radius:12px;color:%3;line-height:1.6;"
    ).arg(panel_bg, panel_border, t.text));
}
