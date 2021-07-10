use std::time::Duration;
use borys::{load_test, Solution, Task, Point, load_submission, Shift, save_solution, local_optimizer};
use borys::helper::Helper;
use borys::rand::Random;
use borys::vizualizer::{Visualizer, AdditionalState, UserEvent};
use sdl2::render::{Canvas};
use std::fs::File;

const TEST_ID: usize = 68;

fn rec_shift(t: &Task, h: &Helper, positions: &mut [Point], changed: &mut [bool], depth: usize, max_depth: usize) -> bool {
    for edge in t.edges.iter() {
        if !h.is_valid_edge(t, edge.fr, edge.to, &positions[edge.fr], &positions[edge.to]) {
            if depth == max_depth {
                return false;
            }
            if changed[edge.fr] && changed[edge.to] {
                return false;
            }
            let to_change = if changed[edge.fr] {
                edge.to
            } else if changed[edge.to] {
                edge.fr
            } else {
                unreachable!();
            };
            let seen = edge.fr + edge.to - to_change;
            for dx in -1..=1 {
                for dy in -1..=1 {
                    if dx == 0 && dy == 0 {
                        continue;
                    }
                    let old_p = positions[to_change];
                    let np = old_p.shift(&Shift { dx, dy });
                    if h.is_valid_edge(t, seen, to_change, &positions[seen], &np) {
                        changed[to_change] = true;
                        positions[to_change] = np;
                        if rec_shift(t, h, positions, changed, depth + 1, max_depth) {
                            return true;
                        }
                        changed[to_change] = false;
                        positions[to_change] = old_p;
                    }
                }
            }
            return false;
        }
    }
    return true;
}

fn try_shift(t: &Task, h: &Helper, positions: &mut [Point], v: usize, shift: &Shift) -> bool {
    let mut changed = vec![false; positions.len()];
    changed[v] = true;
    let old_p = positions[v];
    positions[v] = positions[v].shift(shift);
    for max_depth in 0..=8 {
        if rec_shift(t, h, positions, &mut changed, 0, max_depth) {
            return true;
        }
    }
    positions[v] = old_p;
    return false;
}


pub fn main() {
    let mut f_all = File::create("outputs/all_scores.txt").unwrap();

    let task = load_test(TEST_ID);
    let helper = Helper::create(&task);
    println!("max_c = {}", helper.max_c);
    let mut rnd = Random::new(3342552);
    let vertices = load_submission(&format!("../borys/outputs/{}.ans", TEST_ID));
    let mut solution = Solution::create(vertices, &task, &helper);

    let ttf_context = sdl2::ttf::init().map_err(|e| e.to_string()).unwrap();
    let mut viz = Visualizer::create(&helper, &ttf_context);

    let mut state = AdditionalState::create();

    let mut close_app = false;
    loop {
        let events = viz.render(&task, &helper, &solution, 0, Some(&state));
        for event in events.iter() {
            match event {
                UserEvent::IncreaseChangeProb => {}
                UserEvent::MouseMoved(x, y) => {
                    state.mouse_x = *x;
                    state.mouse_y = *y;
                }
                UserEvent::CloseApp => {
                    close_app = true;
                }
                UserEvent::Selected(id) => {
                    state.selected = Some(*id);
                }
                UserEvent::Shift(shift) => {
                    match state.selected {
                        None => {}
                        Some(id) => {
                            let np = solution.vertices[id].shift(shift);
                            if helper.is_point_inside(&np) {
                                let mut ok = true;
                                for edge in task.edges.iter() {
                                    if edge.fr == id || edge.to == id {
                                        let another = edge.fr + edge.to - id;
                                        if !helper.is_edge_inside(&solution.vertices[another], &np) {
                                            ok = false;
                                            break;
                                        }
                                    }
                                }
                                if ok {
                                    let mut vertices = solution.vertices.clone();
                                    if try_shift(&task, &helper, &mut vertices, id, shift) {
                                        println!("Successfully moved point!");
                                        solution = Solution::create(vertices, &task, &helper);
                                        save_solution(&solution, TEST_ID, &mut f_all, &task);
                                    } else {
                                        println!("Couldn't find solution :(");
                                    }
                                }
                            }
                        }
                    }
                }
                UserEvent::RunLocalOptimizations => {
                    let old_score = solution.dislikes;
                    solution = local_optimizer::optimize_only_optimal(&task, &helper, solution, &mut rnd, &mut None);
                    println!("Local optimizations: {} -> {}", old_score, solution.dislikes);
                }
                UserEvent::MovePoint(to) => {
                    let selected = state.selected.unwrap();
                    solution = solution.move_one_point(selected, *to, &task, &helper);
                    // TODO: check solution is valid?
                }
            }
        }
        if close_app {
            break;
        }
        // The rest of the game loop goes here...

        ::std::thread::sleep(Duration::new(0, 1_000_000_000u32 / 60));
    }
}
