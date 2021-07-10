use std::fs::File;
use std::io::BufReader;
use borys::{Input, PointInput, OutputFormat, drawer, Solution, Edge, conv_input, save_solution, solver};
use borys::rand::Random;
use std::cmp::{max, min, Ordering};
use std::mem::swap;
use std::io::{Write};

use borys::{Point, Task, local_optimizer};
use borys::helper::Helper;


fn solve(t: &Task, rnd: &mut Random) -> Option<Solution> {
    let helper = Helper::create(t);
    match solver::solve_with_helper(t, &helper, rnd) {
        None => None,
        Some(mut solution) => {
            loop {
                let old_dislikes = solution.dislikes;
                let local_optimized_solution = local_optimizer::optimize(t, &helper, solution, rnd);
                let global_optimized = solver::not_local_optimize(t, &helper, rnd, local_optimized_solution);
                solution = global_optimized;
                if solution.dislikes >= old_dislikes {
                    break;
                }
            }
            Some(solution)
        }
    }
}


fn main() {
    const TASK: usize = 75;
    let mut f_all = File::create("outputs/all_scores.txt").unwrap();
    let not_interesting_tests: Vec<_> = (11..=41).chain(vec![9, 43, 45, 46, 47, 49, 51, 52, 53, 54, 63, 64, 65, 68, 70, 72, 73, 74, 75, 78]).collect();

    let mut rnd = Random::new(775741);
    for GLOBAL_ITER in 0..1 {
        println!("GLOBAL ITER: {}", GLOBAL_ITER);
        for problem_id in TASK..=TASK {
            // if not_interesting_tests.contains(&problem_id) {
            //     println!("Skip test: {}", problem_id);
            //     continue;
            // }
            println!("Start test {}", problem_id);
            for _ in 0..1 {
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
}
