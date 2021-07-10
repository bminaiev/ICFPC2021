use crate::*;
use crate::rand::Random;
use std::collections::BTreeSet;


const DRAW_PICTURES: bool = false;

pub fn optimize(t: &Task, helper: &Helper, mut solution: Solution, rnd: &mut Random) -> Solution {
    let n = t.fig.len();
    println!("start local optimizations.. eps = {}, cur score = {}", t.epsilon, solution.dislikes);
    let mut iter = 0;
    let path = "process";
    drawer::reset();
    drawer::save_test(t, &solution, &format!("{}/{:04}.png", path, iter));
    let mut small_shifts = vec![];
    for dx in -1..=1 {
        for dy in -1..=1 {
            if dx == 0 && dy == 0 {
                continue;
            }
            small_shifts.push(Shift { dx, dy });
        }
    }
    let mut b_sol = solution.clone();
    let mut pr_change = 1.0;
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

        let mut cur_positions: Vec<_> = solution.vertices.iter().map(|x| Some(x.clone())).collect();
        for &big_moves in [false, true].iter() {
            if big_moves == true {
                // TODO: remove it back...
                // break;
            }
            for &id in perm.iter() {
                pr_change *= 0.999;
                let p = solution.vertices[id];
                let mut shifts = &small_shifts;
                let mut base_point = p;
                if big_moves {
                    let mut cur_shifts_size = std::usize::MAX;
                    for (e_id, edge) in t.edges.iter().enumerate() {
                        if (edge.fr == id || edge.to == id) && helper.shifts_per_edge[e_id].len() < cur_shifts_size {
                            shifts = &helper.shifts_per_edge[e_id];
                            base_point = cur_positions[edge.fr + edge.to - id].unwrap();
                            cur_shifts_size = shifts.len();
                        }
                    }
                }
                let perm = rnd.gen_perm(shifts.len());
                let shifts: Vec<_> = perm.iter().map(|id| shifts[*id].clone()).collect();
                for shift in shifts.iter() {
                    let shift_x = shift.dx - p.x + base_point.x;
                    let shift_y = shift.dy - p.y + base_point.y;
                    let old_cur_positions = cur_positions.clone();

                    let np = Point { x: p.x + shift_x, y: p.y + shift_y };
                    cur_positions[id] = Some(np);
                    let mut need_rev_back = true;
                    let mut changed_points = BTreeSet::new();
                    changed_points.insert(id);
                    let mut moved_points = 0;
                    loop {
                        let bad_edge = helper.get_bad_edge(&cur_positions, &t);
                        if big_moves && bad_edge.is_some() {
                            break;
                        }
                        moved_points += 1;
                        assert!(moved_points <= t.fig.len() + 2);
                        match bad_edge {
                            None => {
                                let vertices: Vec<_> = cur_positions.iter().map(|x| x.unwrap()).collect();
                                let new_sol = Solution::create(vertices, t, helper);
                                if new_sol.cmp(&solution) == Ordering::Less || rnd.next_double() < pr_change {
                                    solution = new_sol;
                                    if solution.cmp(&b_sol) == Ordering::Less {
                                        b_sol = solution.clone();
                                    }
                                    found = true;
                                    need_rev_back = false;
                                    println!("new score: {}, big move: {}, pr change: {}, overall best: {}", solution.dislikes, big_moves, pr_change, b_sol.dislikes);
                                    iter += 1;
                                    if DRAW_PICTURES {
                                        drawer::save_test(t, &solution, &format!("process/{:04}.png", iter));
                                    }
                                }
                                break;
                            }
                            Some(edge) => {
                                if !changed_points.contains(&edge.fr) {
                                    let fr_p = cur_positions[edge.fr].unwrap();
                                    cur_positions[edge.fr] = Some(Point { x: fr_p.x + shift_x, y: fr_p.y + shift_y });
                                    changed_points.insert(edge.fr);
                                } else if !changed_points.contains(&edge.to) {
                                    let to_p = cur_positions[edge.to].unwrap();
                                    cur_positions[edge.to] = Some(Point { x: to_p.x + shift_x, y: to_p.y + shift_y });
                                    changed_points.insert(edge.to);
                                } else {
                                    break;
                                }
                            }
                        }
                    }
                    if need_rev_back {
                        cur_positions = old_cur_positions;
                    }
                }
            }
            if found {
                break;
            }
        }
        if !found {
            break;
        }
    }
    b_sol
}