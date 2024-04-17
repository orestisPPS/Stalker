
#include <iostream>
#include <cassert>
#include <cmath>
#include "../LinearAlgebra/ContiguousMemoryStorage/NumericalVector.h"
using namespace LinearAlgebra;

namespace Tests{

    class NumericalVectorTest {
    public:
        static void runTests() {
            std::cout << "Running NumericalVector Tests...\n";
            std::cout << "--------------------------------\n";

            testInitialization();
            testSum();
            testMagnitude();
            testAddition();
            testAdditionIntoThis();
            testSubtraction();
            testSubtractionIntoThis();
            testDotProduct();
            testScaling();
            testNormalize();
            testDistance();
            testAngle();
            testAverage();
            testVariance();
            testStandardDeviation();
            testCovariance();
            testCorrelation();
            testNorms();
            //testProjection();
            //testHouseHolderTransformation();
            testSumMultiThread();
            testMagnitudeMultiThread();
            testAdditionMultiThread();
            testAdditionIntoThisMultiThread();
            testSubtractionMultiThread();
            testSubtractionIntoThisMultiThread();
            testDotProductMultiThread();
            testScalingMultiThread();
            testNormalizeMultiThread();
            testDistanceMultiThread();
            testAngleMultiThread();
            testAverageMultiThread();
            testVarianceMultiThread();
            testStandardDeviationMultiThread();
            testCovarianceMultiThread();
            testCorrelationMultiThread();
            testNormsMultiThread();

            std::cout << "\nAll tests passed!" << std::endl;
        }

    private:
        static void logTestStart(const std::string& testName) {
            std::cout << "Running " << testName << "... ";
        }

        static void logTestEnd() {
            std::cout << "\033[1;32m[PASSED]\033[0m\n";  // This adds a green [PASSED] indicator
        }

        static void testInitialization() {
            logTestStart("testInitialization");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
            assert(vec.size() == 10);
            logTestEnd();
        }

        static void testSum() {
            logTestStart("testSum");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
            assert(vec.sum() == 55);
            logTestEnd();
        }

        static void testMagnitude() {
            logTestStart("testMagnitude");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
            assert(std::fabs(vec.magnitude() - 19.6214) < 0.001);
            logTestEnd();
        }

        static void testAddition() {
            logTestStart("testAddition");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
            NumericalVector<double> result(10);
            vec.add(vec, result);
            assert(result.sum() == 110);
            logTestEnd();
        }

        static void testAdditionIntoThis() {
            logTestStart("testAdditionIntoThis");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
            vec.addIntoThis(vec);
            assert(vec.sum() == 110);
            logTestEnd();
        }

        static void testSubtraction() {
            logTestStart("testSubtraction");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
            NumericalVector<double> result(10);
            vec.subtract(vec, result);
            assert(result.sum() == 0);
            logTestEnd();
        }

        static void testSubtractionIntoThis() {
            logTestStart("testSubtractionIntoThis");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
            vec.subtractIntoThis(vec);
            assert(vec.sum() == 0);
            logTestEnd();
        }

        static void testDotProduct() {
            logTestStart("testDotProduct");
            NumericalVector<double> vec1({1, 2, 3, 4, 5});
            NumericalVector<double> vec2({1, 2, 3, 4, 5});
            assert(vec1.dotProduct(vec2) == 55);
            logTestEnd();
        }

        static void testScaling() {
            logTestStart("testScaling");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
            vec.scale(2);
            assert(vec.sum() == 110);
            logTestEnd();
        }

        static void testNormalize() {
            logTestStart("testNormalize");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
            vec.normalize();
            assert(std::fabs(vec.magnitude() - 1) < 0.001);
            logTestEnd();
        }

