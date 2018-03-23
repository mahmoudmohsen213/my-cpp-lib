#ifndef RBTREE_H_
#define RBTREE_H_

#include <tuple>
#include <sstream>
using namespace std;

// require c++11
class rbtree {
private:
  enum color {
    RED, BLACK, DOUBLE_BLACK
  };

  class node {
  public:
    int v;
    rbtree::color c;
    rbtree::node* p;
    rbtree::node* l;
    rbtree::node* r;

    node();
    node(int);
    ~node();
    string to_string();
    string int_to_string(const int &v);
    string color_to_string(const rbtree::color &c);
  };

  rbtree::node* root;

  // helper functions
  rbtree::color get_color(rbtree::node* n);
  rbtree::node* get_sibling(rbtree::node* n);
  rbtree::node* get_sibling(rbtree::node* parent, rbtree::node* n);
  rbtree::node* get_uncle(rbtree::node* n);
  rbtree::node* get_grand_parent(rbtree::node* n);

  rbtree::node* start_rotation(rbtree::node* top, rbtree::node* mid, rbtree::node* bottom);
  rbtree::node* rotate_left(rbtree::node* n);
  rbtree::node* rotate_right(rbtree::node* n);

  rbtree::node* find_reference(int v);

  rbtree::node* insert_leaf(int v);
  tuple<rbtree::node*, rbtree::node*, rbtree::color> erase_node(rbtree::node* n);
  void rebalance_red(rbtree::node* n);
  void rebalance_double_black(rbtree::node* parent, rbtree::node* n);

  string to_string(rbtree::node* n, string indent, const int &position, const bool &last);
public:
  // public interface
  rbtree();
  ~rbtree();
  void clear();
  void insert(int v);
  void erase(int v);
  bool find(int v);
  string to_string();
};

///////////////////////////////////////
///////////////////////////////////////
// node public interface
rbtree::node::node() {
  this->v = 0;
  this->c = rbtree::RED;
  this->p = NULL;
  this->l = NULL;
  this->r = NULL;
}

rbtree::node::node(int v) {
  this->v = v;
  this->c = rbtree::RED;
  this->p = NULL;
  this->l = NULL;
  this->r = NULL;
}

rbtree::node::~node() {
  // cout << "rbtree::node::~node()" << endl;
  this->p = NULL;
  delete this->l;
  delete this->r;
  this->l = NULL;
  this->r = NULL;
}

string rbtree::node::to_string() {
  return this->int_to_string(this->v) + "_" + this->color_to_string(this->c);
}

string rbtree::node::int_to_string(const int &v) {
  stringstream ss; string s; ss << v, ss >> s; return s;
}

string rbtree::node::color_to_string(const rbtree::color &c) {
  return ((c == rbtree::RED) ? "R" : "B");
}

///////////////////////////////////////
// helper functions

rbtree::color rbtree::get_color(rbtree::node* n) {
  if (n == NULL)
    return rbtree::BLACK;
  return n->c;
}

rbtree::node* rbtree::get_sibling(rbtree::node* n) {
  if (n == NULL)
    return NULL;
  if (n->p == NULL)
    return NULL;

  if (n->p->l == n)
    return n->p->r;
  if (n->p->r == n)
    return n->p->l;
  return NULL;
}

rbtree::node* rbtree::get_sibling(rbtree::node* parent, rbtree::node* n) {
  if (parent == NULL)
    return NULL;

  if (parent->l == n)
    return parent->r;
  if (parent->r == n)
    return parent->l;
  return NULL;
}

rbtree::node* rbtree::get_uncle(rbtree::node* n) {
  if (n == NULL)
    return NULL;

  return this->get_sibling(n->p);
}

rbtree::node* rbtree::get_grand_parent(rbtree::node* n) {
  if (n == NULL)
    return NULL;

  if (n->p == NULL)
    return NULL;

  return n->p->p;
}

