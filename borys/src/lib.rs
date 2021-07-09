use serde::{Deserialize, Serialize};
use serde_json::Result;
use std::fs::File;
use std::io::BufReader;

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
