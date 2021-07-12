#pragma once

#include "visualizer.h"
#include "common.h"

Visualizer v;
int capturedPointIndex;
int showIds;
int curVID;

void draw() {
    forn(i, bonusname.size()) {
        if (bonusname[i] == "GLOBALIST") {
            v.p.setPen(yellowPen);
            v.p.setBrush(yellowBrush);
        } else if (bonusname[i] == "BREAK_A_LEG") {
            v.p.setPen(bluePen);
            v.p.setBrush(blueBrush);
        } else if (bonusname[i] == "SUPERFLEX") {
            v.p.setPen(lightBluePen);
            v.p.setBrush(lightBlueBrush);
        } else if (bonusname[i] == "WALLHACK") {
            v.p.setPen(orangePen);
            v.p.setBrush(orangeBrush);
        } else {
            v.p.setPen(magentaPen);
            v.p.setBrush(magentaBrush);
        }

        v.p.drawEllipse(bonus[i].x * SCALE - 3, bonus[i].y * SCALE - 3, 7, 7);
    }    

    v.p.setPen(darkWhitePen);
    v.p.setBrush(darkWhiteBrush);
    
    forn(i, H) {
        v.p.drawLine(hole[i].x * SCALE, hole[i].y * SCALE, hole[i + 1].x * SCALE, hole[i + 1].y * SCALE);
        v.p.drawEllipse(hole[i].x * SCALE - 1, hole[i].y * SCALE - 1, 3, 3);
        int bd = inf;
        forn(j, N) {
            int cd = (hole[i] - points[j]).abs2();
            if (cd < bd) bd = cd;
        }
        if (showIds & 1)
            v.p.drawText(hole[i].x * SCALE - 10, hole[i].y * SCALE - 10, QString::number(i) + ":" + QString::number(bd));
    }

    v.p.setPen(grayPen);
    v.p.setBrush(transparentBrush);
    forn(i, N) {
        if (showIds & 2) {
            v.p.setFont(QFont("Tahoma", i == curVID ? 20 : 10));
            v.p.drawText(points[i].x * SCALE - 10, points[i].y * SCALE - 10, QString::number(i));
        }
        const int R = 2 + glue[i] * 5;
        v.p.drawEllipse(points[i].x * SCALE - R, points[i].y * SCALE - R, 2*R + 1, 2*R + 1);
        for (const auto& e : edges) {
            if ((e.from == capturedPointIndex && e.to == i) || 
                (e.to == capturedPointIndex && e.from == i)) {
                    double r = sqrt((1 + E / 1e6) * e.D);
                    v.p.drawEllipse((points[i].x - r) * SCALE, (points[i].y - r) * SCALE, 2*r * SCALE, 2*r * SCALE);
                    r = sqrt((1 - E / 1e6) * e.D);
                    v.p.drawEllipse((points[i].x - r) * SCALE, (points[i].y - r) * SCALE, 2*r * SCALE, 2*r * SCALE);
                }
        }
    }

    v.p.setPen(grayPen);
    forn(i, N)
        if (mt[i] != -1) {
            v.p.drawLine(points[i].x * SCALE, points[i].y * SCALE, hole[mt[i]].x * SCALE, hole[mt[i]].y * SCALE);
        }

    for (const auto& e : edges) {
        const ll cd = (points[e.from] - points[e.to]).abs2();
        if (1000000LL * abs(cd - e.D) > (ll)E * e.D) v.p.setPen(redPen);
        else v.p.setPen(greenPen);
        v.p.drawLine(points[e.from].x * SCALE, points[e.from].y * SCALE, points[e.to].x * SCALE, points[e.to].y * SCALE);
    }
}