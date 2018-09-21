/**
 * LRU implementation
 */
#include <limits>
#include "buffer/lru_replacer.h"
#include "page/page.h"

namespace cmudb {

template <typename T> LRUReplacer<T>::LRUReplacer() {
    this->_time = 0;
    this->lru_table = new ExtendibleHash<T, size_t >(2);
}

template <typename T> LRUReplacer<T>::~LRUReplacer() {
    delete this->lru_table;
}

/*
 * Insert value into LRU
 */
template <typename T> void LRUReplacer<T>::Insert(const T &value) {
    size_t tmp_val;
    if(not this->lru_table->Find(value, tmp_val))
        this->_buffer.push_back(value);

    this->lru_table->Insert(value, ++this->_time);

}

/* If LRU is non-empty, pop the head member from LRU to argument "value", and
 * return true. If LRU is empty, return false
 */
template <typename T> bool LRUReplacer<T>::Victim(T &value) {
    if(this->_buffer.empty())
        return false;
    if(this->_buffer.size() == 1)
    {
        value = this->_buffer[0];
        this->lru_table->Remove(value);
        return true;
    }
    T victim = this->_buffer[0];
    size_t least_time = std::numeric_limits<size_t>::max();
    size_t index = 0;
    this->lru_table->Find(victim, least_time);
    for(size_t i=1;i < this->_buffer.size(); ++i)
    {
        size_t tmp_time = 0;
        this->lru_table->Find(this->_buffer[i], tmp_time);
        if(tmp_time < least_time)
        {
            victim = this->_buffer[i];
            least_time = tmp_time;
            index = i;
        }

    }
    value = victim;
    this->lru_table->Remove(value);
    this->_buffer.erase(this->_buffer.begin(), this->_buffer.begin() + index);
    return true;
}

/*
 * Remove value from LRU. If removal is successful, return true, otherwise
 * return false
 */
template <typename T> bool LRUReplacer<T>::Erase(const T &value) {
    size_t time;
    if(not this->lru_table->Find(value, time))
        return false;
    this->lru_table->Remove(value);
    for(size_t i=0; i< this->_buffer.size(); i++)
    {
        if(this->_buffer[i] == value)
        {
            this->_buffer.erase(this->_buffer.begin(), this->_buffer.begin() + i);
            break;
        }
    }
    return true;
}

template <typename T> size_t LRUReplacer<T>::Size() { return this->_buffer.size(); }

template class LRUReplacer<Page *>;
// test only
template class LRUReplacer<int>;

} // namespace cmudb
