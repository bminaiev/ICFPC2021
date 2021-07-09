#pragma once

#include "visualizer.h"
#include "common.h"

Visualizer v;
int capturedPointIndex;

void draw() {
    v.p.setPen(darkWhitePen);
    v.p.setBrush(darkWhiteBrush);
    
    forn(i, H) v.p.drawLine(hole[i].x * SCALE, hole[i].y * SCALE, hole[i + 1].x * SCALE, hole[i + 1].y * SCALE);

    v.p.setPen(grayPen);
    v.p.setBrush(transparentBrush);
    forn(i, N) {
        v.p.drawEllipse(points[i].x * SCALE - 2, points[i].y * SCALE - 2, 4, 4);
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

    for (const auto& e : edges) {
        const int cd = dist2(points[e.from], points[e.to]);
        if (abs(cd / double(e.D) - 1) * 1000000 < E) v.p.setPen(greenPen);
        else v.p.setPen(redPen);
        v.p.drawLine(points[e.from].x * SCALE, points[e.from].y * SCALE, points[e.to].x * SCALE, points[e.to].y * SCALE);
    }
}