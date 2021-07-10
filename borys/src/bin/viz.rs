use sdl2::pixels::Color;
use sdl2::event::Event;
use sdl2::keyboard::Keycode;
use std::time::Duration;
use borys::{load_test, Solution, Task, Point};
use borys::helper::Helper;
use borys::rand::Random;
use sdl2::render::{TextureQuery, Canvas};
use sdl2::video::Window;
use sdl2::ttf::Font;

const TEST_ID: usize = 85;

const ZOOM: i32 = 10;

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

fn color_inside(from: Color, to: Color, mut part: f64) -> Color {
    if part > 1.0 {
        part = 1.0;
    }
    if part < 0.0 {
        part = 0.0;
    }
    let mid = |x: u8, y: u8| -> u8 {
        ((x as f64) * (1.0 - part) + (y as f64) * part) as u8
    };
    Color::RGB(mid(from.r, to.r), mid(from.g, to.g), mid(from.b, to.b))
}

fn calc_sum_errors(task: &Task, helper: &Helper, solution: &Solution) -> f64 {
    let mut res = 0.0;
    for edge in task.edges.iter() {
        res += helper.edge_score(&task, edge.fr, edge.to, &solution.vertices[edge.fr], &solution.vertices[edge.to]);
    }
    return res;
}

const GREY: Color = Color::RGB(222u8, 222u8, 222u8);

pub fn main() {
    let ttf_context = sdl2::ttf::init().map_err(|e| e.to_string()).unwrap();
    let mut font = ttf_context.load_font("assets/times.ttf", 20).unwrap();
    font.set_style(sdl2::ttf::FontStyle::BOLD);


    let task = load_test(TEST_ID);
    let helper = Helper::create(&task);
    println!("max_c = {}", helper.max_c);
    let mut rnd = Random::new(3342552);
    let mut solution = not_intersection_solution(&task, &helper, &mut rnd);

    let sdl_context = sdl2::init().unwrap();
    let video_subsystem = sdl_context.video().unwrap();

    let display = video_subsystem.display_bounds(1).unwrap();

    let window = video_subsystem.window("rust-sdl2 demo", (helper.max_c * ZOOM) as u32, (helper.max_c * ZOOM) as u32)
        .position(display.x + 1000, display.y + 400)
        .build()
        .unwrap();

    let mut canvas = window.into_canvas().build().unwrap();

    let conv_point = |p: &Point| {
        sdl2::rect::Point::new(p.x * ZOOM, p.y * ZOOM)
    };

    let mut event_pump = sdl_context.event_pump().unwrap();

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


        canvas.set_draw_color(Color::WHITE);
        canvas.clear();

        let sum_errors = calc_sum_errors(&task, &helper, &solution);


        canvas.set_draw_color(GREY);
        for x in 0..helper.max_c {
            canvas.draw_line(conv_point(&Point { x, y: 0 }), conv_point(&Point { x, y: helper.max_c as i32 })).unwrap();
        }
        for y in 0..helper.max_c {
            canvas.draw_line(conv_point(&Point { x: 0, y }), conv_point(&Point { x: helper.max_c as i32, y })).unwrap();
        }
        canvas.set_draw_color(Color::BLACK);
        for edge in helper.hole_and_first.windows(2) {
            let p1 = conv_point(&edge[0]);
            let p2 = conv_point(&edge[1]);
            canvas.draw_line(p1, p2).unwrap();
        }
        let mut bad_edges = 0;
        for edge in task.edges.iter() {
            let p1 = conv_point(&solution.vertices[edge.fr]);
            let p2 = conv_point(&solution.vertices[edge.to]);
            let score = helper.edge_score(&task, edge.fr, edge.to, &solution.vertices[edge.fr], &solution.vertices[edge.to]);
            if score > 1.0 {
                bad_edges += 1;
            }
            let color = color_inside(Color::GREEN, Color::RED, score);
            canvas.set_draw_color(color);
            canvas.draw_line(p1, p2).unwrap();
        }

        print_text(&mut font, &mut canvas, &format!("sum errors: {}", sum_errors), 0);
        print_text(&mut font, &mut canvas, &format!("bad edges: {}", bad_edges), 30);

        canvas.present();
        for event in event_pump.poll_iter() {
            match event {
                Event::Quit { .. } |
                Event::KeyDown { keycode: Some(Keycode::Escape), .. } => {
                    break 'running;
                }
                _ => {}
            }
        }
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
        let old_p = solution.vertices[v];
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
        let mut vertices = solution.vertices.clone();
        vertices[v] = best_p;
        solution = Solution::create(vertices, &task, &helper);
    }
    return solution;
}

fn print_text(font: &mut Font, canvas: &mut Canvas<Window>, text: &str, y_shift: i32) {
// render a surface, and convert it to a texture bound to the canvas
    let surface = font
        .render(text)
        .blended(Color::BLACK)
        .map_err(|e| e.to_string()).unwrap();

    let texture_creator = canvas.texture_creator();
    let texture = texture_creator.create_texture_from_surface(&surface).unwrap();

    let TextureQuery { width, height, .. } = texture.query();
    canvas.copy(&texture, None, Some(sdl2::rect::Rect::new(0, y_shift, width, height))).unwrap();
}