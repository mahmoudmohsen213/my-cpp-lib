#include <cstdlib>
using namespace std;

template<class type>
class tree{
private:
  class node{
  public:
    node *left;
    node *right;
    type value;
    int height;
    node();
    node(node *_left, node *_right, type _value);
    int getHeight(node *n);
    int getBalance();
  };

  node *root;

  node* rotateLeft(node *n);
  node* rotateRight(node *n);
  node* insert(type value, node* current);
  node* erase(type value, node* current);
public:
  tree();
  ~tree();
  void insert(type value);
  void erase(type value);
  bool find(type value);
  void clear(node *current);
};

///////////////////////////////////////
template<class type>
tree<type>::node::node(){
  left = NULL;
  right = NULL;
  height = 0;
}

template<class type>
tree<type>::node::node(node *_left, node *_right, type _value){
  left = _left;
  right = _right;
  value = _value;
  height = max(getHeight(left),getHeight(right))+1;
}

template<class type>
int tree<type>::node::getHeight(node *n){
  if(n == NULL)
    return 0;
  return n->height;
}

template<class type>
int tree<type>::node::getBalance(){
  return left->height - right->height;
}

///////////////////////////////////////

template<class type>
tree<type>::tree(){
  root = NULL;
}

template<class type>
tree<type>::~tree(){
  this->clear(root);
  root = NULL;
}

template<class type>
typename tree<type>::node* tree<type>::rotateLeft(node *n){
  node *rightChild = n->right;
  node *leftGrandChild = rightChild->left;

  rightChild->left = n;
  n->right = leftGrandChild;

  n->height = max(n->getHeight(left),
            n->getHeight(right))+1;
  rightChild->height = max(rightChild->getHeight(left),
                rightChild->getHeight(right))+1;

  return rightChild;
}

template<class type>
typename tree<type>::node* tree<type>::rotateRight(node *n){
  node *leftChild = n->left;
  node *rightGrandChild = leftChild->right;

  leftChild->right = n;
  n->left = rightGrandChild;

  n->height = max(n->getHeight(left),
              n->getHeight(right))+1;
  leftChild->height = max(leftChild->getHeight(left),
                leftChild->getHeight(right))+1;

  return leftChild;
}

template<class type>
void tree<type>::insert(type value){
  if(root == NULL)
    root = new node(NULL,NULL,value);
  else root = insert(value,root);
}

template<class type>
typename tree<type>::node* tree<type>::insert(type value,
                      node *current){
  if(current == NULL)
    return new node(NULL,NULL,value);
  if(value == current->value)
    return current;

  if(value < current->value)
    current->left = insert(value, current->left);
  else current->right = insert(value, current->right);

  current->height = max(current->getHeight(left),
              current->getHeight(right))+1;

  int balance = current->getBalance();

  // Left Left Case
  if(balance > 1 && value < current->left->value)
    return rotateRight(current);
  // Right Right Case
  else if (balance < -1 && value > current->right->value)
    return rotateLeft(current);
  // Left Right Case
  else if (balance > 1 && value > current->left->value){
    current->left =  rotateLeft(current->left);
    return rotateRight(current);
  }
  // Right Left Case
  else if (balance < -1 && value < current->right->value){
    current->right = rotateRight(current->right);
    return rotateLeft(current);
  }

  return current;
}

template<class type>
void tree<type>::erase(type value){
  root = erase(value,root);
}

template<class type>
typename tree<type>::node* tree<type>::erase(type value,
                    node *current){
  if(current == NULL)
    return NULL;

  if(value < current->value)
    current->left = erase(value,current->left);
  else if(value > current->value)
    current->right = erase(value,current->right);
  else{
    if(current->left == NULL){
      node* temp = current;
      current = current->right;
      delete temp;
    }
    else if(current->right == NULL){
      node* temp = current;
      current = current->left;
      delete temp;
    }
    else{
      node *leftMost = current->right;
      while(leftMost->left != NULL){
        leftMost = leftMost->left;
      }

      current->value = leftMost->value;
      current->right = erase(leftMost->value,current->right);
    }
  }

  if(current == NULL)
    return NULL;

  current->height = max(current->getHeight(left),
              current->getHeight(right))+1;
  int balance = current->getBalance();

  // Left Left Case
  if(balance > 1 && value < current->left->value)
    return rotateRight(current);
  // Right Right Case
  else if (balance < -1 && value > current->right->value)
    return rotateLeft(current);
  // Left Right Case
  else if (balance > 1 && value > current->left->value){
    current->left =  rotateLeft(current->left);
    return rotateRight(current);
  }
  // Right Left Case
  else if (balance < -1 && value < current->right->value){
    current->right = rotateRight(current->right);
    return rotateLeft(current);
  }

  return current;
}

template<class type>
bool tree<type>::find(type value){
  node *current = root;
  while(current!=NULL){
    if(value == current->value)
      return true;

    if(value < current->value)
      current = current->left;
    else current = current->right;
  }

  return false;
}

template<class type>
void tree<type>::clear(node *current){
  if(current == NULL)
    return;
  clear(current->left);
  clear(current->right);
  delete current;
}
