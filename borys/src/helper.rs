use crate::*;
use std::cmp::max;
use std::mem::swap;

pub struct Helper {
    pub is_inside: Vec<Vec<bool>>,
    hole: Vec<Point>,
    hole_and_first: Vec<Point>,
    pub max_c: i32,
}


fn vec_mul(a: &Point, b: &Point, c: &Point) -> i32 {
    return ((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x)).signum();
}

fn scal_mul(a: &Point, b: &Point, c: &Point) -> i32 {
    return ((b.x - a.x) * (c.x - a.x) + (b.y - a.y) * (c.y - a.y)).signum();
}

fn on_seg(a: &Point, b: &Point, p: &Point) -> bool {
    if vec_mul(a, b, p) != 0 {
        return false;
    }
    return scal_mul(a, b, p) >= 0 && scal_mul(b, a, p) >= 0;
}

// [p1..p2] x [p3..p4]
fn seg_intersect_without_ends(p1: &Point, p2: &Point, p3: &Point, p4: &Point) -> bool {
    return vec_mul(p1, p2, p3) * vec_mul(p1, p2, p4) < 0
        && vec_mul(p3, p4, p1) * vec_mul(p3, p4, p2) < 0;
}

impl Helper {
    pub fn is_point_inside(&self, p: &Point) -> bool {
        if p.x < 0 || p.x * 2 >= self.is_inside.len() as i32 {
            return false;
        }
        if p.y < 0 || p.y * 2 >= self.is_inside.len() as i32 {
            return false;
        }
        return self.is_inside[p.x as usize * 2][p.y as usize * 2];
    }

    pub fn is_edge_inside(&self, p1: &Point, p2: &Point) -> bool {
        if !self.is_point_inside(p1) {
            return false;
        }
        if !self.is_point_inside(p2) {
            return false;
        }
        for e in self.hole_and_first.windows(2) {
            if seg_intersect_without_ends(&e[0], &e[1], &p1, &p2) {
                return false;
            }
        }
        let mut intersections = vec![p1, p2];
        for p in self.hole.iter() {
            if on_seg(p1, p2, p) {
                intersections.push(p);
            }
        }
        intersections.sort();
        for neigh in intersections.windows(2) {
            if !self.is_inside[(neigh[0].x + neigh[1].x) as usize][(neigh[0].y + neigh[1].y) as usize] {
                return false;
            }
        }
        return true;
    }

    pub fn create(t: &Task) -> Self {
        for p in t.hole.iter() {
            assert!(p.x >= 0);
            assert!(p.y >= 0);
        }
        let mut max_c = 0;
        for p in t.hole.iter() {
            max_c = max(max_c, p.x);
            max_c = max(max_c, p.y);
        }
        max_c += 1;
        let max_c = max_c as usize;
        let mut is_inside = vec![vec![false; max_c * 2]; max_c * 2];
        let mut hole_x2: Vec<_> = t.hole.iter().map(|p| Point { x: p.x * 2, y: p.y * 2 }).collect();
        hole_x2.push(hole_x2[0]);
        for x in 0..is_inside.len() {
            for y in 0..is_inside.len() {
                let p = Point { x: x as i32, y: y as i32 };
                let mut on_border = false;
                for edge in hole_x2.windows(2) {
                    if on_seg(&edge[0], &edge[1], &p) {
                        on_border = true;
                    }
                }
                if on_border {
                    is_inside[x][y] = true;
                } else {
                    let mut segs_to_up = 0;
                    for edge in hole_x2.windows(2) {
                        let mut p1 = edge[0];
                        let mut p2 = edge[1];
                        if p1.x > p2.x {
                            swap(&mut p1, &mut p2);
                        }
                        if p1.x <= p.x && p.x < p2.x {
                            if vec_mul(&p1, &p2, &p) < 0 {
                                segs_to_up += 1;
                            }
                        }
                    }
                    if segs_to_up % 2 == 1 {
                        is_inside[x][y] = true;
                    }
                }
            }
        }
        let mut hole_and_first = t.hole.clone();
        hole_and_first.push(hole_and_first[0]);
        Helper { is_inside, hole: t.hole.clone(), hole_and_first, max_c: max_c as i32 }
    }
}
