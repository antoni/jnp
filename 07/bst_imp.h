#ifndef BST_IMP_H
#define BST_IMP_H

#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>
#include <string>
#include "bst.h"

namespace {
template <typename T, typename Iter>
BST<T> init_tree(BST<T>&& acc, Iter iter1, Iter iter2) {
    if (iter1 == iter2) return acc;
    Iter next = iter1++;
    return init_tree(std::move(*next + acc), iter1, iter2);
}

template <typename T>
void check_tree(BST<T> bst) {
    if (bst.empty()) throw std::logic_error("The tree is empty");
}
}

template <typename T>
BST<T>::BST(std::initializer_list<T> init_list)
    : BST(init_list.begin(), init_list.end()){};

template <typename T>
template <typename Iter>
BST<T>::BST(Iter iter1, Iter iter2)
    : BST<T>(init_tree(BST<T>(), iter1, iter2)) {}

template <typename T>
BST<T>::BST(T value, BST<T> left, BST<T> right)
    : m_root(std::make_shared<const Node>(
          Node(value, left.m_root, right.m_root))){};

template <typename T>
BST<T> BST<T>::left() const {
    check_tree(*this);
    return m_root->m_left;
}

template <typename T>
BST<T> BST<T>::right() const {
    check_tree(*this);
    return m_root->m_right;
}

template <typename T>
T const& BST<T>::value() const {
    check_tree(*this);
    return m_root->m_value;
}

template <typename T>
bool BST<T>::empty() const {
    return m_root == nullptr;
}

template <typename T>
T const& BST<T>::min() const {
    check_tree(*this);
    return left().empty() ? value() : left().min();
}

template <typename T>
T const& BST<T>::max() const {
    check_tree(*this);
    return right().empty() ? value() : right().max();
}

template <typename T>
template <typename Acc, typename Functor>
Acc BST<T>::fold(Acc a, Functor f) const {
    return empty() ? a : right().empty()
                             ? f(left().fold(a, f), value())
                             : right().fold(f(left().fold(a, f), value()), f);
}

template <typename T>
BST<T> BST<T>::find(T const& t) const {
    if (empty()) throw std::logic_error("Element not found");
    return value() > t ? left().find(t) : value() < t ? right().find(t) : *this;
}

template <typename T>
std::size_t BST<T>::size() const {
    return fold(
        0, [](std::size_t total, T value) -> std::size_t { return ++total; });
}

template <typename T>
std::size_t BST<T>::height() const {
    return empty() ? 0 : std::max(left().height(), right().height()) + 1;
}

template <typename T>
BST<T> spine(BST<T> bst) {
    return bst.fold(BST<T>(), [](BST<T> tree, T next_elem) -> BST<T> {
        return BST<T>(next_elem, tree, BST<T>());
    });
}

template <typename T>
BST<T> operator+(T elem, BST<T> tree) {
    return tree.empty()
               ? BST<T>(elem, BST<T>(), BST<T>())
               : (elem > tree.value()
                      ? BST<T>(tree.value(), tree.left(), tree.right() + elem)
                      : BST<T>(tree.value(), tree.left() + elem, tree.right()));
}

template <typename T>
BST<T> operator+(BST<T> tree, T elem) {
    return elem + tree;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, BST<T> bst) {
    if (!bst.empty())
        os << bst.left(), os << bst.value() << ' ', os << bst.right();
    return os;
}

template <typename T>
T max_diff(BST<T> bst) {
    if (bst.empty() || bst.size() < 2)
        throw std::logic_error(
            "max_diff can only be applied to non-empty trees with at least 2 "
            "nodes");
    T diff = bst.min() - bst.min();
    bst.fold(bst.min(), [&diff](T current, T next) -> T {
        diff = std::max(next - current, diff);
        return next;
    });
    return diff;
}

#endif /* BST_IMP_H */
