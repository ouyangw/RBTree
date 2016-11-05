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
namespace utility
{
template <typename DataType>
class RBTree
{
public:
  // insert data
  void insert(const DataType &data);
  // print the data to string
  std::string to_string() const;

private:
  struct Node_;
#ifdef RBTREE_CXX11
  using UPtr = std::unique_ptr<Node_>;
#else
  typedef boost::scoped_ptr<Node_> UPtr;
#endif
  struct Node_ {
    DataType data;
    Node_ *parent;
    UPtr left, right;

    Node_(const DataType &d, Node_ *p)
        : data(d)
        , parent(p)
        , left()
        , right()
    {
    }
  };

private:
  UPtr m_root;

private:
  Node_ *find_insert_parent(const DataType &data) const;
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
  const DataType &data;
  UPtr left, right;
  int offset;
  PrintNode(const DataType &d)
      : data(d)
      , left()
      , right()
      , offset(0)
  {
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
  if (!m_root)
    m_root.reset(new Node_(data, NULL));
  else {
    Node_ *ptr(find_insert_parent(data));
    if (ptr->data == data)
      return;
    else if (ptr->data < data)
      ptr->right.reset(new Node_(data, ptr));
    else
      ptr->left.reset(new Node_(data, ptr));
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
  UPtrLocal root(new PrintNodeLocal(ptr->data));
  PrintNodeLocal *local_ptr(root.get());
  dfs_stack_type vec;
  vec.push_back(pair_type(ptr, local_ptr));
  do {
    while (ptr->left) {
      ptr = ptr->left.get();
      local_ptr->left.reset(new PrintNodeLocal(ptr->data));
      local_ptr = local_ptr->left.get();
      vec.push_back(pair_type(ptr, local_ptr));
    }
    while (!ptr->right) {
      vec.pop_back();
      if (vec.empty())
        break;
      ptr = vec.back().first;
      local_ptr = vec.back().second;
    }
    if (vec.empty())
      break;
    vec.pop_back();
    ptr = ptr->right.get();
    local_ptr->right.reset(new PrintNodeLocal(ptr->data));
    local_ptr = local_ptr->right.get();
    vec.push_back(pair_type(ptr, local_ptr));
  } while (!vec.empty());

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
      ss << std::setw(2 * half_print_width) << pn.data;
      current_offset = pn.offset + 2;
    }
    queues[queue_idx1].clear();
    ss << '\n';
  }

  return ss.str();
}

////////////////////////////////////////////////////////////////////////////////

template <typename DataType>
typename RBTree<DataType>::Node_ *
RBTree<DataType>::find_insert_parent(const DataType &data) const
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


} // namespace utility
#endif
