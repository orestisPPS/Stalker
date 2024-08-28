//
// Created by hal9000 on 7/11/24.
//

#ifndef STALKER_STALKERVECTOR_PERFORMANCETESTS_H
#define STALKER_STALKERVECTOR_PERFORMANCETESTS_H
#include "../STLKR_PerformanceTestBase.h"
#include "StalkerVector_OperationsTests.h"
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
        using OperationsTests = StalkerVector_OperationsTests<T, unrollFactor>;
    public:
        StalkerVector_PerformanceTests(const std::string &path,
                                                unsigned size,
                                                list<TestConfig_OperationTypes> &operations,
                                                list<unsigned> &availableCores,
                                                list<CoreAffinityConfig> &threadAffinities,
                                                bool prefetching,
                                                TestConfig_FillType fillType,
                                                TestConfig_CompilerFlag flag) :
                                                STLKR_PerformanceTestBase("StalkerVector", path){
            _size = size;
            _operations = &operations;
            _availableCores = &availableCores;
            _fillType = fillType;
            _prefetching = prefetching;
            _affinities = &threadAffinities;
            _flag = flag;
        }
        
        
        void runTest() override {
            for (auto & operation : *_operations){
                _addLogParameters(operation);
                vector source(_size, _manager);
                for (auto i = 0; i < _size; i++) {
                    source[i] = static_cast<T>(i);
                }
                void (OperationsTests::*simdFunction)(vector& source, const std::string &name, Logs& logs, CPU_Manager &manager) = nullptr;
                void (OperationsTests::*stdFunction)(unsigned size, Logs &logs) = nullptr;
                for (auto &coreNum : *_availableCores) {
                    _manager.setAvailableCores(coreNum);
                    for (auto &affinity: *_affinities) {
                        _manager.setCoreAffinityConfig(affinity);
                        std::string name = "simd_cores" + std::to_string(coreNum) + "_" + _threadAffinityConfigToString(affinity);
                        cout<<name<<endl;
                        switch (operation) {
                            case SIMD_Copy:
                                simdFunction = &StalkerVector_OperationsTests<T, unrollFactor>::copySimd;
                                stdFunction = &StalkerVector_OperationsTests<T, unrollFactor>::copySTD;
                                break;
                            case SIMD_Fill:
                                simdFunction = &StalkerVector_OperationsTests<T, unrollFactor>::fillSimd;
                                stdFunction = &StalkerVector_OperationsTests<T, unrollFactor>::fillSTD;
                                break;
                            case SIMD_Scale:
                                simdFunction = &StalkerVector_OperationsTests<T, unrollFactor>::scaleSimd;
                                stdFunction = &StalkerVector_OperationsTests<T, unrollFactor>::scaleSTD;
                                break;
                            case SIMD_AreEqual:
                                simdFunction = &StalkerVector_OperationsTests<T, unrollFactor>::areEqualSimd;
                                stdFunction = &StalkerVector_OperationsTests<T, unrollFactor>::areEqualSTD;
                                break;
                            case SIMD_Sum:
                                simdFunction = &StalkerVector_OperationsTests<T, unrollFactor>::sumSimd;
                                stdFunction = &StalkerVector_OperationsTests<T, unrollFactor>::sumSTD;
                                break;
                            case SIMD_Add:
                                simdFunction = &StalkerVector_OperationsTests<T, unrollFactor>::addSimd;
                                stdFunction = &StalkerVector_OperationsTests<T, unrollFactor>::addSTD;
                                break;
                            case SIMD_Subtract:
                                simdFunction = &StalkerVector_OperationsTests<T, unrollFactor>::subtractSimd;
                                stdFunction = &StalkerVector_OperationsTests<T, unrollFactor>::subtractSTD;
                                break;
                            case SIMD_Multiply:
                                simdFunction = &StalkerVector_OperationsTests<T, unrollFactor>::multiplySimd;
                                stdFunction = &StalkerVector_OperationsTests<T, unrollFactor>::multiplySTD;
                                break;
                            case SIMD_DotProduct:
                                simdFunction = &StalkerVector_OperationsTests<T, unrollFactor>::dotSimd;
                                stdFunction = &StalkerVector_OperationsTests<T, unrollFactor>::dotSTD;
                                break;
                            default:
                                throw std::runtime_error("Unknown operation type");
                        }
                        if (simdFunction)
                            (OperationsTests().*simdFunction)(source, name, logs, _manager);
                    }
                }
                if (stdFunction)
                    (OperationsTests().*stdFunction)(_size, logs);
                
                logs.exportToCSV(_path + "/" + _operationsToString(operation), "StalkerVectorTest");
                logs.clearAllLogs();
            }
        }
        
    private:
        unsigned _size;
        list<TestConfig_OperationTypes>* _operations;
        TestConfig_FillType _fillType;
        list<unsigned> *_availableCores;
        list<CoreAffinityConfig> *_affinities;
        bool _prefetching;
        TestConfig_CompilerFlag _flag;
        

        
        
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

        void _addLogParameters(TestConfig_OperationTypes operation){
            logs.addParameter("operation ", _operationsToString(operation));
            logs.addParameter("_size", std::to_string(_size));
            logs.addParameter("unrollFactor", unrollFactor);
            logs.addParameter("alignment", 64);
            auto i = 0;
            for (auto & affinity : *_affinities){
                logs.addParameter("affinityConfig_" + std::to_string(++i), _threadAffinityConfigToString(affinity));
            }
            logs.addParameter("vector fill", _boolToOnOff(_prefetching));_vectorFillToString();
            logs.addParameter("prefetch", _boolToOnOff(_prefetching));
            logs.addParameter("compiler flag", _compilerFlagToString());
        }

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

        std::string _threadAffinityConfigToString(CoreAffinityConfig config) {
            switch (config) {
                case HyperThreadsEnabled:
                    return "hyperThreadSet";
                case ThreadPoolSet:
                    return "threadPoolSet";
                case SingleThreadSet:
                    return "singleThreadSet";
            }
        }

        std::string _vectorFillToString() {
            switch (_fillType) {
                case Zero:
                    return "Zero";
                case One:
                    return "1";
                case Index:
                    return "++index";
                case Random:
                    return "random";
                case None:
                    return "empty";
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
    };

}
#endif //STALKER_STALKERVECTOR_PERFORMANCETESTS_H
