use serde::{Deserialize, Serialize};
use std::cmp::{min, Ordering};
use std::fs::File;
use std::io::{Write, BufReader, Read};
use std::path::Path;
use crate::helper::Helper;

pub type PointInput = [i32; 2];

#[derive(Serialize, Deserialize, Debug)]
pub struct Figure {
    pub edges: Vec<Vec<usize>>,
    pub vertices: Vec<PointInput>,
}


#[derive(Serialize, Deserialize, Debug)]
pub struct Bonus {
    pub bonus: String,
    pub problem: usize,
    pub position: PointInput,
}

#[derive(Serialize, Deserialize, Debug)]
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
}

impl Solution {
    pub fn cmp(&self, other: &Self) -> Ordering {
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
        Self { vertices, dislikes, edge_scores }
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
}


fn conv_points(pts: &[PointInput]) -> Vec<Point> {
    pts.iter().map(|p| Point { x: p[0], y: p[1] }).collect()
}

pub fn conv_input(t: &Input) -> Task {
    let hole = conv_points(&t.hole);
    let fig = conv_points(&t.figure.vertices);
    let edges: Vec<_> = t.figure.edges.iter().map(|e| Edge { fr: e[0], to: e[1] }).collect();
    return Task { hole, fig, edges, epsilon: t.epsilon };
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

pub mod rand;
pub mod drawer;
pub mod helper;
pub mod local_optimizer;
pub mod solver;
pub mod vizualizer;