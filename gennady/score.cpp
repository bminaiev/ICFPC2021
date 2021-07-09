/**
 *    author:  tourist
 *    created: 09.07.2021 17:21:07       
**/
#include <bits/stdc++.h>
#include "evaluator.h"

using namespace std;

int main(int argc, char** argv) {
  ios::sync_with_stdio(false);
  cin.tie(0);
  if (argc != 2) {
    cerr << "usage: score [test-id]" << '\n';
    return 0;
  }
  int x = atoi(argv[1]);
  if (to_string(x) != argv[1]) {
    cerr << "test-id must be an integer" << '\n';
    return 0;
  }

  ifstream in("../inputs_conv/" + to_string(x) + ".problem");
  if (!in.is_open()) {
    cerr << "input " << x << ".problem doesn't exist (check relative path?)" << '\n';
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

  ifstream out("../outputs/" + to_string(x) + ".ans");
  if (!out.is_open()) {
    cerr << "output " << x << ".ans doesn't exist (check relative path?)" << '\n';
    return 0;
  }
  int new_nv;
  out >> new_nv;
  if (nv != new_nv) {
    cerr << "wrong number of points: expected " << nv << ", found " << new_nv << '\n';
    return 0;
  }
  vector<Point> res(nv);
  for (int i = 0; i < nv; i++) {
    out >> res[i].x >> res[i].y;
    res[i].id = i;
  }
  out.close();

  Evaluator E(poly, vertices, edges, eps);
  cout << E.eval(res) << '\n';
  cout << E.error_msg << '\n';
  return 0;
}
