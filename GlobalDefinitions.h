#ifndef GLOBAL_DEFINITIONS_H
#define GLOBAL_DEFINITIONS_H

#define RAWDOGGY_SIZE true
#define RAWDOGGY_INDEX true
#define RAWDOGGY_MATRIX_INDECES true

constexpr bool RawDoggySize = RAWDOGGY_SIZE;
constexpr bool RawDoggyIndex = RAWDOGGY_INDEX;
constexpr bool RawDoggyMatrixIndeces = RAWDOGGY_MATRIX_INDECES;

constexpr unsigned UnrollFactor = 4;
constexpr unsigned UnrollFactorSIMD = 64;
constexpr unsigned CacheLineSize = 64;
constexpr unsigned Alignment = 64;


#endif // GLOBAL_DEFINITIONS_H