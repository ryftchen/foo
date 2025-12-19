//! @file cache.hpp
//! @author ryftchen
//! @brief The declarations (cache) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <list>
#include <map>
#include <mutex>
#include <numeric>
#include <optional>
#include <unordered_map>
#include <vector>

//! @brief The data structure module.
namespace data_structure // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Cache-related functions in the data structure module.
namespace cache
{
//! @brief Brief function description.
//! @return function description (module_function)
inline static const char* description() noexcept
{
    return "DS_CACHE";
}
extern const char* version() noexcept;

//! @brief The FIFO (first in first out) cache replacement policy.
//! @tparam Key - type of cache key
//! @tparam Value - type of value stored in the cache
template <typename Key, typename Value>
class FIFO
{
public:
    //! @brief Construct a new FIFO object.
    //! @param capacity - maximum number of elements the cache can hold
    //! @param maxLoadFactor - maximum load factor for the internal hash map
    explicit FIFO(const std::size_t capacity, const float maxLoadFactor = 1.0F);

    //! @brief Insert or update an element by using the value by key.
    //! @param key - key associated with the value
    //! @param value - value to be inserted or updated
    void insert(const Key& key, Value value);
    //! @brief Insert a range of key-value pairs.
    //! @tparam Range - type of range of key-value pairs
    //! @param keyValueRange - range of key-value pairs to insert
    //! @return number of elements inserted
    template <typename Range>
    std::size_t insertRange(Range&& keyValueRange);
    //! @brief Erase an element by key.
    //! @param key - key of the element to remove
    //! @return be found and erased or not
    bool erase(const Key& key);
    //! @brief Erase a range of elements by using iterators.
    //! @tparam Iterator - type of pair iterator
    //! @param begin - iterator to the beginning of pairs
    //! @param end - iterator to the ending of pairs
    //! @return number of elements successfully erased
    template <typename Iterator>
    std::size_t erase(Iterator begin, const Iterator end);
    //! @brief Erase a range of elements by using a range of keys.
    //! @tparam Range - type of range of keys
    //! @param keyRange - range of keys to be erased
    //! @return number of elements successfully erased
    template <typename Range>
    std::size_t eraseRange(const Range& keyRange);
    //! @brief Find a value by key.
    //! @param key - key to search for
    //! @return optional value
    std::optional<Value> find(const Key& key);
    //! @brief Find a range of values by keys by using iterators.
    //! @tparam Iterator - type of pair iterator
    //! @param begin - iterator to the beginning of pairs
    //! @param end - iterator to the ending of pairs
    //! @param distance - optional pre-allocation hint for result container size
    //! @return container of key-optional-value pairs
    template <typename Iterator>
    auto find(Iterator begin, const Iterator end, const std::size_t distance = 0)
        -> std::vector<std::pair<Key, std::optional<Value>>>;
    //! @brief Find a range of values by keys by using a range of keys.
    //! @tparam Range - type of range of keys
    //! @param keyRange - range of keys to be searched
    //! @return container of key-optional-value pairs
    template <typename Range>
    auto findRange(const Range& keyRange) -> std::vector<std::pair<Key, std::optional<Value>>>;
    //! @brief Resolve values for a range of keys in-place by using iterators.
    //! @tparam Iterator - type of pair iterator
    //! @param begin - iterator to the beginning of pairs
    //! @param end - iterator to the ending of pairs
    template <typename Iterator>
    void resolveRange(Iterator begin, const Iterator end);
    //! @brief Resolve values for a range of keys in-place by using a range of key-optional-value pairs.
    //! @tparam Range - type of range of key-optional-value pairs
    //! @param keyOptValueRange - range of key-optional-value pairs to resolve
    template <typename Range>
    void resolveRange(Range& keyOptValueRange);

    //! @brief Check whether any elements do not exist in the cache.
    //! @return any elements do not exist or exist
    bool empty() const;
    //! @brief Get the number of elements currently in the cache.
    //! @return number of used slots
    std::size_t size() const;
    //! @brief Get the total capacity of the cache.
    //! @return maximum number of elements the cache can hold
    std::size_t capacity() const;

private:
    struct Element;
    //! @brief Alias for the iterator of the internal FIFO list.
    using FIFOIter = typename std::list<Element>::iterator;
    //! @brief Alias for the iterator of the internal hash map.
    using KeyedIter = typename std::unordered_map<Key, FIFOIter>::iterator;
    //! @brief Element metadata.
    struct Element
    {
        //! @brief Optional iterator pointing to the key in the hash map.
        std::optional<KeyedIter> keyedPosition{};
        //! @brief The value stored in the element.
        Value value{};
    };

