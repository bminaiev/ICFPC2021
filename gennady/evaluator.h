#ifndef _EVAL_H_
#define _EVAL_H_

#include "checker.h"
#include <algorithm>
#include <cassert>

class Evaluator {
  public:
    const int EPS_COEF = 1000000;

    Checker c;
    std::vector<Point> p;
    std::vector<Point> v;
    std::vector<std::pair<int, int>> e;
    int eps;

    std::string error_msg;

    Evaluator(std::vector<Point> poly, std::vector<Point> vertices, std::vector<std::pair<int, int>> edges, int epsilon) {
      c = Checker(poly);
      p = poly;
      v = vertices;
      e = edges;
      eps = epsilon;
      error_msg = "no errors found!";
    }

    long long eval(std::vector<Point> pts) {
      assert(pts.size() == v.size());
      for (int i = 0; i < (int) pts.size(); i++) {
        if (!c.IsPointInside(pts[i])) {
          error_msg = "point " + std::to_string(i) + " is not inside the polygon";
          return -1;
        }
      }
      for (auto& edge : e) {
        int i = edge.first;
        int j = edge.second;
        if (!c.IsSegmentInside(pts[i], pts[j])) {
          error_msg = "segment " + std::to_string(i) + "-" + std::to_string(j) + " is not inside the polygon";
          return -1;
        }
      }
      for (auto& edge : e) {
        int i = edge.first;
        int j = edge.second;
        int old_len = (v[i] - v[j]).abs2();
        int new_len = (pts[i] - pts[j]).abs2();
        long long num = abs(new_len - old_len);
        long long den = old_len;
        if (num * EPS_COEF > eps * den) {
          error_msg = "segment " + std::to_string(i) + "-" + std::to_string(j) + " length differs too much (old = "
             + std::to_string(old_len) + ", new = " + std::to_string(new_len) + ", diff = " + std::to_string((double) num / (double) den)
             + ", eps = " + std::to_string(eps) + ")";
          return -1;
        }
      }
      long long score = 0;
      for (auto& h : p) {
        int cur = (int) 1e9;
        for (auto& q : pts) {
          cur = std::min(cur, (q - h).abs2());
        }
        score += cur;
      }
      return score;
    }
};

#endif
