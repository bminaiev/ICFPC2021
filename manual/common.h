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
#include <array>
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


struct Point {
    int x, y;
};

int dist2(const Point& a, const Point& b) {
    return sqr(a.x - b.x) + sqr(a.y - b.y);
}

struct Edge {
    int from, to, D;
};

int N, M, H, E;
vector<Point> points;
vector<Edge> edges;
vector<Point> hole;
string test_id;

void readInput() {
    const string fname = "../inputs_conv/" + test_id + ".problem";
    freopen(fname.c_str(), "r", stdin);
    cin >> H;
    hole.resize(H + 1);
    forn(i, H) cin >> hole[i].x >> hole[i].y;
    hole[H] = hole[0];
    
    cin >> M;
    edges.resize(M);
    forn(i, M) {
        cin >> edges[i].from >> edges[i].to;        
    }
    
    cin >> N;
    points.resize(N);
    forn(i, N) cin >> points[i].x >> points[i].y;

    forn(i, M) {
        edges[i].D = dist2(points[edges[i].from], points[edges[i].to]);
    }
    
    cin >> E;    
}

void printSolution() {
    const string fname = "../outputs/" + test_id + ".ans";
    freopen(fname.c_str(), "w", stdout);
    cout << N << endl;
    for (const auto& p : points)
        cout << p.x << " " << p.y << endl;

    cerr << "saved to " << fname << endl;   
}