    //! @brief The ordering of the FIFO queue of elements.
    std::list<Element> fifoList{};
    //! @brief Hash map for fast key-to-element lookup.
    std::unordered_map<Key, FIFOIter> keyedElements{};
    //! @brief Number of elements.
    std::size_t usedSize{0};
    //! @brief Mutex for controlling elements.
    mutable std::mutex mtx;

    //! @brief Operation of inserting or updating.
    //! @param key - key associated with the value
    //! @param value - value to be inserted or updated
    void doInsertOrUpdate(const Key& key, Value&& value);
    //! @brief Operation of inserting.
    //! @param key - key associated with the value
    //! @param value - value to be inserted
    void doInsert(const Key& key, Value&& value);
    //! @brief Operation of updating.
    //! @param keyedPos - iterator to the element in the hash map
    //! @param value - value to be updated
    void doUpdate(const KeyedIter keyedPos, Value&& value);
    //! @brief Operation of erasing.
    //! @param fifoPos - iterator to the element in the FIFO list
    void doErase(const FIFOIter fifoPos);
    //! @brief Operation of finding.
    //! @param key - key to search for
    //! @return optional value
    std::optional<Value> doFind(const Key& key);
};

template <typename Key, typename Value>
FIFO<Key, Value>::FIFO(const std::size_t capacity, const float maxLoadFactor) : fifoList{capacity}
{
    keyedElements.max_load_factor(maxLoadFactor);
    keyedElements.reserve(capacity);
}

template <typename Key, typename Value>
void FIFO<Key, Value>::insert(const Key& key, Value value)
{
    const std::lock_guard<std::mutex> lock(mtx);
    doInsertOrUpdate(key, std::move(value));
}

template <typename Key, typename Value>
template <typename Range>
std::size_t FIFO<Key, Value>::insertRange(Range&& keyValueRange)
{
    const std::lock_guard<std::mutex> lock(mtx);
    auto&& range = std::forward<Range>(keyValueRange);
    std::size_t inserted = 0;
    for (auto& [key, value] : range)
    {
        doInsertOrUpdate(key, std::move(value));
        ++inserted;
    }
    return inserted;
}

template <typename Key, typename Value>
bool FIFO<Key, Value>::erase(const Key& key)
{
    const std::lock_guard<std::mutex> lock(mtx);
    if (const auto keyedPos = keyedElements.find(key); keyedPos != keyedElements.cend())
    {
        doErase(keyedPos->second);
        return true;
    }
    return false;
}

template <typename Key, typename Value>
template <typename Iterator>
std::size_t FIFO<Key, Value>::erase(Iterator begin, const Iterator end)
{
    const std::lock_guard<std::mutex> lock(mtx);
    std::size_t erased = 0;
    while (begin != end)
    {
        if (const auto keyedPos = keyedElements.find(*begin); keyedPos != keyedElements.cend())
        {
            ++erased;
            doErase(keyedPos->second);
        }
        ++begin;
    }
    return erased;
}

template <typename Key, typename Value>
template <typename Range>
std::size_t FIFO<Key, Value>::eraseRange(const Range& keyRange)
{
    return erase(std::cbegin(keyRange), std::cend(keyRange));
}

template <typename Key, typename Value>
std::optional<Value> FIFO<Key, Value>::find(const Key& key)
{
    const std::lock_guard<std::mutex> lock(mtx);
    return doFind(key);
}

template <typename Key, typename Value>
template <typename Iterator>
auto FIFO<Key, Value>::find(Iterator begin, const Iterator end, const std::size_t distance)
    -> std::vector<std::pair<Key, std::optional<Value>>>
{
    const std::lock_guard<std::mutex> lock(mtx);
    std::vector<std::pair<Key, std::optional<Value>>> result{};
    if (distance > 0)
    {
        result.reserve(distance);
    }
    while (begin != end)
    {
        result.emplace_back(*begin, doFind(*begin));
        ++begin;
    }
    return result;
}

template <typename Key, typename Value>
template <typename Range>
auto FIFO<Key, Value>::findRange(const Range& keyRange) -> std::vector<std::pair<Key, std::optional<Value>>>
{
    return find(std::cbegin(keyRange), std::cend(keyRange), std::size(keyRange));
}

template <typename Key, typename Value>
template <typename Iterator>
void FIFO<Key, Value>::resolveRange(Iterator begin, const Iterator end)
{
    const std::lock_guard<std::mutex> lock(mtx);
    while (begin != end)
    {
        auto& [key, optValue] = *begin;
        optValue = doFind(key);
        ++begin;
    }
}

template <typename Key, typename Value>
template <typename Range>
void FIFO<Key, Value>::resolveRange(Range& keyOptValueRange)
{
    return resolveRange(std::begin(keyOptValueRange), std::end(keyOptValueRange));
}

template <typename Key, typename Value>
bool FIFO<Key, Value>::empty() const
{
    return usedSize == 0;
}

template <typename Key, typename Value>
std::size_t FIFO<Key, Value>::size() const
{
    return usedSize;
}

template <typename Key, typename Value>
std::size_t FIFO<Key, Value>::capacity() const
{
    return fifoList.size();
}

template <typename Key, typename Value>
void FIFO<Key, Value>::doInsertOrUpdate(const Key& key, Value&& value)
{
    if (const auto keyedPos = keyedElements.find(key); keyedPos != keyedElements.cend())
    {
        doUpdate(keyedPos, std::move(value));
    }
    else
    {
        doInsert(key, std::move(value));
    }
}

template <typename Key, typename Value>
void FIFO<Key, Value>::doInsert(const Key& key, Value&& value)
{
    fifoList.splice(fifoList.cend(), fifoList, fifoList.cbegin());
    auto lastElemPos = std::prev(fifoList.end());
    auto& elem = *lastElemPos;
    if (elem.keyedPosition.has_value())
    {
        keyedElements.erase(elem.keyedPosition.value());
    }
    else
    {
        ++usedSize;
    }

    elem.value = std::move(value);
    elem.keyedPosition = keyedElements.emplace(key, lastElemPos).first;
}

template <typename Key, typename Value>
void FIFO<Key, Value>::doUpdate(const KeyedIter keyedPos, Value&& value)
{
    auto& elem = *keyedPos->second;
    elem.value = std::move(value);
}

template <typename Key, typename Value>
void FIFO<Key, Value>::doErase(const FIFOIter fifoPos)
{
    auto& elem = *fifoPos;
    if (fifoPos != fifoList.cbegin())
    {
        fifoList.splice(fifoList.cbegin(), fifoList, fifoPos);
    }
    if (elem.keyedPosition.has_value())
    {
        keyedElements.erase(elem.keyedPosition.value());
        elem.keyedPosition = std::nullopt;
    }

    --usedSize;
}

template <typename Key, typename Value>
std::optional<Value> FIFO<Key, Value>::doFind(const Key& key)
{
    if (const auto keyedPos = keyedElements.find(key); keyedPos != keyedElements.cend())
    {
        const auto& elem = *keyedPos->second;
        return std::make_optional(elem.value);
    }
    return std::nullopt;
}

//! @brief The LFU (least frequently used) cache replacement policy.
//! @tparam Key - type of cache key
//! @tparam Value - type of value stored in the cache
template <typename Key, typename Value>
class LFU
{
public:
    //! @brief Construct a new LFU object.
    //! @param capacity - maximum number of elements the cache can hold
    //! @param maxLoadFactor - maximum load factor for the internal hash map
    explicit LFU(const std::size_t capacity, const float maxLoadFactor = 1.0F);

