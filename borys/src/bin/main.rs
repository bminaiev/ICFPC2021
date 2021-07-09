use std::fs::File;
use std::io::BufReader;
use borys::{Input, PointInput, OutputFormat, drawer, Solution, Edge, conv_input, save_solution};
use borys::rand::Random;
use std::cmp::{max, min};
use std::mem::swap;
use std::io::{Write};

use borys::{Point, Task, local_optimizer};
use borys::helper::Helper;

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
        return Some(Solution::create(vertices, t));
    }
    need_to_put.sort();
    let v_to_put = need_to_put.last().unwrap().v;
    let mut possible_positions = vec![];
    let edges: Vec<_> = t.edges.iter().filter(|e| e.fr == v_to_put && cur_positions[e.to].is_some() || e.to == v_to_put && cur_positions[e.fr].is_some()).cloned().collect();
    for x in 0..helper.max_c {
        for y in 0..helper.max_c {
            let p = Point { x, y };
            if helper.is_valid_position(v_to_put, &p, &edges, &cur_positions, t) {
                possible_positions.push(p);
            }
        }
    }
    if possible_positions.is_empty() {
        return None;
    }
    let p = possible_positions[rnd.next_in_range(0, possible_positions.len())];
    cur_positions[v_to_put] = Some(p);
    return solve_rec(t, helper, cur_positions, rnd);
}

const MAX_ITERS: usize = 10_000;

fn solve_with_helper(t: &Task, helper: &Helper, rnd: &mut Random) -> Option<Solution> {
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

fn solve(t: &Task, rnd: &mut Random) -> Option<Solution> {
    let helper = Helper::create(t);
    match solve_with_helper(t, &helper, rnd) {
        None => None,
        Some(solution) => {
            let optimized_solution = local_optimizer::optimize(t, &helper, solution, rnd);
            Some(optimized_solution)
        }
    }
}


fn main() {
    const TASK: usize = 59;
    let mut f_all = File::create("outputs/all_scores.txt").unwrap();
    let not_interesting_tests: Vec<_> = (11..=41).chain(vec![9, 43, 45, 46, 47, 49, 51, 52, 53, 54]).collect();

    let mut rnd = Random::new(787788);
    for problem_id in TASK..=TASK {
        if not_interesting_tests.contains(&problem_id) {
            println!("Skip test: {}", problem_id);
            continue;
        }
        println!("Start test {}", problem_id);
        for _ in 0..10 {
            let file = File::open(format!("../inputs/{}.problem", problem_id)).unwrap();
            let reader = BufReader::new(file);

            let input: Input = serde_json::from_reader(reader).unwrap();

            let task = conv_input(&input);
            let res = solve(&task, &mut rnd);
            match res {
                None => {
                    writeln!(f_all, "{}: no solution", problem_id).unwrap();
                }
                Some(solution) => {
                    save_solution(&solution, problem_id, &mut f_all, &task);
                }
            }
            f_all.flush().unwrap();
        }
        // dbg!(input);
    }
}
