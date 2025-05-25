#include "StalkerTestRunner.h"
int main() {
    
    STLKR_Tests::StalkerTestRunner().runTests(); // Calls all header-only tests
    return 0; // Exit with success
}