    //! @brief Insert or update an element by using the value by key.
    //! @param key - key associated with the value
    //! @param value - value to be inserted or updated
    void insert(const Key& key, Value value);
    //! @brief Insert a range of key-value pairs.
    //! @tparam Range - type of range of key-value pairs
    //! @param keyValueRange - range of key-value pairs to insert
    //! @return number of elements inserted
    template <typename Range>
    std::size_t insertRange(Range&& keyValueRange);
    //! @brief Erase an element by key.
    //! @param key - key of the element to remove
    //! @return be found and erased or not
    bool erase(const Key& key);
    //! @brief Erase a range of elements by using a range of keys.
    //! @tparam Range - type of range of keys
    //! @param keyRange - range of keys to be erased
    //! @return number of elements successfully erased
    template <typename Range>
    std::size_t eraseRange(const Range& keyRange);
    //! @brief Find a value by key.
    //! @param key - key to search for
    //! @param peek - whether peek
    //! @return optional value
    std::optional<Value> find(const Key& key, const bool peek = false);
    //! @brief Find a value with refcount by key.
    //! @param key - key to search for
    //! @param peek - whether peek
    //! @return optional value with refcount
    auto findWithRefcount(const Key& key, const bool peek = false) -> std::optional<std::pair<Value, std::size_t>>;
    //! @brief Find a range of values by keys by using a range of keys.
    //! @tparam Range - type of range of keys
    //! @param keyRange - range of keys to be searched
    //! @param peek - whether peek
    //! @return container of key-optional-value pairs
    template <typename Range>
    auto findRange(const Range& keyRange, const bool peek = false) -> std::vector<std::pair<Key, std::optional<Value>>>;
    //! @brief Resolve values for a range of keys in-place by using a range of key-optional-value pairs.
    //! @tparam Range - type of range of key-optional-value pairs
    //! @param keyOptValueRange - range of key-optional-value pairs to resolve
    //! @param peek - whether peek
    template <typename Range>
    void resolveRange(Range& keyOptValueRange, const bool peek = false);

