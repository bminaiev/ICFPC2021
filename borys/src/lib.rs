use serde::{Deserialize, Serialize};

type Point = [i32; 2];

#[derive(Serialize, Deserialize, Debug)]
pub struct Figure {
    pub edges: Vec<Vec<usize>>,
    pub vertices: Vec<Point>,
}

#[derive(Serialize, Deserialize, Debug)]
pub struct Input {
    pub hole: Vec<Point>,
    pub figure: Figure,
    pub epsilon: i64,
}

#[derive(Serialize, Deserialize, Debug)]
pub struct OutputFormat {
    pub vertices: Vec<Point>
}