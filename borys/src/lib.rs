use serde::{Deserialize, Serialize};

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

pub mod rand;