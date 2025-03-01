//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// trie_store.cpp
//
// Identification: src/primer/trie_store.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "primer/trie_store.h"
#include "common/exception.h"

namespace bustub {

/**
 * @brief This function returns a ValueGuard object that holds a reference to the value in the trie. If
 * the key does not exist in the trie, it will return std::nullopt.
 */
template <class T>
auto TrieStore::Get(std::string_view key) -> std::optional<ValueGuard<T>> {
  // Pseudo-code:
  // (1) Take the root lock, get the root, and release the root lock. Don't lookup the value in the
  //     trie while holding the root lock.
  std::unique_lock<std::mutex> lock(root_lock_);

  Trie root = this->root_;

  lock.unlock();  // 释放锁
  // (2) Lookup the value in the trie.

  const T *pValue = root.Get<T>(key);
  // (3) If the value is found, return a ValueGuard object that holds a reference to the value and the
  //     root. Otherwise, return std::nullopt.
  if (pValue == nullptr)
    return std::nullopt;
  else
    return ValueGuard(root, *pValue);
}

/**
 * @brief This function will insert the key-value pair into the trie. If the key already exists in the
 * trie, it will overwrite the value.
 */
template <class T>
void TrieStore::Put(std::string_view key, T value) {
  // You will need to ensure there is only one writer at a time. Think of how you can achieve this.
  // The logic should be somehow similar to `TrieStore::Get`.
  std::unique_lock<std::mutex> write_lock(write_lock_);

  std::unique_lock<std::mutex> root_lock(root_lock_);

  Trie root = this->root_;

  root_lock.unlock();

  root = root.Put<T>(key, std::move(value));

  root_lock.lock();

  this->root_ = root;

  root_lock.unlock();

  write_lock.unlock();
}

/** @brief This function will remove the key-value pair from the trie. */
void TrieStore::Remove(std::string_view key) {
  // You will need to ensure there is only one writer at a time. Think of how you can achieve this.
  // The logic should be somehow similar to `TrieStore::Get`.
  std::unique_lock<std::mutex> write_lock(write_lock_);

  std::unique_lock<std::mutex> root_lock(root_lock_);

  Trie root = this->root_;

  root_lock.unlock();

  root = root.Remove(key);

  root_lock.lock();

  this->root_ = root;

  root_lock.unlock();

  write_lock.unlock();
}

// Below are explicit instantiation of template functions.

template auto TrieStore::Get(std::string_view key) -> std::optional<ValueGuard<uint32_t>>;
template void TrieStore::Put(std::string_view key, uint32_t value);

template auto TrieStore::Get(std::string_view key) -> std::optional<ValueGuard<std::string>>;
template void TrieStore::Put(std::string_view key, std::string value);

// If your solution cannot compile for non-copy tests, you can remove the below lines to get partial score.

using Integer = std::unique_ptr<uint32_t>;

template auto TrieStore::Get(std::string_view key) -> std::optional<ValueGuard<Integer>>;
template void TrieStore::Put(std::string_view key, Integer value);

template auto TrieStore::Get(std::string_view key) -> std::optional<ValueGuard<MoveBlocked>>;
template void TrieStore::Put(std::string_view key, MoveBlocked value);

}  // namespace bustub
