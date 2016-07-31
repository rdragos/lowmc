#include "Grundy.h"
	
vector<deque<int>> Grundy :: SolveGrundy() {
  states = Rec(n);
  vector<int> states_int;
  for (auto deq: states) {
    int conf = 0;
    for (int i = 0; i < deq.size(); ++i) {
      conf += (1 << i) * (deq[i]);
    }
    states_int.push_back(conf);
  }

  for (int i = 1; i < states_int.size(); ++i) {
    int rconf = states_int[i] ^ (states_int[i - 1]);
    int cur_dif = 0;

    for (int j = 0; j < n; ++j) {
      if (rconf & (1 << j)) {
        cur_dif = j;
      }
    }
    changes.push_back(cur_dif);
    ids[states_int[i]] = i;
  }
  return states;
}
const int Grundy::getId(int x) {
  return ids[x];
}
vector<deque<int>> Grundy :: Rec(int k) {
	if (k == 1) {
		deque<int> v1 = {0};
		deque<int> v2 = {1};
		return vector<deque<int>>{v1, v2};
	} else {
    vector<deque<int>> tmp = Rec(k - 1);
    vector<deque<int>> answer;

    for (auto v: tmp) {
      answer.push_back(v);
      answer.back().push_front(0);
    }	
    reverse(tmp.begin(), tmp.end());
    for (auto v: tmp) {
      answer.push_back(v);
      answer.back().push_front(1);
    }
    return answer;
	}
}

vector<int> Grundy :: GetChanges() {
  return changes;
}