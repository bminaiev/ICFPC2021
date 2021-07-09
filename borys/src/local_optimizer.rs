use crate::*;
use crate::helper::*;
use crate::rand::Random;
use std::fs;

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
        for &id in perm.iter() {
            let edges: Vec<_> = t.edges.iter().filter(|e| e.fr == id || e.to == id).cloned().collect();
            let p = solution.vertices[id];
            for dx in -1..=1 {
                for dy in -1..=1 {
                    let np = Point { x: p.x + dx, y: p.y + dy };
                    cur_positions[id] = Some(np);
                    let mut need_rev_back = true;
                    if helper.is_valid_position(id, &np, &edges, &cur_positions, t) {
                        let vertices: Vec<_> = cur_positions.iter().map(|x| x.unwrap()).collect();
                        let new_sol = Solution::create(vertices, t);
                        // println!("valid, check score: {}", new_sol.dislikes);
                        if new_sol.dislikes < solution.dislikes {
                            solution = new_sol;
                            cur_positions[id] = Some(np);
                            found = true;
                            need_rev_back = false;
                            println!("new score: {}", solution.dislikes);
                            iter += 1;
                            drawer::save_test(t, &solution, &format!("process/{:04}.png", iter));
                        }
                    }
                    if need_rev_back {
                        cur_positions[id] = Some(p);
                    }
                }
            }
        }
        if !found {
            break;
        }
    }
    solution
}