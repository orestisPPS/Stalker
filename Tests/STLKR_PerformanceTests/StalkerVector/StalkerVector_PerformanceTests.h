//
// Created by hal9000 on 7/11/24.
//

#ifndef STALKER_STALKERVECTOR_PERFORMANCETESTS_H
#define STALKER_STALKERVECTOR_PERFORMANCETESTS_H
#include "../STLKR_PerformanceTestBase.h"
#include "../../../StalkerVector/StalkerVector.h"

namespace STLKR_Tests{
    
    enum TestConfig_Hyperthreading{
        Enabled,
        Disabled,
        EnabledDisabled
    };
    
    enum TestConfig_TemplateTypes{
        Float,
        Double,
        Int,
        Unsigned,
        Short
    };
    
    enum TestConfig_OperationTypes{
        SIMD_Copy,
        SIMD_Fill,
        SIMD_Scale,
        SIMD_AreEqual,
        SIMD_Sum,
        SIMD_Add,
        SIMD_Subtract,
        SIMD_Multiply,
        SIMD_DotProduct,
    };
    
    enum TestConfig_CompilerFlag{
        O0,
        O1,
        O2,
        O3
    };
    
    enum TestConfig_FillType{
        Zero,
        One,
        Index,
        Random,
        None
    };
    
    template<typename T, unsigned unrollFactor>
    class StalkerVector_PerformanceTests : public STLKR_PerformanceTestBase {
        using vector = StalkerVector<T, unrollFactor>;
    public:
        StalkerVector_PerformanceTests(const std::string &path,
                                                unsigned _size,
                                                list<TestConfig_TemplateTypes>& types, 
                                                list<TestConfig_OperationTypes> &operations,
                                                list<unsigned> &availableCores,
                                                TestConfig_Hyperthreading hyperthreading,
                                                bool prefetching,
                                                TestConfig_FillType fillType,
                                                TestConfig_CompilerFlag flag) :
                                                STLKR_PerformanceTestBase("StalkerVector", path){
            _size = _size;
            _types = &types;
            _operations = &operations;
            _availableCores = &availableCores;
            _fillType = fillType;
            _prefetching = prefetching;
            _hyperthreading = hyperthreading;
            _flag = flag;
        }
        
        
        void runTest() override {
//            for (auto & operation : *_operations){
//                logs.addParameter("operation ", _operationsToString(operation));
//                logs.addParameter("_size", std::to_string(_size));
//                logs.addParameter("unrollFactor", unrollFactor);
//                logs.addParameter("alignment", 64);
//                logs.addParameter("prefetch", _boolToOnOff(_prefetching));
//                logs.addParameter("hyperThreading", _boolToOnOff(_hyperthreading != Disabled));
//                logs.addParameter("compiler flag", _compilerFlagToString());
//                auto iType = 0;
//                for (auto & type : *_types) {
//                    logs.addParameter("type" + std::to_string(++iType), _templateTypesToString(type));
//                    for (auto & core : *_availableCores) {
//                        _manager.setAvailableCores(core);
//                        switch (_hyperthreading) {
//                            case Enabled:
//                                _manager.enableHyperthreading(true);
//                                //do stuff
//                                break;
//                            case Disabled:
//                                _manager.enableHyperthreading(false);
//                                //do stuff
//                                break;
//                            case EnabledDisabled:
//                                _manager.enableHyperthreading(true);
//                                //do stuff
//                                _manager.enableHyperthreading(false);
//                                //do stuff
//                                break;
//                        }
//                    }
//                }
//                
//                iType = 0;
//                logs.exportToCSV(_path + "/" + _operationsToString(operation), "StalkerVectorTest");
//                
//            }
            _runOperationTest();
            logs.clearAllLogs();
        }
        
    private:
        unsigned _size;
        list<TestConfig_TemplateTypes>* _types;
        list<TestConfig_OperationTypes>* _operations;
        TestConfig_FillType _fillType;
        list<unsigned> *_availableCores;
        TestConfig_Hyperthreading _hyperthreading;
        bool _prefetching;
        TestConfig_CompilerFlag _flag;

        std::string _operationsToString(TestConfig_OperationTypes operation){
            switch (operation) {
                case SIMD_Copy:
                    return "Copy";
                case SIMD_Fill:
                    return "Fill";
                case SIMD_Scale:
                    return "Scale";
                case SIMD_AreEqual:
                    return "AreEqual";
                case SIMD_Sum:
                    return "Sum";
                case SIMD_Add:
                    return "Add";
                case SIMD_Subtract:
                    return "Subtract";
                case SIMD_Multiply:
                    return "Multiply";
                case SIMD_DotProduct:
                    return "DotProduct";
            }
        }

        std::string _boolToOnOff(bool on){
            return on ? "on" : "off";
        }
        
        std::string _compilerFlagToString() {
            switch (_flag) {
                case O0:
                    return "-O0";
                case 01:
                    return "-O1";
                case 02:
                    return "-O2";
                case 03:
                    return "-O3";
            }
        }

