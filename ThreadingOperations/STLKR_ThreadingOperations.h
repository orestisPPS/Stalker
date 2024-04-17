//
// Created by hal9000 on 9/1/23.
//

#ifndef UNTITLED_THREADINGOPERATIONS_H
#define UNTITLED_THREADINGOPERATIONS_H


#include <stdexcept>
#include <memory>
#include <thread>
#include <vector>

using namespace std;

template<unsigned availableThreads>
class STLKR_ThreadingOperations {

public:
    

    template<typename ThreadJob>
    static void executeParallelJob(ThreadJob task, size_t arraySize) {
        unsigned int numThreads = std::min(availableThreads, static_cast<unsigned>(arraySize));
        unsigned blockSize = (arraySize / numThreads);

        std::vector<std::thread> threads;
        threads.reserve(numThreads); 
        for (unsigned int i = 0; i < numThreads; ++i) {
            unsigned start = i * blockSize;
            unsigned end = (i == numThreads - 1) ? arraySize : start + blockSize;
            threads.emplace_back(task, start, end);
        }
        for (auto &thread: threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    /**
    * \brief Executes the provided task in parallel across multiple threads with a reduction step.
    * 
    * This method distributes the task across available CPU cores. Each thread operates on a distinct segment
    * of the data and produces a local result. After all threads have completed their work, a reduction step
    * combines these local results into a single global result.
    * 
    * \tparam T The data type of the result (e.g., double, float).
    * \tparam ThreadJob A callable object type (function, lambda, functor).
    *
    * \param size The size of the data being processed.
    * \param task The callable object that describes the work each thread should execute and return a local result.
     * \param availableThreads The number of threads available for processing.
    * \param cacheLineSize An optional parameter to adjust for system's cache line size (default is 64 bytes).
    * 
    * \return The combined result after the reduction step.
    */
    template<typename ThreadJob, typename T>
    static T executeParallelJobWithReduction(ThreadJob task, size_t size) {
        //Determine the number of doubles that fit in a cache line
        unsigned int numThreads = std::min(availableThreads, static_cast<unsigned>(size));
        //Align block size to cache line size. Each block must be a multiple of the cache line size.
        unsigned blockSize = (size + numThreads - 1) / numThreads;

        vector<T> localResults(numThreads); 
        vector<thread> threads;

        for (unsigned int i = 0; i < numThreads; ++i) {
            unsigned start = i * blockSize;
            unsigned end = (i == numThreads - 1) ? size : start + blockSize;
            threads.emplace_back(thread([&](unsigned start, unsigned end, unsigned idx) {
                localResults[idx] = task(start, end);
            }, start, end, i));
        }
        for (auto &thread: threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        T finalResult = 0;
        for (T val: localResults) {
            finalResult += val;
        }
        return finalResult;
    }

    /**
    * \brief Executes the provided task in parallel across multiple threads with an incomplete reduction step.
    * 
    * This method distributes the task across available CPU cores. Each thread operates on a distinct segment
    * of the data and produces a local result. After all threads have completed their work, a reduction step
    * combines these local results into a single global result.
    * 
    * \tparam T The data type of the result (e.g., double, float).
    * \tparam ThreadJob A callable object type (function, lambda, functor).
    *
    * \param size The size of the data being processed.
    * \param task The callable object that describes the work each thread should execute and return a local result.
     * \param availableThreads The number of threads available for processing.
    * \param cacheLineSize An optional parameter to adjust for system's cache line size (default is 64 bytes).
    * 
    * \return The result vector after the reduction step.
    */
    template<typename ThreadJob, typename T>
    static vector<T> executeParallelJobWithIncompleteReduction(ThreadJob task, size_t size, unsigned cacheLineSize = 64) {
        unsigned doublesPerCacheLine = cacheLineSize / sizeof(T);
        unsigned int numThreads = std::min(availableThreads, static_cast<unsigned>(size));

        unsigned blockSize = (size + numThreads - 1) / numThreads;
        blockSize = (blockSize + doublesPerCacheLine - 1) / doublesPerCacheLine * doublesPerCacheLine;

        vector<T> localResults(numThreads);
        vector<thread> threads;

        for (unsigned int i = 0; i < numThreads; ++i) {
            unsigned start = i * blockSize;
            unsigned end = start + blockSize;
            if (start >= size) break;
            end = std::min(end, static_cast<unsigned>(size)); // Ensure 'end' doesn't exceed 'size'
            threads.push_back(thread([&](unsigned start, unsigned end, unsigned idx) {
                localResults[idx] = task(start, end);
            }, start, end, i));
        }

        for (auto &thread: threads) {
            thread.join();
        }
        return localResults;
    }

    template<typename ThreadJob>
    double executeParallelJobWithReductionForDoubles(ThreadJob task, unsigned int size) {
       auto resultsVector = executeParallelJobWithIncompleteReduction(task, size, availableThreads);
        double finalResult = 0;
        for (double val: resultsVector) {
            finalResult += val;
        }
        return finalResult;
    }
    
private:
    static_assert(availableThreads > 0, "The number of threads must be greater than 0.");
    static_assert(availableThreads <= std::thread::hardware_concurrency(), "The number of threads must be less than or equal to the number of available CPU cores.");
};

#endif //UNTITLED_THREADINGOPERATIONS_H
