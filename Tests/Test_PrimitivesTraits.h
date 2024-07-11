//
// Created by hal9000 on 7/11/24.
//

#ifndef STALKER_TEST_PRIMITIVESTRAITS_H
#define STALKER_TEST_PRIMITIVESTRAITS_H


#include <string>
#include <list>

template <typename T>
struct Test_PrimitivesTraits;

template<> struct Test_PrimitivesTraits<float> {
    using Type = float;
    static std::string testName(const std::string& name){
        return name + " (float)";
    }
};

template<> struct Test_PrimitivesTraits<double> {
    using Type = double;
    static std::string testName(const std::string& name){
        return name + " (double)";
    }
};

template<> struct Test_PrimitivesTraits<int> {
    using Type = int;
    static std::string testName(const std::string& name){
        return name + " (int)";
    }
};

template<> struct Test_PrimitivesTraits<short> {
    using Type = short;
    static std::string testName(const std::string& name){
        return name + " (short)";
    }
};

template<> struct Test_PrimitivesTraits<unsigned> {
    using Type = unsigned;
    static std::string testName(const std::string& name){
        return name + " (unsigned)";
    }
};






#endif //STALKER_TEST_PRIMITIVESTRAITS_H
