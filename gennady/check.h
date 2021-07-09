#ifndef _CHECK_H_
#define _CHECK_H_

#include "point.h"
#include <vector>
#include <algorithm>
#include <cassert>

class Checker {
  public:
    std::vector<Point> v;
    int n;
    int max_x;
    int max_y;
    std::vector<std::vector<bool>> inside;

    Checker(std::vector<Point> vv) {
      v = vv;
      n = (int) v.size();
      max_x = -1;
      max_y = -1;
      for (int i = 0; i < n; i++) {
        assert(v[i].x >= 0 && v[i].y >= 0);
        max_x = std::max(max_x, v[i].x);
        max_y = std::max(max_y, v[i].y);
      }
      assert(max_x <= 1000 && max_y <= 1000);
      inside.assign(2 * max_x + 1, std::vector<bool>(2 * max_y + 1));
      for (int x = 0; x <= 2 * max_x; x++) {
        for (int y = 0; y <= 2 * max_y; y++) {
          Point p(x, y);
          for (int i = 0; i < n; i++) {
            int j = (i + 1) % n;
            if (LiesOnSegment(v[i] * 2, v[j] * 2, p)) {
              inside[x][y] = true;
              break;
            }
          }
          if (!inside[x][y]) {
            int cnt = 0;
            for (int i = 0; i < n; i++) {
              int j = (i + 1) % n;
              Point p1 = v[i] * 2;
              Point p2 = v[j] * 2;
              if (p1.x > p2.x) {
                std::swap(p1, p2);
              }
              if (p1.x <= x && p2.x > x) {
                if (vmul(p - p1, p2 - p1) > 0) {
                  cnt += 1;
                }
              }
            }
            if (cnt % 2 == 1) {
              inside[x][y] = 1;
            }
          }
        }
      }
    }

    bool IsPointInside(Point p) {
      if (p.x < 0 || p.y < 0 || p.x > max_x || p.y > max_y) {
        return false;
      }
      return inside[p.x * 2][p.y * 2];
    }

    bool IsHalfPointInside(Point p) {
      if (p.x < 0 || p.y < 0 || p.x > 2 * max_x || p.y > 2 * max_y) {
        return false;
      }
      return inside[p.x][p.y];
    }

    bool IsSegmentInside(Point p, Point q) {
      if (!IsPointInside(p) || !IsPointInside(q)) {
        return false;
      }
      std::vector<Point> pts = {p, q};
      for (int i = 0; i < n; i++) {
        if (LiesOnSegment(p, q, v[i])) {
          pts.push_back(v[i]);
        }
      }
      std::sort(pts.begin(), pts.end(), [&](Point a, Point b) {
        return (a - p).abs2() < (b - p).abs2();
      });
      for (int i = 0; i < (int) pts.size() - 1; i++) {
        if (!IsHalfPointInside(pts[i] + pts[i + 1])) {
          return false;
        }
      }
      return true;
    }
};

#endif