#include <list>
#include <mutex>
#include <iostream>
#include <math.h>
#include <set>

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

    // Init scalar value
    this->bucket_size = size;
    this->gd = 1;
    this->num_bucket = 2;

    // Init bucket list
    this->bucket_list.resize(this->num_bucket);

    // Init bucket directory
    /*
     * 0 -> B0
     * 1 -> B1
     */
    this->bucket_directory.resize(this->num_bucket);
    this->bucket_directory[0] = 0;
    this->bucket_directory[1] = 1;

    // Init bucket local depth
    /*
     * Default
     *  B0: 0
     *  B1: 0
     */
    this->bucket_local_depth.resize(this->num_bucket);
    this->bucket_local_depth[0] = 0;
    this->bucket_local_depth[1] = 0;
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

    return this->bucket_local_depth[bucket_id];
}

/*
 * helper function to return current number of bucket in hash table
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetNumBuckets() const {
    return int(this->bucket_list.size());
}

/*
 * lookup function to find value associate with input key
 */
template <typename K, typename V>
bool ExtendibleHash<K, V>::Find(const K &key, V &value) {
    mtx.lock();
    size_t hash_value = this->HashKey(key);
    int bucket_id = this->bucket_directory[(hash_value % (1 << this->GetGlobalDepth()))];
    for(auto elem = this->bucket_list[bucket_id].begin(); elem != this->bucket_list[bucket_id].end(); ++elem)
    {
        if(elem->first == key)
        {
            value = elem->second;
            mtx.unlock();
            return true;
        }
    }
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
    size_t hash_value = this->HashKey(key);
    int bucket_id = this->bucket_directory[(hash_value % (1 << this->GetGlobalDepth()))];
    int index=0;
    for(auto elem = this->bucket_list[bucket_id].begin(); elem != this->bucket_list[bucket_id].end(); ++elem, ++index)
    {
        if(elem->first == key)
        {
            this->bucket_list[bucket_id].erase(this->bucket_list[bucket_id].begin() + index);
            mtx.unlock();
            return true;
        }
    }
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
    mtx.lock();
    // Check if exist
    V val;
    if(this->Find(key, val)) {
        size_t hash_value = this->HashKey(key);
        int bucket_id = this->bucket_directory[(hash_value % (1 << this->GetGlobalDepth()))];
        for (auto elem = this->bucket_list[bucket_id].begin(); elem != this->bucket_list[bucket_id].end(); ++elem) {
            if (elem->first == key) {
                elem->second = value;
                return;
            }
        }
    }
    // Get hash string base on key
    size_t hash_value = this->HashKey(key);

    int bucket_id = this->bucket_directory[(hash_value % (1 << this->GetGlobalDepth()))];
//    cout<<"Hash value: " << hash_value <<", 2^Global Depth: " << (1 << this->GetGlobalDepth())<<", " << (hash_value % (1 << this->GetGlobalDepth()))<< " ";
//    cout<<"Key ID"<<key<<", Bucket id: "<<bucket_id<<", Global Depth:"<<this->GetGlobalDepth()<<endl;

    // Check if a bucket has available slot
    if(this->bucket_list[bucket_id].size() < this->bucket_size){
        if(this->bucket_list[bucket_id].empty())
            this->bucket_local_depth[bucket_id] = this->GetGlobalDepth();
        this->bucket_list[bucket_id].push_back(make_pair(key, value));

    } else{
        int local_depth = this->GetLocalDepth(bucket_id);

        if (local_depth == this->GetGlobalDepth()){
            // Double the size of bucket directory
            this->num_bucket *= 2;
            this->gd += 1;
            this->bucket_local_depth.resize(this->num_bucket);
            this->bucket_directory.resize(this->num_bucket);
            this->bucket_list.resize(this->num_bucket);
            for(size_t i=this->num_bucket/2; i<this->bucket_directory.size(); ++i)
                this->bucket_directory[i] = -1;

        }
        // Relocation element
        vector<pair<K,V>> temp_cache;
        while (not this->bucket_list[bucket_id].empty())
        {
            temp_cache.push_back(this->bucket_list[bucket_id].back());
            this->bucket_list[bucket_id].pop_back();
        }
        temp_cache.push_back(make_pair(key, value));

        for (auto elem = temp_cache.begin(); elem!=temp_cache.end(); ++elem) {
            size_t elem_hash_value = this->HashKey(elem->first);
            size_t new_bucket_id = (elem_hash_value % (1 << this->GetGlobalDepth()));
            this->bucket_list[new_bucket_id].push_back(*elem);
            this->bucket_local_depth[new_bucket_id] = this->GetGlobalDepth();
        }

        // Re-pointer new bucket directory to exist bucket
        for (size_t i=0; i < this->bucket_directory.size(); ++i)
        {
            if(this->bucket_directory[i] == -1)
            {
                this->bucket_directory[i] = int(i) % (1 << (this->GetGlobalDepth()-1));
            }
            if(this->bucket_directory[i] == bucket_id)
            {
                this->bucket_directory[i] = int(i) % (1 << (this->GetLocalDepth(bucket_id)));
            }
        }
    }
    this->num_bucket += 1;
    mtx.unlock();
}

template class ExtendibleHash<page_id_t, Page *>;
template class ExtendibleHash<Page *, std::list<Page *>::iterator>;
// test purpose
template class ExtendibleHash<int, std::string>;
template class ExtendibleHash<int, std::list<int>::iterator>;
template class ExtendibleHash<int, int>;
//template class ExtendibleHash<int, unsigned long>;
//template class ExtendibleHash<Page*, unsigned long>;
} // namespace cmudb
