#include "common.h"
#include "drawer.h"

#define LOCAL

bool exited;

void debug_out() { cerr << endl; }

template <typename Head, typename... Tail>
void debug_out(Head H, Tail... T) {
  cerr << " " << to_string(H);
  debug_out(T...);
}

#ifdef LOCAL
#define debug(...) cerr << "[" << #__VA_ARGS__ << "]:", debug_out(__VA_ARGS__)
#else
#define debug(...) 42
#endif

void bruteforce() {
    int nv = N;
    int np = H;
    int eps = E;
  int max_x = 0;
  int max_y = 0;
  for (auto& p : poly) {
    max_x = max(max_x, p.x);
    max_y = max(max_y, p.y);
  }
  vector<vector<bool>> has_edge(nv, vector<bool>(nv, false));
  for (auto& e : edgePairs) {
    has_edge[e.first][e.second] = has_edge[e.second][e.first] = true;
  }

  vector<vector<double>> max_dist(nv, vector<double>(nv, 1e9));
  for (int i = 0; i < nv; i++) {
    max_dist[i][i] = 0;
  }
  for (int i = 0; i < nv; i++) {
    for (int j = 0; j < nv; j++) {
      if (has_edge[i][j]) {
        long long old_len = (srcPoints[i] - srcPoints[j]).abs2();
        // (new_len - old_len) * EPS_COEF <= eps * old_len
        // new_len <= eps * old_len / EPS_COEF + old_len
        double new_len = (double) old_len + (double) (eps * old_len) / (double) EPS_COEF;
        max_dist[i][j] = sqrt(new_len);
      }
    }
  }
  for (int k = 0; k < nv; k++) {
    for (int i = 0; i < nv; i++) {
      for (int j = 0; j < nv; j++) {
        max_dist[i][j] = min(max_dist[i][j], max_dist[i][k] + max_dist[k][j]);
      }
    }
  }

  vector<int> order(nv);
  iota(order.begin(), order.end(), 0);
  debug(nv, np, eps);
  if (nv <= 10) {
    auto best_order = order;
    auto best_seq = vector<int>(nv, 0);
    do {
      vector<int> seq(nv, 0);
      for (int i = 0; i < nv; i++) {
        for (int j = 0; j < i; j++) {
          if (has_edge[order[i]][order[j]]) {
            seq[i] += 1;
          }
        }
      }
      if (seq > best_seq) {
        best_seq = seq;
        best_order = order;
      }
    } while (next_permutation(order.begin(), order.end()));
    debug(best_order);
    debug(best_seq);
    order = best_order;
  } else {
    auto best_order = order;
    auto best_seq = vector<int>(nv, 0);
    mt19937 rng(60);
    for (auto iter = 0; iter < (int) 1e6; iter++) {
      shuffle(order.begin(), order.end(), rng);
      vector<int> seq(nv, 0);
      for (int i = 0; i < nv; i++) {
        for (int j = 0; j < i; j++) {
          if (has_edge[order[i]][order[j]]) {
            seq[i] += 1;
          }
        }
      }
      if (seq > best_seq) {
        best_seq = seq;
        best_order = order;
      }
    }
    debug(best_order);
    debug(best_seq);
    order = best_order;
  }

  vector<Point> v(nv, Point(-1, -1));
  int best_score = (int) 1e9;
  auto best_v = v;
  bool found = false;
  function<void(int)> Dfs = [&](int ii) {
      if (found) return;
    if (ii > 6) {
        forn(i, v.size()) 
          if (v[i].x > 0) {
            points[i].x = v[i].x;
            points[i].y = v[i].y;
          }
        found = true;
    }
    if (ii == nv) {
      int score = (int) ev.eval(v);
      if (score != -1) {
        if (score < best_score) {
          best_score = score;
          best_v = v;
          debug(best_v);
          debug(score, best_score);
        //   ofstream out("../outputs/" + to_string(test_id) + ".ans");
        //   out << best_v.size() << '\n';
          forn(i, best_v.size()) {
            points[i].x = best_v[i].x;
            points[i].y = best_v[i].y;
          }
        //   for (auto& p : best_v) {
        //     out << p.x << " " << p.y << '\n';
        //   }
        //   out.close();
          found = true;
        }
      }
      return;
    }
    if (v[order[ii]].x != -1) {
      Dfs(ii + 1);
      return;
    }
    if (v[0] == Point(0, 31)) {
//      debug(ii, v);
    }
    for (int x = 0; x <= max_x; x++) {
      for (int y = 0; y <= max_y; y++) {
        if (v[0] == Point(0, 31) && ii == 1 && x == 0 && y == 65) {
//          debug(ii, v, x, y);
        }
        if (ev.c.IsPointInside(Point(x, y))) {
//          debug("hi");
          v[order[ii]] = Point(x, y);
          bool ok = true;
          for (int jj = 0; jj < nv; jj++) {
            if (ii == jj || v[order[jj]].x == -1) {
              continue;
            }
            int i = order[ii];
            int j = order[jj];
            if (has_edge[i][j]) {
              if (!ev.c.IsSegmentInside(v[i], v[j])) {
                ok = false;
                break;
              }
              int new_len = (v[i] - v[j]).abs2();
              int old_len = (srcPoints[i] - srcPoints[j]).abs2();
              long long num = abs(new_len - old_len);
              long long den = old_len;
              if (v[0] == Point(0, 31) && ii == 1 && x == 0 && y == 65) {
//                debug(new_len, old_len);
//                debug(num * EPS_COEF, eps * den);
              }
              if (num * EPS_COEF > eps * den) {
                ok = false;
                break;
              }
            }
          }
          if (ok) {
            Dfs(ii + 1);
          }
          v[order[ii]] = Point(-1, -1);
        }
      }
    }
  };

  vector<int> test(np, -1);
  forn(i, N) if (mt[i] != -1) test[mt[i]] = i;

  vector<bool> taken(nv, false);
  for (int i = 0; i < np; i++) if (test[i] != -1) taken[test[i]] = true;

  function<void(int)> DfsZero = [&](int ii) {
      if (found) return;
    if (ii == np) {
      debug(v);
      Dfs(0);
      return;
    }
    for (int i = 0; i < nv; i++) {
      if (v[i].x == -1) {
        if (test[ii] != -1 && i != test[ii]) {
          continue;
        }
        if (test[ii] == -1 && taken[i]) {
          continue;
        }
        v[i] = poly[ii];
        bool ok = true;
        for (int j = 0; j < nv; j++) {
          if (i == j || v[j].x == -1) {
            continue;
          }
          {
            double dist = sqrt((v[i] - v[j]).abs2());
            if (dist > max_dist[i][j]) {
              ok = false;
              break;
            }
          }
          if (has_edge[i][j]) {
            if (!ev.c.IsSegmentInside(v[i], v[j])) {
              ok = false;
              break;
            }
            int new_len = (v[i] - v[j]).abs2();
            int old_len = (srcPoints[i] - srcPoints[j]).abs2();
            long long num = abs(new_len - old_len);
            long long den = old_len;
            if (num * EPS_COEF > eps * den) {
              ok = false;
              break;
            }
          }
        }
        if (ok) {
          DfsZero(ii + 1);
        }
        v[i] = Point(-1, -1);
      }
    }
  };
  
  DfsZero(0);
  cout << "done" << '\n';
}

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
        double m = forces[i].abs();
        if (m > 20) forces[i] = forces[i] / m * 20;
        points[i] = points[i] + forces[i];
    }
}

