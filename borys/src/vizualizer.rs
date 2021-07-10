use sdl2::pixels::Color;
use crate::{Task, Solution, Point, Shift};
use crate::helper::Helper;
use sdl2::render::{WindowCanvas, Canvas, TextureQuery};
use sdl2::event::Event;
use sdl2::keyboard::Keycode;
use sdl2::ttf::{Font, Sdl2TtfContext};
use sdl2::video::Window;
use sdl2::EventPump;

pub struct Visualizer<'ttf> {
    canvas: WindowCanvas,
    event_pump: EventPump,
    font: Font<'ttf, 'static>,
}


const ZOOM: i32 = 10;

fn color_inside(from: Color, to: Color, mut part: f64) -> Color {
    // if part > 1.0 {
    //     part = 1.0;
    // }
    // if part < 0.0 {
    //     part = 0.0;
    // }
    // let mid = |x: u8, y: u8| -> u8 {
    //     ((x as f64) * (1.0 - part) + (y as f64) * part) as u8
    // };
    // Color::RGB(mid(from.r, to.r), mid(from.g, to.g), mid(from.b, to.b))
    if part > 1.0 {
        to
    } else {
        from
    }
}

fn calc_sum_errors(task: &Task, helper: &Helper, solution: &Solution) -> f64 {
    let mut res = 0.0;
    for edge in task.edges.iter() {
        res += helper.edge_score(&task, edge.fr, edge.to, &solution.vertices[edge.fr], &solution.vertices[edge.to]);
    }
    return res;
}

const GREY: Color = Color::RGB(222u8, 222u8, 222u8);

pub enum UserEvent {
    IncreaseChangeProb,
    MouseMoved(i32, i32),
    CloseApp,
    Selected(usize),
    Shift(Shift),
}

pub struct AdditionalState {
    pub mouse_x: i32,
    pub mouse_y: i32,
    pub selected: Option<usize>,
}

impl AdditionalState {
    pub fn create() -> Self {
        Self { mouse_x: 0, mouse_y: 0, selected: None }
    }
}

impl<'a> Visualizer<'a> {
    pub fn create(helper: &Helper, ttf_context: &'a Sdl2TtfContext) -> Self {
        let sdl_context = sdl2::init().unwrap();
        let video_subsystem = sdl_context.video().unwrap();

        let display = video_subsystem.display_bounds(1).unwrap();

        let window = video_subsystem.window("rust-sdl2 demo", (helper.max_c * ZOOM) as u32, (helper.max_c * ZOOM) as u32)
            .position(display.x + 1000, display.y + 400)
            .build()
            .unwrap();

        let canvas = window.into_canvas().build().unwrap();
        let event_pump = sdl_context.event_pump().unwrap();

        let font = ttf_context.load_font("assets/times.ttf", 20).unwrap();

        Self { canvas, event_pump, font }
    }

    fn conv_point(p: &Point) -> sdl2::rect::Point {
        sdl2::rect::Point::new(p.x * ZOOM, p.y * ZOOM)
    }

