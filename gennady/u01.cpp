/**
 *    author:  tourist
 *    created: 09.07.2021 17:21:07
**/
#undef _GLIBCXX_DEBUG

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

  vector<Point> v(nv, Point(-1, -1));
/*  ifstream in_ans("../outputs/" + to_string(xid) + ".ans");
  int nv_test;
  in_ans >> nv_test;
  assert(nv == nv_test);
  for (int i = 0; i < nv; i++) {
    in_ans >> v[i].x >> v[i].y;
  }
  in_ans.close();*/

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

  vector<vector<double>> max_dist(nv, vector<double>(nv, 1e9));
  for (int i = 0; i < nv; i++) {
    max_dist[i][i] = 0;
  }
  for (int i = 0; i < nv; i++) {
    for (int j = 0; j < nv; j++) {
      if (has_edge[i][j]) {
        long long old_len = (vertices[i] - vertices[j]).abs2();
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

  vector<int> mt(np, -1);
  while (true) {
    vector<int> max_path;
    int max_start = -1;
    for (int i = 0; i < np; i++) {
      if (mt[i] == -1) {
        for (int j = 0; j < nv; j++) {
          if (v[j].x == -1) {
            int III = i;
            vector<bool> used(nv, false);
            vector<int> path(1, j);
            used[j] = true;
            function<void(int, int)> Dfs = [&](int ip, int iv) {
              if (1.0 * clock() / CLOCKS_PER_SEC > 1.0) return;
              if ((int) max_path.size() == np) {
                return;
              }
              { // validation start
                for (int i = 0; i < (int) path.size(); i++) {
                  int ip = (III + i) % np;
                  int iv = path[i];
                  mt[ip] = iv;
                  v[iv] = poly[ip];
                }

                bool fail = false;
                for (int i = 0; i < nv; i++) {
                  for (int j = i + 1; j < nv; j++) {
                    if (v[i].x != -1 && v[j].x != -1 && has_edge[i][j]) {
                      if (!(E.c.IsSegmentInside(v[i], v[j]))) {
                        fail = true;
                        break;
                      }
                      int new_len = (v[i] - v[j]).abs2();
                      if (new_len > max_dist[i][j] * max_dist[i][j] + 1e-9) {
                        fail = true;
                        break;
                      }
                      int old_len = (vertices[i] - vertices[j]).abs2();
                      long long num = abs(new_len - old_len);
                      long long den = old_len;
                      if (!(num * EPS_COEF <= eps * den)) {
                        fail = true;
                        break;
                      }
                    }
                    if (fail) {
                      break;
                    }
                  }
                }
                for (int i = 0; i < (int) path.size(); i++) {
                  int ip = (III + i) % np;
                  int iv = path[i];
                  mt[ip] = -1;
                  v[iv] = Point(-1, -1);
                }
                if (fail) {
                  return;
                }
              } // validation end


              if (path.size() > max_path.size()) {
                max_path = path;
                max_start = i;
                debug(np, nv, max_path.size(), max_path, max_start);
              }
              int jp = (ip + 1) % np;
              if ((int) path.size() == np || mt[jp] != -1) {
                return;
              }
              int new_len = (poly[ip] - poly[jp]).abs2();
              for (int jv = 0; jv < nv; jv++) {
                if (!used[jv] && v[jv].x == -1) {
                  int old_len = (vertices[iv] - vertices[jv]).abs2();
                  long long num = abs(new_len - old_len);
                  long long den = old_len;
                  if (num * EPS_COEF <= eps * den) {
                    used[jv] = true;
                    path.push_back(jv);
                    Dfs(jp, jv);
                    path.pop_back();
                    used[jv] = false;
                  }
                }
              }
            };
            Dfs(i, j);
          }
        }
      }
    }
    debug(np, nv, max_path, max_start);
    if (max_path.size() <= 3) {
      break;
    }
    for (int i = 0; i < (int) max_path.size(); i++) {
      int ip = (max_start + i) % np;
      int iv = max_path[i];
      mt[ip] = iv;
      v[iv] = poly[ip];
    }
  }
 
  debug(v);

  for (int i = 0; i < nv; i++) {
    for (int j = i + 1; j < nv; j++) {
      if (v[i].x != -1 && v[j].x != -1 && has_edge[i][j]) {
        assert(E.c.IsSegmentInside(v[i], v[j]));
        int new_len = (v[i] - v[j]).abs2();
        int old_len = (vertices[i] - vertices[j]).abs2();
        long long num = abs(new_len - old_len);
        long long den = old_len;
        assert(num * EPS_COEF <= eps * den);
      }
    }
  }

  long long best_score = (int) 1e9;
  auto best_v = v;

  while (true) {
    vector<bool> done(nv, false);
    for (int i = 0; i < nv; i++) {
      if (v[i].x != -1) {
        done[i] = true;
      }
    }
    vector<int> order;
    while (true) {
      int mx = -1;
      int who = -1;
      for (int i = 0; i < nv; i++) {
        if (!done[i]) {
          int cnt = 0;
          for (int j = 0; j < nv; j++) {
            if (done[j] && has_edge[i][j]) {
              ++cnt;
            }
          }
          if (cnt > mx) {
            mx = cnt;
            who = i;
          }
        }
      }
  //    debug(mx, who);
      if (who == -1) {
        break;
      }
      order.push_back(who);
      done[who] = true;
    }
    function<void(int)> Dfs = [&](int id) {
      if (id == (int) order.size()) {
        auto score = E.eval(v);
        debug(score, best_score, order, v);
        if (score < best_score) {
          best_score = score;
          best_v = v;
          ofstream out("../outputs/" + to_string(xid) + ".ansu");
          out << best_v.size() << '\n';
          for (auto& p : best_v) {
            out << p.x << " " << p.y << '\n';
          }
          out.close();
          if (score == 0) {
            exit(0);
          }
        }
        return;
      }
      int i = order[id];
      for (int x = 0; x <= max_x; x++) {
        for (int y = 0; y <= max_y; y++) {
          if (!E.c.IsPointInside(Point(x, y))) {
            continue;
          }
          v[i] = Point(x, y);
          bool ok = true;
          for (int j = 0; j < nv; j++) {
            if (v[j].x != -1 && has_edge[i][j]) {
  //            debug(i, j, v[j], v[i], max_dist[4][3], max_dist[4][7], max_dist[3][7]);
              if (!E.c.IsSegmentInside(v[j], v[i])) {
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
            if (v[j].x != -1) {
              auto dist = (v[i] - v[j]).abs2();
              if (dist > max_dist[i][j] * max_dist[i][j] + 1e-9) {
                ok = false;
                break;
              }
            }
          }
          if (ok) {
            Dfs(id + 1);
          }
          v[i] = Point(-1, -1);
        }
      }
    };
    Dfs(0);
    if (best_score < (int) 1e9) {
      break;
    }
    debug("oops... fail, removing a point");
    mt19937 rng(58);
    int id = rng() % nv;
    while (v[id].x == -1) {
      id = rng() % nv;
    }
    v[id] = Point(-1, -1);
  }

  return 0;
}
