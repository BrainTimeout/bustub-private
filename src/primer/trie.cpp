//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// trie.cpp
//
// Identification: src/primer/trie.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "primer/trie.h"
#include <string_view>
#include "common/exception.h"

namespace bustub {

/**
 * @brief Get the value associated with the given key.
 * 1. If the key is not in the trie, return nullptr.
 * 2. If the key is in the trie but the type is mismatched, return nullptr.
 * 3. Otherwise, return the value.
 */
template <class T>
auto Trie::Get(std::string_view key) const -> const T * {
  std::shared_ptr<const TrieNode> node = root_;
  if (node == nullptr) return nullptr;
  for (char ch : key) {
    auto it = node->children_.find(ch);
    if (it == node->children_.end())
      return nullptr;
    else
      node = it->second;
  }
  // 如果到达的节点不是一个终止节点，返回 nullptr
  if (!node->is_value_node_) return nullptr;

  // 如果找到终止节点，使用 dynamic_cast 来检查类型是否匹配
  // 如果类型不匹配，返回 nullptr
  auto derived_node = std::dynamic_pointer_cast<const TrieNodeWithValue<T>>(node);
  if (!derived_node) return nullptr;  // 类型不匹配

  // 如果转换成功，返回节点的值
  return derived_node->value_.get();  // 获取原始指针并返回

  // You should walk through the trie to find the node corresponding to the key. If the node doesn't exist, return
  // nullptr. After you find the node, you should use `dynamic_cast` to cast it to `const TrieNodeWithValue<T> *`. If
  // dynamic_cast returns `nullptr`, it means the type of the value is mismatched, and you should return nullptr.
  // Otherwise, return the value.
}

/**
 * @brief Put a new key-value pair into the trie. If the key already exists, overwrite the value.
 * @return the new trie.
 */
template <class T>
auto Trie::Put(std::string_view key, T value) const -> Trie {
  std::shared_ptr<TrieNode> node = nullptr;
  if (root_ != nullptr) {
    if (key.size() == 0) {
      return Trie(std::make_shared<TrieNodeWithValue<T>>(root_->children_, std::make_shared<T>(std::move(value))));
    } else {
      node = root_->Clone();
    }
  } else {
    if (key.size() == 0) {
      return Trie(std::make_shared<TrieNodeWithValue<T>>(std::make_shared<T>(std::move(value))));
    }
    node = std::make_shared<TrieNode>();
  }
  Trie trie(node);
  size_t i = 0;
  char ch;
  while (i < key.size() - 1) {
    ch = key[i];
    auto it = node->children_.find(ch);
    if (it == node->children_.end())
      node->children_[ch] = std::make_shared<TrieNode>();
    else
      node->children_[ch] = it->second->Clone();
    node = std::const_pointer_cast<bustub::TrieNode>(node->children_[ch]);
    i++;
  }
  ch = key[i];
  auto it = node->children_.find(ch);
  std::shared_ptr<TrieNodeWithValue<T>> value_node;
  if (it == node->children_.end())
    value_node = std::make_shared<TrieNodeWithValue<T>>(std::make_shared<T>(std::move(value)));
  else
    value_node = std::make_shared<TrieNodeWithValue<T>>(it->second->children_, std::make_shared<T>(std::move(value)));
  node->children_[ch] = value_node;

  return trie;
  // Note that `T` might be a non-copyable type. Always use `std::move` when creating `shared_ptr` on that value.
  // throw NotImplementedException("Trie::Put is not implemented.");

  // You should walk through the trie and create new nodes if necessary. If the node corresponding to the key already
  // exists, you should create a new `TrieNodeWithValue`.
}

/**
 * @brief Remove the key from the trie.
 * @return If the key does not exist, return the original trie. Otherwise, returns the new trie.
 */
auto Trie::Remove(std::string_view key) const -> Trie {
  std::vector<std::shared_ptr<const bustub::TrieNode>> nodeStk;
  std::shared_ptr<const TrieNode> node = root_;
  for (char ch : key) {
    auto it = node->children_.find(ch);
    if (it == node->children_.end()) {
      return *this;
    } else {
      nodeStk.push_back(node);
      node = it->second;
    }
  }
  if (node->is_value_node_ == false) {
    return *this;
  } else {
    if (node->children_.empty())
      node = nullptr;
    else
      node = std::make_shared<TrieNode>(node->children_);
    std::shared_ptr<TrieNode> parentNode;
    for (auto it = key.rbegin(); it != key.rend(); ++it) {
      char ch = *it;
      parentNode = nodeStk.back()->Clone();
      nodeStk.pop_back();
      if (node != nullptr) {
        parentNode->children_[ch] = node;
      } else {
        if (parentNode->children_.size() <= 1 && parentNode->is_value_node_ == false)
          parentNode = nullptr;
        else
          parentNode->children_.erase(ch);
      }
      node = parentNode;
    }
    return Trie(node);
  }
  // You should walk through the trie and remove nodes if necessary. If the node doesn't contain a value any more,
  // you should convert it to `TrieNode`. If a node doesn't have children any more, you should remove it.
}

// Below are explicit instantiation of template functions.
//
// Generally people would write the implementation of template classes and functions in the header file. However, we
// separate the implementation into a .cpp file to make things clearer. In order to make the compiler know the
// implementation of the template functions, we need to explicitly instantiate them here, so that they can be picked up
// by the linker.

template auto Trie::Put(std::string_view key, uint32_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint32_t *;

template auto Trie::Put(std::string_view key, uint64_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint64_t *;

template auto Trie::Put(std::string_view key, std::string value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const std::string *;

// If your solution cannot compile for non-copy tests, you can remove the below lines to get partial score.

using Integer = std::unique_ptr<uint32_t>;

template auto Trie::Put(std::string_view key, Integer value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const Integer *;

template auto Trie::Put(std::string_view key, MoveBlocked value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const MoveBlocked *;

}  // namespace bustub
