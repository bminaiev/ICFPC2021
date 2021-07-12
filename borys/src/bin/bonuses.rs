use borys::{load_test, load_submission, Point};
use std::cmp::min;

fn main() {
    let mut can_use_globalist = vec![];

    for test_id in 1..=132 {
        let task = load_test(test_id);
        // let submit_vertices = load_submission(&format!("../download_outputs/{}.ans", test_id));


        for bonus in task.bonuses.iter() {
            let mut min_d2 = std::i64::MAX;
            let bonus_p = Point { x: bonus.position[0], y: bonus.position[1] };
            // for v in submit_vertices.iter() {
            //     min_d2 = min(min_d2, v.d2(&bonus_p));
            //     if *v == bonus_p {
            //         println!("wow! in test {}, we have point {:?}. Can use bonus {} for solving test {}", test_id, v, bonus.bonus, bonus.problem);
            // }
            // }
            // println!("for test {}, d2 to closest point: {}", test_id, min_d2);
            if bonus.bonus == "GLOBALIST" {
                println!("can get bonus {:?} if solve cool task: {}", bonus, test_id);
                can_use_globalist.push(bonus.problem);
            }
        }
    }
    can_use_globalist.sort();
    for x in can_use_globalist.iter() {
        println!("{}", x);
    }
}