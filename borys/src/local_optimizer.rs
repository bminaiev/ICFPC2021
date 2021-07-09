use crate::*;
use crate::helper::*;
use crate::rand::Random;
use std::fs;
use std::collections::BTreeSet;

#[derive(Copy, Clone)]
struct Shift {
    dx: i32,
    dy: i32,
}

pub fn optimize(t: &Task, helper: &Helper, mut solution: Solution, rnd: &mut Random) -> Solution {
    let n = t.fig.len();
    println!("start local optimizations.. eps = {}", t.epsilon);
    let mut iter = 0;
    let path = "process";
    fs::remove_dir_all(path).unwrap();
    fs::create_dir(path).unwrap();
    drawer::save_test(t, &solution, &format!("{}/{:04}.png", path, iter));
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
            for &id in perm.iter() {
                let p = solution.vertices[id];
                let from_x = if big_moves { 0 } else { p.x - 1 };
                let to_x = if big_moves { helper.max_c } else { p.x + 2 };
                let from_y = if big_moves { 0 } else { p.y - 1 };
                let to_y = if big_moves { helper.max_c } else { p.y + 2 };

                let mut shifts = vec![];
                for nx in from_x..to_x {
                    for ny in from_y..to_y {
                        let shift_x = nx - p.x;
                        let shift_y = ny - p.y;
                        shifts.push(Shift { dx: shift_x, dy: shift_y });
                    }
                }
                let perm = rnd.gen_perm(shifts.len());
                let shifts: Vec<_> = perm.iter().map(|id| shifts[*id].clone()).collect();
                for shift in shifts.iter() {
                    let shift_x = shift.dx;
                    let shift_y = shift.dy;
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
                                let new_sol = Solution::create(vertices, t);
                                if new_sol.dislikes < solution.dislikes {
                                    solution = new_sol;
                                    found = true;
                                    need_rev_back = false;
                                    println!("new score: {}, big move: {}", solution.dislikes, big_moves);
                                    iter += 1;
                                    drawer::save_test(t, &solution, &format!("process/{:04}.png", iter));
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
    solution
}