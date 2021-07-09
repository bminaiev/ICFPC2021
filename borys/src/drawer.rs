use crate::*;
use crate::helper::*;

extern crate image;
extern crate imageproc;

use image::{Rgb, RgbImage};
use imageproc::drawing::draw_line_segment_mut;
use self::imageproc::rect::Rect;
use rusttype::Font;

const C: i32 = 8;

const BLACK: Rgb<u8> = Rgb([0u8, 0u8, 0u8]);
const RED: Rgb<u8> = Rgb([255u8, 0u8, 0u8]);
const GREEN: Rgb<u8> = Rgb([0u8, 255u8, 0u8]);
const GREY: Rgb<u8> = Rgb([192u8, 192u8, 192u8]);

fn color_inside(from: Rgb<u8>, to: Rgb<u8>, part: f64) -> Rgb<u8> {
    let mid = |x: u8, y: u8| -> u8 {
        ((x as f64) * (1.0 - part) + (y as f64) * part) as u8
    };
    Rgb([mid(from.0[0], to.0[0]), mid(from.0[1], to.0[1]), mid(from.0[2], to.0[2])])
}

fn draw_line(img: &mut RgbImage, p1: &Point, p2: &Point, color: Rgb<u8>) {
    draw_line_segment_mut(
        img,
        ((p1.x * C) as f32, (p1.y * C) as f32),
        ((p2.x * C) as f32, (p2.y * C) as f32),
        color, // RGB colors
    );
}

pub fn save_test(task: &Task, solution: &Solution, path: &str) {
    let helper = Helper::create(task);

    let sz = helper.max_c * C;

    let mut img = RgbImage::new(sz as u32, sz as u32);
    imageproc::drawing::draw_filled_rect_mut(&mut img, Rect::at(0, 0).of_size(sz as u32, sz as u32), Rgb([255u8, 255u8, 255u8]));
    for x in 0..helper.max_c {
        draw_line(&mut img, &Point { x, y: 0 }, &Point { x, y: helper.max_c as i32 }, GREY);
    }
    for y in 0..helper.max_c {
        draw_line(&mut img, &Point { x: 0, y }, &Point { x: helper.max_c as i32, y }, GREY);
    }
    for i in 0..task.hole.len() {
        let p1 = task.hole[i];
        let p2 = task.hole[(i + 1) % task.hole.len()];
        draw_line(&mut img, &p1, &p2, BLACK);
    }
    for e in task.edges.iter() {
        let p1 = solution.vertices[e.fr];
        let p2 = solution.vertices[e.to];
        let score = helper.edge_score(task, e.fr, e.to, &p1, &p2);
        let color = color_inside(GREEN, RED, score);
        draw_line(&mut img, &p1, &p2, color);
    }
    let font_data: &[u8] = include_bytes!("../assets/times.ttf");
    let font: Font<'static> = Font::try_from_bytes(font_data).unwrap();
    imageproc::drawing::draw_text_mut(&mut img, BLACK, 0, 0, rusttype::Scale::uniform(20.0), &font, &format!("dislikes: {}", solution.dislikes));

    img.save(path).unwrap();
}