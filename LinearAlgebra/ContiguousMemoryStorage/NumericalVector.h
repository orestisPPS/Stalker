////
//// Created by hal9000 on 8/16/23.
////
//
//#ifndef UNTITLED_NUMERICALVECTOR_H
//#define UNTITLED_NUMERICALVECTOR_H
//
//#include <vector>
//#include <stdexcept>
//#include <memory>
//#include <type_traits>
//#include <valarray>
//#include <random>
//#include <iostream>
//#include <filesystem>
//#include <fstream>
//#include "../../ThreadingOperations/STLKR_ThreadingOperations.h"
//#include "Norms.h"
//
//#include "VectorOperations.h"
//using namespace LinearAlgebra;
//using namespace std;
//
//namespace STLKR_LinearAlgebra {
//
//    
//    template<typename T, size_t size, size_t availableThreads = 1>
//    class NumericalVector {
//
//    public:
//        
//        /**
//        * @brief Constructs a new NumericalVector object.
//        * 
//        * @param size Size of the numerical vector.
//        * @param initialValue Default value for vector elements.
//        * @param parallelizationMethod Parallelization method to be used for vector operations.
//        */
//        explicit NumericalVector(T initialValue = 0){
//            
//            static_assert(std::is_arithmetic<T>::value, "Template type T must be an arithmetic type (integral or floating-point)");
//            _values = vector<T>(size, initialValue);
//            norms = Norms<size, T, availableThreads>(_values.data());
//        }
//
//        /**
//        * @brief Constructs a new NumericalVector object.
//        * @param values Initial values for the vector.
//        * @param parallelizationMethod Parallelization method to be used for vector operations.
//        */
//        NumericalVector(std::initializer_list<T> values) {
//            
//            static_assert(std::is_arithmetic<T>::value, "Template type T must be an arithmetic type (integral or floating-point)");
//            _values = vector<T>(values);
//            norms = Norms<size, T, availableThreads>(_values.data());
//        }
//
//
//
//        /**
//        * @brief Destructor for NumericalVector.
//        * 
//        * Cleans up and deallocates the vector.
//        */
//        ~NumericalVector() = default;
//
//        //=================================================================================================================//
//        //=================================================== Operators ====================================================//
//        //=================================================================================================================//
//
//        /**
//        * @brief Copy constructor.
//        * 
//        * @param other The source object to be copied from.
//        */
//        template<typename InputType>
//        explicit NumericalVector(const NumericalVector<T, size, availableThreads> &other) {
//            static_assert(std::is_arithmetic<T>::value, "Template type T must be an arithmetic type (integral or floating-point)");
//            _values = other._values;
//            deepCopy(other);
//        }
//        
//
//        /**
//        * @brief Overloaded assignment operator.
//        * 
//        * @param other The source object to be copied from.
//        * @return Reference to the current object.
//        */
//        template<typename InputType>
//        NumericalVector &operator=(const InputType &other) {
//            
//            if (this != &other) {
//                deepCopy(other);
//            }
//            return *this;
//        }
//
//        /**
//        * @brief Overloaded equality operator.
//        * 
//        * @param other The object to be compared with.
//        * @return true if the objects have the same data, false otherwise.
//        */
//        template<typename InputType>
//        bool operator==(const InputType &other) const {
//            _checkInputType(other);
//            // Using the dereference trait to obtain the pointer to the source data
//            T *otherData = dereference_trait<InputType>::dereference(other);
//            unsigned int otherSize = dereference_trait<InputType>::size(other);
//            return _areElementsEqual(otherData, otherSize);
//        }
//
//        /**
//        * @brief Overloaded inequality operator.
//        * 
//        * @param other The object to be compared with.
//        * @return true if the objects do not have the same data, false otherwise.
//        */
//        template<typename InputType>
//        bool operator!=(const InputType &other) const {
//            return !(*this == other);
//        }
//
//        /**
//        * @brief Accesses the element at the specified index.
//        * 
//        * @param index Index of the element to access.
//        * @return T& Reference to the element at the specified index.
//        */
//        T &operator[](unsigned int index) {
//            if (index >= _values.size()) {
//                throw out_of_range("Index out of range.");
//            }
//            return _values[index];
//        }
//
//
//        /**
//        * @brief Accesses the element at the specified index (const version).
//        * 
//        * @param index Index of the element to access.
//        * @return const T& Constant reference to the element at the specified index.
//        */
//        const T &operator[](unsigned int index) const {
//            if (index >= _values.size()) {
//                throw out_of_range("Index out of range.");
//            }
//            return _values[index];
//        }
//
//        /**
//        * @brief Accesses the element at the specified index with bounds checking.
//        * 
//        * Throws std::out_of_range exception if index is out of bounds.
//        * 
//        * @param index Index of the element to access.
//        * @return T& Reference to the element at the specified index.
//        */
//        T &at(unsigned int index) {
//            if (index >= _values.size()) {
//                throw out_of_range("Index out of range.");
//            }
//            return _values[index];
//        }
//
//        /**
//        * @brief Accesses the element at the specified index with bounds checking (const version).
//        * 
//        * Throws std::out_of_range exception if index is out of bounds.
//        * 
//        * @param index Index of the element to access.
//        * @return const T& Constant reference to the element at the specified index.
//        */
//        const T &at(unsigned int index) const {
//            if (index >= _values.size()) {
//                throw out_of_range("Index out of range.");
//            }
//            return _values[index];
//        }
//        
//        //=================================================================================================================//
//        //=================================================== Iterators ===================================================//
//        //=================================================================================================================//
//        
//        /**
//        * \brief Type definition for iterator.
//        */
//        using iterator = typename std::vector<T>::iterator;
//
//        /**
//         * \brief Type definition for constant iterator.
//         */
//        using const_iterator = typename std::vector<T>::const_iterator;
//
//        /**
//         * \brief Returns an iterator to the beginning of the vector.
//         * \return An iterator to the beginning of the vector.
//         */
//        iterator begin() {
//            return _values.begin();
//        }
//
//        /**
//         * \brief Returns an iterator to the end of the vector.
//         * \return An iterator to the end of the vector.
//         */
//        iterator end() {
//            return _values.end();
//        }
//
//        /**
//         * \brief Returns a constant iterator to the beginning of the vector.
//         * \return A constant iterator to the beginning of the vector.
//         */
//        const_iterator begin() const {
//            return _values.begin();
//        }
//
//        /**
//         * \brief Returns a constant iterator to the end of the vector.
//         * \return A constant iterator to the end of the vector.
//         */
//        const_iterator end() const {
//            return _values.end();
//        }
//
//        /**
//         * \brief Returns a constant iterator to the beginning of the vector.
//         * \return A constant iterator to the beginning of the vector.
//         */
//        const_iterator cbegin() const {
//            return _values.cbegin();
//        }
//
//        /**
//         * \brief Returns a constant iterator to the end of the vector.
//         * \return A constant iterator to the end of the vector.
//         */
//        const_iterator cend() const {
//            return _values.cend();
//        }
//
//        /**
//         * \brief Provides a range of iterators for a specific subrange of the vector.
//         * \param start The starting index of the range.
//         * \param end The ending index of the range.
//         * \return A pair of iterators representing the beginning and end of the specified subrange.
//         */
//        std::pair<iterator, iterator> range(unsigned start, unsigned end) {
//            if (start >= _values.size() || end > _values.size() || start > end) {
//                throw std::out_of_range("Invalid range specified.");
//            }
//            return {_values.begin() + start, _values.begin() + end};
//        }
//
//        /**
//         * \brief Provides a range of constant iterators for a specific subrange of the vector.
//         * \param start The starting index of the range.
//         * \param end The ending index of the range.
//         * \return A pair of constant iterators representing the beginning and end of the specified subrange.
//         */
//        std::pair<const_iterator, const_iterator> range(unsigned start, unsigned end) const {
//            if (start >= _values.size() || end > _values.size() || start > end) {
//                throw std::out_of_range("Invalid range specified.");
//            }
//            return {_values.begin() + start, _values.begin() + end};
//        }
//        
//        
//        
//        //=================================================================================================================//
//        //=================================================== Utility ====================================================//
//        //=================================================================================================================//
//
//        /**
//        * @brief Returns the size of the vector.
//        * 
//        * @return unsigned int Size of the vector.
//        */
//        [[nodiscard]] unsigned int getSize() const {
//            return _values.size();
//        }
//        
//        [[nodiscard]] double sizeInKB() const {
//            return _values.size() * sizeof(T) / 1024.0;
//        }
//        
//        
//        /**
//         * @brief Checks if the vector is empty.
//         * @return true if the vector is empty, false otherwise.
//         */
//        [[nodiscard]] bool empty() const {
//            return _values.empty();
//        }
//        
//        /**
//        * @brief Fills the vector with the specified value.
//        * @param value The value to fill the vector with.
//        */
//        void fill(T value) {
//            auto fillJob = [&](unsigned int start, unsigned int end) {
//                for (unsigned int i = start; i < end; i++) {
//                    _values[i] = value;
//                }
//            };
//            STLKR_ThreadingOperations<availableThreads>::executeParallelJob(fillJob, _values.size());
//        }
//
//        /**
//        * @brief Performs a deep copy from the source to the current object.
//        * 
//        * This method uses the dereference_trait to handle various types of sources 
//        * such as raw pointers, unique pointers, shared pointers, and direct objects.
//        * 
//        * @param source The source object to be copied from.
//        */
//        
//        void deepCopy(const NumericalVector<T, size, availableThreads> &source) {
//            
//            const T *sourceData = source.getDataPointer();
//
//            auto deepCopyThreadJob = [&](unsigned start, unsigned end) {
//                for (unsigned i = start; i < end && i < _values.size(); ++i) {
//                    _values[i] = sourceData[i];
//                }
//            };
//            STLKR_ThreadingOperations<availableThreads>::executeParallelJob(deepCopyThreadJob, _values.size());
//        }
//
//        /**
//        * \brief Fills the vector with random values between the specified minimum and maximum.
//        * 
//        * This function uses the Mersenne Twister random number generator to generate 
//        * random values. The generator is seeded with a device-dependent random number 
//        * to ensure different sequences in different program runs.
//        * 
//        * \tparam T The data type of the vector components (e.g., double, float).
//        * 
//        * \param min The minimum value for the random numbers.
//        * \param max The maximum value for the random numbers.
//        */
//        void fillRandom(T min, T max) {
//            //Mersenne Twister generator seeded with a device-dependent random number
//            // Static to retain state across calls
//            static std::mt19937 generator(std::random_device{}());
//            std::uniform_real_distribution<T> distribution(min, max);
//
//            for (auto &component: *_values) {
//                component = distribution(generator);
//            }
//        }
//        
//        
//        /**
//        * @brief Clears the vector calling the clear() method of the underlying std::vector<T>.
//        */
//        void clear() {
//            _values.clear();
//        }
//        
//        /**
//         * @brief Returns a pointer to the underlying data.
//         * @return T* Pointer to the underlying data.
//         */
//        T *getDataPointer() const {
//            return _values.data();
//        }
//
//        //=================================================================================================================//
//        //============================================== Numerical Operations =============================================//
//        //=================================================================================================================//
//        
//        T sum2() {
//            
//            auto sumElementsJob = [&](unsigned start, unsigned end) -> T {
//                return VectorOperations<T>::sumOfElementsUnrolledThreadJob<start, end>(_values.data());
//            };
//            return STLKR_ThreadingOperations<availableThreads>::executeParallelJobWithReduction(sumElementsJob, _values.size());
//
//        }
//        
//
//        
//        /**
//         * @brief Projects one vector onto another and stores the result in a third vector.
//         *
//         * This function computes the projection of the provided input vector onto the current instance vector
//         * and stores the result in the result vector.
//         *
//         * The projection of vector B (toBeProjected) onto vector A (current instance) is computed using the formula:
//         * proj_A B = (B . A) / (A . A) * A
//         * This result is stored in the result vector.
//         *
//         * @tparam InputType1 The type of the input vector that needs to be projected. Can be a raw pointer,
//         *         a shared pointer, a unique pointer, or a direct object of NumericalVector.
//         * @tparam InputType2 The type of the result vector where the projection result will be stored. Can be
//         *         a raw pointer, a shared pointer, a unique pointer, or a direct object of NumericalVector.
//         * @param toBeProjected The vector that is to be projected onto the current instance.
//         * @param result The vector where the projection result will be stored.
//         * @throws std::invalid_argument If any of the vectors are not of the same size.
//         * @throws std::runtime_error If the magnitude of the input vector to be projected is zero.
//         */
//        template<typename InputType1, typename InputType2>
//        void project(const InputType1 &toBeProjected, InputType2 &result){
//
//            _checkInputType(toBeProjected);
//            _checkInputType(result);
//            if (size() != dereference_trait<InputType1>::size(toBeProjected)) {
//                throw invalid_argument("Vectors must be of the same size.");
//            }
//            if (size() != dereference_trait<InputType2>::size(result)) {
//                throw invalid_argument("Vectors must be of the same size.");
//            }
//            const T *inputVectorData1 = dereference_trait<InputType1>::dereference(toBeProjected);
//            T *resultData = dereference_trait<InputType2>::dereference(result);
//
//            double dot = dotProduct(toBeProjected);
//
//            auto inputMagnitudeThreadJob = [&](unsigned start, unsigned end) -> double {
//                double sum = 0;
//                for (unsigned i = start; i < end && i < size(); ++i) {
//                    sum += inputVectorData1[i] * inputVectorData1[i];
//                }
//                return sum;
//            };
//
//            double inputMagnitudeSquared = _threading.executeParallelJobWithReductionForDoubles(inputMagnitudeThreadJob, size(), _availableThreads);
//            double inputMagnitude = sqrt(inputMagnitudeSquared);
//
//            if (inputMagnitude == 0) {
//                throw std::runtime_error("Cannot project on a vector with magnitude 0.");
//            }
//            double scalar = dot / inputMagnitudeSquared;
//            
//            auto projectionThreadJob = [&](unsigned start, unsigned end) {
//                for (unsigned i = start; i < end && i < size(); ++i) {
//                    resultData[i] = inputVectorData1[i] * scalar;
//                }
//            };
//            _threading.executeParallelJob(projectionThreadJob, size(), _availableThreads);
//        }
//
//        /**
//         * @brief Projects a given vector onto the current instance vector.
//         *
//         * This function computes the projection of the provided input vector onto the current instance vector
//         * and updates the current instance with the result.
//         *
//         * The projection of vector B (toBeProjected) onto vector A (current instance) is computed using the formula:
//         * proj_A B = (B . A) / (A . A) * A
//         * This result updates the current instance vector.
//         *
//         * @tparam InputType The type of the vector that needs to be projected. Can be a raw pointer,
//         *         a shared pointer, a unique pointer, or a direct object of NumericalVector.
//         * @param toBeProjected The vector that is to be projected onto the current instance.
//         * @throws std::invalid_argument If the vectors are not of the same size.
//         * @throws std::runtime_error If the magnitude of the vector to be projected is zero.
//         */
//        template<typename InputType>
//        void projectOntoThis(const InputType &toBeProjected) {
//            _checkInputType(toBeProjected);
//            if (size() != dereference_trait<InputType>::size(toBeProjected)) {
//                throw invalid_argument("Vectors must be of the same size.");
//            }
//            const T *inputVectorData1 = dereference_trait<InputType>::dereference(toBeProjected);
//
//            double dot = dotProduct(toBeProjected);
//
//            auto inputMagnitudeThreadJob = [&](unsigned start, unsigned end) -> double {
//                double sum = 0;
//                for (unsigned i = start; i < end && i < size(); ++i) {
//                    sum += inputVectorData1[i] * inputVectorData1[i];
//                }
//                return sum;
//            };
//
//            double inputMagnitudeSquared = _threading.executeParallelJobWithReductionForDoubles(inputMagnitudeThreadJob, size(), _availableThreads);
//            double inputMagnitude = sqrt(inputMagnitudeSquared);
//
//            if (inputMagnitude == 0) {
//                throw std::runtime_error("Cannot project on a vector with magnitude 0.");
//            }
//            double scalar = dot / inputMagnitudeSquared;
//
//            auto projectionThreadJob = [&](unsigned start, unsigned end) {
//                for (unsigned i = start; i < end && i < size(); ++i) {
//                    _values[i] = inputVectorData1[i] * scalar;
//                }
//            };
//            _threading.executeParallelJob(projectionThreadJob, size(), _availableThreads);
//        }
//
//        /**
//         * @brief Applies the Householder transformation to a given vector.
//         * 
//         * Householder transformation is a method used to zero out certain components of a vector.
//         * It reflects a given vector about a plane or hyperplane. This function applies the
//         * transformation  to the current instance vector and stores the result in the result vector.
//         * 
//         * Mathematically, the Householder matrix is defined as:
//         *    H = I - 2 * (v * v^T) / (v^T * v)
//         * where `v` is the reflection vector. This function uses this concept to reflect the
//         * the current instance vector and stores the result in the result vector.
//         * 
//         * @tparam InputType The type of the result vector. The function supports various types
//         *                   like raw pointers, shared pointers, and the NumericalVector itself.
//         * 
//         * @param targetVector 
//         * 
//         * @throw invalid_argument if the size of the input vector and the current vector don't match.
//         */
//        template<typename InputType>
//        void houseHolderTransformation() {
//            
//            //Lambda function to calculate the sign of a number
//            auto sign = [](double x) -> int {
//                return x >= 0.0 ? 1 : -1;
//            };
//            
//            //Calculate the norm of the current vector
//            double norm = normL2();
//            double alpha = -sign((*_values)[0]) * norm;
//            _values[0] -= alpha;
//            normalize();
//        }
//
//        /**
//         * @brief Applies the Householder transformation to a given vector.
//         * 
//         * Householder transformation is a method used to zero out certain components of a vector.
//         * It reflects a given vector about a plane or hyperplane. This function applies the
//         * transformation  to the current instance vector and stores the result in the result vector.
//         * 
//         * Mathematically, the Householder matrix is defined as:
//         *    H = I - 2 * (v * v^T) / (v^T * v)
//         * where `v` is the reflection vector. This function uses this concept to reflect the current instance 
//         * of the vector.
//         */
//        template<typename InputType>
//        void houseHolderTransformationIntoThis(){
//            auto sign = [](double x) -> int {
//                return x >= 0.0 ? 1 : -1;
//            };
//            double norm = normL2();
//            double alpha = -sign((*_values)[0]) * norm;
//            auto range = std::make_pair(1, size());
//            _values[0] -= alpha;
//        }
//        
//
//        /**
//        * @brief Calculates the sum of all the elements of this vector.
//        * @return The sum of all components of the vector.
//        * 
//        * Given a vector v = [v1, v2, ..., vn], the sum is calculated as:
//        * average(v) = (v1 + v2 + ... + vn) / n
//        */
//        double average() {
//            return sum() / _values.size();
//        }
//
//
//        /**
//        @brief Calculates the variance of the elements of this vector.
//        @return The variance of the vector.
//        * 
//        * The variance of a vector is a measure of how spread out its values are. It is calculated as the average of the
//        * squared differences between each element and the mean of the vector.
//        * Given a vector x with n elements, the variance is calculated as:
//        * variance(x) = (1/n) * (sum of (x[i] - mean(x))^2 for i=1 to n)
//        */
//        double variance() {
//            double averageOfElements = average();
//
//            auto varianceJob = [&](unsigned start, unsigned end) -> double {
//                double sum = 0;
//                for (unsigned i = start; i < end && i < _values.size(); ++i) {
//                    double diff = _values[i] - averageOfElements;
//                    sum += diff * diff;
//                }
//                return sum;
//            };
//
//            return _threading.executeParallelJobWithReductionForDoubles(varianceJob, size(), _availableThreads) / size();
//        }
//        
//        double averageAbsoluteDeviationFromMean(){
//            double averageOfElements = average();
//            auto averageAbsoluteDeviationFromMeanJob = [&](unsigned start, unsigned end) -> double {
//                double sum = 0;
//                for (unsigned i = start; i < end && i < _values.size(); ++i) {
//                    sum += abs((*_values)[i] - averageOfElements);
//                }
//                return sum;
//            };
//            return _threading.executeParallelJobWithReductionForDoubles(averageAbsoluteDeviationFromMeanJob, size(), _availableThreads) / size();
//        }
//
//        /**
//        * @brief Calculates the standard deviation of the elements of this vector.
//        * @return The standard deviation of the vector.
//        * 
//        * The standard deviation of a vector is the square root of its variance. It is a measure of how spread out its values
//        * are, but it is expressed in the same units as the original data.
//        */
//        double standardDeviation() {
//            return sqrt(variance());
//        }
//
//        /**
//        * @brief Calculates the covariance between this vector and the input vector.
//        * @param inputVector Input vector to calculate the covariance to.
//        * @return The covariance between the two vectors.
//        * 
//        * The covariance between two vectors is a measure of how they vary together. If the covariance is positive, the values of
//        * one vector tend to be high when the values of the other vector are high, and low when the values of the other vector are
//        * low. If the covariance is negative, the values of one vector tend to be high when the values of the other vector are low,
//        * and vice versa. A covariance of zero means that the vectors are uncorrelated, i.e., their values do not vary together.
//        * Given two vectors X = [x1, x2, ..., xn] and Y = [y1, y2, ..., yn], their covariance is calculated as:
//        * cov(X, Y) = 1/n * sum((xi - mean(X)) * (yi - mean(Y))), where mean(X) and mean(Y) are the means of X and Y, respectively.
//        * If the two vectors have the same length, the formula simplifies to:
//        * cov(X, Y) = 1/n * dot(X - mean(X), Y - mean(Y)), where dot() is the dot product.
//        */
//        template<typename InputType>
//        double covariance(const InputType &inputVector) {
//
//            _checkInputType(inputVector);
//            if (size() != dereference_trait<InputType>::size(inputVector)) {
//                throw invalid_argument("Vectors must be of the same size.");
//            }
//
//            const T *inputVectorData = dereference_trait<InputType>::dereference(inputVector);
//
//            double averageOfThis = average();
//
//            auto averageOfInputJob = [&](unsigned start, unsigned end) -> double {
//                double sum = 0;
//                for (unsigned i = start; i < end; ++i) {
//                    sum += inputVectorData[i];
//                }
//                return sum;
//            };
//
//            double averageOfInput = _threading.executeParallelJobWithReductionForDoubles(averageOfInputJob, size(), _availableThreads) / _values.size();
//
//            auto covarianceJob = [&](unsigned start, unsigned end) -> double {
//                double sum = 0;
//                for (unsigned i = start; i < end && i < _values.size(); ++i) {
//                    sum += ((*_values)[i] - averageOfThis) * (inputVectorData[i] - averageOfInput);
//                }
//                return sum;
//            };
//
//            return _threading.executeParallelJobWithReductionForDoubles(covarianceJob, size(), _availableThreads) / size();
//        }
//
//        /**
//        * @brief Calculates the correlation between this vector and the input vector.
//        * @param inputVector Input vector to calculate the correlation to.
//        * @return The correlation between the two vectors.
//        * 
//        * The correlation coefficient between two vectors is a measure of how strong the linear relationship is between them. It
//        * ranges from -1 (perfect negative correlation) to 1 (perfect positive correlation), with 0 indicating no correlation.
//        * Given two vectors X = [x1, x2, ..., xn] and Y = [y1, y2, ..., yn], their correlation coefficient is calculated as:
//        * cor(X, Y) = cov(X, Y) / (sd(X) * sd(Y)), where cov(X, Y) is the covariance between X and Y, and sd(X) and sd(Y) are the
//        * standard deviations of X and Y, respectively.
//        */
//        template<typename InputType>
//        double correlation(const InputType &inputVector) {
//
//            _checkInputType(inputVector);
//            if (size() != dereference_trait<InputType>::size(inputVector)) {
//                throw invalid_argument("Vectors must be of the same size.");
//            }
//
//            T *inputVectorData = dereference_trait<InputType>::dereference(inputVector);
//            double covarianceOfVectors = covariance(inputVector);
//            double stdDevOfThis = standardDeviation();
//
//            // Calculate mean of inputVector
//            auto meanOfInputJob = [&](unsigned start, unsigned end) -> double {
//                double sum = 0;
//                for (unsigned i = start; i < end; ++i) {
//                    sum += inputVectorData[i];
//                }
//                return sum;
//            };
//            double meanOfInput = _threading.executeParallelJobWithReductionForDoubles(meanOfInputJob, size(), _availableThreads) / _values.size();
//
//            // Calculate variance of inputVector
//            auto varianceOfInputJob = [&](unsigned start, unsigned end) -> double {
//                double sum = 0;
//                for (unsigned i = start; i < end; ++i) {
//                    double diff = inputVectorData[i] - meanOfInput;
//                    sum += diff * diff;
//                }
//                return sum;
//            };
//            double varianceOfInput = _threading.executeParallelJobWithReductionForDoubles(varianceOfInputJob, size(), _availableThreads) / _values.size();
//            double stdDevOfInput = sqrt(varianceOfInput);
//
//            if (stdDevOfThis == 0 || stdDevOfInput == 0) {
//                throw std::runtime_error("Cannot compute correlation between vectors with standard deviation 0.");
//            }
//
//            return covarianceOfVectors / (stdDevOfThis * stdDevOfInput);
//        }
//        
//        Norms<numberOfElements, T, availableThreads> norms{};
//
//     
//        template<typename InputType1, typename InputType2>
//        void crossProduct(const InputType1 &inputVector, InputType2 &result, unsigned userDefinedThreads = 0) {
//            
//            _checkInputType(inputVector);
//            _checkInputType(result);
//            if (size() != dereference_trait<InputType1>::size(inputVector)) {
//                throw invalid_argument("Vectors must be of the same size.");
//            }
//            if (size() != dereference_trait<InputType2>::size(result)) {
//                throw invalid_argument("Vectors must be of the same size.");
//            }
//            const T *otherData = dereference_trait<InputType1>::dereference(inputVector);
//            T *resultData = dereference_trait<InputType2>::dereference(result);
//            switch (size()){
//                case 2:
//                    resultData[0] = _values[0] * otherData[1] - _values[1] * otherData[0];
//                    break;
//                case 3:
//                    resultData[0] = _values[1] * otherData[2] - _values[2] * otherData[1];
//                    resultData[1] = _values[2] * otherData[0] - _values[0] * otherData[2];
//                    resultData[2] = _values[0] * otherData[1] - _values[1] * otherData[0];
//                    break;
//                default:
//                    throw std::runtime_error("Cross product is only defined for 2D and 3D vectors.");
//            }
//        }
//
//   
//
//     
//        void printVertically(const string& name = " "){
//            cout << " name = [ " << endl;
//            for (auto &value: *_values){
//                cout << value << endl;
//            }
//            cout << " ] " << endl;
//        }
//
//        void printVerticallyWithIndex(const string& name = " "){
//            cout << name << " = [ " << endl;
//            for (unsigned i = 0; i < _values.size() - 1; ++i){
//                cout << i << " : " << _values[i] << endl;
//            }
//            cout << _values.size() - 1 << " : " << _values[_values.size() - 1] << " ] " << endl;
//        }
//
//
//        void printHorizontally(const string& name = " "){
//            std::ostream os (std::cout.rdbuf());
//            os << name << " = [ ";
//            for (const auto &value: *_values){
//                os << value << " ";
//            }
//            os << " ] " << "\n" << endl;
//        }
//        
//        void printHorizontallyWithIndex(const string& name = " "){
//            std::ostream os (std::cout.rdbuf());
//            os << name << " = [ ";
//            for (unsigned i = 0; i < _values.size(); ++i){
//                os << i << " : " << _values[i] << " ";
//            }
//            os << " ] " << "\n" << endl;
//        }
//
//
//        void exportCSV(const std::string& filePath, const std::string& fileName) const {
//            std::ofstream file;
//            std::string fullPath = filePath + "/" + fileName + ".csv";
//            cout << "Exporting to " << fullPath << endl;
//
//            file.open(fullPath);
//            if (!file.is_open()) {
//                throw std::runtime_error("Could not open file.");
//            }
//
//            for (unsigned i = 0; i < _values.size(); ++i) {
//                file << _values[i] << '\n';  // Write each value in a new line
//            }
//
//            file.close();
//        }
//
//
//    protected:
//        vector<T> _values; ///< The underlying data.
//        
//        /**
//        * @brief Checks if the elements of the current object are equal to those of the provided source.
//        * 
//        * This method uses parallelization to perform the comparison and then reduces the results to determine 
//        * if all elements are equal.
//        * 
//        * @param source The source object to be compared with.
//        * @return true if all elements are equal, false otherwise.
//        */
//        bool _areElementsEqual(T * const &sourceData, size_t sourceSize) const {
//
//            if (_values.size() != size) {
//                throw std::invalid_argument("Source vector must be the same size as the destination vector.");
//            }
//
//            auto compareElementsJob = [&](unsigned start, unsigned end) -> bool {
//                for (unsigned i = start; i < end && i < _values.size(); ++i) {
//                    if ((*_values)[i] != source[i]) {
//                        return false;
//                    }
//                }
//                return true;
//            };
//
//            return _threading.executeParallelJobWithReduction(compareElementsJob, _values.size(), _availableThreads);
//        }
//        
//
//
//        //=================================================================================================================//
//        //============================================ Dereference Traits =================================================//
//        //=================================================================================================================//
//
//        /**
//        * \brief Trait to standardize dereferencing of various types.
//        *
//        * This trait provides a unified way to dereference types such as raw pointers,
//        * unique pointers, shared pointers, and direct objects.
//        */
//        template<typename U>
//        struct dereference_trait;
//
//        /**
//        * \brief Base trait for raw pointers and direct objects.
//        *
//        * This trait provides a unified way to dereference types like NumericalVector and 
//        * raw pointers to NumericalVector.
//        */
//        template<typename U>
//        struct dereference_trait_base {
//            /**
//            * \brief Dereferences the source.
//            * \param source A pointer to the source object.
//            * \return A pointer to the data of the source.
//            */
//            static U *dereference(U *source) {
//                static_assert(std::is_arithmetic<U>::value, "Template type T must be an arithmetic type (integral or floating-point)");
//
//                if (!source) throw std::runtime_error("Null pointer dereferenced");
//                return source->getDataPointer();
//            }
//
//            /**
//            * \brief Fetches the size of the source.
//            * \param source A pointer to the source object.
//            * \return The size of the source.
//            */
//            static unsigned size(U *source) {
//                if (!source) throw std::runtime_error("Null pointer dereferenced");
//                return source->size();
//            }
//        };
//
//        /// Specialization for NumericalVector<U>.
//        template<typename U>
//        struct dereference_trait<NumericalVector<U>> {
//
//            /**
//            * \brief Dereferences the source.
//            * \param source A smart pointer to the source object.
//            * \return A pointer to the data of the source.
//            */
//            static U* dereference(const NumericalVector<U> &source) {
//                static_assert(std::is_arithmetic<U>::value, "Template type T must be an arithmetic type (integral or floating-point)");
//                return source.getDataPointer();
//            }
//            
//            /**
//            * \brief Fetches the size of the source.
//            * \param source A smart pointer to the source object.
//            * \return The size of the source.
//            */
//            static unsigned size(const NumericalVector<U> &source) {
//                return source.size();
//            }
//        };
//
//
//        /// Specialization for raw pointer to NumericalVector<U>.
//        template<typename U>
//        struct dereference_trait<NumericalVector<U> *> : public dereference_trait_base<NumericalVector<U>> {
//        };
//
//        /**
//        * \brief Base trait for smart pointers.
//        *
//        * This trait provides a unified way to dereference types like std::unique_ptr and 
//        * std::shared_ptr.
//        */
//        template<template<typename, typename...> class PtrType, typename U>
//        struct dereference_trait_pointer_base {
//            /**
//            * \brief Dereferences the source.
//            * \param source A smart pointer to the source object.
//            * \return A pointer to the data of the source.
//            */
//            static U *dereference(const PtrType<NumericalVector<U>> &source) {
//                static_assert(std::is_arithmetic<U>::value, "Template type T must be an arithmetic type (integral or floating-point)");
//                if (!source) throw std::runtime_error("Null pointer dereferenced");
//                return source->getDataPointer();
//            }
//
//            static STLKR_ThreadingOperations<_availableThreads> &threadingOperations(const PtrType<NumericalVector<U>> &source) {
//                return source->_threading;
//            }
//            
//            /**
//            * \brief Fetches the size of the source.
//            * \param source A smart pointer to the source object.
//            * \return The size of the source.
//            */
//            static unsigned size(const PtrType<NumericalVector<U>> &source) {
//                if (!source) throw std::runtime_error("Null pointer dereferenced");
//                return source->size();
//            }
//        };
//        
//        /// Specialization for std::unique_ptr<NumericalVector<U>>.
//        template<typename U>
//        struct dereference_trait<std::unique_ptr<NumericalVector<U>>>
//                : public dereference_trait_pointer_base<std::unique_ptr, U> {
//        };
//
//        /// Specialization for std::shared_ptr<NumericalVector<U>>.
//        template<typename U>
//        struct dereference_trait<std::shared_ptr<NumericalVector<U>>>
//                : public dereference_trait_pointer_base<std::shared_ptr, U> {
//        };
//        
//        template<typename InputType>
//        void _checkInputType(InputType const &input) const {
//            static_assert(std::is_same<InputType, NumericalVector<T>>::value
//                          || std::is_same<InputType, std::shared_ptr<NumericalVector<T>>>::value
//                          || std::is_same<InputType, std::unique_ptr<NumericalVector<T>>>::value
//                          || std::is_same<InputType, NumericalVector<T>*>::value,
//                          "Input must be a NumericalVector, its pointer, or its smart pointers.");
//        }
//    };
//    
//}
//
//#endif //UNTITLED_NUMERICALVECTOR_H
