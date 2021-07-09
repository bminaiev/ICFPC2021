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
    for i in 0..task.hole.len() {
        let p1 = task.hole[i];
        let p2 = task.hole[(i + 1) % task.hole.len()];
        draw_line(&mut img, &p1, &p2, BLACK);
    }
    for e in task.edges.iter() {
        let p1 = solution.vertices[e.fr];
        let p2 = solution.vertices[e.to];
        draw_line(&mut img, &p1, &p2, RED);
    }
    let font_data: &[u8] = include_bytes!("../assets/times.ttf");
    let font: Font<'static> = Font::try_from_bytes(font_data).unwrap();
    imageproc::drawing::draw_text_mut(&mut img, BLACK, 0, 0, rusttype::Scale::uniform(20.0), &font, &format!("dislikes: {}", solution.dislikes));

    img.save(path).unwrap();
}