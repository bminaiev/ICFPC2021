use crate::helper::Helper;
use crate::{Solution, Task, Shift, Point, Edge};
use crate::rand::Random;
use std::collections::{BTreeSet, BTreeMap};

struct State<'a> {
    vertices: Vec<(usize, &'a Vec<Shift>)>,
    best_positions: Vec<Point>,
    min_dislikes: i64,
    edges_to_check: Vec<Edge>,
    positions: Vec<Point>,
    task: &'a Task,
    helper: &'a Helper,
    base_p: Point,
    used_v: BTreeMap<usize, usize>,
}

fn rec(pos: usize, s: &mut State) {
    if pos == s.vertices.len() {
        let solution = Solution::create(s.positions.clone(), &s.task, &s.helper);
        if solution.dislikes < s.min_dislikes {
            s.min_dislikes = solution.dislikes;
            s.best_positions = solution.vertices;
        }
    } else {
        let v = s.vertices[pos].0;
        for shift in s.vertices[pos].1.iter() {
            let np = s.base_p.shift(shift);
            if !s.helper.is_point_inside(&np) {
                continue;
            }
            let mut ok = true;
            for edge in s.edges_to_check.iter() {
                if edge.fr != v && edge.to != v {
                    continue;
                }
                let another = edge.fr + edge.to - v;
                match s.used_v.get(&another) {
                    None => {
                        if !s.helper.is_valid_edge(s.task, v, another, &np, &s.positions[another]) {
                            ok = false;
                            break;
                        }
                    }
                    Some(id) => {
                        if *id < pos {
                            if !s.helper.is_valid_edge(s.task, v, another, &np, &s.positions[another]) {
                                ok = false;
                                break;
                            }
                        }
                    }
                }
            }
            if ok {
                s.positions[v] = np;
                rec(pos + 1, s);
            }
        }
    }
}


pub fn optimize_one(task: &Task, helper: &Helper, solution: Solution, v: usize, max_d: i32, max_vertices: usize) -> Solution {
    let mut b_sol = solution.clone();
    let mut first_shifts = vec![];
    for dx in -max_d..=max_d {
        for dy in -max_d..=max_d {
            first_shifts.push(Shift { dx, dy });
        }
    }
    println!("Optimizing... {}", v);
    let mut vertices_find = vec![(v, &first_shifts)];
    for (e_id, edge) in task.edges.iter().enumerate() {
        if edge.fr != v && edge.to != v {
            continue;
        }
        let another = edge.fr + edge.to - v;
        vertices_find.push((another, &helper.shifts_per_edge[e_id]));
    }
    while vertices_find.len() > max_vertices {
        vertices_find.pop();
    }
    let used_v: BTreeMap<usize, usize> = vertices_find.iter().enumerate().map(|(id, v)| (v.0, id)).collect();
    let base_p = solution.vertices[v];
    let mut edges_to_check = vec![];
    for edge in task.edges.iter() {
        if used_v.contains_key(&edge.fr) || used_v.contains_key(&edge.to) {
            edges_to_check.push(edge.clone());
        }
    }
    let mut state = State {
        vertices: vertices_find,
        best_positions: b_sol.vertices.clone(),
        min_dislikes: b_sol.dislikes,
        edges_to_check,
        positions: b_sol.vertices.clone(),
        task,
        helper,
        base_p,
        used_v,
    };
    rec(0, &mut state);
    if state.min_dislikes < b_sol.dislikes {
        println!("Optimized: {} -> {}", b_sol.dislikes, state.min_dislikes);
        b_sol = Solution::create(state.best_positions, task, helper);
    }
    b_sol
}

pub fn optimize(task: &Task, helper: &Helper, mut solution: Solution) -> Solution {
    const MAX_D: i32 = 5;
    const MAX_VERTICES: usize = 3;
    for v in 0..task.fig.len() {
        solution = optimize_one(task, helper, solution, v, MAX_D, MAX_VERTICES);
    }
    solution
}