rbtree::node* rbtree::start_rotation(rbtree::node* top, rbtree::node* mid, rbtree::node* bottom) {
  // this function examines the configuration of the three provided
  // nodes, and determine the proper set of rotations according to
  // the four cases, then it returns a reference to the new top
  // of the three provided nodes after all rotations

  if(top == NULL)
    throw "ERROR at: rbtree::start_rotation(rbtree::node* top, rbtree::node* mid, rbtree::node* bottom): the first argument 'top' can not be null";

  if(mid == NULL)
    throw "ERROR at: rbtree::start_rotation(rbtree::node* top, rbtree::node* mid, rbtree::node* bottom): the second argument 'mid' can not be null";

  if(mid == NULL)
    throw "ERROR at: rbtree::start_rotation(rbtree::node* top, rbtree::node* mid, rbtree::node* bottom): the third argument 'bottom' can not be null";

  rbtree::node* new_top = NULL;

  // left left case
  if(top->l == mid && mid->l == bottom) {
    new_top = this->rotate_right(top);
  }

  // right right case
  if(top->r == mid && mid->r == bottom) {
    new_top = this->rotate_left(top);
  }

  // left right case
  if(top->l == mid && mid->r == bottom) {
    this->rotate_left(mid);
    new_top = this->rotate_right(top);
  }

  // right left case
  if(top->r == mid && mid->l == bottom) {
    this->rotate_right(mid);
    new_top = this->rotate_left(top);
  }

  return new_top;
}

rbtree::node* rbtree::rotate_left(rbtree::node* n){
  // n points to the top node in the rotated nodes

  if(n == NULL)
    throw "ERROR at: rbtree::rotate_left(rbtree::node* n): the first argument can not be null";

  rbtree::node* parent = n->p;
  rbtree::node* right_child = n->r;

  n->r = right_child->l;
  if(right_child->l != NULL)
    right_child->l->p = n;

  right_child->l = n;
  n->p = right_child;

  if(parent != NULL) {
    if(parent->l == n) parent->l = right_child;
    else if(parent->r == n) parent->r = right_child;
  }
  right_child->p = parent;

  if(this->root == n) this->root = right_child;
  return right_child;
}

rbtree::node* rbtree::rotate_right(rbtree::node* n){
  // n points to the top node in the rotated nodes

  if(n == NULL)
    throw "ERROR at: rbtree::rotate_right(rbtree::node* n): the first argument can not be null";

  rbtree::node* parent = n->p;
  rbtree::node* left_child = n->l;

  n->l = left_child->r;
  if(left_child->r != NULL)
    left_child->r->p = n;

  left_child->r = n;
  n->p = left_child;

  if(parent != NULL) {
    if(parent->l == n) parent->l = left_child;
    else if(parent->r == n) parent->r = left_child;
  }
  left_child->p = parent;

  if(this->root == n) this->root = left_child;
  return left_child;
}

rbtree::node* rbtree::find_reference(int v) {
  rbtree::node* n = this->root;
  while (n != NULL) {
    if (n->v == v) return n;
    n = ((v < n->v) ? n->l : n->r);
  }

  return NULL;
}

rbtree::node* rbtree::insert_leaf(int v) {
  if (this->root == NULL)
    return this->root = new rbtree::node(v);

  rbtree::node* parent = NULL;
  rbtree::node* current = this->root;
  while (current != NULL) {
    if (v == current->v)
      return NULL;

    parent = current;
    current = ((v < current->v) ? current->l : current->r);
  }

  rbtree::node* new_node = new rbtree::node();
  new_node->v = v;
  new_node->p = parent;

  return ((v < parent->v) ?
      (parent->l = new_node) :
      (parent->r = new_node));
}

