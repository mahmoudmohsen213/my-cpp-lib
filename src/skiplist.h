#ifndef SKIPLIST_H_
#define SKIPLIST_H_

#include <cstdlib>
#include <sstream>
#include <time.h>
using namespace std;

const int SKIPLIST_MAX_LEVELS = 16;

class skiplist {
private:
  class node {
  public:
    int v;
    node** next;
    node();
    ~node();

    bool isTail();
    bool isTail(const int &level);
    string toString();
  };

  node** head;
  node* tail;
  int siz;

  static bool flipCoin();
  static string iToString(const int &v);
  static string nToString(skiplist::node *n);
public:
  skiplist();
  string toString();
  void insert(int v);
  void remove(int v);
  bool find(int v);
  int size();
  ~skiplist();
};

///////////////////////////////////////
///////////////////////////////////////

skiplist::node::node() {
  v = 0;
  next = new skiplist::node*[SKIPLIST_MAX_LEVELS];
  for (int i = 0; i < SKIPLIST_MAX_LEVELS; ++i)
    next[i] = 0;
}

skiplist::node::~node() {
  // clean up
  for (int i = 0; i < SKIPLIST_MAX_LEVELS; ++i)
    next[i] = 0;

  delete[] next;
}

bool skiplist::node::isTail() {
  return this->next[0] == 0;
}

bool skiplist::node::isTail(const int &level) {
  if (level >= SKIPLIST_MAX_LEVELS)
    throw "invalid level value";
  return this->next[level] == 0;
}

string skiplist::node::toString() {
  return ((isTail()) ? "T" : skiplist::iToString(v));
}

///////////////////////////////////////
///////////////////////////////////////

skiplist::skiplist() {
  // should be done once, but won't cause a problem if the
  // seed is initialized multiple times, but this makes the
  // class self contained, as there is no need to anything
  // in the main before using it
  srand(time(0));

  this->tail = new skiplist::node();
  this->head = new skiplist::node*[SKIPLIST_MAX_LEVELS];
  this->siz = 0;

  for (int i = 0; i < SKIPLIST_MAX_LEVELS; ++i)
    this->head[i] = this->tail;
}

bool skiplist::flipCoin() {
  return ((rand() % 100) >= 50);
}

string skiplist::iToString(const int &v) {
  stringstream ss; string s; ss << v, ss >> s; return s;
}

string skiplist::nToString(skiplist::node *n) {
  return ((n->isTail()) ? "T" : skiplist::iToString(n->v));
}

string skiplist::toString() {
  string skipliststr = "";
  for (int i = SKIPLIST_MAX_LEVELS - 1; i >= 0; --i) {
    skipliststr += "LVL" + skiplist::iToString(i) + ": ";
    skiplist::node* current = this->head[i];
    while (current)
      skipliststr += skiplist::nToString(current) + "  ", current = current->next[i];
    skipliststr += "\n";
  }

  return skipliststr;
}

void skiplist::insert(int v) {
  int currentLevel = SKIPLIST_MAX_LEVELS - 1;
  skiplist::node* current = this->head[currentLevel];
  skiplist::node* prev[SKIPLIST_MAX_LEVELS];
  for (int i = 0; i < SKIPLIST_MAX_LEVELS; ++i)
    prev[i] = 0;

  while (((current->isTail()) || (current->v > v)) && (currentLevel > 0))
    current = this->head[--currentLevel];

  // this condition means that we reached the bottom level
  // and yet pointing to the tail or a greater value, this
  // will happen if the list is empty or if all the elements
  // are greater than v, so we should insert v as the head
  // of all its levels
  if (current->isTail() || (current->v > v)) {
    skiplist::node* newNode = new node();
    ++(this->siz);
    newNode->v = v;
    newNode->next[0] = this->head[0];
    this->head[0] = newNode;
    ++currentLevel;
    while ((currentLevel < SKIPLIST_MAX_LEVELS) && skiplist::flipCoin()) {
      newNode->next[currentLevel] = this->head[currentLevel];
      this->head[currentLevel] = newNode;
      ++currentLevel;
    }

    return;
  }

  while (currentLevel >= 0) {
    while (!(current->next[currentLevel]->isTail())
        && (current->next[currentLevel]->v <= v))
      current = current->next[currentLevel];

    // here current and prev[currentLevel] points to v,
    // or the last node less than v
    prev[currentLevel] = current;

    // if current and prev[currentLevel] points to the
    // last node less than v, then in the future if v is
    // not found in any lower level we will need to update
    // prev[currentLevel], and we need to resume the search
    // in the lower level starting from current

    // however if current points to v, then the node already
    // exists, and no change in the list is needed
    if (current->v == v) return;

    --currentLevel;
  }

  skiplist::node* newNode = new node();
  ++(this->siz);
  newNode->v = v;
  newNode->next[0] = prev[0]->next[0];
  prev[0]->next[0] = newNode;
  currentLevel = 1;
  while ((currentLevel < SKIPLIST_MAX_LEVELS) && skiplist::flipCoin()) {
    // no node less than v in currentLevel,
    // then v is the new head
    if (prev[currentLevel] == 0) {
      newNode->next[currentLevel] = this->head[currentLevel];
      this->head[currentLevel] = newNode;
    }
    else {
      newNode->next[currentLevel] = prev[currentLevel]->next[currentLevel];
      prev[currentLevel]->next[currentLevel] = newNode;
    }

    ++currentLevel;
  }

  return;
}

