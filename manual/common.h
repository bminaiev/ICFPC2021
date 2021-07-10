#pragma once
#pragma GCC optimize("-O3")
#pragma GCC optimize("inline")
#pragma GCC optimize("omit-frame-pointer")
#pragma GCC optimize("unroll-loops")
#include <algorithm>
#include <vector>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
#include <array>
#include <random>
#include <thread>
#include <set>
#include <cassert>
#include <tuple>
#include <cmath>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <memory.h>
#include <cstdlib>

#include "point.h"
#include "evaluator.h"

using namespace std;

#define forn(i, n) for (int i = 0; i < (int)(n); i++)
typedef long long ll;
typedef unsigned long long ull;
typedef pair<int, int> pii;

const int dx[] = {-1, 0, 1, 0, 0};
const int dy[] = {0, -1, 0, 1, 0};

#define sqr(x) (x) * (x)
// #define contains(v, item) std::find(v.begin(), v.end(), item) != v.end()

const int inf = 1e9;


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

struct Edge {
    int from, to, D;
};

int N, M, H, E;
vector<PointD> points;
vector<Point> srcPoints;
vector<Edge> edges;
vector<pii> edgePairs;
vector<PointD> hole;
vector<Point> poly;
vector<int> mt;
int test_id;
Evaluator ev;
vector<vector<int>> g;
Point bonus;
string bonusname;

void readInput() {
    const string fname = "../inputs_conv/" + to_string(test_id) + ".problem";
    freopen(fname.c_str(), "r", stdin);
    cin >> H;
    hole.resize(H + 1);
    forn(i, H) cin >> hole[i].x >> hole[i].y;
    hole[H] = hole[0];
    
    cin >> M;
    edges.resize(M);
    edgePairs.resize(M);
    forn(i, M) {
        cin >> edges[i].from >> edges[i].to;        
        edgePairs[i] = {edges[i].from, edges[i].to};
    }
    
    cin >> N;
    points.resize(N);
    srcPoints.resize(N);
    g.clear();
    g.resize(N);
    forn(i, N) {
        cin >> srcPoints[i].x >> srcPoints[i].y;
        points[i].x = srcPoints[i].x;
        points[i].y = srcPoints[i].y;
    }
    mt.assign(N, -1);

    forn(i, M) {
        edges[i].D = (points[edges[i].from] - points[edges[i].to]).abs2();
        g[edges[i].from].push_back(edges[i].to);
        g[edges[i].to].push_back(edges[i].from);
    }
    
    cin >> E;

    int B, bp;
    cin >> B;
    assert(B == 1);
    cin >> bonusname >> bp >> bonus.x >> bonus.y;    

    poly.resize(H);
    forn(i, H) {
        poly[i].x = round(hole[i].x);
        poly[i].y = round(hole[i].y);
    }
    ev = Evaluator(poly, srcPoints, edgePairs, E);
    cerr << "read input " << test_id << ", contains bonus " << bonusname << " for problem " << bp << endl;
}

void loadSolution() {
    try {
        const string fname = "../outputs_romka/" + to_string(test_id) + ".ans";
        ifstream fin(fname);
        int nn;
        if (!(fin >> nn)) return;
        assert(nn == N);
        for (auto& p : points)
            fin >> p.x >> p.y;
        fin.close();

        cerr << "loaded from " << fname << endl;   
    } catch (const std::exception& e) {
        cerr << e.what() << endl;
    }
}

void saveSolution() {
    const string fname = "../outputs_romka/" + to_string(test_id) + ".ans";
    ofstream fout(fname);
    fout << N << endl;
    for (const auto& p : points)
        fout << round(p.x) << " " << round(p.y) << endl;
    fout.close();

    cerr << "saved to " << fname << endl;   
}

void saveHint() {
    const string fname = "../hints/" + to_string(test_id) + ".txt";
    ofstream fout(fname);
    vector<int> tp(H, -1);
    forn(i, N) if (mt[i] != -1) tp[mt[i]] = i;
    for (int x : tp) fout << x << " ";
    fout.close();

    cerr << "hint saved to " << fname << endl;   
}