use crate::*;
use crate::rand::*;

#[derive(Ord, PartialOrd, Eq, PartialEq)]
struct NeedToPut {
    edges: usize,
    v: usize,
}

fn solve_rec(t: &Task, helper: &Helper, cur_positions: &mut Vec<Option<Point>>, rnd: &mut Random) -> Option<Solution> {
    let mut need_to_put = vec![];
    for i in 0..cur_positions.len() {
        if cur_positions[i].is_none() {
            let mut edges = 0;
            for e in t.edges.iter() {
                if e.fr == i && cur_positions[e.to].is_some() || e.to == i && cur_positions[e.fr].is_some() {
                    edges += 1;
                }
            }
            need_to_put.push(NeedToPut { edges, v: i });
        }
    }
    if need_to_put.is_empty() {
        let vertices: Vec<_> = cur_positions.iter().map(|x| x.unwrap()).collect();
        return Some(Solution::create(vertices, t, helper));
    }
    need_to_put.sort();
    let v_to_put = need_to_put.last().unwrap().v;
    let mut possible_positions = vec![];
    let edges: Vec<_> = t.edges.iter().filter(|e| e.fr == v_to_put && cur_positions[e.to].is_some() || e.to == v_to_put && cur_positions[e.fr].is_some()).cloned().collect();
    if rnd.next_in_range(0, 5) == 0 {
        for p in t.hole.iter() {
            if helper.is_valid_position(v_to_put, &p, &edges, &cur_positions, t) {
                possible_positions.push(*p);
            }
        }
    }
    if possible_positions.is_empty() {
        for x in 0..helper.max_c {
            for y in 0..helper.max_c {
                let p = Point { x, y };
                if helper.is_valid_position(v_to_put, &p, &edges, &cur_positions, t) {
                    possible_positions.push(p);
                }
            }
        }
    }
    if possible_positions.is_empty() {
        return None;
    }
    let p = possible_positions[rnd.next_in_range(0, possible_positions.len())];
    cur_positions[v_to_put] = Some(p);
    return match solve_rec(t, helper, cur_positions, rnd) {
        None => {
            cur_positions[v_to_put] = None;
            None
        }
        Some(x) => Some(x)
    };
}

const MAX_ITERS: usize = 10_000;

fn split_by_edge(t: &Task, split_edge: &Edge) -> Option<(Vec<usize>, Vec<usize>)> {
    let mut comp_id = vec![0; t.fig.len()];
    comp_id[split_edge.fr] = 1;
    comp_id[split_edge.to] = 2;
    let mut colored = 2;
    while colored != t.fig.len() {
        for &edge in t.edges.iter() {
            if edge == *split_edge {
                continue;
            }
            if comp_id[edge.fr] != comp_id[edge.to] {
                if comp_id[edge.fr] + comp_id[edge.to] == 3 {
                    return None;
                }
                if comp_id[edge.fr] == 0 {
                    comp_id[edge.fr] = comp_id[edge.to];
                } else {
                    comp_id[edge.to] = comp_id[edge.fr];
                }
                colored += 1;
            }
        }
    }
    let n = comp_id.len();
    let comp1 = (0..n).filter(|id| comp_id[*id] == 1).collect();
    let comp2 = (0..n).filter(|id| comp_id[*id] == 2).collect();
    return Some((comp1, comp2));
}

pub fn not_local_optimize(t: &Task, helper: &Helper, rnd: &mut Random, solution: Solution) -> Solution {
    let mut can_delete = vec![];
    const MAX_DELETE_SIZE: usize = 5;
    for edge in t.edges.iter() {
        match split_by_edge(t, edge) {
            None => {}
            Some((c1, c2)) => {
                if c1.len() < MAX_DELETE_SIZE {
                    can_delete.push(c1);
                }
                if c2.len() < MAX_DELETE_SIZE {
                    can_delete.push(c2);
                }
            }
        }
    }
    if !can_delete.is_empty() {
        const MAX_ITERS: usize = 10;
        for _ in 0..MAX_ITERS {
            let to_delete = can_delete[rnd.next_in_range(0, can_delete.len())].clone();
            let mut cur_positions: Vec<_> = solution.vertices.iter().map(|x| Some(x.clone())).collect();
            for &v in to_delete.iter() {
                cur_positions[v] = None;
            }
            match solve_rec(t, helper, &mut cur_positions, rnd) {
                None => {
                    continue;
                }
                Some(next_sol) => {
                    if next_sol.cmp(&solution) == Ordering::Less {
                        println!("Global optimized: {} -> {}", solution.dislikes, next_sol.dislikes);
                        return next_sol;
                    }
                }
            }
        }
    }
    return solution;
}

pub fn solve_with_helper(t: &Task, helper: &Helper, rnd: &mut Random) -> Option<Solution> {
    // for x in 0..helper.is_inside.len() {
    //     for y in 0..helper.is_inside.len() {
    //         if helper.is_inside[x][y] {
    //             print!("x");
    //         } else {
    //             print!(".");
    //         }
    //     }
    //     println!();
    // }

    for it in 0..MAX_ITERS {
        let solution = solve_rec(t, helper, &mut vec![None; t.fig.len()], rnd);
        if solution.is_some() {
            println!("wow!!! score = {}", solution.clone().unwrap().dislikes);
            return solution;
        } else {
            if it % 100 == 0 {
                println!("bad.. {}", it);
            }
        }
    }
    return None;
}
