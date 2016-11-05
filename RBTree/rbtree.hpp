#ifndef RBTREE_HPP
#define RBTREE_HPP
#include <string>
#include <sstream>
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
  using uptr = std::unique_ptr<Node_>;
#else
  typedef boost::scoped_ptr<Node_> uptr;
#endif
  struct Node_ {
    DataType data;
    Node_ *parent;
    uptr left, right;

    Node_(const DataType &d)
        : data(d)
        , parent(NULL)
        , left()
        , right()
    {
    }
  };
  uptr root_;
};

////////////////////////////////////////////////////////////////////////////////
// implementations
////////////////////////////////////////////////////////////////////////////////

template <typename DataType>
void RBTree<DataType>::insert(const DataType &data)
{
  if (!root_)
    root_.reset(new Node_(data));
  else {
    Node_ *ptr(root_.get());
    while (ptr->right)
      ptr = ptr->right.get();
    ptr->right.reset(new Node_(data));
  }
}

////////////////////////////////////////////////////////////////////////////////

template <typename DataType>
std::string RBTree<DataType>::to_string() const
{
  Node_ *ptr(root_.get());
  std::stringstream ss;
  while (ptr) {
    ss << ptr->data << ' ';
    ptr = ptr->right.get();
  }
  ss << '\n';
  return ss.str();
}
} // namespace utility
#endif
