use std::io;
use std::io::{Write, Read};
use borys::*;

pub fn main() {
    let mut full_text = String::new();
    std::io::stdin().read_to_string(&mut full_text).unwrap();

    let input: Input = serde_json::from_str(&full_text).unwrap();

    println!("{}", input.hole.len());
    for v in input.hole.iter() {
        println!("{} {}", v[0], v[1]);
    }
    println!("{}", input.figure.edges.len());
    for e in input.figure.edges.iter() {
        println!("{} {}", e[0], e[1]);
    }
    println!("{}", input.figure.vertices.len());
    for v in input.figure.vertices.iter() {
        println!("{} {}", v[0], v[1]);
    }
    println!("{}", input.epsilon);
}
