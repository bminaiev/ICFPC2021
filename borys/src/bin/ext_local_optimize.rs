use std::io::{BufReader};

use borys::*;
use std::fs::File;
use borys::helper::Helper;
use borys::rand::Random;
use std::path::Path;
use borys::vizualizer::Visualizer;
use std::time::Duration;

/**************************************************

    START OF TEMPLATE CODE

 *************************************************/
#[allow(unused_macros)]
macro_rules! dbg {
    ($first_val:expr, $($val:expr),+ $(,)?) => {
        eprint!("[{}:{}] {} = {:?}",
                    file!(), line!(), stringify!($first_val), &$first_val);
        ($(eprint!(", {} = {:?}", stringify!($val), &$val)),+,);
        eprintln!();
    };
    ($first_val:expr) => {
        eprintln!("[{}:{}] {} = {:?}",
                    file!(), line!(), stringify!($first_val), &$first_val);
    };
}

enum InputSource {
    Stdin,
    FromFile(Vec<String>),
}

struct Scanner {
    buffer: Vec<String>,
    input_source: InputSource,
}


impl Scanner {
    #[allow(dead_code)]
    fn new() -> Self {
        Self { buffer: vec![], input_source: InputSource::Stdin }
    }

    #[allow(dead_code)]
    fn new_file(filename: &str) -> Self {
        let file = std::fs::read_to_string(filename).unwrap();
        let mut lines: Vec<String> = file.lines().map(|line| String::from(line)).collect();
        lines.reverse();
        Self { buffer: vec![], input_source: InputSource::FromFile(lines) }
    }


    #[allow(dead_code)]
    fn i64(&mut self) -> i64 {
        self.next::<i64>()
    }

    #[allow(dead_code)]
    fn i32(&mut self) -> i32 {
        self.next::<i32>()
    }

    #[allow(dead_code)]
    fn usize(&mut self) -> usize {
        self.next::<usize>()
    }

    #[allow(dead_code)]
    fn vec<T: std::str::FromStr>(&mut self, n: usize) -> Vec<T> {
        (0..n).map(|_| self.next::<T>()).collect()
    }

    fn parse_next_line(&mut self) -> bool {
        let mut input = String::new();
        match &mut self.input_source {
            | InputSource::Stdin => {
                if std::io::stdin().read_line(&mut input).expect("Failed read") == 0 {
                    return false;
                }
            }
            | InputSource::FromFile(lines) => {
                match lines.pop() {
                    Some(line) => input = line,
                    None => return false,
                }
            }
        }

        self.buffer = input.split_whitespace().rev().map(String::from).collect();
        return true;
    }

    fn next<T: std::str::FromStr>(&mut self) -> T {
        loop {
            if let Some(token) = self.buffer.pop() {
                return token.parse().ok().expect("Failed parse");
            }

            self.parse_next_line();
        }
    }

    #[allow(dead_code)]
    fn has_more_elements(&mut self) -> bool {
        loop {
            if !self.buffer.is_empty() {
                return true;
            }
            if !self.parse_next_line() {
                return false;
            }
        }
    }


    #[allow(dead_code)]
    fn string(&mut self) -> Vec<u8> {
        self.next::<String>().into_bytes()
    }
}

/**************************************************

    END OF TEMPLATE CODE

 *************************************************/


const LOAD_MY: bool = true;

pub fn main() {
    let mut f_all = File::create("outputs/all_scores.txt").unwrap();

    let outputs_suffix = ""; // "_romka"

    const TEST: usize = 85;
    for test in TEST..=TEST {
        println!("TEST: {}", test);
        let mut vertices: Vec<_> = if LOAD_MY {
            load_submission(&format!("../borys/outputs/{}.ans", test))
        } else {
            let romka_path = format!("../outputs{}/{}.ans", outputs_suffix, test);
            if !Path::new(&romka_path).exists() {
                continue;
            }
            let mut sc = Scanner::new_file(&romka_path);
            let n = sc.usize();
            (0..n).map(|_| {
                let x = sc.i32();
                let y = sc.i32();
                Point { x, y }
            }).collect()
        };

        let file = File::open(format!("../inputs/{}.problem", test)).unwrap();
        let reader = BufReader::new(file);

        let input: Input = serde_json::from_reader(reader).unwrap();

        let task = conv_input(&input);
        let helper = Helper::create(&task);

        let ttf_context = sdl2::ttf::init().map_err(|e| e.to_string()).unwrap();
        let mut viz = None;// Some(Visualizer::create(&helper, &ttf_context));


        let mut rnd = Random::new(45444);

        loop {
            let initial_sol = Solution::create(vertices.clone(), &task, &helper);
            let mut optimized = false;
            for _ in 0..5 {
                let mut solution = Solution::create(vertices.clone(), &task, &helper);

                loop {
                    let old_dislikes = solution.dislikes;
                    let local_optimized_solution = local_optimizer::optimize(&task, &helper, solution, &mut rnd, &mut viz);
                    let global_optimized = solver::not_local_optimize(&task, &helper, &mut rnd, local_optimized_solution);
                    solution = global_optimized;
                    save_solution(&solution, test, &mut f_all, &task);
                    if solution.dislikes >= old_dislikes {
                        break;
                    }
                }

                save_solution(&solution, test, &mut f_all, &task);
                if solution.dislikes < initial_sol.dislikes {
                    optimized = true;
                }
            }
            if !optimized {
                break;
            }
        }
    }
}
