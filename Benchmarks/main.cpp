#include <iostream>
#include "Memory.h"
#include "MathematicsBenchmarks.h"
#include <papi.h>
using namespace std;

int main() {
    cout << "ta quifsa ropt, te qifsha kurve, ha mut, m'qifsh karin, qift qeni nonen" << std::endl;
    
    auto path = std::filesystem::current_path().string() + "/Benchmarks/logs";
    auto memory = Benchmarks::MemoryBenchmarks(path);
    memory.run();

    auto math = Benchmarks::MathematicsBenchmarks(path);
    math.run();


    return 0;
}