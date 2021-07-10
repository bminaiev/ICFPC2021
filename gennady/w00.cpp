/**
 *    author:  tourist
 *    created: 09.07.2021 17:21:07
**/
#undef _GLIBCXX_DEBUG

#include <random>
#include <vector>
#include <cassert>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <stack>
#include <bitset>
#include <algorithm>
#include <functional>
#include <numeric>
#include <utility>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "evaluator.h"

using namespace std;

template <typename A, typename B>
string to_string(pair<A, B> p);

template <typename A, typename B, typename C>
string to_string(tuple<A, B, C> p);

template <typename A, typename B, typename C, typename D>
string to_string(tuple<A, B, C, D> p);

string to_string(const string& s) {
  return '"' + s + '"';
}

string to_string(const char* s) {
  return to_string((string) s);
}

string to_string(bool b) {
  return (b ? "true" : "false");
}

string to_string(vector<bool> v) {
  bool first = true;
  string res = "{";
  for (int i = 0; i < static_cast<int>(v.size()); i++) {
    if (!first) {
      res += ", ";
    }
    first = false;
    res += to_string(v[i]);
  }
  res += "}";
  return res;
}

template <size_t N>
string to_string(bitset<N> v) {
  string res = "";
  for (size_t i = 0; i < N; i++) {
    res += static_cast<char>('0' + v[i]);
  }
  return res;
}

template <typename A>
string to_string(A v) {
  bool first = true;
  string res = "{";
  for (const auto &x : v) {
    if (!first) {
      res += ", ";
    }
    first = false;
    res += to_string(x);
  }
  res += "}";
  return res;
}

template <typename A, typename B>
string to_string(pair<A, B> p) {
  return "(" + to_string(p.first) + ", " + to_string(p.second) + ")";
}

template <typename A, typename B, typename C>
string to_string(tuple<A, B, C> p) {
  return "(" + to_string(get<0>(p)) + ", " + to_string(get<1>(p)) + ", " + to_string(get<2>(p)) + ")";
}

template <typename A, typename B, typename C, typename D>
string to_string(tuple<A, B, C, D> p) {
  return "(" + to_string(get<0>(p)) + ", " + to_string(get<1>(p)) + ", " + to_string(get<2>(p)) + ", " + to_string(get<3>(p)) + ")";
}

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

template <typename T>
class hungarian {
 public:
  int n;
  int m;
  vector<vector<T>> a;
  vector<T> u;
  vector<T> v;
  vector<int> pa;
  vector<int> pb;
  vector<int> way;
  vector<T> minv;
  vector<bool> used;
  T inf;

  hungarian(int _n, int _m) : n(_n), m(_m) {
    assert(n <= m);
    a = vector<vector<T>>(n, vector<T>(m));
    u = vector<T>(n + 1);
    v = vector<T>(m + 1);
    pa = vector<int>(n + 1, -1);
    pb = vector<int>(m + 1, -1);
    way = vector<int>(m, -1);
    minv = vector<T>(m);
    used = vector<bool>(m + 1);
    inf = numeric_limits<T>::max();
  }

  inline void add_row(int i) {
    fill(minv.begin(), minv.end(), inf);
    fill(used.begin(), used.end(), false);
    pb[m] = i;
    pa[i] = m;
    int j0 = m;
    do {
      used[j0] = true;
      int i0 = pb[j0];
      T delta = inf;
      int j1 = -1;
      for (int j = 0; j < m; j++) {
        if (!used[j]) {
          T cur = a[i0][j] - u[i0] - v[j];
          if (cur < minv[j]) {
            minv[j] = cur;
            way[j] = j0;
          }
          if (minv[j] < delta) {
            delta = minv[j];
            j1 = j;
          }
        }
      }
      for (int j = 0; j <= m; j++) {
        if (used[j]) {
          u[pb[j]] += delta;
          v[j] -= delta;
        } else {
          minv[j] -= delta;
        }
      }
      j0 = j1;
    } while (pb[j0] != -1);
    do {
      int j1 = way[j0];
      pb[j0] = pb[j1];
      pa[pb[j0]] = j0;
      j0 = j1;
    } while (j0 != m);
  }

  inline T current_score() {
    return -v[m];
  }

  inline T solve() {
    for (int i = 0; i < n; i++) {
      add_row(i);
    }
    return current_score();
  }
};

const int EPS_COEF = 1000000;