void skiplist::remove(int v) {
  int currentLevel = SKIPLIST_MAX_LEVELS - 1;
  skiplist::node* current = this->head[currentLevel];
  skiplist::node* target = 0;
  skiplist::node* prev[SKIPLIST_MAX_LEVELS];
  bool isHead[SKIPLIST_MAX_LEVELS];

  for (int i = 0; i < SKIPLIST_MAX_LEVELS; ++i) {
    prev[i] = 0;
    isHead[i] = ((!(this->head[i]->isTail())) && (this->head[i]->v == v));
  }

  while (((current->isTail()) || (current->v > v)) && (currentLevel > 0))
    current = this->head[--currentLevel];

  // this condition means that we reached the bottom level
  // and yet pointing to the tail or a greater value, this
  // will happen if the list is empty or if all the elements
  // are greater than v, then v does not exist
  if (current->isTail() || (current->v > v))
    return;

  while (currentLevel >= 0) {
    while (!(current->next[currentLevel]->isTail())
        && (current->next[currentLevel]->v <= v)) {
      prev[currentLevel] = current;
      current = current->next[currentLevel];
    }

    // if v is not the head of the current level, then
    // here prev[currentLevel] points to the node preceding
    // v, and current points to v, or the last node less than v

    // if current points to the last one less than v, then
    // v does not exist in the current level, and we don't
    // need to update anything, so reset prev pointer to null
    if (current->v != v)
      prev[currentLevel] = 0;
    // however if current points to v, then we keep prev[currentLevel]
    // as it is to be updated, now we need to get a reference to the
    // node preceding v in the lower level as well, thats why we
    // can't restart the search in the lower level from v itself, we
    // have to restart the search in level currentLevel - 1 from the
    // node preceding v in the currentLevel, or from the head of level
    // currentLevel - 1 if v is the head of currentLevel
    else
      target = current, current =
          (isHead[currentLevel] && currentLevel > 0) ?
              this->head[currentLevel - 1] : prev[currentLevel];

    --currentLevel;
  }

  if (!target) return;

  for (int i = 0; i < SKIPLIST_MAX_LEVELS; ++i) {
    if (isHead[i])
      this->head[i] = this->head[i]->next[i];
    else if (prev[i])
      prev[i]->next[i] = prev[i]->next[i]->next[i];
  }

  if (target)
    delete target, --(this->siz);
  return;
}

bool skiplist::find(int v) {
  int currentLevel = SKIPLIST_MAX_LEVELS - 1;
  skiplist::node* current = this->head[currentLevel];
  while (((current->isTail()) || (current->v > v)) && (currentLevel > 0))
    current = this->head[--currentLevel];

  // this condition means that we reached the bottom level
  // and yet pointing to the tail or a greater value, this
  // will happen if the list is empty or if all the elements
  // are greater than v, then v does not exist
  if (current->isTail() || (current->v > v))
    return false;

  while (currentLevel >= 0) {
    while (!(current->next[currentLevel]->isTail())
        && (current->next[currentLevel]->v <= v)) {
      current = current->next[currentLevel];
    }

    if (current->v == v)
      return true;

    --currentLevel;
  }

  return false;
}

int skiplist::size() {
  return this->siz;
}

skiplist::~skiplist() {
  skiplist::node* current = this->head[0];
  skiplist::node* temp;

  // just nullifying the head pointers, not necessary
  for (int i = 0; i < SKIPLIST_MAX_LEVELS; ++i)
    this->head[i] = 0;

  // actually deleting all the nodes
  while (current) {
    temp = current;
    current = current->next[0];
    delete temp;
  }

  // deleting the head array
  delete[] this->head;

  // clean up to avoid dangling pointers, although they
  // are going to be removed from the memory anyway
  this->tail = 0;
  this->head = 0;
  this->siz = 0;
}

#endif