tuple<rbtree::node*, rbtree::node*, rbtree::color> rbtree::erase_node(rbtree::node* n) {
  // returns a tuple containing a reference to the parent of the new
  // node that replaced the actually deleted node, a reference to the
  // new node, and the color of the new node. the deleted node may be
  // the provided node, or a substitute node if the provided node has
  // two children. this is done as the caller need to know about the
  // double black node to start rebalancing, regardless of which node
  // was actually deleted. the function also colors the new node with
  // the proper color. if the deleted node had no children, this
  // function will return NULL in the second place of the tuple, and
  // the color in the third place will be double black to signal the
  // caller to deal with a double black situation with no reference
  // to a new node

  if(n == NULL)
    throw "ERROR at: rbtree::erase_node(rbtree::node* n): the first argument can not be null";

  rbtree::node* parent = n->p;
  rbtree::node* new_node = NULL;
  rbtree::color new_node_color = rbtree::DOUBLE_BLACK;

  bool is_root = (parent == NULL);

  if(n->l == NULL && n->r == NULL) {
    if(parent != NULL) {
      if(parent->l == n) parent->l = NULL;
      else if(parent->r == n) parent->r = NULL;
    }

    if(n->c == rbtree::RED)
      new_node_color = rbtree::BLACK;
    else new_node_color = rbtree::DOUBLE_BLACK;

    new_node = NULL;
  }
  else if(n->l == NULL) {
    rbtree::node* right_child = n->r;

    if(parent != NULL) {
      if(parent->l == n) parent->l = right_child;
      else if(parent->r == n) parent->r = right_child;
    }

    if(n->c == rbtree::RED || right_child->c == rbtree::RED)
      right_child->c = new_node_color = rbtree::BLACK;
    else right_child->c = new_node_color = rbtree::DOUBLE_BLACK;

    right_child->p = parent;
    new_node = right_child;
  }
  else if(n->r == NULL) {
    rbtree::node* left_child = n->l;

    if(parent != NULL) {
      if(parent->l == n) parent->l = left_child;
      else if(parent->r == n) parent->r = left_child;
    }

    if(n->c == rbtree::RED || left_child->c == rbtree::RED)
      left_child->c = new_node_color = rbtree::BLACK;
    else left_child->c = new_node_color = rbtree::DOUBLE_BLACK;

    left_child->p = parent;
    new_node = left_child;
  }
  else {
    // always replacing with the leftmost node in the right subtree
    rbtree::node* left_most = n->r;
    while(left_most->l != NULL)
      left_most = left_most->l;

    n->v = left_most->v;
    return this->erase_node(left_most);
  }

  if(is_root) this->root = new_node;

  n->p = NULL;
  n->l = NULL;
  n->r = NULL;
  delete n;
  return make_tuple(parent, new_node, new_node_color);
}

void rbtree::rebalance_red(rbtree::node* n) {
  if(n == NULL)
    return;

  if(this->root == n) {
    this->root->c = rbtree::BLACK;
    return;
  }

  if(this->get_color(n) == rbtree::BLACK)
    return;

  if(this->get_color(n->p) == rbtree::BLACK)
    return;

  rbtree::node* parent = n->p;
  rbtree::node* grand_parent = this->get_grand_parent(n);
  rbtree::node* uncle = this->get_uncle(n);
  rbtree::color uncle_color = this->get_color(uncle);

  if(uncle != NULL && uncle_color == rbtree::RED) {
    parent->c = rbtree::BLACK;
    uncle->c = rbtree::BLACK;
    grand_parent->c = rbtree::RED;
    this->rebalance_red(grand_parent);
    return;
  }
  else {
    rbtree::node* new_top = this->start_rotation(grand_parent, parent, n);
    new_top->c = rbtree::BLACK;
    if(new_top->l != NULL) new_top->l->c = rbtree::RED;
    if(new_top->r != NULL) new_top->r->c = rbtree::RED;
    return;
  }
}

