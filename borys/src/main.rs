use std::fs::File;
use std::io::BufReader;
use borys::Input;

fn main() {
    for problem_id in 1..=59 {
        let file = File::open(format!("../inputs/{}.problem", problem_id)).unwrap();
        let reader = BufReader::new(file);

        let input: Input = serde_json::from_reader(reader).unwrap();

        let max_x = input.figure.vertices.iter().max_by_key(|s| s[0]).unwrap()[0];

        let max_y= input.figure.vertices.iter().max_by_key(|s| s[1]).unwrap()[1];
        dbg!(max_x, max_y, input.epsilon);
        // dbg!(input);
    }
}
