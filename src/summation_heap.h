#ifndef SUMMATION_HEAP_H_
#define SUMMATION_HEAP_H_

#include <set>
#include <unordered_map>
using namespace std;

// a data structure that offers the summation of the largest k elements in a
// set of elements in constant time. That is, given a set of elements, we
// pick the largest k elements from this set, and get their summation in
// constant time

// this data structure also offers adding and removing an element from/to the
// set of elements in logarithmic time

// requires c++11

class summation_heap {
private:
  multiset<long long> all, largest_k;
  unordered_map<long long, int> all_count_lookup, largest_k_count_lookup;
  int k, all_sum, largest_k_sum;

public:
  summation_heap(const int &k);
  summation_heap(multiset<long long> values, const int &k);

  long long get_k() const;
  long long get_sum() const;
  long long get_largest_k_sum() const;

  multiset<long long> get_all() const;
  multiset<long long> get_largest_k() const;

  void add(const long long &num);
  void remove(const long long &num);
};

///////////////////////////////////////

summation_heap::summation_heap(const int &k) {
  this->k = k, all_sum = 0, largest_k_sum = 0;
}

summation_heap::summation_heap(multiset<long long> values, const int &k) : all(values) {
  this->k = k, all_sum = 0, largest_k_sum = 0;

  for(multiset<long long>::iterator it = all.begin(); it != all.end(); ++it) {
    all_sum += (*it);
    all_count_lookup[*it]++;
  }

  multiset<long long>::reverse_iterator rit = all.rbegin();
  for(int i = 0; (i < k) && (rit != all.rend()); ++i, ++rit) {
    largest_k.insert(*rit);
    largest_k_count_lookup[*rit]++;
    largest_k_sum += (*rit);
  }
}

long long summation_heap::get_k() const {
  return k;
}

long long summation_heap::get_sum() const {
  return all_sum;
}

long long summation_heap::get_largest_k_sum() const {
  return largest_k_sum;
}

multiset<long long> summation_heap::get_all() const {
  return all;
}

multiset<long long> summation_heap::get_largest_k() const {
  return largest_k;
}

void summation_heap::add(const long long &num) {
  if(((int)largest_k.size() < k) && (all.size() > largest_k.size()))
      throw "invalid_state";

  all.insert(num);
  all_count_lookup[num]++;
  all_sum += num;

  if(k ==0) return;

  if((int)largest_k.size() < k) {
    largest_k.insert(num);
    largest_k_count_lookup[num]++;
    largest_k_sum += num;
  }
  else{
    multiset<long long>::iterator largest_k_it = largest_k.begin();
    if(num > (*largest_k_it)){
      largest_k_sum -= (*largest_k_it);
      largest_k_count_lookup[*largest_k_it]--;
      largest_k.erase(largest_k_it);

      largest_k.insert(num);
      largest_k_count_lookup[num]++;
      largest_k_sum += num;
    }
  }
}

void summation_heap::remove(const long long &num) {
  multiset<long long>::iterator all_it, largest_k_it;

  all_it = all.find(num);
  if(all_it == all.end()) return;

  all.erase(all_it);
  all_count_lookup[num]--;
  all_sum -= num;

  if(k == 0) return;

  if(all_count_lookup[num] < largest_k_count_lookup[num]){
    largest_k_it = largest_k.find(num);
    if(largest_k_it != largest_k.end()) {
      largest_k.erase(largest_k_it);
      largest_k_count_lookup[num]--;
      largest_k_sum -= num;
    }
  }

  if(((int)largest_k.size() < k) && (all.size() > largest_k.size())){
    largest_k_it = largest_k.begin();
    if(all_count_lookup[*largest_k_it] > largest_k_count_lookup[*largest_k_it]) {
      largest_k.insert(*largest_k_it);
      largest_k_count_lookup[*largest_k_it]++;
      largest_k_sum += (*largest_k_it);
    }
    else {
      all_it = all.lower_bound(*largest_k_it), --all_it;
      largest_k.insert(*all_it);
      largest_k_count_lookup[*all_it]++;
      largest_k_sum += (*all_it);
    }
  }
}

#endif /* SUMMATION_HEAP_H_ */
