use serde::{Deserialize, Serialize};
use std::cmp::min;
use std::fs::File;
use std::io::{Write};


pub type PointInput = [i32; 2];

#[derive(Serialize, Deserialize, Debug)]
pub struct Figure {
    pub edges: Vec<Vec<usize>>,
    pub vertices: Vec<PointInput>,
}

#[derive(Serialize, Deserialize, Debug)]
pub struct Input {
    pub hole: Vec<PointInput>,
    pub figure: Figure,
    pub epsilon: i64,
}

#[derive(Serialize, Deserialize, Debug)]
pub struct OutputFormat {
    pub vertices: Vec<PointInput>
}

#[derive(Clone)]
pub struct Solution {
    pub dislikes: i64,
    pub vertices: Vec<Point>,
}

impl Solution {
    pub fn create(vertices: Vec<Point>, t: &Task) -> Self {
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
}

#[derive(Debug, Clone, Copy)]
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

pub fn save_solution(solution: &Solution, test: usize, f_all: &mut File, task: &Task) {
    let vertices = solution.vertices.iter().map(|p| [p.x, p.y]).collect();
    let out = OutputFormat { vertices };
    let mut f = File::create(format!("outputs/{}.ans", test)).unwrap();
    writeln!(f, "{}", serde_json::to_string(&out).unwrap()).unwrap();
    let mut f_score = File::create(format!("outputs/{}.score", test)).unwrap();
    writeln!(f_score, "{}", solution.dislikes).unwrap();
    writeln!(f_all, "{}: {}", test, solution.dislikes).unwrap();
    f_all.flush();
    drawer::save_test(&task, &solution, &format!("outputs_pics/{}.png", test));
}

pub mod rand;
pub mod drawer;
pub mod helper;
pub mod local_optimizer;