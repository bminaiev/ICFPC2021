use serde::{Deserialize, Serialize};
use std::cmp::{min, Ordering};
use std::fs::File;
use std::io::{Write, BufReader, Read};
use std::path::Path;
use crate::helper::Helper;

pub type PointInput = [i32; 2];

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct Figure {
    pub edges: Vec<Vec<usize>>,
    pub vertices: Vec<PointInput>,
}


#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct Bonus {
    pub bonus: String,
    pub problem: usize,
    pub position: PointInput,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct Input {
    pub hole: Vec<PointInput>,
    pub figure: Figure,
    pub epsilon: i64,
    pub bonuses: Vec<Bonus>,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct UsedBonus {
    pub bonus: String,
    pub problem: usize,
}


fn empty_vec() -> Vec<UsedBonus> {
    vec![]
}

#[derive(Serialize, Deserialize, Debug)]
pub struct OutputFormat {
    pub vertices: Vec<PointInput>,
    #[serde(default = "empty_vec")]
    pub bonuses: Vec<UsedBonus>,
}

#[derive(Clone)]
pub struct Solution {
    pub dislikes: i64,
    pub vertices: Vec<Point>,
    pub edge_scores: Vec<f64>,
    pub crossed_edges: usize,
    pub bad_edges: usize,
    pub got_bonuses: usize,
    pub used_bonuses: Vec<UsedBonus>,
    pub sum_diffs: f64,
}

impl Solution {
    pub fn cmp(&self, other: &Self) -> Ordering {
        if self.got_bonuses != other.got_bonuses {
            return self.got_bonuses.cmp(&other.got_bonuses).reverse();
        }
        if self.bad_edges != other.bad_edges {
            return self.bad_edges.cmp(&other.bad_edges);
        }
        if self.dislikes != other.dislikes {
            return self.dislikes.cmp(&other.dislikes);
        }
        if self.sum_diffs.partial_cmp(&other.sum_diffs).unwrap() != Ordering::Equal {
            return self.sum_diffs.partial_cmp(&other.sum_diffs).unwrap();
        }
        for (my, other) in self.edge_scores.iter().zip(other.edge_scores.iter()) {
            match my.partial_cmp(&other).unwrap() {
                Ordering::Less => return Ordering::Less,
                Ordering::Greater => return Ordering::Greater,
                Ordering::Equal => {}
            }
        }
        return Ordering::Equal;
    }

    pub fn cmp_with_eps(&self, other: &Self) -> Ordering {
        if self.got_bonuses != other.got_bonuses {
            return self.got_bonuses.cmp(&other.got_bonuses).reverse();
        }
        if self.dislikes != other.dislikes {
            return self.dislikes.cmp(&other.dislikes);
        }
        if self.sum_diffs.partial_cmp(&other.sum_diffs).unwrap() != Ordering::Equal {
            return self.sum_diffs.partial_cmp(&other.sum_diffs).unwrap();
        }
        return Ordering::Equal;
    }

    pub fn cmp_with_edges(&self, other: &Self) -> Ordering {
        if self.got_bonuses != other.got_bonuses {
            return self.got_bonuses.cmp(&other.got_bonuses).reverse();
        }
        if self.bad_edges != other.bad_edges {
            return self.bad_edges.cmp(&other.bad_edges);
        }
        if self.crossed_edges != other.crossed_edges {
            return self.crossed_edges.cmp(&other.crossed_edges);
        }
        return self.cmp(&other);
    }
}

struct WantBonus {
    problem_id: usize,
    bonus: &'static str,
}

const WANT_BONUSES: [WantBonus; 3] = [
    WantBonus { problem_id: 81, bonus: "GLOBALIST" },
    WantBonus { problem_id: 7, bonus: "GLOBALIST" },
    WantBonus { problem_id: 60, bonus: "GLOBALIST" }];

impl Solution {
    pub fn create(vertices: Vec<Point>, t: &Task, h: &Helper) -> Self {
        let mut dislikes = 0;
        for hole in t.hole.iter() {
            let mut min_dist = std::i64::MAX;
            for p in vertices.iter() {
                min_dist = min(min_dist, p.d2(&hole));
            }
            dislikes += min_dist;
        }
        let mut edge_scores: Vec<_> = t.edges.iter().map(|e|
            h.edge_score(t, e.fr, e.to, &vertices[e.fr], &vertices[e.to])).collect();
        edge_scores.sort_by(|a, b| a.partial_cmp(&b).unwrap().reverse());
        let mut crossed_edges = 0;
        for edge1 in t.edges.iter() {
            for edge2 in t.edges.iter() {
                let p1 = vertices[edge1.fr];
                let p2 = vertices[edge1.to];
                let p3 = vertices[edge2.fr];
                let p4 = vertices[edge2.to];
                if helper::seg_intersect_without_ends(&p1, &p2, &p3, &p4) {
                    crossed_edges += 1;
                }
            }
        }
        let mut bad_edges = 0;
        for edge in t.edges.iter() {
            if !h.is_valid_edge(t, edge.fr, edge.to, &vertices[edge.fr], &vertices[edge.to]) {
                bad_edges += 1;
            }
        }
        let mut got_bonuses = 0;
        for bonus in t.bonuses.iter() {
            let mut want_it = false;
            for want in WANT_BONUSES.iter() {
                if want.bonus == bonus.bonus && want.problem_id == bonus.problem {
                    want_it = true;
                }
            }
            let mut has_point = false;
            for p in vertices.iter() {
                if p.x == bonus.position[0] && p.y == bonus.position[1] {
                    has_point = true;
                }
            }
            if want_it && has_point {
                got_bonuses += 1;
            }
        }
        let mut sum_diffs = 0.0;
        for edge in t.edges.iter() {
            let my_d2 = vertices[edge.fr].d2(&vertices[edge.to]) as f64;
            let expected_d2 = t.fig[edge.fr].d2(&t.fig[edge.to]) as f64;
            let diff = (my_d2 / expected_d2 - 1.0).abs();
            sum_diffs += diff;
        }
        Self { vertices, dislikes, edge_scores, crossed_edges, bad_edges, got_bonuses, used_bonuses: vec![], sum_diffs }
    }

    pub fn move_one_point(self, v: usize, p: Point, t: &Task, h: &Helper) -> Self {
        let mut vertices = self.vertices;
        vertices[v] = p;
        Self::create(vertices, t, h)
    }
}


#[derive(Copy, Clone, Ord, PartialOrd, Eq, PartialEq, Debug)]
pub struct Point {
    pub x: i32,
    pub y: i32,
}

impl Point {
    pub fn d2(&self, another: &Point) -> i64 {
        let dx = (self.x - another.x) as i64;
        let dy = (self.y - another.y) as i64;
        return dx * dx + dy * dy;
    }

    pub fn ok(&self) -> bool {
        return self.x >= 0 && self.y >= 0;
    }

    pub fn shift(self, shift: &Shift) -> Self {
        Self { x: self.x + shift.dx, y: self.y + shift.dy }
    }
}

#[derive(Copy, Clone)]
pub struct Shift {
    pub dx: i32,
    pub dy: i32,
}


#[derive(Debug, Clone, Copy, Eq, PartialEq)]
pub struct Edge {
    pub fr: usize,
    pub to: usize,
}

#[derive(Debug)]
pub struct Task {
    pub hole: Vec<Point>,
    pub fig: Vec<Point>,
    pub edges: Vec<Edge>,
    pub epsilon: i64,
    pub bonuses: Vec<Bonus>,
}


fn conv_points(pts: &[PointInput]) -> Vec<Point> {
    pts.iter().map(|p| Point { x: p[0], y: p[1] }).collect()
}

pub fn conv_input(t: &Input) -> Task {
    let hole = conv_points(&t.hole);
    let fig = conv_points(&t.figure.vertices);
    let edges: Vec<_> = t.figure.edges.iter().map(|e| Edge { fr: e[0], to: e[1] }).collect();
    let mut bonuses = vec![];
    for bonus in t.bonuses.iter() {
        for want in WANT_BONUSES.iter() {
            if want.bonus == bonus.bonus && want.problem_id == bonus.problem {
                bonuses.push(bonus.clone())
            }
        }
    }
    return Task { hole, fig, edges, epsilon: t.epsilon, bonuses };
}

fn get_old_solution(test_id: usize, t: &Task) -> Option<Solution> {
    let path = format!("../borys/outputs/{}.ans", test_id);
    if Path::new(&path).exists() {
        let sol = load_submission(&path);
        let h = Helper::create(t);
        return Some(Solution::create(sol, t, &h));
    } else {
        return None;
    }
}

pub fn force_save_solution(solution: &Solution, test: usize, f_all: &mut File, task: &Task) {
    let vertices = solution.vertices.iter().map(|p| [p.x, p.y]).collect();
    let out = OutputFormat { vertices, bonuses: solution.used_bonuses.clone() };
    let mut f = File::create(format!("outputs/{}.ans", test)).unwrap();
    writeln!(f, "{}", serde_json::to_string(&out).unwrap()).unwrap();
    let mut f_score = File::create(format!("outputs/{}.score", test)).unwrap();
    writeln!(f_score, "{}", solution.dislikes).unwrap();
    writeln!(f_all, "{}: {}", test, solution.dislikes).unwrap();
    f_all.flush().unwrap();
    drawer::save_test(&task, &solution, &format!("outputs_pics/{}.png", test));
}

pub fn save_solution(solution: &Solution, test: usize, f_all: &mut File, task: &Task) {
    match get_old_solution(test, task) {
        None => {}
        Some(old_sol) => {
            if old_sol.cmp(&solution) != Ordering::Greater {
                println!("skip writing answer for test {}, as cur score {} is not better than prev {}", test, solution.dislikes, old_sol.dislikes);
                return;
            }
        }
    }
    force_save_solution(solution, test, f_all, task);
}

pub fn load_test(test_id: usize) -> Task {
    let file = File::open(format!("../inputs/{}.problem", test_id)).unwrap();
    let reader = BufReader::new(file);

    let input: Input = serde_json::from_reader(reader).unwrap();

    conv_input(&input)
}

const USE_ONLY_MY: bool = false;

pub fn load_best_solution(test_id: usize) -> Vec<Point> {
    let test = load_test(test_id);
    let helper = Helper::create(&test);
    let mut res: Option<Solution> = None;
    if !USE_ONLY_MY
    {
        let path = format!("../download_outputs/{}.ans", test_id);
        if Path::new(&path).exists() {
            let vertices = load_submission(&path);
            let solution = Solution::create(vertices, &test, &helper);
            let need_change = match res {
                None => { true }
                Some(ref res) => {
                    res.cmp(&solution) == Ordering::Greater
                }
            };
            if need_change {
                res = Some(solution);
            }
        }
    }
    {
        let path = format!("../borys/outputs/{}.ans", test_id);
        if Path::new(&path).exists() {
            let vertices = load_submission(&path);
            let solution = Solution::create(vertices, &test, &helper);
            let need_change = match res {
                None => { true }
                Some(ref res) => {
                    res.cmp(&solution) == Ordering::Greater
                }
            };
            if need_change {
                res = Some(solution);
            }
        }
    }
    if !USE_ONLY_MY
    {
        let path = format!("../outputs_romka/{}.ans", test_id);
        if Path::new(&path).exists() {
            let vertices = cp_format_loader::load(&path);
            let solution = Solution::create(vertices, &test, &helper);
            let need_change = match res {
                None => { true }
                Some(ref res) => {
                    res.cmp(&solution) == Ordering::Greater
                }
            };
            if need_change {
                res = Some(solution);
            }
        }
    }
    assert!(res.is_some());
    return res.unwrap().vertices;
}

pub fn load_submission(path: &str) -> Vec<Point> {
    let file = File::open(path).unwrap();
    let reader = BufReader::new(file);

    let output: OutputFormat = serde_json::from_reader(reader).unwrap();
    output.vertices.iter().map(|o| Point { x: o[0], y: o[1] }).collect()
}

pub mod rand;
pub mod drawer;
pub mod helper;
pub mod local_optimizer;
pub mod solver;
pub mod vizualizer;
pub mod cp_format_loader;
pub mod rec_optimizer;
pub mod rec_optimizer2;
pub mod eps_optimizer;