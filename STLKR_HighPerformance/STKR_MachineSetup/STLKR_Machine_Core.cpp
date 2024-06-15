//
// Created by hal9000 on 6/15/24.
//

#include "STLKR_Machine_Core.h"

#include <utility>

STLKR_Machine_Core::STLKR_Machine_Core(unsigned id, std::vector<STLKR_Machine_Thread> threads) {
    _id = id;
    _threads = std::move(threads);
}