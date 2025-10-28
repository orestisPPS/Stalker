#include <iostream>
#include "Memory.h"
#include <papi.h>
using namespace std;

int main() {
    cout << "ta quifsa ropt, te qifsha kurve, ha mut, m'qifsh karin, qift qeni nonen" << std::endl;
    auto op = Benchmarks::Memory();
    op.run();

    //     // --- Initialize PAPI ---
    // if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
    //     std::cerr << "PAPI init error\n";
    //     return 1;
    // }

    // // --- Create an event set ---
    // int eventSet = PAPI_NULL;
    // if (PAPI_create_eventset(&eventSet) != PAPI_OK) {
    //     std::cerr << "Failed to create event set\n";
    //     return 1;
    // }

    // // --- Add events to count ---
    // if (PAPI_add_event(eventSet, PAPI_TOT_CYC) != PAPI_OK)
    //     std::cerr << "Could not add PAPI_TOT_CYC\n";
    // if (PAPI_add_event(eventSet, PAPI_FP_OPS) != PAPI_OK)
    //     std::cerr << "Could not add PAPI_FP_OPS\n";

    // // --- Start counting ---
    // if (PAPI_start(eventSet) != PAPI_OK) {
    //     std::cerr << "Failed to start counters\n";
    //     return 1;
    // }

    // // --- Do something measurable ---
    // volatile double sum = 0.0;
    // for (int i = 0; i < 10000000; ++i)
    //     sum += 1.618 * 3.1415;  // simple FP math

    // // --- Stop and read counters ---
    // long long values[2] = {0, 0};
    // if (PAPI_stop(eventSet, values) != PAPI_OK) {
    //     std::cerr << "Failed to stop counters\n";
    //     return 1;
    // }

    // std::cout << "Sum = " << sum << "\n";
    // std::cout << "Total cycles: " << values[0] << "\n";
    // std::cout << "Floating-point ops: " << values[1] << "\n";

    // PAPI_shutdown();
    return 0;
}