    //! @brief Check whether any elements do not exist in the cache.
    //! @return any elements do not exist or exist
    bool empty() const;
    //! @brief Get the number of elements currently in the cache.
    //! @return number of used slots
    std::size_t size() const;
    //! @brief Get the total capacity of the cache.
    //! @return maximum number of elements the cache can hold
    std::size_t capacity() const;

private:
    struct Element;
    //! @brief Alias for the iterator of the internal open slot list.
    using OpenSlotIter = typename std::list<Element>::iterator;
    //! @brief Alias for the iterator of the internal LFU list.
    using LFUIter = typename std::multimap<std::size_t, OpenSlotIter>::iterator;
    //! @brief Alias for the iterator of the internal hash map.
    using KeyedIter = typename std::unordered_map<Key, OpenSlotIter>::iterator;
    //! @brief Element metadata.
    struct Element
    {
        //! @brief Iterator pointing to the key in the hash map.
        KeyedIter keyedPosition{};
        //! @brief Iterator pointing to the LFU list.
        LFUIter lfuPosition{};
        //! @brief The value stored in the element.
        Value value{};
    };

    //! @brief The open list of unused slots for elements.
    std::list<Element> openSlotList{};
    //! @brief The end of the open list. It is used to pull open slots from.
    typename std::list<Element>::iterator openSlotListEnd{};
    //! @brief The sorted map stores the number of times each element has been used.
    std::multimap<std::size_t, OpenSlotIter> lfuList{};
    //! @brief Hash map for fast key-to-element lookup.
    std::unordered_map<Key, OpenSlotIter> keyedElements{};
    //! @brief Number of elements.
    std::size_t usedSize{0};
    //! @brief Mutex for controlling elements.
    mutable std::mutex mtx;

