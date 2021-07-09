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

struct Edge {
    int from, to, D;
};

int N, M, H, E;
vector<PointD> points;
vector<Point> srcPoints;
vector<Edge> edges;
vector<pii> edgePairs;
vector<PointD> hole;
vector<int> mt;
int test_id;
Evaluator ev;

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
    forn(i, N) {
        cin >> srcPoints[i].x >> srcPoints[i].y;
        points[i].x = srcPoints[i].x;
        points[i].y = srcPoints[i].y;
    }
    mt.assign(N, -1);

    forn(i, M) {
        edges[i].D = (points[edges[i].from] - points[edges[i].to]).abs2();
    }
    
    cin >> E;

    vector<Point> holeInt(H);
    forn(i, H) {
        holeInt[i].x = round(hole[i].x);
        holeInt[i].y = round(hole[i].y);
    }
    ev = Evaluator(holeInt, srcPoints, edgePairs, E);
    cerr << "read input " << test_id << endl;    
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