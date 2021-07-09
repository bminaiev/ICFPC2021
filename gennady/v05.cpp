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


  vector<vector<vector<Point>>> delta1(nv, vector<vector<Point>>(nv));
  vector<vector<vector<vector<bool>>>> is_good(nv, vector<vector<vector<bool>>>(nv, vector<vector<bool>>(2 * max_x + 1, vector<bool>(2 * max_y + 1))));
  for (int i = 0; i < nv; i++) {
    for (int j = 0; j < nv; j++) {
      if (has_edge[i][j]) {
        for (int dx = -max_x; dx <= max_x; dx++) {
          for (int dy = -max_y; dy <= max_y; dy++) {
            int new_len = Point(dx, dy).abs2();
            int old_len = (vertices[i] - vertices[j]).abs2();
            long long num = abs(new_len - old_len);
            long long den = old_len;
            if (num * EPS_COEF > eps * den) {
              continue;
            }
            delta1[i][j].push_back(Point(dx, dy));
            is_good[i][j][dx + max_x][dy + max_y] = true;
          }
        }
        debug(i, j, delta1[i][j].size());
      }
    }
  }

  vector<vector<vector<vector<vector<vector<Point>>>>>> delta2(nv,
    vector<vector<vector<vector<vector<Point>>>>>(nv,
    vector<vector<vector<vector<Point>>>>(nv,
    vector<vector<vector<Point>>>(2 * max_x + 1,
    vector<vector<Point>>(2 * max_y + 1)))));
  for (int i = 0; i < nv; i++) {
    for (int j = 0; j < nv; j++) {
      for (int k = 0; k < nv; k++) {
        if (i != j && i != k && j != k && has_edge[i][k] && has_edge[j][k]) {
          for (int dx = -max_x; dx <= max_x; dx++) {
            for (int dy = -max_y; dy <= max_y; dy++) {
              for (auto& d : delta1[i][k]) {
                int nx = dx - d.x;
                int ny = dy - d.y;
                if (nx >= -max_x && ny >= -max_y && nx <= max_x && ny <= max_y) {
                  if (is_good[k][j][nx + max_x][ny + max_y]) {
                    delta2[i][j][k][dx + max_x][dy + max_y].push_back(d);
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  vector<int> order(nv);
  iota(order.begin(), order.end(), 0);
  debug(nv, eps);
  if (nv <= 10) {
    auto best_order = order;
    auto best_seq = vector<int>(nv, 0);
    do {
      vector<int> seq(2 * nv, 0);
      for (int i = 0; i < nv; i++) {
        for (int j = 0; j < i; j++) {
          if (has_edge[order[i]][order[j]]) {
            seq[i] += 1000000;
            seq[i + nv] += - (int) delta1[order[i]][order[j]].size();
          }
        }
      }
      if (seq >= best_seq) {
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
    mt19937 rng(58);
    for (auto iter = 0; iter < (int) 1e6; iter++) {
      shuffle(order.begin(), order.end(), rng);
      vector<int> seq(2 * nv, 0);
      for (int i = 0; i < nv; i++) {
        for (int j = 0; j < i; j++) {
          if (has_edge[order[i]][order[j]]) {
            seq[i] += 1000000;
            seq[i + nv] += - (int) delta1[order[i]][order[j]].size();
          }
        }
      }
      if (seq >= best_seq) {
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
    int id1 = -1;
    int id2 = -1;
    for (int jj = 0; jj < ii; jj++) {
      int i = order[ii];
      int j = order[jj];
      if (has_edge[i][j]) {
        if (id1 == -1) {
          id1 = j;
        } else {
          if (id2 == -1) {
            id2 = j;
          }
        }
      }
    }
    if (id1 == -1) {
      for (int x = 0; x <= max_x; x++) {
        for (int y = 0; y <= max_y; y++) {
          if (E.c.IsPointInside(Point(x, y))) {
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
      return;
    }
    if (id2 == -1) {
      for (auto& delta : delta1[id1][order[ii]]) {
        int x = v[id1].x + delta.x;
        int y = v[id1].y + delta.y;
        if (E.c.IsPointInside(Point(x, y))) {
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
      return;
    }
    auto dd = v[id2] - v[id1];
    for (auto& delta : delta2[id1][id2][order[ii]][dd.x + max_x][dd.y + max_y]) {
      int x = v[id1].x + delta.x;
      int y = v[id1].y + delta.y;
      if (E.c.IsPointInside(Point(x, y))) {
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
  };
  Dfs(0);
  cout << "done" << '\n';

/*  ofstream out("../outputs/" + to_string(xid) + ".ans");
  out << best_v.size() << '\n';
  for (auto& p : best_v) {
    out << p.x << " " << p.y << '\n';
  }
  out.close();*/
  return 0;
}
