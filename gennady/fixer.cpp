/**
 *    author:  tourist
 *    created: 09.07.2021 17:21:07
**/
#undef _GLIBCXX_DEBUG

const bool can_move_from_holes = false;

#include <chrono>
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

const int VERTEX_ID = 27;

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
  
  
  
  
  string FILENAME = "../outputs_romka/" + to_string(xid) + ".ans";



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

  vector<Point> v(nv, Point(-1, -1));
  ifstream in_ans(FILENAME);
  int nv_test;
  in_ans >> nv_test;
  assert(nv == nv_test);
  for (int i = 0; i < nv; i++) {
    in_ans >> v[i].x >> v[i].y;
  }
  in_ans.close();


  double coef = 0;
  double pw = 2;

  auto CalcEdges = [&]() {
    double cnt = 0;
    for (auto& e : edges) {
      int i = e.first;
      int j = e.second;
      int new_len = (v[i] - v[j]).abs2();
      int old_len = (vertices[i] - vertices[j]).abs2();
      long long num = abs(new_len - old_len);
      long long den = old_len;
      if (num * EPS_COEF > eps * den) {
        double ratio = 1.0 * new_len / old_len;
        double diff = abs(ratio - 1) - 1.0 * eps / EPS_COEF;
        cnt += pow(diff, pw) + coef;
//        cnt += pow(1.0 * (num * EPS_COEF) / (eps * den) - 1, 2) + 0.1;
      }
    }
    return cnt;
  };

  vector<vector<bool>> is_hole(max_x + 1, vector<bool>(max_y + 1));
  for (auto& p : poly) {
    is_hole[p.x][p.y] = true;
  }

  vector<bool> can_move(nv);
  for (int i = 0; i < nv; i++) {
    can_move[i] = !is_hole[v[i].x][v[i].y];
  }

  mt19937 rng((unsigned int) chrono::steady_clock::now().time_since_epoch().count());

  for (int very_outer = 0; very_outer < 100; very_outer++) {
//  coef = 0;
//  auto best_score = CalcEdges();
//  coef = min(best_score * 0.2, 0.3);
  coef = (1.0 * rng() / (1LL << 32)) * 0.3;
  pw = (1.0 * rng() / (1LL << 32)) * 2 + 1;
  auto best_score = CalcEdges();
  auto best_v = v;
    for (auto& e : edges) {
      int i = e.first;
      int j = e.second;
      int new_len = (best_v[i] - best_v[j]).abs2();
      int old_len = (vertices[i] - vertices[j]).abs2();
      long long num = abs(new_len - old_len);
      long long den = old_len;
      if (num * EPS_COEF > eps * den) {
        double ratio = 1.0 * new_len / old_len;
        double diff = abs(ratio - 1) - 1.0 * eps / EPS_COEF;
        debug(i, j, ratio, diff);
      }
    }
  debug(coef, pw, best_score);

//  double init_temp = 10;
//  double final_temp = 0.00001;
  double init_temp = 0.05;
  double final_temp = 0.000001;
  const int ITERS = 2000;
  can_move[VERTEX_ID] = false;
  for (int outer = 0; outer < ITERS; outer++) {
    double temp = init_temp * pow(final_temp / init_temp, outer * 1.0 / ITERS);
    for (int rep = 0; rep < nv; rep++) {
      int i;
      do {
        i = rng() % nv;
      } while (!can_move_from_holes && !can_move[i]);
      int dx = (int) (rng() % 3) - 1;
      int dy = (int) (rng() % 3) - 1;
      if (dx == 0 && dy == 0) continue;
/*      if (rng() % 2 == 0) {
        dx = (int) (rng() % 101) - 50;
        dy = (int) (rng() % 101) - 50;
      }*/
      auto new_vi = v[i] + Point(dx, dy);
      if (E.c.IsPointInside(new_vi)) {
        vector<bool> moved(nv, false);
        vector<int> que(1, i);
        moved[i] = true;
        bool fail = false;
        for (int b = 0; b < (int) que.size(); b++) {
          for (int j = 0; j < nv; j++) {
            if (has_edge[que[b]][j]) {
              if (!moved[j]) {
                bool bad_len = false;
                Point new_vq = v[que[b]] + Point(dx, dy);
                int now = (new_vq - v[j]).abs2();
                int old = (vertices[j] - vertices[que[b]]).abs2();
                double ratio = 1.0 * now / old;
                if (abs(ratio - 1) > 1.0 * eps / EPS_COEF + 1e-9) {
//                  bad_len = true;
                }
                if (bad_len || !E.c.IsSegmentInside(v[que[b]] + Point(dx, dy), v[j])) {
                  if (!E.c.IsPointInside(v[j] + Point(dx, dy))) {
                    fail = true;
                    break;
                  }
                  if (!can_move_from_holes && !can_move[j]) {
                    fail = true;
                    break;
                  }
                  que.push_back(j);
                  moved[j] = true;
                }
              } else {
                bool bad_len = false;
                Point new_vq = v[que[b]] + Point(dx, dy);
                Point new_vj = v[j] + Point(dx, dy);
                int now = (new_vq - new_vj).abs2();
                int old = (vertices[j] - vertices[que[b]]).abs2();
                double ratio = 1.0 * now / old;
                if (abs(ratio - 1) > 1.0 * eps / EPS_COEF + 1e-9) {
//                  bad_len = true;
                }
                if (bad_len || !E.c.IsSegmentInside(v[que[b]] + Point(dx, dy), v[j] + Point(dx, dy))) {
                  fail = true;
                  break;
                }
              }
            }
          }
          if (fail) {
            break;
          }
        }
        if (fail) {
          continue;
        }
        auto old_res = CalcEdges();
        auto old_v = v;
        for (int ii : que) {
          v[ii] = v[ii] + Point(dx, dy);
        }
        auto new_res = CalcEdges();
        auto func = new_res - old_res;
  //            debug(func);
        if (func <= 0 || (func > 0 && 1.0 * rng() / (1LL << 32) < exp(-func / temp))) {
          // ok!
/*          debug("hi", que);
          for (int i = 0; i < nv; i++) {
            for (int j = i + 1; j < nv; j++) {
              if (has_edge[i][j]) {
                int now = (v[i] - v[j]).abs2();
                int old = (vertices[j] - vertices[i]).abs2();
                double ratio = 1.0 * now / old;
                if (abs(ratio - 1) > 1.0 * eps / EPS_COEF + 1e-9) {
                  debug(i, j, que);
                  return 0;
                }
              }
            }
          }*/
        } else {
          v = old_v;
        }
      }
    }
    auto score = CalcEdges();
    if (score < best_score) {
      best_score = score;
      best_v = v;
    }
//    E.eval(v);
//    debug(E.error_msg);
    if (outer % 100 == 0)
      debug(temp, score, best_score);
//    cout << E.error_msg << '\n';
  }

    for (auto& e : edges) {
      int i = e.first;
      int j = e.second;
      int new_len = (best_v[i] - best_v[j]).abs2();
      int old_len = (vertices[i] - vertices[j]).abs2();
      long long num = abs(new_len - old_len);
      long long den = old_len;
      if (num * EPS_COEF > eps * den) {
        double ratio = 1.0 * new_len / old_len;
        double diff = abs(ratio - 1) - 1.0 * eps / EPS_COEF;
        debug(i, j, ratio, diff);
      }
    }

  debug(best_score);

  ofstream out(FILENAME);
  out << best_v.size() << '\n';
  for (auto& p : best_v) {
    out << p.x << " " << p.y << '\n';
  }
  out.close();

  v = best_v;
  }