        static void testDistance() {
            logTestStart("testDistance");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
            NumericalVector<double> other({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
            assert(vec.distance(other) == 0);
            logTestEnd();
        }

        static void testAngle() {
            logTestStart("testAngle");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
            NumericalVector<double> other({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
            assert(std::fabs(vec.angle(other)) < 0.001);
            logTestEnd();
        }

        static void testAverage() {
            logTestStart("testAverage");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
            assert(vec.average() == 5.5);
            //assert(vec.average() == 10);
            logTestEnd();
        }

        static void testVariance() {
            logTestStart("testVariance");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
            assert(std::fabs(vec.variance() - 8.25) < 0.001);
            logTestEnd();
        }

        static void testStandardDeviation() {
            logTestStart("testStandardDeviation");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
            assert(std::fabs(vec.standardDeviation() - 2.87228) < 0.001);
            logTestEnd();
        }

        static void testCovariance() {
            logTestStart("testCovariance");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
            NumericalVector<double> other({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
            assert(std::fabs(vec.covariance(other) - 8.25) < 0.001);
            logTestEnd();
        }

        static void testCorrelation() {
            logTestStart("testCorrelation");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
            NumericalVector<double> other({10, 9, 8, 7, 6, 5, 4, 3, 2, 1});
            assert(std::fabs(vec.correlation(other) - (-1)) < 0.001);
            logTestEnd();
        }

        static void testNorms() {
            logTestStart("testNorms");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
            assert(std::fabs(vec.normL1() - 55) < 0.001);
            assert(std::fabs(vec.normL2() - 19.6214) < 0.001);
            assert(std::fabs(vec.normLInf() - 10) < 0.001);
            logTestEnd();
        }

        static void testIteratorsAndRanges() {
            logTestStart("testIteratorsAndRanges");

            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
            auto beginIter = vec.begin();
            auto endIter = vec.end();

            assert(*beginIter == 1);
            assert(*(endIter - 1) == 10);

            auto range = vec.range(2, 7);
            assert(*range.first == 3);
            assert(*range.second == 8);

            logTestEnd();
        }

        static void testProjection() {
            logTestStart("testProjection");
            NumericalVector<double> vec({1, 2, 3});
            NumericalVector<double> toBeProjected({2, 3, 4});
            NumericalVector<double> result(3);
            vec.project(toBeProjected, result);
            assert(std::fabs(result[0] - 1.42857143) < 0.001);
            assert(std::fabs(result[1] - 2.85714286) < 0.001);
            assert(std::fabs(result[2] - 4.28571429) < 0.001);
            logTestEnd();
        }

/*    static void testHouseHolderTransformation() {
        logTestStart("testHouseHolderTransformation");
        NumericalVector<double> vec({1, 2, 3});
        NumericalVector<double> result(3);
        vec.houseHolderTransformation(result);
        assert(std::fabs(result[0] + 3.74165739) < 0.001);
        assert(std::fabs(result[1]) < 0.001); // close to zero
        assert(std::fabs(result[2]) < 0.001); // close to zero
        logTestEnd();
    }*/



        static void testSumMultiThread() {
            logTestStart("testSumMultiThread");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, MultiThread);
            assert(vec.sum() == 55);
            logTestEnd();
        }

        static void testMagnitudeMultiThread() {
            logTestStart("testMagnitudeMultiThread");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, MultiThread);
            assert(std::fabs(vec.magnitude() - 19.6214) < 0.001);
            logTestEnd();
        }

        static void testAdditionMultiThread() {
            logTestStart("testAdditionMultiThread");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, MultiThread);
            NumericalVector<double> result(10);
            vec.add(vec, result);
            assert(result.sum() == 110);
            logTestEnd();
        }

        static void testAdditionIntoThisMultiThread() {
            logTestStart("testAdditionIntoThisMultiThread");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, MultiThread);
            vec.addIntoThis(vec);
            assert(vec.sum() == 110);
            logTestEnd();
        }

        static void testSubtractionMultiThread() {
            logTestStart("testSubtractionMultiThread");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, MultiThread);
            NumericalVector<double> result(10);
            vec.subtract(vec, result);
            assert(result.sum() == 0);
            logTestEnd();
        }

        static void testSubtractionIntoThisMultiThread() {
            logTestStart("testSubtractionIntoThisMultiThread");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, MultiThread);
            vec.subtractIntoThis(vec);
            assert(vec.sum() == 0);
            logTestEnd();
        }

        static void testDotProductMultiThread() {
            logTestStart("testDotProductMultiThread");
            NumericalVector<double> vec1({1, 2, 3, 4, 5}, MultiThread);
            NumericalVector<double> vec2({1, 2, 3, 4, 5}, MultiThread);
            assert(vec1.dotProduct(vec2) == 55);
            logTestEnd();
        }

        static void testScalingMultiThread() {
            logTestStart("testScalingMultiThread");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, MultiThread);
            vec.scale(2);
            assert(vec.sum() == 110);
            logTestEnd();
        }

        static void testNormalizeMultiThread() {
            logTestStart("testNormalizeMultiThread");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, MultiThread);
            vec.normalize();
            assert(std::fabs(vec.magnitude() - 1) < 0.001);
            logTestEnd();
        }

        static void testDistanceMultiThread() {
            logTestStart("testDistanceMultiThread");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, MultiThread);
            NumericalVector<double> other({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, MultiThread);
            assert(vec.distance(other) == 0);
            logTestEnd();
        }

        static void testAngleMultiThread() {
            logTestStart("testAngleMultiThread");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, MultiThread);
            NumericalVector<double> other({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, MultiThread);
            assert(std::fabs(vec.angle(other)) < 0.001);
            logTestEnd();
        }

        static void testAverageMultiThread() {
            logTestStart("testAverageMultiThread");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, MultiThread);
            assert(vec.average() == 5.5);
            //assert(vec.average() == 10);
            logTestEnd();
        }

        static void testVarianceMultiThread() {
            logTestStart("testVarianceMultiThread");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, MultiThread);
            assert(std::fabs(vec.variance() - 8.25) < 0.001);
            logTestEnd();
        }

        static void testStandardDeviationMultiThread() {
            logTestStart("testStandardDeviationMultiThread");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, MultiThread);
            assert(std::fabs(vec.standardDeviation() - 2.87228) < 0.001);
            logTestEnd();
        }

        static void testCovarianceMultiThread() {
            logTestStart("testCovarianceMultiThread");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, MultiThread);
            NumericalVector<double> other({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, MultiThread);
            assert(std::fabs(vec.covariance(other) - 8.25) < 0.001);
            logTestEnd();
        }

        static void testCorrelationMultiThread() {
            logTestStart("testCorrelationMultiThread");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, MultiThread);
            NumericalVector<double> other({10, 9, 8, 7, 6, 5, 4, 3, 2, 1}, MultiThread);
            assert(std::fabs(vec.correlation(other) - (-1)) < 0.001);
            logTestEnd();
        }

        static void testNormsMultiThread() {
            logTestStart("testNormsMultiThread");
            NumericalVector<double> vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, MultiThread);
            assert(std::fabs(vec.normL1() - 55) < 0.001);
            assert(std::fabs(vec.normL2() - 19.6214) < 0.001);
            assert(std::fabs(vec.normLInf() - 10) < 0.001);
            logTestEnd();
        }
    };
}
