//
// Created by hal9000 on 8/28/24.
//

#ifndef STALKER_STALKERVECTOR_OPERATIONSTESTS_H
#define STALKER_STALKERVECTOR_OPERATIONSTESTS_H

#include <string>
#include "Units/STLKR_Units.h"
#include "StalkerVector.h"
#include "Logs.h"

template <typename T, unsigned unrollFactor>
class StalkerVector_OperationsTests{
public:
    
    inline void copySimd(StalkerVector<T, unrollFactor>& source, const std::string &name, Logs& logs, CPU_Manager &manager){
        StalkerVector<T, unrollFactor> destination(source.size(), manager);
        logs.startSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        destination.copy(source);
        logs.stopSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
    }

    inline void fillSimd(StalkerVector<T, unrollFactor>& source, const std::string &name, Logs& logs, CPU_Manager &manager){
        logs.startSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        source.fill(33);
        logs.stopSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
    }

    inline void scaleSimd(StalkerVector<T, unrollFactor>& source, const std::string &name, Logs& logs, CPU_Manager &manager){
        logs.startSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        source.scale(33);
        logs.stopSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
    }

    inline void areEqualSimd(StalkerVector<T, unrollFactor>& source, const std::string &name, Logs& logs, CPU_Manager &manager){
        StalkerVector<T, unrollFactor> other(source.size(), manager);
        logs.startSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        bool result = source.areEqual(other);
        logs.stopSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
    }

    inline void sumSimd(StalkerVector<T, unrollFactor>& source, const std::string &name, Logs& logs, CPU_Manager &manager){
        logs.startSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        auto sum = source.sum();
        logs.stopSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
    }

    inline void addSimd(StalkerVector<T, unrollFactor>& source, const std::string &name, Logs& logs, CPU_Manager &manager){
        StalkerVector<T, unrollFactor> other(source.size(), manager);
        StalkerVector<T, unrollFactor> result(source.size(), manager);
        logs.startSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        source.add(other, result, 1, 1);
        logs.stopSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
    }

    inline void subtractSimd(StalkerVector<T, unrollFactor>& source, const std::string &name, Logs& logs, CPU_Manager &manager){
        StalkerVector<T, unrollFactor> other(source.size(), manager);
        StalkerVector<T, unrollFactor> result(source.size(), manager);
        logs.startSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        source.subtract(other, result, 1, 1);
        logs.stopSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
    }

    inline void multiplySimd(StalkerVector<T, unrollFactor>& source, const std::string &name, Logs& logs, CPU_Manager &manager){
        StalkerVector<T, unrollFactor> other(source.size(), manager);
        StalkerVector<T, unrollFactor> result(source.size(), manager);
        logs.startSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        source.multiply(other, result, 1, 1);
        logs.stopSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
    }

    inline void dotSimd(StalkerVector<T, unrollFactor>& source, const std::string &name, Logs& logs, CPU_Manager &manager){
        StalkerVector<T, unrollFactor> other(source.size(), manager);
        logs.startSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        auto dot = source.dotProduct(other);
        logs.stopSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
    }

    inline void copySTD(unsigned size, Logs &logs){
        T* source = new T[size];
        T* destination1 = new T[size];
        std::fill(source, source + size, 0);
        std::fill(destination1, destination1 + size, 0);

        auto name = "memcpy_noSIMD";
        logs.startSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        std::memcpy(destination1, source, size * sizeof(T));
        logs.stopSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        cout<<"memcpy_noSIMD"<<endl;
        delete[] destination1;

        T* destination2 = new T[size];
        std::fill(destination2, destination2 + size, 0);
        
        name = "loopCopy_noSIMD";
        logs.startSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        for (auto i = 0; i < size; i++) {
            destination2[i] = source[i];
        }
        logs.stopSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        cout<<name<<endl;

        delete[] destination2;
        delete[] source;
    }

    inline void fillSTD(unsigned size, Logs &logs) {
        T *data = new T[size];
        std::fill(data, data + size, 0);
        
        auto name = "fill_noSIMD";
        logs.startSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        std::fill(data, data + size, static_cast<T>(33));
        logs.stopSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        cout << name << endl;

        name = "loopFill_noSIMD";
        logs.startSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        for (auto i = 0; i < size; i++) {
            *data = 33;
        }
        logs.stopSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        cout << name << endl;
    }

