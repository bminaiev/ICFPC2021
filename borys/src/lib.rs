use serde::{Deserialize, Serialize};
use std::cmp::min;

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

#[derive(Debug)]
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

pub mod rand;
pub mod drawer;
pub mod helper;