    //! @brief Operation of inserting or updating.
    //! @param key - key associated with the value
    //! @param value - value to be inserted or updated
    void doInsertOrUpdate(const Key& key, Value&& value);
    //! @brief Operation of inserting.
    //! @param key - key associated with the value
    //! @param value - value to be inserted
    void doInsert(const Key& key, Value&& value);
    //! @brief Operation of updating.
    //! @param keyedPos - iterator to the element in the hash map
    //! @param value - value to be updated
    void doUpdate(const KeyedIter keyedPos, Value&& value);
    //! @brief Operation of erasing.
    //! @param openSlotPos - iterator to the element in the open slot list
    void doErase(const OpenSlotIter openSlotPos);
    //! @brief Operation of finding.
    //! @param key - key to search for
    //! @param peek - whether peek
    //! @return optional value
    std::optional<Value> doFind(const Key& key, const bool peek);
    //! @brief Operation of finding with refcount.
    //! @param key - key to search for
    //! @param peek - whether peek
    //! @return optional value with refcount
    auto doFindWithRefcount(const Key& key, const bool peek) -> std::optional<std::pair<Value, std::size_t>>;
    //! @brief Operation of accessing.
    //! @param elem - element to be accessed
    void doAccess(Element& elem);
    //! @brief Operation of pruning.
    void doPrune();
};

template <typename Key, typename Value>
LFU<Key, Value>::LFU(const std::size_t capacity, const float maxLoadFactor) :
    openSlotList{capacity}, openSlotListEnd{openSlotList.begin()}
{
    keyedElements.max_load_factor(maxLoadFactor);
    keyedElements.reserve(capacity);
}

template <typename Key, typename Value>
void LFU<Key, Value>::insert(const Key& key, Value value)
{
    const std::lock_guard<std::mutex> lock(mtx);
    doInsertOrUpdate(key, std::move(value));
}

template <typename Key, typename Value>
template <typename Range>
std::size_t LFU<Key, Value>::insertRange(Range&& keyValueRange)
{
    const std::lock_guard<std::mutex> lock(mtx);
    auto&& range = std::forward<Range>(keyValueRange);
    std::size_t inserted = 0;
    for (auto& [key, value] : range)
    {
        doInsertOrUpdate(key, std::move(value));
        ++inserted;
    }
    return inserted;
}

template <typename Key, typename Value>
bool LFU<Key, Value>::erase(const Key& key)
{
    const std::lock_guard<std::mutex> lock(mtx);
    if (const auto keyedPos = keyedElements.find(key); keyedPos != keyedElements.cend())
    {
        doErase(keyedPos->second);
        return true;
    }
    return false;
}

template <typename Key, typename Value>
template <typename Range>
std::size_t LFU<Key, Value>::eraseRange(const Range& keyRange)
{
    const std::lock_guard<std::mutex> lock(mtx);
    std::size_t erased = 0;
    for (const auto& key : keyRange)
    {
        if (const auto keyedPos = keyedElements.find(key); keyedPos != keyedElements.cend())
        {
            ++erased;
            doErase(keyedPos->second);
        }
    }
    return erased;
}

template <typename Key, typename Value>
std::optional<Value> LFU<Key, Value>::find(const Key& key, const bool peek)
{
    const std::lock_guard<std::mutex> lock(mtx);
    return doFind(key, peek);
}

template <typename Key, typename Value>
auto LFU<Key, Value>::findWithRefcount(const Key& key, const bool peek) -> std::optional<std::pair<Value, std::size_t>>
{
    const std::lock_guard<std::mutex> lock(mtx);
    return doFindWithRefcount(key, peek);
}

template <typename Key, typename Value>
template <typename Range>
auto LFU<Key, Value>::findRange(const Range& keyRange, const bool peek)
    -> std::vector<std::pair<Key, std::optional<Value>>>
{
    const std::lock_guard<std::mutex> lock(mtx);
    std::vector<std::pair<Key, std::optional<Value>>> result{};
    result.reserve(std::size(keyRange));
    for (const auto& key : keyRange)
    {
        result.emplace_back(key, doFind(key, peek));
    }
    return result;
}

template <typename Key, typename Value>
template <typename Range>
void LFU<Key, Value>::resolveRange(Range& keyOptValueRange, const bool peek)
{
    const std::lock_guard<std::mutex> lock(mtx);
    for (auto& [key, optValue] : keyOptValueRange)
    {
        optValue = doFind(key, peek);
    }
}

template <typename Key, typename Value>
bool LFU<Key, Value>::empty() const
{
    return usedSize == 0;
}

template <typename Key, typename Value>
std::size_t LFU<Key, Value>::size() const
{
    return usedSize;
}

template <typename Key, typename Value>
std::size_t LFU<Key, Value>::capacity() const
{
    return openSlotList.size();
}

template <typename Key, typename Value>
void LFU<Key, Value>::doInsertOrUpdate(const Key& key, Value&& value)
{
    if (const auto keyedPos = keyedElements.find(key); keyedPos != keyedElements.cend())
    {
        doUpdate(keyedPos, std::move(value));
    }
    else
    {
        doInsert(key, std::move(value));
    }
}

template <typename Key, typename Value>
void LFU<Key, Value>::doInsert(const Key& key, Value&& value)
{
    if (usedSize >= openSlotList.size())
    {
        doPrune();
    }

    const auto keyedPos = keyedElements.emplace(key, openSlotListEnd).first;
    const auto lfuPos = lfuList.emplace(1, openSlotListEnd);
    auto& elem = *openSlotListEnd;
    elem.value = std::move(value);
    elem.keyedPosition = keyedPos;
    elem.lfuPosition = lfuPos;

    ++openSlotListEnd;
    ++usedSize;
}

template <typename Key, typename Value>
void LFU<Key, Value>::doUpdate(const KeyedIter keyedPos, Value&& value)
{
    auto& elem = *keyedPos->second;
    elem.value = std::move(value);

    doAccess(elem);
}

template <typename Key, typename Value>
void LFU<Key, Value>::doErase(const OpenSlotIter openSlotPos)
{
    auto& elem = *openSlotPos;
    if (openSlotPos != std::prev(openSlotListEnd))
    {
        openSlotList.splice(openSlotListEnd, openSlotList, openSlotPos);
    }
    --openSlotListEnd;

    keyedElements.erase(elem.keyedPosition);
    lfuList.erase(elem.lfuPosition);
    --usedSize;
}

template <typename Key, typename Value>
std::optional<Value> LFU<Key, Value>::doFind(const Key& key, const bool peek)
{
    if (const auto keyedPos = keyedElements.find(key); keyedPos != keyedElements.cend())
    {
        auto& elem = *keyedPos->second;
        if (!peek)
        {
            doAccess(elem);
        }
        return std::make_optional(elem.value);
    }
    return std::nullopt;
}

template <typename Key, typename Value>
auto LFU<Key, Value>::doFindWithRefcount(const Key& key, const bool peek)
    -> std::optional<std::pair<Value, std::size_t>>
{
    if (const auto keyedPos = keyedElements.find(key); keyedPos != keyedElements.cend())
    {
        auto& elem = *keyedPos->second;
        if (!peek)
        {
            doAccess(elem);
        }
        return std::make_optional(std::make_pair(elem.value, elem.lfuPosition->first));
    }
    return std::nullopt;
}

template <typename Key, typename Value>
void LFU<Key, Value>::doAccess(Element& elem)
{
    const auto refcount = elem.lfuPosition->first;
    lfuList.erase(elem.lfuPosition);
    elem.lfuPosition = lfuList.emplace(refcount + 1, elem.keyedPosition->second);
}

template <typename Key, typename Value>
void LFU<Key, Value>::doPrune()
{
    if (usedSize > 0)
    {
        doErase(lfuList.cbegin()->second);
    }
}

//! @brief The LRU (least recently used) cache replacement policy.
//! @tparam Key - type of cache key
//! @tparam Value - type of value stored in the cache
template <typename Key, typename Value>
class LRU
{
public:
    //! @brief Construct a new LRU object.
    //! @param capacity - maximum number of elements the cache can hold
    //! @param maxLoadFactor - maximum load factor for the internal hash map
    explicit LRU(const std::size_t capacity, const float maxLoadFactor = 1.0F);

