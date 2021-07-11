use crate::helper::Helper;
use crate::{Solution, Task, Shift, Point, Edge};
use crate::rand::Random;
use std::collections::{BTreeSet, BTreeMap};
use std::time::Instant;

struct State<'a> {
    need_to_change: Vec<bool>,
    positions: Vec<Point>,
    task: &'a Task,
    helper: &'a Helper,
    rnd: &'a mut Random,
}

fn rec(s: &mut State) -> bool {
    for (e_id, edge) in s.task.edges.iter().enumerate() {
        if s.need_to_change[edge.fr] == s.need_to_change[edge.to] {
            continue;
        }
        let v = if s.need_to_change[edge.fr] { edge.fr } else { edge.to };
        s.need_to_change[v] = false;
        let base = edge.fr + edge.to - v;
        let base_p = s.positions[base];
        let shifts = &s.helper.shifts_per_edge[e_id];
        let mut cnt_available = 0;
        let mut use_shift = Shift { dx: 0, dy: 0 };
        for shift in shifts.iter() {
            let np = base_p.shift(shift);
            s.positions[v] = np;
            if s.helper.is_edge_inside(&np, &base_p) {
                let mut ok = true;
                for e in s.task.edges.iter() {
                    if !s.need_to_change[e.fr] && !s.need_to_change[e.to] {
                        if !s.helper.is_valid_edge(s.task, e.fr, e.to, &s.positions[e.fr], &s.positions[e.to]) {
                            ok = false;
                            break;
                        }
                    }
                }
                if ok {
                    cnt_available += 1;
                    if s.rnd.next_in_range(0, cnt_available) == 0 {
                        use_shift = shift.clone();
                    }
                }
            }
        }
        if cnt_available == 0 {
            s.need_to_change[v] = true;
            return false;
        } else {
            s.positions[v] = base_p.shift(&use_shift);
            return rec(s);
        }
    }
    return true;
}


pub fn optimize(task: &Task, helper: &Helper, solution: Solution, v_to_change: &[usize], rnd: &mut Random) -> Solution {
    let old_score = solution.dislikes;

    let mut b_sol = solution.clone();
    let mut b_score = std::i64::MAX;
    let start_time = Instant::now();
    let mut total_iters = 0;
    while start_time.elapsed().as_millis() < 2000 {
        total_iters += 1;
        let mut need_to_change = vec![false; task.fig.len()];
        for v in v_to_change.iter() {
            need_to_change[*v] = true;
        }

        let mut state = State {
            need_to_change,
            positions: solution.vertices.clone(),
            task,
            helper,
            rnd,
        };

        if rec(&mut state) {
            let cur_sol = Solution::create(state.positions, task, helper);
            if cur_sol.dislikes < b_score {
                b_score = cur_sol.dislikes;
                b_sol = cur_sol;
            }
        }
    }
    if b_score != std::i64::MAX {
        println!("Found some solution, with score {} -> {}, iters: {}", old_score, b_sol.dislikes, total_iters);
    } else {
        println!("Couldn't find any sol :( iters: {}", total_iters);
    }

    b_sol
}