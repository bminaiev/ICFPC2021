#include "common.h"
#include "drawer.h"

bool exited;

void solve() {
    readInput();
}

int main(int argc, char* argv[])
{
    test_id = string(argv[1]);
    v.setSize(1800, 1040);
    v.setOnKeyPress([](const QKeyEvent& ev) {
        if (ev.key() == Qt::Key_Escape) { exited = true; }
        if (ev.key() == Qt::Key_Space) { printSolution(); }
    });

    v.setOnMouseClick([](const QMouseEvent& ev, double sx, double sy, double wx, double wy) {
        cerr << wx << " " << wy << endl;
        capturedPointIndex = -1;
        forn(i, N)
            if (sqr(points[i].x - wx) + sqr(points[i].y - wy) < 10) {
                capturedPointIndex = i;
            }
        // clickedPointWorld = Vec2Float(wx, wy);
        // clickedPointScreen = Vec2Float(sx, sy);
    });

    v.setOnMouseMove([](const QMouseEvent& ev, double sx, double sy, double wx, double wy) {
        if (capturedPointIndex == -1) return;
        points[capturedPointIndex].x = int(wx + 0.5);
        points[capturedPointIndex].y = int(wy + 0.5);
    });

    thread solveThread(solve);

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