    //! @brief Insert or update an element by using the value by key.
    //! @param key - key associated with the value
    //! @param value - value to be inserted or updated
    void insert(const Key& key, Value value);
    //! @brief Insert a range of key-value pairs.
    //! @tparam Range - type of range of key-value pairs
    //! @param keyValueRange - range of key-value pairs to insert
    //! @return number of elements inserted
    template <typename Range>
    std::size_t insertRange(Range&& keyValueRange);
    //! @brief Erase an element by key.
    //! @param key - key of the element to remove
    //! @return be found and erased or not
    bool erase(const Key& key);
    //! @brief Erase a range of elements by using a range of keys.
    //! @tparam Range - type of range of keys
    //! @param keyRange - range of keys to be erased
    //! @return number of elements successfully erased
    template <typename Range>
    std::size_t eraseRange(const Range& keyRange);
    //! @brief Find a value by key.
    //! @param key - key to search for
    //! @param peek - whether peek
    //! @return optional value
    std::optional<Value> find(const Key& key, const bool peek = false);
    //! @brief Find a range of values by keys by using a range of keys.
    //! @tparam Range - type of range of keys
    //! @param keyRange - range of keys to be searched
    //! @param peek - whether peek
    //! @return container of key-optional-value pairs
    template <typename Range>
    auto findRange(const Range& keyRange, const bool peek = false) -> std::vector<std::pair<Key, std::optional<Value>>>;
    //! @brief Resolve values for a range of keys in-place by using a range of key-optional-value pairs.
    //! @tparam Range - type of range of key-optional-value pairs
    //! @param keyOptValueRange - range of key-optional-value pairs to resolve
    //! @param peek - whether peek
    template <typename Range>
    void resolveRange(Range& keyOptValueRange, const bool peek = false);