void doIterWithMt() {
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
        double m = forces[i].abs();
        if (m > 20) forces[i] = forces[i] / m * 20;
        if (mt[i] != -1) forces[i] = PointD(0, 0);
        points[i] = points[i] + forces[i];
    }
}

void doRepulse() {
    vector<PointD> forces(N);
    
    set<pii> ee;
    double avg = 0;
    for (const Edge& e : edges) {
        double cd = (points[e.from] - points[e.to]).abs2();
        ee.insert(pii(e.from, e.to));
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
        avg += sqrt(e.D);
    }

    avg /= edges.size();

    forn(i, N) forn(j, N) if (j != i) {
        pii q(i, j);
        if (ee.find(q) != ee.end()) continue;

        double cd = (points[i] - points[j]).abs();
        if (cd < avg && cd > 1) {
            PointD v = points[i] - points[j];
            v = v / cd;
            v = v * ((avg / cd) / 10);
            forces[j] = forces[j] - v;
            forces[i] = forces[i] + v;
        }
    }

    forn(i, N)
        if (mt[i] != -1) {
            double cd = (hole[mt[i]] - points[i]).abs();
            forces[i] = forces[i] + (hole[mt[i]] - points[i]) / (cd + 1) * 0.777;
        }

    forn(i, N) {
        double m = forces[i].abs();
        if (m > 20) forces[i] = forces[i] / m * 20;
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
        double m = forces[i].abs();
        if (m > 20) forces[i] = forces[i] / m * 20;
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
        double m = forces[i].abs();
        if (m > 20) forces[i] = forces[i] / m * 20;
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

void doAvg() {
    forn(i, N) {
        if (mt[i] != -1) continue;
        points[i] = PointD(0, 0);
        for (int x : g[i])
            points[i] = points[i] + points[x];
        points[i] = points[i] / g[i].size();
    }
}

void doShake() {
    if (capturedPointIndex == -1) return;
    for (int v : g[capturedPointIndex]) {
        if (mt[v] != -1) continue;
        int cd = (points[v] - points[capturedPointIndex]).abs();
        cd = cd / 2 + 1;
        forn(it, 100) {
            Point np(points[v].x + rand() % cd - rand() % cd, points[v].y + rand() % cd - rand() % cd);
            if (ev.c.IsPointInside(np)) {
                points[v].x = np.x;
                points[v].y = np.y;
                break;
            }
        }
    }    
}

void doFix() {
    forn(i, N) {
        if (mt[i] != -1) continue;

        auto calcPen = [&](const PointD& sh) -> double {
            double pen = 0;
            for (int v : g[i]) {
                int cd = (points[i] + sh - points[v]).abs2();
                int od = (srcPoints[i] - srcPoints[v]).abs2();
                if (abs(cd / double(od) - 1) > E / 1e6) {
                    pen += abs(sqrt(cd) - sqrt(od));
                }
            }
            return pen;
        };

        double best = 1e9;
        PointD bsh(0, 0);
        for (int dx = -10; dx <= 10; dx++)
            for (int dy = -10; dy <= 10; dy++) {
                PointD sh(dx, dy);
                Point t(points[i].x + dx, points[i].y + dy);
                if (!ev.c.IsPointInside(t)) continue;
                double cscore = calcPen(sh);
                if (cscore < best) {
                    best = cscore;
                    bsh = sh;
                }
            }

        points[i] = points[i] + bsh;
    }
}

void doReset() {
    forn(i, N) {
        if (mt[i] != -1) points[i] = hole[mt[i]];
        else points[i] = PointD(70, 70);
    }
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

void doBindAll() {
    forn(cpp, N) {
        double bd = inf;
        int bi = -1;
        forn(i, H) {
            double cd = (hole[i] - points[cpp]).abs2();
            if (cd < bd) {
                bd = cd;
                bi = i;
            }
        }

        mt[cpp] = bd < 27 ? bi : -1;
    }
}


void doUnbind() {
    if (capturedPointIndex == -1) return;
    mt[capturedPointIndex] = -1;
}

int main(int argc, char* argv[])
{
    test_id = 1;
    readInput();
    v.setSize(2000, 1400);
    const int TC = 88;
    v.setOnKeyPress([](const QKeyEvent& ev) {
        if (ev.key() == Qt::Key_Escape) { exited = true; }
        if (ev.key() == Qt::Key_S) { saveSolution(); }
        if (ev.key() == Qt::Key_H) { saveHint(); }
        if (ev.key() == Qt::Key_A) { loadSolution(); }
        if (ev.key() == Qt::Key_I) { makeInt(); }
        if (ev.key() == Qt::Key_O) { test_id--; if (test_id == 0) test_id = TC; readInput(); }
        if (ev.key() == Qt::Key_P) { test_id++; if (test_id == TC + 1) test_id = 1; readInput(); }
        if (ev.key() == Qt::Key_1) { doIter(); }
        if (ev.key() == Qt::Key_2) { spread(); }
        if (ev.key() == Qt::Key_3) { doPull(); }
        if (ev.key() == Qt::Key_4) { doAvg(); }
        if (ev.key() == Qt::Key_5) { doReset(); }
        if (ev.key() == Qt::Key_6) { doRepulse(); }
        if (ev.key() == Qt::Key_7) { doShake(); }
        if (ev.key() == Qt::Key_8) { doIterWithMt(); }
        if (ev.key() == Qt::Key_Z) { check(); }
        if (ev.key() == Qt::Key_B) { doBind(); }
        if (ev.key() == Qt::Key_M) { doBindAll(); }
        if (ev.key() == Qt::Key_U) { doUnbind(); }
        if (ev.key() == Qt::Key_F) { bruteforce(); }
        if (ev.key() == Qt::Key_G) { doFix(); }
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