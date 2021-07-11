use sdl2::pixels::Color;
use sdl2::event::Event;
use sdl2::keyboard::Keycode;
use std::time::Duration;
use borys::{load_test, Solution, Task, Point};
use borys::helper::Helper;
use borys::rand::Random;
use borys::vizualizer::Visualizer;
use sdl2::render::{TextureQuery, Canvas};
use sdl2::video::Window;
use sdl2::ttf::Font;

const TEST_ID: usize = 80;


pub fn try_remove_bad_edges(vertices: &mut [Point], rnd: &mut Random, task: &Task, helper: &Helper) -> bool {
    let count_bad_edges = |vertices: &[Point], bad_vertices: &mut Vec<usize>| {
        let mut bad_edges = 0;
        for edge in task.edges.iter() {
            let v1 = vertices[edge.fr];
            let v2 = vertices[edge.to];
            if !helper.is_edge_inside(&v1, &v2) {
                bad_edges += 1;
                bad_vertices.push(edge.fr);
                bad_vertices.push(edge.to);
            }
        }
        return bad_edges;
    };
    let mut bad_vertices = vec![];
    let bad_edges = count_bad_edges(&vertices, &mut bad_vertices);
    if bad_edges == 0 {
        return true;
    }
    println!("bad edges: {}", bad_edges);
    let v = bad_vertices[rnd.next_in_range(0, bad_vertices.len())];
    let x = rnd.next_in_range(0, helper.max_c as usize);
    let y = rnd.next_in_range(0, helper.max_c as usize);
    let p = Point { x: x as i32, y: y as i32 };
    if !helper.is_point_inside(&p) {
        return false;
    }
    let old_p = vertices[v].clone();
    vertices[v] = p;
    let next_bad_edges = count_bad_edges(&vertices, &mut bad_vertices);
    if next_bad_edges <= bad_edges || rnd.next_in_range(0, 10) == 0 {
        return false;
    }
    vertices[v] = old_p;
    return false;
}

/**
    This function could return edges, which  intersect boundary of hole :(
*/
pub fn not_intersection_solution(task: &Task, helper: &Helper, rnd: &mut Random) -> Solution {
    let mut vertices = vec![];
    for i in 0..task.fig.len() {
        let mut try_without_bad_edges = 10000i32;
        loop {
            let x = rnd.next_in_range(0, helper.max_c as usize);
            let y = rnd.next_in_range(0, helper.max_c as usize);
            let p = Point { x: x as i32, y: y as i32 };
            if !helper.is_point_inside(&p) {
                continue;
            }
            let mut ok = true;
            for edge in task.edges.iter() {
                if edge.fr == i || edge.to == i {
                    let another = edge.fr + edge.to - i;
                    if another < i {
                        let another_p = vertices[another];
                        if !helper.is_edge_inside(&p, &another_p) {
                            ok = false;
                            break;
                        }
                    }
                }
            }
            if !ok && try_without_bad_edges > 0 {
                try_without_bad_edges -= 1;
                continue;
            }
            vertices.push(p);
            break;
        }
    }
    return Solution::create(vertices, task, helper);
}


pub fn main() {
    let task = load_test(TEST_ID);
    let helper = Helper::create(&task);
    println!("max_c = {}", helper.max_c);
    let mut rnd = Random::new(3342552);
    let mut solution = not_intersection_solution(&task, &helper, &mut rnd);


    let ttf_context = sdl2::ttf::init().map_err(|e| e.to_string()).unwrap();
    let mut viz = Visualizer::create(&helper, &ttf_context);

    'running: loop {
        let mut vertices = solution.vertices.clone();
        let mut exist_bad_edges = true;
        for _ in 0..50 {
            if try_remove_bad_edges(&mut vertices, &mut rnd, &task, &helper) {
                exist_bad_edges = false;
                break;
            }
        }
        solution = Solution::create(vertices, &task, &helper);

        if !exist_bad_edges {
            solution = shrink_edges(&task, &helper, solution);
            let mut bad_vertices = vec![];
            for edge in task.edges.iter() {
                let score = helper.edge_score(&task, edge.fr, edge.to, &solution.vertices[edge.fr], &solution.vertices[edge.to]);
                if score > 1.0 {
                    bad_vertices.push(edge.fr);
                    bad_vertices.push(edge.to);
                }
            }
            if bad_vertices.is_empty() {
                println!("WOWOWOWOWOW??? We found reasonable solution?");
            } else {
                if rnd.next_in_range(0, 10) == 0 {
                    for _ in 0..10 {
                        let v = bad_vertices[rnd.next_in_range(0, bad_vertices.len())];
                        let next_place = find_ok_point(&task, &helper, &mut rnd, &mut solution, v);
                        solution = solution.move_one_point(v, next_place, &task, &helper);
                    }
                }
            }
        }

        viz.render(&task, &helper, &solution, 0, None);
        // The rest of the game loop goes here...

        ::std::thread::sleep(Duration::new(0, 1_000_000_000u32 / 600));
    }
}

fn find_ok_point(task: &Task, helper: &Helper, rnd: &mut Random, solution: &mut Solution, v: usize) -> Point {
    let next_place = {
        let mut old_p = solution.vertices[v];
        for _ in 0..100 {
            let x = rnd.next_in_range(0, helper.max_c as usize);
            let y = rnd.next_in_range(0, helper.max_c as usize);
            let np = Point { x: x as i32, y: y as i32 };
            let mut ok = true;
            for edge in task.edges.iter() {
                if edge.fr == v || edge.to == v {
                    let another = edge.fr + edge.to - v;
                    if !helper.is_edge_inside(&np, &solution.vertices[another]) {
                        ok = false;
                        break;
                    }
                }
            }
            if ok {
                old_p = np;
                break;
            }
        }
        old_p
    };
    next_place
}

fn shrink_edges(task: &Task, helper: &Helper, mut solution: Solution) -> Solution {
    for v in 0..task.fig.len() {
        let mut cur_sum_errors = 0.0;
        for edge in task.edges.iter() {
            if edge.fr == v || edge.to == v {
                cur_sum_errors += helper.edge_score(&task, edge.fr, edge.to, &solution.vertices[edge.fr], &solution.vertices[edge.to]);
            }
        }
        let mut old_p = solution.vertices[v];
        loop {
            let mut best_p = old_p;
            for dx in -1..=-1 {
                for dy in -1..=1 {
                    if dx == 0 && dy == 0 {
                        continue;
                    }
                    let p = Point { x: old_p.x + dx, y: old_p.y + dy };
                    let mut next_sum_errors = 0.0;
                    let mut ok = true;
                    for edge in task.edges.iter() {
                        if edge.fr == v || edge.to == v {
                            let another = edge.fr + edge.to - v;
                            next_sum_errors += helper.edge_score(&task, v, another, &p, &solution.vertices[another]);
                            if !helper.is_edge_inside(&p, &solution.vertices[another]) {
                                ok = false;
                                break;
                            }
                        }
                    }
                    if ok && next_sum_errors < cur_sum_errors {
                        cur_sum_errors = next_sum_errors;
                        best_p = p;
                    }
                }
            }
            if best_p == old_p {
                break;
            } else {
                old_p = best_p;
            }
        }
        let mut vertices = solution.vertices.clone();
        vertices[v] = old_p;
        solution = Solution::create(vertices, &task, &helper);
    }
    return solution;
}