    inline void scaleSTD(unsigned size, Logs &logs) {
        T *data = new T[size];
        std::fill(data, data + size, 0);
        
        auto name = "loopScale_noSIMD";
        logs.startSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        for (auto i = 0; i < size; i++) {
            *data *= 33;
        }
        logs.stopSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        cout << name << endl;
    }

    inline void areEqualSTD(unsigned size, Logs &logs) {
        T *data1 = new T[size];
        T *data2 = new T[size];
        std::fill(data1, data1 + size, 0);
        std::fill(data2, data2 + size, 0);

        auto name = "memcmp_noSIMD";
        logs.startSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        bool isEqual = std::memcmp(data1, data2, size * sizeof(T)) == 0;
        logs.stopSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        cout << name << ": " << (isEqual ? "Equal" : "Not Equal") << endl;

        name = "loopEqual_noSIMD";
        logs.startSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        isEqual = true;
        for (auto i = 0; i < size; i++) {
            if (data1[i] != data2[i]) {
                isEqual = false;
                break;
            }
        }
        logs.stopSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        cout << name << ": " << (isEqual ? "Equal" : "Not Equal") << endl;

        delete[] data1;
        delete[] data2;
    }

    inline void sumSTD(unsigned size, Logs &logs) {
        T *data = new T[size];
        std::fill(data, data + size, 0);

        auto name = "loopSum_noSIMD";
        logs.startSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        T sum = 0;
        for (auto i = 0; i < size; i++) {
            sum += data[i];
        }
        logs.stopSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        cout << name << ": " << sum << endl;

        delete[] data;
    }

    inline void addSTD(unsigned size, Logs &logs) {
        T *data1 = new T[size];
        T *data2 = new T[size];
        T *result = new T[size];
        std::fill(data1, data1 + size, 0);
        std::fill(data2, data2 + size, 0);
        std::fill(result, result + size, 0);
        
        auto name = "loopAdd_noSIMD";
        logs.startSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        for (auto i = 0; i < size; i++)
            result[i] = 33 * data1[i] + 33 * data2[i];
        logs.stopSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        cout << name << endl;

        delete[] data1;
        delete[] data2;
        delete[] result;
    }

    inline void subtractSTD(unsigned size, Logs &logs) {
        T *data1 = new T[size];
        T *data2 = new T[size];
        T *result = new T[size];
        std::fill(data1, data1 + size, 0);
        std::fill(data2, data2 + size, 0);
        std::fill(result, result + size, 0);

        auto name = "loopSubtract_noSIMD";
        logs.startSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        for (auto i = 0; i < size; i++)
            result[i] = 33 * data1[i] - 33 * data2[i];
        logs.stopSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        cout << name << endl;

        delete[] data1;
        delete[] data2;
        delete[] result;
    }

    inline void multiplySTD(unsigned size, Logs &logs) {
        T *data1 = new T[size];
        T *data2 = new T[size];
        T *result = new T[size];
        std::fill(data1, data1 + size, 0);
        std::fill(data2, data2 + size, 0);
        std::fill(result, result + size, 0);


        auto name = "loopMultiply_noSIMD";
        logs.startSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        for (auto i = 0; i < size; i++)
            result[i] = 33 * data1[i] * 33 * data2[i];
        logs.stopSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        cout << name << endl;

        delete[] data1;
        delete[] data2;
        delete[] result;
    }

    inline void dotSTD(unsigned size, Logs &logs) {
        T *data1 = new T[size];
        T *data2 = new T[size];
        std::fill(data1, data1 + size, 0);
        std::fill(data2, data2 + size, 0);

        auto name = "loopDotProduct_noSIMD";
        logs.startSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        T dotProduct = 0;
        for (auto i = 0; i < size; i++) {
            dotProduct += data1[i] * data2[i];
        }
        logs.stopSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
        cout << name << ": " << dotProduct << endl;

        delete[] data1;
        delete[] data2;
    }

};
#endif //STALKER_STALKERVECTOR_OPERATIONSTESTS_H
//parent->getAvailableThreadsCount() > 1 ? _hyperThreadCorePool[parent] = true : _ecoCorePool[parent] = true;