/*  vector<Point> v(nv, Point(-1, -1));
  int best_score = (int) 1e9;
  auto best_v = v;
  function<void(int)> Dfs = [&](int ii) {
    if (ii == nv) {
      int score = (int) E.eval(v);
      if (score != -1) {
        if (score < best_score) {
          best_score = score;
          best_v = v;
          debug(best_v);
          debug(score, best_score);
          ofstream out("../outputs/" + to_string(xid) + ".ans");
          out << best_v.size() << '\n';
          for (auto& p : best_v) {
            out << p.x << " " << p.y << '\n';
          }
          out.close();
        }
      }
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
        if (E.c.IsPointInside(Point(x, y))) {
//          debug("hi");
          v[order[ii]] = Point(x, y);
          bool ok = true;
          for (int jj = 0; jj < ii; jj++) {
            int i = order[ii];
            int j = order[jj];
            if (has_edge[i][j]) {
              if (!E.c.IsSegmentInside(v[i], v[j])) {
                ok = false;
                break;
              }
              int new_len = (v[i] - v[j]).abs2();
              int old_len = (vertices[i] - vertices[j]).abs2();
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
  Dfs(0);
  cout << "done" << '\n';

  ofstream out("../outputs/" + to_string(xid) + ".ans");
  out << best_v.size() << '\n';
  for (auto& p : best_v) {
    out << p.x << " " << p.y << '\n';
  }
  out.close();*/
  return 0;
}
