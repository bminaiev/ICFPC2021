#include "common.h"
#include "drawer.h"

bool exited;

void doIter() {
    vector<PointD> forces(N);
    
    for (const Edge& e : edges) {
        double cd = (points[e.from] - points[e.to]).abs2();
        if (cd > e.D) {
            double f = (sqrt(cd) - sqrt(e.D)) / sqrt(e.D) * 0.1;
            PointD v = (points[e.to] - points[e.from]) * f;
            forces[e.from] = forces[e.from] + v;
            forces[e.to] = forces[e.to] - v;
        } else {
            double f = (sqrt(e.D) - sqrt(cd)) / (sqrt(cd) + 1) * 0.1;
            PointD v = (points[e.to] - points[e.from]) * f;
            forces[e.from] = forces[e.from] - v;
            forces[e.to] = forces[e.to] + v;
        }
    }

    forn(i, N) {
        points[i] = points[i] + forces[i];
    }
}

void doPull() {
    vector<PointD> forces(N);
    
    forn(j, N) {
        if (j == capturedPointIndex) continue;
        double cd = (points[j] - points[capturedPointIndex]).abs();
        PointD v = (points[capturedPointIndex] - points[j]) * 0.1 / (cd + 1);
        forces[j] = forces[j] + v;
    } 

    forn(i, N) {
        points[i] = points[i] + forces[i];
    }
}

void spread() {
    vector<PointD> forces(N);
    
    for (const Edge& e : edges) {
        double cd = (points[e.from] - points[e.to]).abs2();
        if (cd > e.D) {
            double f = (sqrt(cd) - sqrt(e.D)) / sqrt(e.D) * 0.1;
            PointD v = (points[e.to] - points[e.from]) * f;
            forces[e.from] = forces[e.from] + v;
            forces[e.to] = forces[e.to] - v;
        } else {
            double f = (sqrt(e.D) - sqrt(cd)) / (sqrt(cd) + 1) * 0.1;
            PointD v = (points[e.to] - points[e.from]) * f;
            forces[e.from] = forces[e.from] - v;
            forces[e.to] = forces[e.to] + v;
        }
    }

    forn(i, N)
        if (mt[i] != -1) {
            double cd = (hole[mt[i]] - points[i]).abs();
            forces[i] = forces[i] + (hole[mt[i]] - points[i]) / (cd + 1) * 0.32;
        }

    forn(i, N) {
        points[i] = points[i] + forces[i];
    }
}

void makeInt() {
    for (auto& p : points) {
        p.x = round(p.x);
        p.y = round(p.y);
    }
}

void check() {
    vector<Point> pts;
    for (const auto& p : points)
        pts.push_back(Point(round(p.x), round(p.y)));

    ll score = ev.eval(pts);
    cerr << "score: " << score << endl;
    if (score == -1) cerr << ev.error_msg << endl;
}

void doBind() {
    if (capturedPointIndex == -1) return;

    double bd = inf;
    int bi = -1;
    forn(i, H) {
        double cd = (hole[i] - points[capturedPointIndex]).abs2();
        if (cd < bd) {
            bd = cd;
            bi = i;
        }
    }

    mt[capturedPointIndex] = bi;
}

void doUnbind() {
    if (capturedPointIndex == -1) return;
    mt[capturedPointIndex] = -1;
}

int main(int argc, char* argv[])
{
    test_id = 1;
    readInput();
    v.setSize(1800, 1040);
    v.setOnKeyPress([](const QKeyEvent& ev) {
        if (ev.key() == Qt::Key_Escape) { exited = true; }
        if (ev.key() == Qt::Key_S) { saveSolution(); }
        if (ev.key() == Qt::Key_A) { loadSolution(); }
        if (ev.key() == Qt::Key_I) { makeInt(); }
        if (ev.key() == Qt::Key_O) { test_id--; if (test_id == 0) test_id = 59; readInput(); }
        if (ev.key() == Qt::Key_P) { test_id++; if (test_id == 60) test_id = 1; readInput(); }
        if (ev.key() == Qt::Key_1) { doIter(); }
        if (ev.key() == Qt::Key_2) { spread(); }
        if (ev.key() == Qt::Key_3) { doPull(); }
        if (ev.key() == Qt::Key_Z) { check(); }
        if (ev.key() == Qt::Key_B) { doBind(); }
        if (ev.key() == Qt::Key_U) { doUnbind(); }
    });

    v.setOnMouseClick([](const QMouseEvent& ev, double sx, double sy, double wx, double wy) {
        capturedPointIndex = -1;
        int bd = inf, bi = -1;
        forn(i, N) {
            int cd = sqr(points[i].x - wx) + sqr(points[i].y - wy);
            if (cd < bd) { bd = cd; bi = i; }
        }

        if (bd < 10) {
            capturedPointIndex = bi;
            cerr << "capture " << bi << endl;
        }
    });

    v.setOnMouseMove([](const QMouseEvent& ev, double sx, double sy, double wx, double wy) {
        if (capturedPointIndex == -1) return;
        points[capturedPointIndex].x = int(wx + 0.5);
        points[capturedPointIndex].y = int(wy + 0.5);
    });

    bool moved = true;
    while (!exited) {
        if (!moved) {
            auto w = v.app_->activeWindow();
            if (w) {
                auto g = QGuiApplication::screens()[1]->availableGeometry();
                std::cerr << g.x() << " " << g.y() << std::endl;
                w->move(g.x(), g.y());
                moved = true;
            }
        }
        RenderCycle r(v);
        draw();
    }
    
    return 0;
}