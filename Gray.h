#ifndef __Gray_h__
#define __Gray_h__

#include <vector>
#include <deque>
#include <algorithm>

using namespace std;

class Gray {
 public:
  Gray(int _n) {
    n = _n;
    ids.assign(1<<n, 0);
  }

  int getSize() {
    return n;
  }
  vector<deque<int>> SolveGray();
  vector<int> GetChanges();
  const int getId(int x);
 private:
  vector<deque<int>> Rec(int k);
  vector<deque<int>> states;
  vector<int> changes;
  int n;
  vector<int> ids;
};

#endif
