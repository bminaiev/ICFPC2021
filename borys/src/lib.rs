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

#[derive(Serialize, Deserialize, Debug)]
pub struct OutputFormat {
    pub vertices: Vec<PointInput>
}

#[derive(Clone)]
pub struct Solution {
    pub dislikes: i64,
    pub vertices: Vec<Point>,
    pub edge_scores: Vec<f64>,
    pub crossed_edges: usize,
    pub bad_edges: usize,
}

impl Solution {
    pub fn cmp(&self, other: &Self) -> Ordering {
        if self.bad_edges != other.bad_edges {
            return self.bad_edges.cmp(&other.bad_edges);
        }
        if self.dislikes != other.dislikes {
            return self.dislikes.cmp(&other.dislikes);
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

    pub fn cmp_with_edges(&self, other: &Self) -> Ordering {
        if self.bad_edges != other.bad_edges {
            return self.bad_edges.cmp(&other.bad_edges);
        }
        if self.crossed_edges != other.crossed_edges {
            return self.crossed_edges.cmp(&other.crossed_edges);
        }
        return self.cmp(&other);
    }
}

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
        Self { vertices, dislikes, edge_scores, crossed_edges, bad_edges }
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
    return Task { hole, fig, edges, epsilon: t.epsilon, bonuses: t.bonuses.clone() };
}

fn get_old_score(test: usize) -> i64 {
    let path = format!("outputs/{}.score", test);
    if !Path::new(&path).exists() {
        return std::i64::MAX;
    }
    let f = File::open(path).unwrap();
    let mut s = String::new();
    BufReader::new(f).read_to_string(&mut s).unwrap();
    s.trim().parse().unwrap()
}

pub fn save_solution(solution: &Solution, test: usize, f_all: &mut File, task: &Task) {
    let old_score = get_old_score(test);
    if old_score <= solution.dislikes {
        println!("skip writing answer for test {}, as cur score {} is not better than prev {}", test, solution.dislikes, old_score);
        return;
    }
    let vertices = solution.vertices.iter().map(|p| [p.x, p.y]).collect();
    let out = OutputFormat { vertices };
    let mut f = File::create(format!("outputs/{}.ans", test)).unwrap();
    writeln!(f, "{}", serde_json::to_string(&out).unwrap()).unwrap();
    let mut f_score = File::create(format!("outputs/{}.score", test)).unwrap();
    writeln!(f_score, "{}", solution.dislikes).unwrap();
    writeln!(f_all, "{}: {}", test, solution.dislikes).unwrap();
    f_all.flush().unwrap();
    drawer::save_test(&task, &solution, &format!("outputs_pics/{}.png", test));
}

pub fn load_test(test_id: usize) -> Task {
    let file = File::open(format!("../inputs/{}.problem", test_id)).unwrap();
    let reader = BufReader::new(file);

    let input: Input = serde_json::from_reader(reader).unwrap();

    conv_input(&input)
}

pub fn load_best_solution(test_id : usize) -> Vec<Point> {
    let test = load_test(test_id);
    let helper = Helper::create(&test);
    let mut res =vec![];
    let mut res_dislikes = std::i64::MAX;
    {
        let path = format!("../download_outputs/{}.ans", test_id);
        if Path::new(&path).exists() {
            let vertices = load_submission(&path);
            let solution = Solution::create(vertices, &test, &helper);
            if solution.dislikes < res_dislikes {
                res_dislikes = solution.dislikes;
                res = solution.vertices.clone();
            }
        }
    }
    {
        let path = format!("../borys/outputs/{}.ans", test_id);
        if Path::new(&path).exists() {
            let vertices = load_submission(&path);
            let solution = Solution::create(vertices, &test, &helper);
            if solution.dislikes < res_dislikes {
                res_dislikes = solution.dislikes;
                res = solution.vertices.clone();
            }
        }
    }
    {
        let path = format!("../outputs_romka/{}.ans", test_id);
        if Path::new(&path).exists() {
            let vertices = cp_format_loader::load(&path);
            let solution = Solution::create(vertices, &test, &helper);
            if solution.dislikes < res_dislikes {
                res_dislikes = solution.dislikes;
                res = solution.vertices.clone();
            }
        }
    }
    assert!(res.len() > 0);
    return res;

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