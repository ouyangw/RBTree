#include "rbtree.hpp"
#include <iostream>

int main()
{
  rbtree::RBTree<int> tree;
  tree.insert(1);
  tree.insert(5);
  tree.insert(2);
  tree.insert(3);
  tree.insert(4);
  tree.insert(7);
  tree.insert(6);
  tree.insert(8);
  tree.insert(0);

  tree.insert(-1);
  tree.insert(-2);
  tree.insert(-3);
  tree.insert(-4);
  std::cout << tree.to_string() << '\n';
  return 0;
}