int main(int argc, char** argv) {
  ios::sync_with_stdio(false);
  cin.tie(0);
  if (argc != 2) {
    cerr << "usage: sol [test-id]" << '\n';
    return 0;
  }
  int xid = atoi(argv[1]);
  if (to_string(xid) != argv[1]) {
    cerr << "test-id must be an integer" << '\n';
    return 0;
  }

  ifstream in("../inputs_conv/" + to_string(xid) + ".problem");
  if (!in.is_open()) {
    cerr << "input " << xid << ".problem doesn't exist (check relative path?)" << '\n';
    return 0;
  }
  vector<Point> poly;
  vector<Point> vertices;
  vector<pair<int, int>> edges;
  int np;
  in >> np;
  poly.resize(np);
  for (int i = 0; i < np; i++) {
    in >> poly[i].x >> poly[i].y;
    poly[i].id = i;
  }
  int ne;
  in >> ne;
  edges.resize(ne);
  for (int i = 0; i < ne; i++) {
    in >> edges[i].first >> edges[i].second;
  }
  int nv;
  in >> nv;
  vertices.resize(nv);
  for (int i = 0; i < nv; i++) {
    in >> vertices[i].x >> vertices[i].y;
    vertices[i].id = i;
  }
  int eps;
  in >> eps;
  in.close();

  Evaluator E(poly, vertices, edges, eps);
  int max_x = 0;
  int max_y = 0;
  for (auto& p : poly) {
    max_x = max(max_x, p.x);
    max_y = max(max_y, p.y);
  }
  vector<vector<bool>> has_edge(nv, vector<bool>(nv, false));
  for (auto& e : edges) {
    has_edge[e.first][e.second] = has_edge[e.second][e.first] = true;
  }

  auto v = vertices;
  cout << E.eval(v) << '\n';
  cout << E.error_msg << '\n';

  for (int outer = 0; outer < 1000; outer++) {
    vector<double> dx(nv), dy(nv);

    hungarian<int> h(np, nv);
    for (int i = 0; i < np; i++) {
      for (int j = 0; j < nv; j++) {
        h.a[i][j] = (poly[i] - v[j]).abs2();
      }
    }
    h.solve();
    for (int i = 0; i < np; i++) {
      int j = h.pa[i];
      assert(j != -1);
      int d = h.a[i][j];
      if (d == 0) {
        continue;
      }
      double coeff = 0.5 * pow(d, -0.5);
      dx[j] += (poly[i] - v[j]).x * coeff;
      dy[j] += (poly[i] - v[j]).y * coeff;
    }

    for (int i = 0; i < nv; i++) {
      for (int j = 0; j < nv; j++) {
        if (has_edge[i][j]) {
          int d = (v[i] - v[j]).abs2();
          int o = (vertices[i] - vertices[j]).abs2();
          double ratio = 1.0 * d / o;
          if (ratio > 1 + 1.0 * eps / EPS_COEF) {
            double coeff = pow(d, -0.5) / (1 + exp(1 - ratio));
            dx[i] += (v[j] - v[i]).x * coeff;
            dy[i] += (v[j] - v[i]).y * coeff;
          }
          if (ratio > 0 && ratio < 1 - 1.0 * eps / EPS_COEF) {
            double coeff = pow(d, -0.5) / (1 + exp(ratio - 1));
            dx[i] -= (v[j] - v[i]).x * coeff;
            dy[i] -= (v[j] - v[i]).y * coeff;
          }
        }
      }
    }

    for (int i = 0; i < nv; i++) {
      if (!E.c.IsPointInside(v[i])) {
        double min_dist = 1e30;
        double bx = -1;
        double by = -1;
        for (int j = 0; j < np; j++) {
          double d = sqrt((v[i] - poly[j]).abs2());
          if (d < min_dist) {
            min_dist = d;
            bx = poly[j].x;
            by = poly[j].y;
          }
        }
        for (int j = 0; j < np; j++) {
          int k = (j + 1) % np;
          double a = sqrt((v[i] - poly[j]).abs2());
          double b = sqrt((v[i] - poly[k]).abs2());
          double c = sqrt((poly[j] - poly[k]).abs2());
          if (a * a < b * b + c * c && b * b < a * a + c * c) {
            double A = poly[j].y - poly[k].y;
            double B = poly[k].x - poly[j].x;
            double C = -A * poly[j].x - B * poly[j].y;
            double D = abs(A * v[i].x + B * v[i].y + C) / sqrt(A * A + B * B);
            if (D < min_dist) {
              min_dist = D;
              double AA = -B;
              double BB = A;
              double CC = -AA * v[i].x - BB * v[i].y;
              double DD = A * BB - B * AA;
              double DX = (B * CC - C * BB) / DD;
              double DY = (C * AA - A * CC) / DD;
              bx = DX;
              by = DY;
            }
          }
        }
        dx[i] += bx - v[i].x + (bx - v[i].x > 0 ? 1 : -1);
        dy[i] += by - v[i].y + (by - v[i].y > 0 ? 1 : -1);
      }
    }

    for (int i = 0; i < nv; i++) {
      v[i].x += (int) dx[i];
      v[i].y += (int) dy[i];
      if (dx[i] > 0) v[i].x += 1;
      if (dx[i] < 0) v[i].x -= 1;
      if (dy[i] > 0) v[i].y += 1;
      if (dy[i] < 0) v[i].y -= 1;
//      debug(v[i].x, v[i].y);
      if (!E.c.IsPointInside(v[i])) {
        debug("test", i);
        bool found = false;
        for (int xd = -1; xd <= 1; xd++) {
          for (int yd = -1; yd <= 1; yd++) {
            if (E.c.IsPointInside(Point(v[i].x + xd, v[i].y + yd))) {
              v[i].x += xd;
              v[i].y += yd;
              found = true;
              break;
            }
          }
          if (found) break;
        }
        if (!found) debug("fail", i);
      }
    }
    debug(v, dx, dy);
    cout << E.eval(v) << '\n';
    cout << E.error_msg << '\n';
  }

  cout << "done" << '\n';

  ofstream out("../outputs/" + to_string(xid) + ".answ");
  out << v.size() << '\n';
  for (auto& p : v) {
    out << p.x << " " << p.y << '\n';
  }
  out.close();
  return 0;
}