        std::string _templateTypesToString(TestConfig_TemplateTypes t){
            switch (t) {
                case Float:
                    return "float";
                case Double:
                    return "double";
                case Int:
                    return "int";
                case Unsigned:
                    return "unsigned";
                case Short:
                    return "short";
            }
        }
        
        void _runOperationTest(){
            StalkerVector<T, unrollFactor> source(_size, _manager);
            for (auto i = 0; i < _size; i++) {
                source[i] = static_cast<T>(i);
            }
            _manager.setCoreAffinityConfig(SingleThreadSet);
            vector v1(_size, _manager);
            _manager.setAvailableCores(1);
            logs.startSingleObservationTimer("SIMD_cores1", STLKR_TimeUnit::microseconds);
            v1.copy(source);
            logs.stopSingleObservationTimer("SIMD_cores1", STLKR_TimeUnit::microseconds);

            vector v2(_size, _manager);
            _manager.setAvailableCores(2);
            logs.startSingleObservationTimer("SIMD_cores2", STLKR_TimeUnit::microseconds);
            v2.copy(source);
            logs.stopSingleObservationTimer("SIMD_cores2", STLKR_TimeUnit::microseconds);

            vector v3(_size, _manager);
            _manager.setAvailableCores(4);
            logs.startSingleObservationTimer("SIMD_cores4", STLKR_TimeUnit::microseconds);
            v3.copy(source);
            logs.stopSingleObservationTimer("SIMD_cores4", STLKR_TimeUnit::microseconds);
            T* data1 = new T[_size];
            T* data2 = new T[_size];
            for (auto i = 0; i < _size; i++) {
                data1[i] = static_cast<T>(i);
            }
            logs.startSingleObservationTimer("memcpy_noSIMD", STLKR_TimeUnit::microseconds);
            std::memcpy(data1, data2, _size * sizeof(T));
            logs.stopSingleObservationTimer("memcpy_noSIMD", STLKR_TimeUnit::microseconds);
            delete[] data1;
            delete[] data2;

            T* data3 = new T[_size];
            T* data4 = new T[_size];
            logs.startSingleObservationTimer("loopCopy_noSIMD", STLKR_TimeUnit::microseconds);
            for (auto i = 0; i < _size; i++) {
                data3[i] = data4[i];
            }
            logs.stopSingleObservationTimer("loopCopy_noSIMD", STLKR_TimeUnit::microseconds);

            delete[] data3;
            delete[] data4;
        }
        
        void performSimdOperation(TestConfig_OperationTypes operation, vector* v1, vector* v2, vector* v3 ){
            switch (operation) {
                case SIMD_Copy:
                    v1->copy(*v2);
                    break;
                case SIMD_Fill:
                    v1->fill(0);
                case SIMD_Scale:
                    v1->scale(2);
                case SIMD_AreEqual:
                    v1->areEqual(*v2);
                case SIMD_Sum:
                    v1->sum();
                case SIMD_Add:
                    v1->add(*v2, *v3, 1, 1);
                case SIMD_Subtract:
                    v1->subtract(*v2, *v3, 1, 1);
                case SIMD_Multiply:
                    v1->multiply(*v2, *v3, 1, 1);
                case SIMD_DotProduct:
                    v1->SIMD_DotProduct(*v2);
            }
        }

        vector** _createVectors(unsigned numVectors){
            auto vectorOfVectors = new vector*[numVectors];
            for (int i = 0; i < numVectors; ++i) {
                switch (_fillType) {
                    case Zero:
                        vectorOfVectors[i] = new vector(_size, 0, _manager);
                        break;
                    case One:
                        vectorOfVectors[i] = new vector(_size, 1, _manager);
                        break;
                    case None:
                        vectorOfVectors[i] = new vector(_size, _manager);
                        break;
                    case Index:
                        vectorOfVectors[i] = new vector(_size, _manager);
                        for (auto &element: *vectorOfVectors[i])
                            element = i;
                        break;
                    case Random:
                        vectorOfVectors[i] = new vector(_size, _manager);
                        std::random_device rd;
                        std::mt19937 gen(rd());  // Use Mersenne Twister as the random number generator

                        // Depending on the type T, choose an appropriate distribution
                        // Conditional creation of distributions based on T
                        std::uniform_int_distribution<typename std::enable_if<std::is_integral<T>::value, T>::type> intDist(
                                std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
                        std::uniform_real_distribution<typename std::enable_if<std::is_floating_point<T>::value, T>::type> floatDist(0.0, 1.0);
                        for (auto &element: *vectorOfVectors[i]) {
                            if constexpr (std::is_integral<T>::value) {
                                element = intDist(gen);  // For integral types (int, short, unsigned)
                            } else if constexpr (std::is_floating_point<T>::value) {
                                element = floatDist(gen);  // For floating-point types (float, double)
                            }
                        }
                        break;

                }
                
            }
        }
    };
    

}
#endif //STALKER_STALKERVECTOR_PERFORMANCETESTS_H
