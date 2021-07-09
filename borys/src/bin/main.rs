use std::fs::File;
use std::io::BufReader;
use borys::{Input, PointInput, OutputFormat, drawer, Solution, Edge};
use borys::rand::Random;
use std::cmp::{max, min};
use std::mem::swap;
use std::io::{Write};

use borys::{Point, Task, local_optimizer};
use borys::helper::Helper;

fn conv_points(pts: &[PointInput]) -> Vec<Point> {
    pts.iter().map(|p| Point { x: p[0], y: p[1] }).collect()
}

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
    for x in 0..helper.is_inside.len() {
        for y in 0..helper.is_inside.len() {
            if helper.is_inside[x][y] {
                print!("x");
            } else {
                print!(".");
            }
        }
        println!();
    }

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

fn solve(t: &Task) -> Option<Solution> {
    let mut rnd = Random::new(787788);
    let helper = Helper::create(t);
    match solve_with_helper(t, &helper, &mut rnd) {
        None => None,
        Some(solution) => {
            let optimized_solution = local_optimizer::optimize(t, &helper, solution, &mut rnd);
            Some(optimized_solution)
        }
    }
}

fn conv_input(t: &Input) -> Task {
    let hole = conv_points(&t.hole);
    let fig = conv_points(&t.figure.vertices);
    let edges: Vec<_> = t.figure.edges.iter().map(|e| Edge { fr: e[0], to: e[1] }).collect();
    return Task { hole, fig, edges, epsilon: t.epsilon };
}

fn main() {
    const TASK: usize = 2;
    let mut f_all = File::create("outputs/all_scores.txt").unwrap();
    for problem_id in TASK..=TASK {
        println!("Start test {}", problem_id);
        let file = File::open(format!("../inputs/{}.problem", problem_id)).unwrap();
        let reader = BufReader::new(file);

        let input: Input = serde_json::from_reader(reader).unwrap();

        let task = conv_input(&input);
        let res = solve(&task);
        match res {
            None => {
                writeln!(f_all, "{}: no solution", problem_id).unwrap();
            }
            Some(solution) => {
                let vertices = solution.vertices.iter().map(|p| [p.x, p.y]).collect();
                let out = OutputFormat { vertices };
                let mut f = File::create(format!("outputs/{}.ans", problem_id)).unwrap();
                writeln!(f, "{}", serde_json::to_string(&out).unwrap()).unwrap();
                let mut f_score = File::create(format!("outputs/{}.score", problem_id)).unwrap();
                writeln!(f_score, "{}", solution.dislikes).unwrap();
                writeln!(f_all, "{}: {}", problem_id, solution.dislikes).unwrap();
                drawer::save_test(&task, &solution, &format!("outputs_pics/{}.png", problem_id));
            }
        }
        f_all.flush().unwrap();
        // dbg!(input);
    }
}
