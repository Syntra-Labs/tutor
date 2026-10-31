#pragma once

#include <cstddef>
#include <list>
#include <optional>
#include <unordered_map>

template <typename K, typename V>
class LruCache {
public:
    explicit LruCache(size_t capacity) : capacity_(capacity) {}

    std::optional<V> get(const K& key) {
        // TODO: Implement O(1) lookup & promotion
        (void)key;
        return std::nullopt;
    }

    void put(const K& key, const V& value) {
        // TODO: Implement O(1) insertion & LRU eviction
        (void)key;
        (void)value;
    }

    [[nodiscard]] size_t size() const noexcept {
        return items_.size();
    }

private:
    size_t capacity_;
    std::list<std::pair<K, V>> items_;
    std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> index_;
};
