/*
 * IconPainters.cpp — Implementasi Ikon Dashboard
 * Padanan Python: _draw_icon_* (baris 521-646 di main.py)
 */
#include "widgets/IconPainters.h"
#include <QPen>
#include <QPainterPath>
#include <QColor>
#include <cmath>
#include <algorithm>

static QColor iconCol() { return QColor(0x3B, 0x6A, 0x5C); }

// ═══════════════════ LINECHART ═══════════════════
void drawIconLinechart(QPainter *p, int cx, int cy, int size)
{
    p->save(); p->translate(cx, cy);
    QColor col = iconCol();
    double lw = std::max(1.5, size * 0.06); double s = size * 0.42;
    QPen pen(col, lw, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    p->setPen(pen); p->setBrush(Qt::NoBrush);
    double ax_x=-s*0.80, ax_top=-s*0.85, ax_bot=s*0.75, ax_right=s*0.85;
    p->drawLine(int(ax_x),int(ax_bot),int(ax_x),int(ax_top));
    p->drawLine(int(ax_x),int(ax_bot),int(ax_right),int(ax_bot));
    double ah=s*0.18;
    p->drawLine(int(ax_x),int(ax_top),int(ax_x-ah*0.45),int(ax_top+ah));
    p->drawLine(int(ax_x),int(ax_top),int(ax_x+ah*0.45),int(ax_top+ah));
    p->drawLine(int(ax_right),int(ax_bot),int(ax_right-ah),int(ax_bot-ah*0.45));
    p->drawLine(int(ax_right),int(ax_bot),int(ax_right-ah),int(ax_bot+ah*0.45));
    double ratios[][2] = {{0.05,0.12},{0.25,0.28},{0.48,0.50},{0.68,0.65},{0.88,0.82}};
    double pts[5][2];
    for(int i=0;i<5;i++){pts[i][0]=ax_x+(ax_right-ax_x)*ratios[i][0]; pts[i][1]=ax_bot-(ax_bot-ax_top)*ratios[i][1];}
    for(int i=0;i<4;i++) p->drawLine(int(pts[i][0]),int(pts[i][1]),int(pts[i+1][0]),int(pts[i+1][1]));
    int dr=std::max(2,int(size*0.058)); p->setPen(Qt::NoPen); p->setBrush(col);
    for(int i=0;i<5;i++) p->drawEllipse(int(pts[i][0])-dr,int(pts[i][1])-dr,dr*2,dr*2);
    p->restore();
}

// ═══════════════════ BOX3D ═══════════════════
void drawIconBox3d(QPainter *p, int cx, int cy, int size)
{
    p->save(); p->translate(cx, cy);
    QColor col = iconCol();
    double lw = std::max(1.5, size*0.06); double h = size*0.32;
    QPen pen(col, lw, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    p->setPen(pen); p->setBrush(Qt::NoBrush);
    double c30=std::cos(M_PI/6), s30=std::sin(M_PI/6);
    auto iso=[&](double rx,double ry,double rz,double &ox,double &oy){
        ox=(rx*c30-rz*c30)*h; oy=(-ry+rx*s30*0.5+rz*s30*0.5)*h+h*0.15;
    };
    const char* names[]={"FBL","FBR","FTL","FTR","BBL","BBR","BTL","BTR"};
    double coords[8][3]={{-1,-1,1},{1,-1,1},{-1,1,1},{1,1,1},{-1,-1,-1},{1,-1,-1},{-1,1,-1},{1,1,-1}};
    double c2d[8][2];
    for(int i=0;i<8;i++) iso(coords[i][0],coords[i][1],coords[i][2],c2d[i][0],c2d[i][1]);
    // Faces
    int faces[][5]={{0,1,3,2,18},{2,3,7,6,32},{1,3,7,5,10}};
    for(auto &f : faces){
        QColor fc(col); fc.setAlpha(f[4]); p->setBrush(fc);
        QPainterPath path; path.moveTo(c2d[f[0]][0],c2d[f[0]][1]);
        for(int j=1;j<4;j++) path.lineTo(c2d[f[j]][0],c2d[f[j]][1]);
        path.closeSubpath(); p->drawPath(path);
    }
    p->setBrush(Qt::NoBrush);
    int edges[][2]={{0,1},{1,3},{3,2},{2,0},{2,6},{3,7},{6,7},{1,5},{7,5}};
    for(auto &e : edges) p->drawLine(int(c2d[e[0]][0]),int(c2d[e[0]][1]),int(c2d[e[1]][0]),int(c2d[e[1]][1]));
    p->restore();
}

// ═══════════════════ DICE ═══════════════════
void drawIconDice(QPainter *p, int cx, int cy, int size)
{
    p->save(); p->translate(cx, cy);
    QColor col = iconCol();
    double lw = std::max(1.5, size*0.06); int body = int(size*0.38);
    QPen pen(col, lw, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    p->setPen(pen); QColor bf(col); bf.setAlpha(14); p->setBrush(bf);
    int corner=int(body*0.35); p->drawRoundedRect(-body,-body,body*2,body*2,corner,corner);
    int dr=std::max(2,int(size*0.067)); int m=int(body*0.52);
    p->setPen(Qt::NoPen); p->setBrush(col);
    int dots[][2]={{-m,-m},{m,-m},{0,0},{-m,m},{m,m}};
    for(auto &d : dots) p->drawEllipse(d[0]-dr,d[1]-dr,dr*2,dr*2);
    p->restore();
}

// ═══════════════════ EQUATION ═══════════════════
void drawIconEquation(QPainter *p, int cx, int cy, int size)
{
    p->save(); p->translate(cx, cy);
    QColor col = iconCol();
    double lw = std::max(1.5, size*0.06); double s = size*0.42;
    p->setPen(QPen(col, lw, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p->setBrush(Qt::NoBrush);
    double y1=-s*0.35, y2=s*0.35;
    p->drawLine(int(-s*0.6),int(y1),int(s*0.6),int(y1));
    p->drawLine(int(-s*0.6),int(y2),int(s*0.6),int(y2));
    double ps=s*0.15;
    p->drawLine(int(-s*0.75),0,int(-s*0.75+ps*2),0);
    p->drawLine(int(-s*0.75+ps),int(-ps),int(-s*0.75+ps),int(ps));
    double eo=s*0.08;
    for(double yr : {y1, y2}){
        p->drawLine(int(s*0.3),int(yr-eo),int(s*0.5),int(yr-eo));
        p->drawLine(int(s*0.3),int(yr+eo),int(s*0.5),int(yr+eo));
    }
    int dr=std::max(2,int(size*0.045)); p->setPen(Qt::NoPen); p->setBrush(col);
    for(double xp : {-s*0.45, -s*0.15})
        for(double yp : {y1, y2})
            p->drawEllipse(int(xp)-dr,int(yp)-dr,dr*2,dr*2);
    p->restore();
}

// ═══════════════════ TRIANGLE ═══════════════════
void drawIconTriangle(QPainter *p, int cx, int cy, int size)
{
    p->save(); p->translate(cx, cy);
    QColor col = iconCol();
    double lw = std::max(1.5, size*0.06); double s = size*0.38;
    p->setPen(QPen(col, lw, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    double pt1x=-s, pt1y=s*0.7, pt2x=s, pt2y=s*0.7, pt3x=-s, pt3y=-s*0.7;
    QPainterPath path; path.moveTo(pt1x,pt1y); path.lineTo(pt2x,pt2y); path.lineTo(pt3x,pt3y); path.closeSubpath();
    QColor fill(col); fill.setAlpha(18); p->setBrush(fill); p->drawPath(path);
    double sq=s*0.25; p->setBrush(Qt::NoBrush);
    p->drawRect(int(-s),int(s*0.7-sq),int(sq),int(sq));
    p->restore();
}

// ═══════════════════ CHART ═══════════════════
void drawIconChart(QPainter *p, int cx, int cy, int size)
{
    p->save(); p->translate(cx, cy);
    QColor col = iconCol();
    double lw = std::max(1.5, size*0.06); double s = size*0.38;
    p->setPen(Qt::NoPen);
    double bw=s*0.35, sp=s*0.15; double heights[]={0.9,0.6,0.8,0.5}; double xs=-s*0.85;
    for(int i=0;i<4;i++){
        double bh=s*1.4*heights[i]; double x=xs+i*(bw+sp); double y=s*0.7-bh;
        QColor fill(col); fill.setAlpha(int(25+heights[i]*30)); p->setBrush(fill);
        p->drawRoundedRect(int(x),int(y),int(bw),int(bh),3,3);
    }
    p->setPen(QPen(col, lw)); p->setBrush(Qt::NoBrush);
    p->drawLine(int(-s*0.9),int(s*0.7),int(s*0.9),int(s*0.7));
    p->drawLine(int(-s*0.9),int(-s*0.7),int(-s*0.9),int(s*0.7));
    p->restore();
}

// ═══════════════════ CALC ═══════════════════
void drawIconCalc(QPainter *p, int cx, int cy, int size)
{
    p->save(); p->translate(cx, cy);
    QColor col = iconCol();
    double lw = std::max(1.5, size*0.06); double s = size*0.38;
    QPen pen(col, lw, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    p->setPen(pen); QColor bf(col); bf.setAlpha(14); p->setBrush(bf);
    p->drawRoundedRect(int(-s*0.72),int(-s*0.9),int(s*1.44),int(s*1.8),int(s*0.18),int(s*0.18));
    QColor df(col); df.setAlpha(30); p->setBrush(df);
    p->drawRoundedRect(int(-s*0.58),int(-s*0.78),int(s*1.16),int(s*0.45),int(s*0.06),int(s*0.06));
    p->setPen(Qt::NoPen); p->setBrush(col);
    int dr=std::max(2,int(s*0.12)); int gap=int(s*0.36);
    for(int row=0;row<3;row++)
        for(int ci=0;ci<3;ci++){
            int bx=int(-s*0.4+ci*gap); int by=int(-s*0.1+row*gap);
            p->drawRoundedRect(bx-dr,by-dr,dr*2,dr*2,dr/2,dr/2);
        }
    p->restore();
}

// ═══════════════════ DISPATCH ═══════════════════
void drawIcon(QPainter *p, const QString &key, int cx, int cy, int size)
{
    if (key == ICON_LINECHART) drawIconLinechart(p, cx, cy, size);
    else if (key == ICON_BOX3D) drawIconBox3d(p, cx, cy, size);
    else if (key == ICON_DICE) drawIconDice(p, cx, cy, size);
    else if (key == ICON_EQUATION) drawIconEquation(p, cx, cy, size);
    else if (key == ICON_TRIANGLE) drawIconTriangle(p, cx, cy, size);
    else if (key == ICON_CHART) drawIconChart(p, cx, cy, size);
    else if (key == ICON_CALC) drawIconCalc(p, cx, cy, size);
}
