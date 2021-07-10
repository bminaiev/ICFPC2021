#[allow(dead_code)]
pub struct Random {
    state: usize
}

impl Random {
    fn next(&mut self) -> usize {
        let mut x = self.state;
        x ^= x << 13;
        x ^= x >> 7;
        x ^= x << 17;
        self.state = x;
        x
    }

    #[allow(dead_code)]
    pub fn next_in_range(&mut self, from: usize, to: usize) -> usize {
        assert!(from < to);
        from + self.next() % (to - from)
    }

    #[allow(dead_code)]
    pub fn next_double(&mut self) -> f64 {
        (self.next() as f64) / (std::usize::MAX as f64)
    }

    #[allow(dead_code)]
    pub fn new(seed: usize) -> Self {
        assert_ne!(seed, 0);
        Self {
            state: seed,
        }
    }

    pub fn gen_perm(&mut self, n: usize) -> Vec<usize> {
        let mut res: Vec<_> = (0..n).collect();
        for i in 1..n {
            let pos = self.next_in_range(0, i + 1);
            res.swap(i, pos);
        }
        return res;
    }

}