void rbtree::rebalance_double_black(rbtree::node* parent, rbtree::node* n) {
  // this function takes a double black node n, and its parent
  // and work to fix the double black situation

  // tree is empty
  if(parent == NULL && n == NULL)
    return;

  // n is root
  if(parent == NULL) {
    n->c = rbtree::BLACK;
    return;
  }

  // this function should be called only if n is double black, and
  // will always return with n as black
  if(n != NULL) n->c = rbtree::BLACK;

  rbtree::node* sibling = this->get_sibling(parent, n);
  rbtree::color sibling_color = this->get_color(sibling);

  // no sibling, same as black sibling with two black children
  // we recolor the parent only, though this case should not happen
  if(sibling == NULL) {
    if(parent->c == rbtree::RED) {
      parent->c = rbtree::BLACK;
      return;
    }
    else {
      parent->c = rbtree::DOUBLE_BLACK;
      return this->rebalance_double_black(parent->p, parent);
    }
  }
  else if(sibling_color == rbtree::BLACK) {
    rbtree::color left_nephew_color = this->get_color(sibling->l);
    rbtree::color right_nephew_color = this->get_color(sibling->r);
    if(left_nephew_color == rbtree::BLACK && right_nephew_color == rbtree::BLACK) {
      sibling->c = rbtree::RED;
      if(parent->c == rbtree::RED) {
        parent->c = rbtree::BLACK;
        return;
      }
      else {
        parent->c = rbtree::DOUBLE_BLACK;
        return this->rebalance_double_black(parent->p, parent);
      }
    }
    else if(left_nephew_color == rbtree::RED) {
      rbtree::node* new_top = this->start_rotation(parent, sibling, sibling->l);
      new_top->c = rbtree::BLACK;
      if(new_top->l != NULL) new_top->l->c = rbtree::BLACK;
      if(new_top->r != NULL) new_top->r->c = rbtree::BLACK;
      return;
    }
    else if(right_nephew_color == rbtree::RED) {
      rbtree::node* new_top = this->start_rotation(parent, sibling, sibling->r);
      new_top->c = rbtree::BLACK;
      if(new_top->l != NULL) new_top->l->c = rbtree::BLACK;
      if(new_top->r != NULL) new_top->r->c = rbtree::BLACK;
      return;
    }
  }
  else if(sibling_color == rbtree::RED) {
    // we need to move the sibling up, so we must know which child
    // the sibling is to its parent
    if(sibling == parent->l)
      this->rotate_right(parent);
    else if(sibling == parent->r)
      this->rotate_left(parent);

    sibling->c = rbtree::BLACK;
    parent->c = rbtree::RED;
    if(n != NULL) n->c = rbtree::DOUBLE_BLACK;
    return this->rebalance_double_black(parent, n);
  }
}

string rbtree::to_string(rbtree::node* n, string indent, const int &position, const bool &last) {
  string s = indent;
  if (last) s += "--", indent += "  ";
  else s += "|-", indent += "| ";

  const int LEFT = 101;
  const int RIGHT = 102;

  if (position == LEFT) s += "L_" + n->to_string();
  else if (position == RIGHT) s += "R_" + n->to_string();
  else s += n->to_string();
  s+='\n';

  if (n->l != NULL) s += this->to_string(n->l, indent, LEFT, (n->r == NULL));
  if (n->r != NULL) s += this->to_string(n->r, indent, RIGHT, true);

  return s;
}

///////////////////////////////////////
// public interface

rbtree::rbtree() {
  this->root = NULL;
}

rbtree::~rbtree() {
  this->clear();
}

void rbtree::clear() {
  delete this->root;
  this->root = NULL;
}

void rbtree::insert(int v) {
  rbtree::node* inserted_node = this->insert_leaf(v);
  this->rebalance_red(inserted_node);
  return;
}

void rbtree::erase(int v) {
  rbtree::node* target_node = this->find_reference(v);
  if(target_node == NULL) return;

  auto erase_tuple = this->erase_node(target_node);
  rbtree::color new_node_color = get<2>(erase_tuple);
  if(new_node_color == rbtree::DOUBLE_BLACK)
    this->rebalance_double_black(get<0>(erase_tuple), get<1>(erase_tuple));
  return;
}

bool rbtree::find(int v) {
  return (this->find_reference(v) != NULL);
}

string rbtree::to_string() {
  return ((this->root == NULL) ? "" : this->to_string(this->root, "", 0, true));
}

#endif /* RBTREE_H_ */
