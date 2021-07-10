use borys::{load_test, load_submission, Point};

fn main() {
    for test_id in 1..88 {
        let task = load_test(test_id);
        let submit_vertices = load_submission(&format!("../download_outputs/{}.ans", test_id));

        for bonus in task.bonuses.iter() {
            let bonus_p = Point { x: bonus.position[0], y: bonus.position[1] };
            for v in submit_vertices.iter() {
                if *v == bonus_p {
                    println!("wow! in test {}, we have point {:?}. Can use bonus {} for solving test {}", test_id, v, bonus.bonus, bonus.problem);
                }
            }
        }
    }
}