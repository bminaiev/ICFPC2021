use crate::*;
use crate::rand::Random;
use std::collections::BTreeSet;
use crate::vizualizer::{Visualizer, UserEvent};


pub fn optimize(t: &Task, helper: &Helper, mut solution: Solution, rnd: &mut Random) -> Solution {
    let n = t.fig.len();
    println!("start eps optimizations.. eps = {}, cur score = {}", solution.sum_diffs, solution.dislikes);
    let mut small_shifts = vec![];
    const D: i32 = 5;
    for dx in -D..=D {
        for dy in -D..=D {
            if dx == 0 && dy == 0 {
                continue;
            }
            small_shifts.push(Shift { dx, dy });
        }
    }
    loop {
        let mut perm = vec![];
        for _ in 0..n {
            loop {
                let x = rnd.next_in_range(0, n);
                if perm.contains(&x) {
                    continue;
                }
                perm.push(x);
                break;
            }
        }
        let mut found = false;

        let cur_positions: Vec<_> = solution.vertices.clone();
        for &id in perm.iter() {
            let p = solution.vertices[id];
            let shifts = &small_shifts;
            let perm = rnd.gen_perm(shifts.len());
            let shifts: Vec<_> = perm.iter().map(|id| shifts[*id].clone()).collect();
            for shift in shifts.iter() {
                let np = p.shift(shift);

                let mut ok = true;
                for edge in t.edges.iter() {
                    if edge.fr == id || edge.to == id {
                        let another = edge.fr + edge.to - id;
                        if !helper.is_edge_inside(&cur_positions[another], &np) {
                            ok = false;
                            break;
                        }
                    }
                }
                if ok {
                    let mut vertices = cur_positions.clone();
                    vertices[id] = np;
                    let new_sol = Solution::create(vertices, t, helper);
                    if solution.dislikes == new_sol.dislikes && new_sol.cmp_with_eps(&solution) == Ordering::Less {
                        println!("better eps: {}", new_sol.sum_diffs);

                        solution = new_sol;

                        found = true;
                    }
                }
            }
        }
        if !found {
            break;
        }
    }
    println!("now eps: {}", solution.sum_diffs);
    solution
}