    //! @brief Check whether any elements do not exist in the cache.
    //! @return any elements do not exist or exist
    bool empty() const;
    //! @brief Get the number of elements currently in the cache.
    //! @return number of used slots
    std::size_t size() const;
    //! @brief Get the total capacity of the cache.
    //! @return maximum number of elements the cache can hold
    std::size_t capacity() const;

private:
    //! @brief Alias for the iterator of the internal LRU list.
    using LRUIter = std::list<std::size_t>::iterator;
    //! @brief Alias for the iterator of the internal hash map.
    using KeyedIter = typename std::unordered_map<Key, std::size_t>::iterator;
    //! @brief Element metadata.
    struct Element
    {
        //! @brief Iterator pointing to the key in the hash map.
        KeyedIter keyedPosition{};
        //! @brief Iterator pointing to the LRU list.
        LRUIter lruPosition;
        //! @brief The value stored in the element.
        Value value{};
    };

    //! @brief The main store for the key-value pairs.
    std::vector<Element> elements{};
    //! @brief The sorted list stores the index of each element.
    std::list<std::size_t> lruList;
    //! @brief The end of the LRU list. It is used to denote how many elements are in use.
    LRUIter lruListEnd;
    //! @brief Hash map for fast key-to-element lookup.
    std::unordered_map<Key, std::size_t> keyedElements{};
    //! @brief Number of elements.
    std::size_t usedSize{0};
    //! @brief Mutex for controlling elements.
    mutable std::mutex mtx;

