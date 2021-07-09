use std::fs::File;
use std::io::BufReader;
use borys::{Input, PointInput, OutputFormat, drawer, Solution, Edge};
use borys::rand::Random;
use std::cmp::{max, min};
use std::mem::swap;
use std::io::{Write};

use borys::{Point, Task};
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
    let edges: Vec<_> = t.edges.iter().filter(|e| e.fr == v_to_put && cur_positions[e.to].is_some() || e.to == v_to_put && cur_positions[e.fr].is_some()).collect();
    for x in 0..helper.max_c {
        for y in 0..helper.max_c {
            let p = Point { x, y };
            if !helper.is_point_inside(&p) {
                continue;
            }
            let mut ok = true;
            for edge in edges.iter() {
                let another = edge.to + edge.fr - v_to_put;
                let another_p = cur_positions[another].unwrap();
                if !helper.is_edge_inside(&p, &another_p) {
                    ok = false;
                    break;
                }
                let init_d2 = t.fig[v_to_put].d2(&t.fig[another]);
                let cur_d2 = p.d2(&another_p);
                let delta = (init_d2 - cur_d2).abs();
                // delta / init_d2 <= eps / 10^6
                // delta * 10^6 <= eps * init_d2
                if delta * 1_000_000 > t.epsilon * init_d2 {
                    ok = false;
                    break;
                }
            }
            if !ok {
                continue;
            }
            possible_positions.push(p);
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

fn solve_with_helper(t: &Task, helper: &Helper) -> Option<Solution> {
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

    let mut rnd = Random::new(787788);
    for it in 0..MAX_ITERS {
        let solution = solve_rec(t, helper, &mut vec![None; t.fig.len()], &mut rnd);
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
    let helper = Helper::create(t);
    return solve_with_helper(t, &helper);
}

fn conv_input(t: &Input) -> Task {
    let hole = conv_points(&t.hole);
    let fig = conv_points(&t.figure.vertices);
    let edges: Vec<_> = t.figure.edges.iter().map(|e| Edge { fr: e[0], to: e[1] }).collect();
    return Task { hole, fig, edges, epsilon: t.epsilon };
}

fn main() {
    // const TASK: usize = 14;
    let mut f_all = File::create("outputs/all_scores.txt").unwrap();
    for problem_id in 4..=4 {
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
