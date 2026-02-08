#include "MemoryBenchmarks.h"
#include "VectorMathBenchmarks.h"
using namespace std;

int main() {
    
    auto path = std::filesystem::current_path().string() + "/Benchmarks/logs";
    
    auto memory = Benchmarks::MemoryBenchmarks(path);
    auto vectorMath = Benchmarks::VectorMathBenchmarks(path);
    
    memory.run();
    vectorMath.run();


    return 0;
}