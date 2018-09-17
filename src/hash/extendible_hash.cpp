#include <list>
#include <mutex>
#include <iostream>
#include <math.h>

#include "hash/extendible_hash.h"
#include "page/page.h"

std::mutex mtx;

namespace cmudb {

/*
 * constructor
 * array_size: fixed array size for each bucket
 */
template <typename K, typename V>
ExtendibleHash<K, V>::ExtendibleHash(size_t size) {
    this->bucket_size = size;
    this->gd = 1;
    this->bucket_directory.resize(2);
    this->bucket_local_depth.resize(2);
}

/*
 * helper function to calculate the hashing address of input key
 */
template <typename K, typename V>
size_t ExtendibleHash<K, V>::HashKey(const K &key) {
    size_t hash_value = this->hash_f(key);
    return hash_value;
}

/*
 * helper function to return global depth of hash table
 * NOTE: you must implement this function in order to pass test
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetGlobalDepth() const {
    return this->gd;
}

/*
 * helper function to return local depth of one specific bucket
 * NOTE: you must implement this function in order to pass test
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetLocalDepth(int bucket_id) const {

    return 0;
}

/*
 * helper function to return current number of bucket in hash table
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetNumBuckets() const {
    mtx.lock();

    mtx.unlock();
    return 0;
}

/*
 * lookup function to find value associate with input key
 */
template <typename K, typename V>
bool ExtendibleHash<K, V>::Find(const K &key, V &value) {
    mtx.lock();

    mtx.unlock();
    return false;
}

/*
 * delete <key,value> entry in hash table
 * Shrink & Combination is not required for this project
 */
template <typename K, typename V>
bool ExtendibleHash<K, V>::Remove(const K &key) {
    mtx.lock();

    mtx.unlock();
    return false;
}

/*
 * insert <key,value> entry in hash table
 * Split & Redistribute bucket when there is overflow and if necessary increase
 * global depth
 */
template <typename K, typename V>
void ExtendibleHash<K, V>::Insert(const K &key, const V &value) {
    // Lấy giá trị hash theo khoá K
    size_t hash_value = this->HashKey(key);

    std::cout << hash_value;

    size_t bucket_id = (hash_value % (1 << this->gd));

    // Kiểm tra bucket còn chỗ trống
    if(this->bucket_directory[bucket_id].size() < this->bucket_size){
        this->bucket_directory[bucket_id].push_back(value);

    } else{
        int local_depth = this->GetLocalDepth(bucket_id);
        // Nếu local depth of bucket < global depth
        if (local_depth < this->gd){

        }else{
            // Double the size of bucket directory
        }
    }


}

template class ExtendibleHash<page_id_t, Page *>;
template class ExtendibleHash<Page *, std::list<Page *>::iterator>;
// test purpose
template class ExtendibleHash<int, std::string>;
template class ExtendibleHash<int, std::list<int>::iterator>;
template class ExtendibleHash<int, int>;
} // namespace cmudb
