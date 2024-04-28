//
// Created by hal9000 on 4/28/24.
//

#ifndef STALKER_TESTBASE_H
#define STALKER_TESTBASE_H
#include <chrono>
#include <iostream>

class TestBase {
protected:
    //start timer
    static inline std::chrono::time_point<std::chrono::high_resolution_clock> startTimer(){
        return std::chrono::high_resolution_clock::now();
    }
    static inline std::chrono::time_point<std::chrono::high_resolution_clock> endTimer(){
        return std::chrono::high_resolution_clock::now();
    }
    static inline std::chrono::duration<double> getDuration(std::chrono::time_point<std::chrono::high_resolution_clock> start,
                                                            std::chrono::time_point<std::chrono::high_resolution_clock> end){
        return std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    }
}
#endif //STALKER_TESTBASE_H
