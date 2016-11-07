#ifndef RBTREE_HPP
#define RBTREE_HPP
#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <cassert>
#if __cplusplus > 201100L
#include <memory>
#define RBTREE_CXX11
#else
#include <boost/scoped_ptr.hpp>
#endif
namespace rbtree
{
template <typename DataType>
class RBTree
{
public:
  enum Color { Red, Black };

public:
  // insert data
  void insert(const DataType &data);
  // print the data to string
  std::string to_string() const;
  // check red black tree invariances (just for debugging)
  bool check_rbtree_invariances() const;

private:
  struct Node_;
#ifdef RBTREE_CXX11
  using UPtr_ = std::unique_ptr<Node_>;
#else
  typedef boost::scoped_ptr<Node_> UPtr_;
#endif
  struct Node_ {
    DataType data;
    Node_ *parent;
    UPtr_ left, right;
    Color color;

    Node_(const DataType &d, Node_ *p)
        : data(d)
        , parent(p)
        , left()
        , right()
        , color(Red)
    {
    }
  };

private:
  UPtr_ m_root;

private:
  Node_ *find_insert_parent_(const DataType &data) const;
  Node_ *get_grandparent_(Node_ *ptr) const;
  Node_ *get_uncle_(Node_ *ptr) const;
  void rotate_right_(Node_ *ptr);
  void rotate_left_(Node_ *ptr);
};

namespace
{
template <typename DataType>
struct PrintNode {
#ifdef RBTREE_CXX11
  using UPtr = std::unique_ptr<PrintNode>;
#else
  typedef boost::scoped_ptr<PrintNode> UPtr;
#endif
  typedef RBTree<DataType> tree_type;
  typedef typename tree_type::Color color_type;
  const DataType &data;
  int color_code;
  UPtr left, right;
  int offset;
  PrintNode(const DataType &d, color_type color)
      : data(d)
      , color_code(-1)
      , left()
      , right()
      , offset(0)
  {
    if (color == tree_type::Red)
      color_code = 1;
    else if (color == tree_type::Black)
      color_code = 0;
  }
};

template <typename DataType>
int dfs_build_printtree_offset(PrintNode<DataType> *ptr, int padding);
} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// implementations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// main class members
////////////////////////////////////////////////////////////////////////////////

template <typename DataType>
void RBTree<DataType>::insert(const DataType &data)
{
  if (!m_root) {
    m_root.reset(new Node_(data, NULL));
    m_root->color = Black;
  } else {
    Node_ *ptr(find_insert_parent_(data));
    if (ptr->data == data)
      return;
    else if (ptr->data < data) {
      ptr->right.reset(new Node_(data, ptr));
      ptr = ptr->right.get();
    }
    else {
      ptr->left.reset(new Node_(data, ptr));
      ptr = ptr->left.get();
    }

    // fix the tree
    while (ptr) {
      // red propagated to the root
      if (ptr->parent == NULL) {
        ptr->color = Black;
        break;
      }
      // parent is black, done
      if (ptr->parent->color == Black)
        break;
      Node_ *gp(get_grandparent_(ptr)), *uncle(get_uncle_(ptr));
      assert(gp != NULL);
      Node_ *parent(ptr->parent);
      // uncle is red, flip color and propagate red to grandparant
      if (uncle != NULL && uncle->color == Red) {
        uncle->color = parent->color = Black;
        gp->color = Red;
        ptr = gp;
      }
      // otherwise, rotate twice to fix
      else {
        if (gp->left.get() == parent) {
          if (ptr == parent->right.get()) {
            rotate_left_(parent);
            std::swap(ptr, parent);
          }
          rotate_right_(gp);
          std::swap(parent->color, gp->color);
        } else {
          if (ptr == parent->left.get()) {
            rotate_right_(parent);
            std::swap(ptr, parent);
          }
          rotate_left_(gp);
          std::swap(parent->color, gp->color);
        }
        break;
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

template <typename DataType>
std::string RBTree<DataType>::to_string() const
{
  if (!m_root)
    return "";
  typedef PrintNode<DataType> PrintNodeLocal;
  typedef std::pair<Node_ *, PrintNodeLocal *> pair_type;
  typedef typename PrintNodeLocal::UPtr UPtrLocal;
  typedef std::vector<pair_type> dfs_stack_type;
  // depth first search build
  Node_ *ptr(m_root.get());
  UPtrLocal root(new PrintNodeLocal(ptr->data, ptr->color));
  PrintNodeLocal *local_ptr(root.get());
  dfs_stack_type dfs_stack;
  while (true) {
    while (true) {
      if (ptr->right) {
        local_ptr->right.reset(
            new PrintNodeLocal(ptr->right->data, ptr->right->color));
        dfs_stack.push_back(
            pair_type(ptr->right.get(), local_ptr->right.get()));
      }
      if (!ptr->left)
        break;
      ptr = ptr->left.get();
      local_ptr->left.reset(new PrintNodeLocal(ptr->data, ptr->color));
      local_ptr = local_ptr->left.get();
    }
    if (dfs_stack.empty())
      break;
    ptr = dfs_stack.back().first;
    local_ptr = dfs_stack.back().second;
    dfs_stack.pop_back();
  }

  // build offsets
  dfs_build_printtree_offset(root.get(), 0);

  std::stringstream ss;
  // breadth first search print
  typedef std::vector<PrintNodeLocal *> bfs_queue_type;
  typedef typename bfs_queue_type::iterator queue_iter_type;
  bfs_queue_type queues[2];
  std::size_t queue_idx1(0), queue_idx2(1);
  int current_offset(0);
  const int half_print_width(3);
  queues[queue_idx2].push_back(root.get());
  while (!queues[queue_idx2].empty()) {
    std::swap(queue_idx1, queue_idx2);
    current_offset = 0;
    for (queue_iter_type it(queues[queue_idx1].begin());
         it != queues[queue_idx1].end(); ++it) {
      const PrintNodeLocal &pn(**it);
      if (pn.left)
        queues[queue_idx2].push_back(pn.left.get());
      if (pn.right)
        queues[queue_idx2].push_back(pn.right.get());
      assert(pn.offset >= current_offset);
      if (pn.offset > current_offset)
        ss << std::string((pn.offset - current_offset) * half_print_width, ' ');
      ss << std::setw(2 * half_print_width - 2) << pn.data 
         << ',' << pn.color_code;
      current_offset = pn.offset + 2;
    }
    queues[queue_idx1].clear();
    ss << '\n';
  }

  return ss.str();
}

////////////////////////////////////////////////////////////////////////////////

template <typename DataType>
bool RBTree<DataType>::check_rbtree_invariances() const
{
  if (!m_root)
    return true;
  if (m_root->color != Black)
    return false;
  // depth first search to verify black heights
  typedef std::pair<Node_ *, int> pair_type;
  typedef std::vector<pair_type> dfs_stack_type;
  Node_ *ptr(m_root.get());
  dfs_stack_type stack;
  int tmp_height(1), checkpoint_height(-1);
  // first, calculate a height
  while (true) {
    if (ptr->right)
      stack.push_back(pair_type(ptr->right.get(), tmp_height));
    else if (checkpoint_height == -1)
      checkpoint_height = tmp_height;
    if (!ptr->left)
      break;
    ptr = ptr->left.get();
    if (ptr->color == Black)
      ++tmp_height;
  }
  if (checkpoint_height != tmp_height)
    return false;
  const int black_height(tmp_height);
  // second, compare other heights with this one
  while (!stack.empty()) {
    ptr = stack.back().first;
    int current_height(stack.back().second);
    if (ptr->color == Black)
      ++current_height;
    stack.pop_back();
    while (true) {
      if (ptr->right)
        stack.push_back(pair_type(ptr->right.get(), current_height));
      if (!ptr->left)
        break;
      ptr = ptr->left.get();
      if (ptr->color == Black)
        ++current_height;
    }
    if (current_height != black_height)
      return false;
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////

template <typename DataType>
typename RBTree<DataType>::Node_ *
RBTree<DataType>::find_insert_parent_(const DataType &data) const
{
  if (!m_root)
    return NULL;
  Node_ *ptr(m_root.get());
  while (true) {
    if (ptr->data == data)
      return ptr;
    if (ptr->data > data) {
      if (ptr->left)
        ptr = ptr->left.get();
      else
        return ptr;
    } else {
      if (ptr->right)
        ptr = ptr->right.get();
      else
        return ptr;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

template <typename DataType>
typename RBTree<DataType>::Node_ *
RBTree<DataType>::get_grandparent_(Node_ *ptr) const
{
  if (ptr != NULL && ptr->parent != NULL)
    return ptr->parent->parent;
  else
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////

template <typename DataType>
typename RBTree<DataType>::Node_ *
RBTree<DataType>::get_uncle_(Node_ *ptr) const
{
  Node_ *grandparent(get_grandparent_(ptr));
  if (grandparent == NULL)
    return NULL;
  if (grandparent->left.get() == ptr->parent)
    return grandparent->right.get();
  else
    return grandparent->left.get();
}

////////////////////////////////////////////////////////////////////////////////

template <typename DataType>
void RBTree<DataType>::rotate_left_(Node_ *ptr)
{
  Node_ *right_ptr(ptr->right.get());
  if (right_ptr == NULL)
    return;
  Node_ *parent(ptr->parent);
  ptr->right.swap(right_ptr->left);
  ptr->parent = right_ptr;
  right_ptr->parent = parent;
  UPtr_ *uptr_ptr(&m_root);
  if (parent != NULL) {
    if (parent->left.get() == ptr)
      uptr_ptr = &parent->left;
    else
      uptr_ptr = &parent->right;
  }
  uptr_ptr->swap(right_ptr->left);
}

////////////////////////////////////////////////////////////////////////////////

template <typename DataType>
void RBTree<DataType>::rotate_right_(Node_ *ptr)
{
  Node_ *left_ptr(ptr->left.get());
  if (left_ptr == NULL)
    return;
  Node_ *parent(ptr->parent);
  ptr->left.swap(left_ptr->right);
  ptr->parent = left_ptr;
  left_ptr->parent = parent;
  UPtr_ *uptr_ptr(&m_root);
  if (parent != NULL) {
    if (parent->left.get() == ptr)
      uptr_ptr = &parent->left;
    else
      uptr_ptr = &parent->right;
  }
  uptr_ptr->swap(left_ptr->right);
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// local classes and functions
////////////////////////////////////////////////////////////////////////////////

namespace
{
template <typename DataType>
int dfs_build_printtree_offset(PrintNode<DataType> *ptr, int padding)
{
  int left_width(1), right_width(1);
  if (ptr->left)
    left_width = dfs_build_printtree_offset(ptr->left.get(), padding);
  if (ptr->right)
    right_width =
        dfs_build_printtree_offset(ptr->right.get(), left_width + padding);
  ptr->offset = padding + left_width - 1;
  return left_width + right_width;
}
} // anonymous namespace


} // namespace rbtree
#endif
