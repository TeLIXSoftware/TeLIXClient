#include <unordered_map>
#include <string>
#include <iostream>

class Cache {
public:
    std::string get(const std::string& key) {
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            return it->second;
        }
        return ""; // Or handle cache miss
    }

    void put(const std::string& key, const std::string& value) {
        cache_[key] = value;
    }

private:
    std::unordered_map<std::string, std::string> cache_;
};