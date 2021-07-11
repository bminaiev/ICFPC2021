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
  vector<vector<int>> g(nv);
  for (auto& e : edges) {
    has_edge[e.first][e.second] = has_edge[e.second][e.first] = true;
    g[e.first].push_back(e.second);
    g[e.second].push_back(e.first);
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

  vector<vector<long long>> ol(nv, vector<long long>(nv, 1e9));
  for (int i = 0; i < nv; i++)
    for (int j = 0; j < nv; j++)
      ol[i][j] = (vertices[i] - vertices[j]).abs2();

  vector<Point> inner;
  for (int x = 0; x <= max_x; x++) {
    for (int y = 0; y <= max_y; y++) {
      if (E.c.IsPointInside(Point(x, y))) {
        inner.emplace_back(x, y);
      }
    }
  }  

  vector<Point> v(nv, Point(-1, -1));
  int best_score = (int) 1e9;
  auto best_v = v;
  bool found = false;

  vector<int> test(np, -1);
  ifstream tin("../hints/" + to_string(xid) + ".txt");
  if (!tin.is_open()) {
    cerr << "hint for " << xid << "doesn't exist (check relative path?)" << '\n';
    return 0;
  }
  for (int i = 0; i < np; i++) tin >> test[i];
  debug(test);

  vector<bool> taken(nv, false);
  int tkc = 0;
  for (int i = 0; i < np; i++)
    if (test[i] != -1) {
      taken[test[i]] = true;
      tkc++;
      v[test[i]] = poly[i];
    }

  vector<vector<Point>> oknp(nv);
  for (int i = 0; i < nv; i++) {
    if (taken[i]) continue;
    for (const auto& tp : inner) {
      bool ok = true;
      for (size_t jj = 0; jj < g[i].size(); jj++) {
        int j = g[i][jj];
        if (v[j].x == -1) continue;
        if (!E.c.IsSegmentInside(tp, v[j])) {
          ok = false;
          break;
        }
        int new_len = (tp - v[j]).abs2();
        int old_len = (vertices[i] - vertices[j]).abs2();
        long long num = abs(new_len - old_len);
        long long den = old_len;
        if (num * EPS_COEF > eps * den) {
          ok = false;
          break;
        }
      }
      for (int j = 0; j < nv && ok; j++) {
        if (!taken[j]) continue;
        auto dist = (tp - v[j]).abs2();
        if (dist > max_dist[i][j] * max_dist[i][j] + 1e-9) {
          ok = false;
          break;
        }
      }      
      if (ok) oknp[i].push_back(tp);
    }
  }

  vector<int> order(nv);
  mt19937 rng(60);
  iota(order.begin(), order.end(), 0);
  shuffle(order.begin(), order.end(), rng);
  sort(order.begin(), order.end(), [&](int i, int j) { return taken[i] > taken[j]; });
  debug(nv, np, eps);
  if (nv <= 10) {
    iota(order.begin(), order.end(), 0);
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
    for (auto iter = 0; iter < (int)1e5 * nv / (nv - tkc); iter++) {
      // shuffle(order.begin(), order.end(), rng);
      int qi = iter % order.size();
      if (taken[order[qi]]) continue;
      int qj = rand() % order.size();
      if (taken[order[qj]]) continue;
      if (qi == qj) continue;
      swap(order[qi], order[qj]);
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
      } else {
        swap(order[qi], order[qj]);
      }
    }
    debug(best_order);
    debug(best_seq);
    order = best_order;
  }

  const bool checksort = true;

  function<void(int)> Dfs = [&](int ii) {
    if (found) return;
    // cerr << ii << " of " << nv << endl;
    if (ii == nv) {
      int score = (int) E.eval(v);
      if (score != -1) {
        if (score < best_score) {
          best_score = score;
          best_v = v;
          debug(best_v);
          debug(score, best_score);
          ofstream out("../outputs_romka/" + to_string(xid) + ".ans");
          out << best_v.size() << '\n';
          for (auto& p : best_v) {
            out << p.x << " " << p.y << '\n';
          }
          out.close();
          exit(0);
        }
      }
      return;
    }
    int i = order[ii];
    if (v[i].x != -1) {
      Dfs(ii + 1);
      return;
    }
    for (size_t qq = 0; qq < oknp[i].size(); qq++) {
      const Point& pc = oknp[i][qq];
      v[i] = pc;
      bool ok = true;      
      for (size_t jj = 0; jj < g[i].size(); jj++) {
        int j = g[i][jj];
        if (v[j].x == -1) continue;
        if (!E.c.IsSegmentInside(v[i], v[j])) {
          ok = false;
          break;
        }
        int new_len = (v[i] - v[j]).abs2();
        int old_len = ol[i][j];
        long long num = abs(new_len - old_len);
        if (num * EPS_COEF > eps * old_len) {
          ok = false;
          break;
        }
      }

      for (int jj = 0; jj < nv && ok; jj++) {
        if (ii == jj || v[order[jj]].x == -1) {
          continue;
        }
        
        int j = order[jj];
        if (j < i && v[j].x > v[i].x && checksort) {
          ok = false;
          break;
        }
        auto dist = (v[i] - v[j]).abs2();
        if (dist > max_dist[i][j] * max_dist[i][j] + 1e-9) {
          ok = false;
          break;
        }
      }
      if (ok) {
        Dfs(ii + 1);
      }
      v[i] = Point(-1, -1);
    }
  };

  function<void(int, int)> DfsZero = [&](int ii, int left) {
    int i = order[ii];
    if (nv - ii < left) return;
    // cerr << ii << "/" << nv << ", " << left << endl;
    if (ii == nv) {
      debug(v);
      ofstream out("../outputs_romka/" + to_string(xid) + ".ans");
      out << v.size() << '\n';
      for (auto& p : v) {
        out << p.x << " " << p.y << '\n';
      }
      out.close();
      cerr << "saved to " << xid << ".ans\n";
      Dfs(0);
      return;
    }
    if (taken[i]) {
      DfsZero(ii + 1, left);
      return;
    }
    for (int qp = 0; qp < np; qp++) {
      if (test[qp] == -1) {
        test[qp] = i;
        v[i] = poly[qp];
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
          if (j < i && v[j].x > v[i].x && checksort) {
            ok = false;
            break;
          }
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
          DfsZero(ii + 1, left - 1);
        }
        v[i] = Point(-1, -1);
        test[qp] = -1;
      }
    }
    DfsZero(ii + 1, left);
  };
  
  DfsZero(0, np - tkc);
  cout << "done" << '\n';

/*  ofstream out("../outputs/" + to_string(xid) + ".ans");
  out << best_v.size() << '\n';
  for (auto& p : best_v) {
    out << p.x << " " << p.y << '\n';
  }
  out.close();*/
  return 0;
}
