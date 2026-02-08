/*
 * Copyright 2024-2026 Papas Christoforos Orestis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//
// Created by hal9000 on 6/15/24.
//

#ifndef STALKER_CACHELEVEL_H
#define STALKER_CACHELEVEL_H

#include <iostream>
#include <vector>
#include <unordered_map>

namespace Stalker::Threading {

enum CacheLevelType {
    L1_Data = 0,
    L1_Instructions = 1,
    L2 = 2,
    L3 = 3,
    LNone = -1
};

class CacheLevel {
public:
    CacheLevel(CacheLevelType level, unsigned size , const std::vector<unsigned> &threads) :
    _level(level), _size(size), _threads(threads) {}
    CacheLevel() : _level(LNone), _size(0), _threads(std::vector<unsigned>()) {}
    ~CacheLevel() = default;
    
    [[nodiscard]] inline unsigned getLevel() const { return _level; }
    [[nodiscard]] inline unsigned size() const { return _size; }
    [[nodiscard]] inline constexpr unsigned getSizeInCacheLines() const { return (_size + 63) / 64; }
    [[nodiscard]] inline const std::vector<unsigned> &getThreads() const{ return _threads; }
private:
    CacheLevelType _level;
    unsigned _size;
    std::vector<unsigned> _threads;
};


class SharedCache {
public:
    SharedCache(CacheLevel *l1Data, CacheLevel *l1Instructions, CacheLevel *l2, CacheLevel *l3) {
        _cacheLevels = { {L1_Data, l1Data}, {L1_Instructions, l1Instructions}, {L2, l2}, {L3, l3} };
    }

    ~SharedCache() = default;

    inline const CacheLevel* getLevel(CacheLevelType level) const {
        auto it = _cacheLevels.find(level);
        if (it != _cacheLevels.end()) {
            return it->second;
        }
        return nullptr;
    }
private:
    std::unordered_map<CacheLevelType, CacheLevel*> _cacheLevels;
};

} // namespace Stalker::Threading



#endif //STALKER_CACHELEVEL_H