    //! @brief Operation of inserting or updating.
    //! @param key - key associated with the value
    //! @param value - value to be inserted or updated
    void doInsertOrUpdate(const Key& key, Value&& value);
    //! @brief Operation of inserting.
    //! @param key - key associated with the value
    //! @param value - value to be inserted
    void doInsert(const Key& key, Value&& value);
    //! @brief Operation of updating.
    //! @param keyedPos - iterator to the element in the hash map
    //! @param value - value to be updated
    void doUpdate(const KeyedIter keyedPos, Value&& value);
    //! @brief Operation of erasing.
    //! @param elemIdx - index of element
    void doErase(const std::size_t elemIdx);
    //! @brief Operation of finding.
    //! @param key - key to search for
    //! @param peek - whether peek
    //! @return optional value
    std::optional<Value> doFind(const Key& key, const bool peek);
    //! @brief Operation of accessing.
    //! @param elem - element to be accessed
    void doAccess(Element& elem);
    //! @brief Operation of pruning.
    void doPrune();
};

template <typename Key, typename Value>
LRU<Key, Value>::LRU(const std::size_t capacity, const float maxLoadFactor) : elements{capacity}, lruList(capacity)
{
    std::iota(lruList.begin(), lruList.end(), 0);
    lruListEnd = lruList.begin();

    keyedElements.max_load_factor(maxLoadFactor);
    keyedElements.reserve(capacity);
}

template <typename Key, typename Value>
void LRU<Key, Value>::insert(const Key& key, Value value)
{
    const std::lock_guard<std::mutex> lock(mtx);
    doInsertOrUpdate(key, std::move(value));
}

template <typename Key, typename Value>
template <typename Range>
std::size_t LRU<Key, Value>::insertRange(Range&& keyValueRange)
{
    const std::lock_guard<std::mutex> lock(mtx);
    auto&& range = std::forward<Range>(keyValueRange);
    std::size_t inserted = 0;
    for (auto& [key, value] : range)
    {
        doInsertOrUpdate(key, std::move(value));
        ++inserted;
    }
    return inserted;
}

template <typename Key, typename Value>
bool LRU<Key, Value>::erase(const Key& key)
{
    const std::lock_guard<std::mutex> lock(mtx);
    if (const auto keyedPos = keyedElements.find(key); keyedPos != keyedElements.cend())
    {
        doErase(keyedPos->second);
        return true;
    }
    return false;
}

template <typename Key, typename Value>
template <typename Range>
std::size_t LRU<Key, Value>::eraseRange(const Range& keyRange)
{
    const std::lock_guard<std::mutex> lock(mtx);
    std::size_t erased = 0;
    for (const auto& key : keyRange)
    {
        if (const auto keyedPos = keyedElements.find(key); keyedPos != keyedElements.cend())
        {
            ++erased;
            doErase(keyedPos->second);
        }
    }
    return erased;
}

template <typename Key, typename Value>
std::optional<Value> LRU<Key, Value>::find(const Key& key, const bool peek)
{
    const std::lock_guard<std::mutex> lock(mtx);
    return doFind(key, peek);
}

template <typename Key, typename Value>
template <typename Range>
auto LRU<Key, Value>::findRange(const Range& keyRange, const bool peek)
    -> std::vector<std::pair<Key, std::optional<Value>>>
{
    const std::lock_guard<std::mutex> lock(mtx);
    std::vector<std::pair<Key, std::optional<Value>>> result{};
    result.reserve(std::size(keyRange));
    for (const auto& key : keyRange)
    {
        result.emplace_back(key, doFind(key, peek));
    }
    return result;
}

template <typename Key, typename Value>
template <typename Range>
void LRU<Key, Value>::resolveRange(Range& keyOptValueRange, const bool peek)
{
    const std::lock_guard<std::mutex> lock(mtx);
    for (auto& [key, optValue] : keyOptValueRange)
    {
        optValue = doFind(key, peek);
    }
}

template <typename Key, typename Value>
bool LRU<Key, Value>::empty() const
{
    return usedSize == 0;
}

template <typename Key, typename Value>
std::size_t LRU<Key, Value>::size() const
{
    return usedSize;
}

template <typename Key, typename Value>
std::size_t LRU<Key, Value>::capacity() const
{
    return elements.size();
}

template <typename Key, typename Value>
void LRU<Key, Value>::doInsertOrUpdate(const Key& key, Value&& value)
{
    if (const auto keyedPos = keyedElements.find(key); keyedPos != keyedElements.cend())
    {
        doUpdate(keyedPos, std::move(value));
    }
    else
    {
        doInsert(key, std::move(value));
    }
}

template <typename Key, typename Value>
void LRU<Key, Value>::doInsert(const Key& key, Value&& value)
{
    if (usedSize >= elements.size())
    {
        doPrune();
    }

    const auto elemIdx = *lruListEnd;
    const auto keyedPos = keyedElements.emplace(key, elemIdx).first;
    auto& elem = elements[elemIdx];
    elem.value = std::move(value);
    elem.lruPosition = lruListEnd;
    elem.keyedPosition = keyedPos;

    ++lruListEnd;
    ++usedSize;

    doAccess(elem);
}

template <typename Key, typename Value>
void LRU<Key, Value>::doUpdate(const KeyedIter keyedPos, Value&& value)
{
    auto& elem = elements[keyedPos->second];
    elem.value = std::move(value);

    doAccess(elem);
}

template <typename Key, typename Value>
void LRU<Key, Value>::doErase(const std::size_t elemIdx)
{
    auto& elem = elements[elemIdx];
    if (elem.lruPosition != std::prev(lruListEnd))
    {
        lruList.splice(lruListEnd, lruList, elem.lruPosition);
    }
    --lruListEnd;

    keyedElements.erase(elem.keyedPosition);
    --usedSize;
}

template <typename Key, typename Value>
std::optional<Value> LRU<Key, Value>::doFind(const Key& key, const bool peek)
{
    if (const auto keyedPos = keyedElements.find(key); keyedPos != keyedElements.cend())
    {
        const std::size_t elemIdx = keyedPos->second;
        auto& elem = elements[elemIdx];
        if (!peek)
        {
            doAccess(elem);
        }
        return std::make_optional(elem.value);
    }
    return std::nullopt;
}

template <typename Key, typename Value>
void LRU<Key, Value>::doAccess(Element& elem)
{
    lruList.splice(lruList.cbegin(), lruList, elem.lruPosition);
}

template <typename Key, typename Value>
void LRU<Key, Value>::doPrune()
{
    if (usedSize > 0)
    {
        doErase(lruList.back());
    }
}
} // namespace cache
} // namespace data_structure
