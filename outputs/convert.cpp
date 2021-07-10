/**
 *    author:  tourist
 *    created: 10.07.2021 05:37:22       
**/
#include <bits/stdc++.h>

using namespace std;

int main(int argc, char** argv) {
  ios::sync_with_stdio(false);
  cin.tie(0);
  assert(argc == 2);
  string test = argv[1];
  ifstream in(test + ".ans");
  string s;
  in >> s;
  in.close();
  if (s[0] == '{') {
    stringstream ss;
    for (char c : s) if (isdigit(c)) ss << c; else ss << " ";
    vector<int> a;
    int x;
    while (ss >> x) a.push_back(x);
    assert(a.size() % 2 == 0);
    ofstream out(test + ".ans");
    out << a.size() / 2 << '\n';
    for (int i = 0; i < (int) a.size(); i += 2) out << a[i] << " " << a[i + 1] << '\n';
    out.close();
  }
  return 0;
}
