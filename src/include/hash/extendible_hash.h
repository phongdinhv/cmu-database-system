/*
 * extendible_hash.h : implementation of in-memory hash table using extendible
 * hashing
 *
 * Functionality: The buffer pool manager must maintain a page table to be able
 * to quickly map a PageId to its corresponding memory location; or alternately
 * report that the PageId does not match any currently-buffered page.
 */

#pragma once

#include <cstdlib>
#include <vector>
#include <string>
#include <mutex>


#include "hash/hash_table.h"

using namespace std;

namespace cmudb {

template <typename K, typename V>
class ExtendibleHash : public HashTable<K, V> {
public:
  // constructor
  ExtendibleHash(size_t size);
  // helper function to generate hash addressing
  size_t HashKey(const K &key);
  // helper function to get global & local depth
  size_t GetGlobalDepth() const;
  size_t GetLocalDepth(int bucket_id) const;
    size_t GetNumBuckets() const;
  // lookup and modifier
  bool Find(const K &key, V &value) override;
  bool Remove(const K &key) override;
  void Insert(const K &key, const V &value) override;

private:
  // add your own member variables here
  hash<K> hash_f;
  vector<vector<pair<K, V>>> bucket_list;
  vector<size_t > bucket_directory;
  vector<size_t > bucket_local_depth;
  size_t bucket_size;
  size_t gd;
  size_t num_bucket;
  std::mutex mtx;
  size_t directory_size;


};
} // namespace cmudb