    pub fn render(&mut self, task: &Task, helper: &Helper, solution: &Solution, generation: i64, state: Option<&AdditionalState>) -> Vec<UserEvent> {
        self.canvas.set_draw_color(Color::WHITE);
        self.canvas.clear();

        let sum_errors = calc_sum_errors(&task, &helper, &solution);


        self.canvas.set_draw_color(GREY);
        for x in 0..helper.max_c {
            self.canvas.draw_line(Self::conv_point(&Point { x, y: 0 }), Self::conv_point(&Point { x, y: helper.max_c as i32 })).unwrap();
        }
        for y in 0..helper.max_c {
            self.canvas.draw_line(Self::conv_point(&Point { x: 0, y }), Self::conv_point(&Point { x: helper.max_c as i32, y })).unwrap();
        }
        self.canvas.set_draw_color(Color::BLACK);
        for edge in helper.hole_and_first.windows(2) {
            let p1 = Self::conv_point(&edge[0]);
            let p2 = Self::conv_point(&edge[1]);
            self.canvas.draw_line(p1, p2).unwrap();
        }
        let mut bad_edges = 0;
        for edge in task.edges.iter() {
            let p1 = Self::conv_point(&solution.vertices[edge.fr]);
            let p2 = Self::conv_point(&solution.vertices[edge.to]);
            let score = helper.edge_score(&task, edge.fr, edge.to, &solution.vertices[edge.fr], &solution.vertices[edge.to]);
            if score > 1.0 {
                bad_edges += 1;
            }
            let color = color_inside(Color::GREEN, Color::RED, score);
            self.canvas.set_draw_color(color);
            self.canvas.draw_line(p1, p2).unwrap();
        }

        const Y_SHIFT: i32 = 30;
        Self::print_text(&mut self.font, &mut self.canvas, &format!("sum errors: {}", sum_errors), 0);
        Self::print_text(&mut self.font, &mut self.canvas, &format!("bad edges: {}", bad_edges), Y_SHIFT * 1);
        Self::print_text(&mut self.font, &mut self.canvas, &format!("dislikes: {}", solution.dislikes), Y_SHIFT * 2);
        Self::print_text(&mut self.font, &mut self.canvas, &format!("generation: {}", generation), Y_SHIFT * 3);
        Self::print_text(&mut self.font, &mut self.canvas, &format!("crossed edges: {}", solution.crossed_edges), Y_SHIFT * 4);

        let closest_point = |x: i32, y: i32| {
            let mut closest_point = 0;
            let mut best_d2 = std::i32::MAX;
            for v in 0..solution.vertices.len() {
                let p = Self::conv_point(&solution.vertices[v]);
                let dx2 = (p.x - x) * (p.x - x);
                let dy2 = (p.y - y) * (p.y - y);
                let d2 = dx2 + dy2;
                if d2 < best_d2 {
                    best_d2 = d2;
                    closest_point = v;
                }
            }
            if best_d2 < 100 * 100 {
                return Some(closest_point);
            }
            return None;
        };

        match state {
            None => {}
            Some(state) => {
                Self::print_text(&mut self.font, &mut self.canvas, &format!("mouse: {} {}", state.mouse_x, state.mouse_y), Y_SHIFT * 5);
                match closest_point(state.mouse_x, state.mouse_y) {
                    None => {}
                    Some(closest_point) => {
                        let p = Self::conv_point(&solution.vertices[closest_point]);
                        self.canvas.set_draw_color(Color::BLACK);
                        let s = 3i32;
                        self.canvas.fill_rect(sdl2::rect::Rect::new(p.x - s, p.y - s, (s * 2) as u32, (s * 2) as u32)).unwrap();
                    }
                }
                match state.selected {
                    None => {}
                    Some(v) => {
                        let p = Self::conv_point(&solution.vertices[v]);
                        self.canvas.set_draw_color(Color::RED);
                        let s = 3i32;
                        self.canvas.fill_rect(sdl2::rect::Rect::new(p.x - s, p.y - s, (s * 2) as u32, (s * 2) as u32)).unwrap();
                    }
                }
            }
        }


        self.canvas.present();
        let mut events = vec![];
        for event in self.event_pump.poll_iter() {
            match event {
                Event::Quit { .. } |
                Event::KeyDown { keycode: Some(Keycode::Right), .. } => {
                    events.push(UserEvent::IncreaseChangeProb);
                    events.push(UserEvent::Shift(Shift { dx: 1, dy: 0 }));
                }
                Event::KeyDown { keycode: Some(Keycode::Left), .. } => {
                    events.push(UserEvent::Shift(Shift { dx: -1, dy: 0 }));
                }
                Event::KeyDown { keycode: Some(Keycode::Down), .. } => {
                    events.push(UserEvent::Shift(Shift { dx: 0, dy: 1 }));
                }
                Event::KeyDown { keycode: Some(Keycode::Up), .. } => {
                    events.push(UserEvent::Shift(Shift { dx: 0, dy: -1 }));
                }
                Event::MouseMotion {
                    x,
                    y,
                    ..
                } => {
                    events.push(UserEvent::MouseMoved(x, y))
                }
                Event::KeyDown { keycode: Some(Keycode::Escape), .. } => {
                    events.push(UserEvent::CloseApp)
                }
                Event::MouseButtonUp { x, y, .. } => {
                    match closest_point(x, y) {
                        None => {}
                        Some(p) => {
                            events.push(UserEvent::Selected(p));
                        }
                    }
                }
                _ => {}
            }
        }
        return events;
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
}