#pragma once

#include "visualizer.h"
#include "common.h"

Visualizer v;
int capturedPointIndex;

void draw() {
    if (bonusname == "GLOBALIST") {
        v.p.setPen(yellowPen);
        v.p.setBrush(yellowBrush);
    } else if (bonusname == "BREAK_A_LEG") {
        v.p.setPen(bluePen);
        v.p.setBrush(blueBrush);
    } else {
        v.p.setPen(magentaPen);
        v.p.setBrush(magentaBrush);
    }

    v.p.drawEllipse(bonus.x * SCALE - 4, bonus.y * SCALE - 4, 9, 9);

    v.p.setPen(darkWhitePen);
    v.p.setBrush(darkWhiteBrush);
    
    forn(i, H) {
        v.p.drawLine(hole[i].x * SCALE, hole[i].y * SCALE, hole[i + 1].x * SCALE, hole[i + 1].y * SCALE);
        int bd = inf;
        forn(j, N) {
            int cd = (hole[i] - points[j]).abs2();
            if (cd < bd) bd = cd;
        }
        v.p.drawText(hole[i].x * SCALE - 10, hole[i].y * SCALE - 10, QString::number(i) + ":" + QString::number(bd));
    }

    v.p.setPen(grayPen);
    v.p.setBrush(transparentBrush);
    forn(i, N) {
        v.p.drawEllipse(points[i].x * SCALE - 2, points[i].y * SCALE - 2, 5, 5);
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
        const int cd = (points[e.from] - points[e.to]).abs2();
        if (abs(cd / double(e.D) - 1) * 1000000 > E) v.p.setPen(redPen);
        else v.p.setPen(greenPen);
        v.p.drawLine(points[e.from].x * SCALE, points[e.from].y * SCALE, points[e.to].x * SCALE, points[e.to].y * SCALE);
    }
}