use std::fs::File;
use std::io::BufReader;
use borys::{Input, PointInput, OutputFormat};
use borys::rand::Random;
use std::cmp::{max, min};
use std::mem::swap;
use std::io::{Write};

#[derive(Copy, Clone, Ord, PartialOrd, Eq, PartialEq, Debug)]
struct Point {
    x: i32,
    y: i32,
}

impl Point {
    fn d2(&self, another: &Point) -> i64 {
        let dx = (self.x - another.x) as i64;
        let dy = (self.y - another.y) as i64;
        return dx * dx + dy * dy;
    }
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

#[derive(Debug)]
struct Edge {
    fr: usize,
    to: usize,
}

fn conv_points(pts: &[PointInput]) -> Vec<Point> {
    pts.iter().map(|p| Point { x: p[0], y: p[1] }).collect()
}


#[derive(Debug)]
struct Task {
    hole: Vec<Point>,
    fig: Vec<Point>,
    edges: Vec<Edge>,
    epsilon: i64,
}

struct Helper {
    is_inside: Vec<Vec<bool>>,
    hole: Vec<Point>,
    hole_and_first: Vec<Point>,
    max_c: i32,
}

impl Helper {
    fn is_point_inside(&self, p: &Point) -> bool {
        if p.x < 0 || p.x * 2 >= self.is_inside.len() as i32 {
            return false;
        }
        if p.y < 0 || p.y * 2 >= self.is_inside.len() as i32 {
            return false;
        }
        return self.is_inside[p.x as usize * 2][p.y as usize * 2];
    }

    fn is_edge_inside(&self, p1: &Point, p2: &Point) -> bool {
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
}

#[derive(Clone)]
struct Solution {
    dislikes: i64,
    vertices: Vec<Point>,
}

impl Solution {
    fn create(vertices: Vec<Point>, t: &Task) -> Self {
        let mut dislikes = 0;
        for hole in t.hole.iter() {
            let mut min_dist = std::i64::MAX;
            for p in vertices.iter() {
                min_dist = min(min_dist, p.d2(&hole));
            }
            dislikes += min_dist;
        }
        Self { vertices, dislikes }
    }
}

#[derive(Ord, PartialOrd, Eq, PartialEq)]
struct NeedToPut {
    edges: usize,
    v: usize,
}

fn solve_rec(t: &Task, helper: &Helper, cur_positions: &mut Vec<Option<Point>>, rnd: &mut Random) -> Option<Solution> {
    let mut need_to_put = vec![];
    for i in 0..cur_positions.len() {
        if cur_positions[i].is_none() {
            let mut edges = 0;
            for e in t.edges.iter() {
                if e.fr == i && cur_positions[e.to].is_some() || e.to == i && cur_positions[e.fr].is_some() {
                    edges += 1;
                }
            }
            need_to_put.push(NeedToPut { edges, v: i });
        }
    }
    if need_to_put.is_empty() {
        let vertices: Vec<_> = cur_positions.iter().map(|x| x.unwrap()).collect();
        return Some(Solution::create(vertices, t));
    }
    need_to_put.sort();
    let v_to_put = need_to_put.last().unwrap().v;
    let mut possible_positions = vec![];
    let edges: Vec<_> = t.edges.iter().filter(|e| e.fr == v_to_put && cur_positions[e.to].is_some() || e.to == v_to_put && cur_positions[e.fr].is_some()).collect();
    for x in 0..helper.max_c {
        for y in 0..helper.max_c {
            let p = Point { x, y };
            if !helper.is_point_inside(&p) {
                continue;
            }
            let mut ok = true;
            for edge in edges.iter() {
                let another = edge.to + edge.fr - v_to_put;
                let another_p = cur_positions[another].unwrap();
                if !helper.is_edge_inside(&p, &another_p) {
                    ok = false;
                    break;
                }
                let init_d2 = t.fig[v_to_put].d2(&t.fig[another]);
                let cur_d2 = p.d2(&another_p);
                let delta = (init_d2 - cur_d2).abs();
                // delta / init_d2 <= eps / 10^6
                // delta * 10^6 <= eps * init_d2
                if delta * 1_000_000 > t.epsilon * init_d2 {
                    ok = false;
                    break;
                }
            }
            if !ok {
                continue;
            }
            possible_positions.push(p);
        }
    }
    if possible_positions.is_empty() {
        return None;
    }
    let p = possible_positions[rnd.next_in_range(0, possible_positions.len())];
    cur_positions[v_to_put] = Some(p);
    return solve_rec(t, helper, cur_positions, rnd);
}

const MAX_ITERS: usize = 10_000;

fn solve_with_helper(t: &Task, helper: &Helper) -> Option<Solution> {
    for x in 0..helper.is_inside.len() {
        for y in 0..helper.is_inside.len() {
            if helper.is_inside[x][y] {
                print!("x");
            } else {
                print!(".");
            }
        }
        println!();
    }

    let mut rnd = Random::new(787788);
    for it in 0..MAX_ITERS {
        let solution = solve_rec(t, helper, &mut vec![None; t.fig.len()], &mut rnd);
        if solution.is_some() {
            println!("wow!!! score = {}", solution.clone().unwrap().dislikes);
            return solution;
        } else {
            if it % 100 == 0 {
                println!("bad.. {}", it);
            }
        }
    }
    return None;
}

fn solve(t: &Task) -> Option<Solution> {
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
    let helper = Helper { is_inside, hole: t.hole.clone(), hole_and_first, max_c: max_c as i32 };
    return solve_with_helper(t, &helper);
}

fn solve_input(t: &Input) -> Option<Solution> {
    let hole = conv_points(&t.hole);
    let fig = conv_points(&t.figure.vertices);
    let edges: Vec<_> = t.figure.edges.iter().map(|e| Edge { fr: e[0], to: e[1] }).collect();
    let task = Task { hole, fig, edges, epsilon: t.epsilon };
    return solve(&task);
}

fn main() {
    // const TASK: usize = 14;
    let mut f_all = File::create("outputs/all_scores.txt").unwrap();
    for problem_id in 1..=59 {
        println!("Start test {}", problem_id);
        let file = File::open(format!("../inputs/{}.problem", problem_id)).unwrap();
        let reader = BufReader::new(file);

        let input: Input = serde_json::from_reader(reader).unwrap();

        let res = solve_input(&input);
        match res {
            None => {
                writeln!(f_all, "{}: no solution", problem_id).unwrap();
            }
            Some(solution) => {
                let vertices = solution.vertices.iter().map(|p| [p.x, p.y]).collect();
                let out = OutputFormat { vertices };
                let mut f = File::create(format!("outputs/{}.ans", problem_id)).unwrap();
                writeln!(f, "{}", serde_json::to_string(&out).unwrap()).unwrap();
                let mut f_score = File::create(format!("outputs/{}.score", problem_id)).unwrap();
                writeln!(f_score, "{}", solution.dislikes).unwrap();
                writeln!(f_all, "{}: {}", problem_id, solution.dislikes).unwrap();
            }
        }
        f_all.flush().unwrap();
        // dbg!(input);
    }
}
