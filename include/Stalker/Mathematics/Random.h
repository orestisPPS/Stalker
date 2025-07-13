#pragma once

#include <random>
#include <type_traits>
#include <stdexcept>
#include <cmath>

namespace Stalker::Mathematics {

enum class DistributionType {
    Uniform,     // [min, max] range
    Normal,      // (mean, stddev)
    Exponential, // (lambda) rate parameter
    Bernoulli,   // (p) success probability
    Poisson,     // (mean) lambda
    Gamma,       // (alpha shape, beta scale)
    Weibull,     // (a shape, b scale)
    LogNormal    // (m logmean, s logstddev)
};

class Random {
public:
    static void setSeed(unsigned int seed) {
        engine.seed(seed);
    }

    template<typename T>
    inline static T uniform(T min = T{0}, T max = T{1}) {
        static_assert(std::is_floating_point_v<T> || std::is_integral_v<T>, 
            "Type must be floating point or integral");
        
        if (min > max) throw std::invalid_argument("Uniform min must be <= max");
        if (min == max) return min;
        
        if constexpr (std::is_floating_point_v<T>)
            return std::uniform_real_distribution<T>(min, max)(engine);
        else
            return std::uniform_int_distribution<T>(min, max)(engine);

    }

    template<typename T>
    inline static void uniform(size_t size, T *data, T min = T{0}, T max = T{1}) {
        static_assert(std::is_floating_point_v<T> || std::is_integral_v<T>, 
            "Type must be floating point or integral");
        
        if (min > max) throw std::invalid_argument("Uniform min must be <= max");
        if (min == max) std::fill(data, data + size, min);
        
        if constexpr (std::is_floating_point_v<T>){
            auto distribution = std::uniform_real_distribution<T>(min, max);
            _fill(size, data, distribution);
        }
        else {
            auto distribution = std::uniform_int_distribution<T>(min, max);
            _fill(size, data, distribution);
        }
    }
    template<typename T>
    inline static T normal(double mean = 1, double stddev = 1) {
        static_assert(std::is_floating_point_v<T>, "Type must be floating point");
        if (stddev <= 0) throw std::invalid_argument("Standard deviation must be > 0");
        return std::normal_distribution<T>(mean, stddev)(engine);
    }

    template<typename T>
    inline static void normal(size_t size, T *data, double mean = 0, double stddev = 0) {
        static_assert(std::is_floating_point_v<T>, "Type must be floating point");
        if (stddev <= 0) throw std::invalid_argument("Standard deviation must be > 0");
        auto distribution = std::normal_distribution<T>(mean, stddev);
        _fill(size, data, distribution);
    }

    template<typename T>
    inline static T exponential(double lambda = T{1}) {
        static_assert(std::is_floating_point_v<T>, "Type must be floating point");
        if (lambda <= 0) throw std::invalid_argument("Lambda must be > 0");
        return std::exponential_distribution<T>(lambda)(engine);
    }

    template<typename T>
    inline static void exponential(size_t size, T *data, double lambda = T{1}) {
        static_assert(std::is_floating_point_v<T>, "Type must be floating point");
        if (lambda <= 0) throw std::invalid_argument("Lambda must be > 0");
        auto distribution = std::exponential_distribution<T>(lambda);
        _fill(size, data, distribution);
    }

    template<typename T>
    inline static T bernoulli(double p = 0.5) {
        static_assert(std::is_integral_v<T>, "Type must be integral");
        if (p < 0 || p > 1) throw std::invalid_argument("Probability p must be in [0, 1]");
        return static_cast<T>(std::bernoulli_distribution(p)(engine));
    }

    template<typename T>
    inline static void bernoulli(size_t size, T *data, double p = 0.5) {
        static_assert(std::is_integral_v<T>, "Type must be integral");
        if (p < 0 || p > 1) throw std::invalid_argument("Probability p must be in [0, 1]");
        auto distribution = std::bernoulli_distribution(p);
        _fill(size, data, distribution);
    }

    template<typename T>
    inline static T poisson(double mean = 1.0) {
        static_assert(std::is_integral_v<T>, "Type must be integral");
        if (mean <= 0) throw std::invalid_argument("Mean must be > 0");
        return static_cast<T>(std::poisson_distribution<int>(mean)(engine));
    }

    template<typename T>
    inline static void poisson(size_t size, T *data, double mean = 1.0) {
        static_assert(std::is_integral_v<T>, "Type must be integral");
        if (mean <= 0) throw std::invalid_argument("Mean must be > 0");
        auto distribution = std::poisson_distribution<int>(mean);
        _fill(size, data, distribution);
    }

    template<typename T>
    inline static T gamma(double alpha = 1.0, double beta = 1.0) {
        static_assert(std::is_floating_point_v<T>, "Type must be floating point");
        if (alpha <= 0 || beta <= 0) throw std::invalid_argument("Gamma parameters must be > 0");
        return std::gamma_distribution<T>(alpha, beta)(engine);
    }

    template<typename T>
    inline static void gamma(size_t size, T *data, double alpha = 1.0, double beta = 1.0) {
        static_assert(std::is_floating_point_v<T>, "Type must be floating point");
        if (alpha <= 0 || beta <= 0) throw std::invalid_argument("Gamma parameters must be > 0");
        auto distribution = std::gamma_distribution<T>(alpha, beta);
        _fill(size, data, distribution);
    }

    template<typename T>
    inline static T weibull(double shape = 1.0, double scale = 1.0) {
        static_assert(std::is_floating_point_v<T>, "Type must be floating point");
        if (shape <= 0 || scale <= 0) throw std::invalid_argument("Weibull parameters must be > 0");
        return std::weibull_distribution<T>(shape, scale)(engine);
    }

    template<typename T>
    inline static void weibull(size_t size, T *data, double shape = 1.0, double scale = 1.0) {
        static_assert(std::is_floating_point_v<T>, "Type must be floating point");
        if (shape <= 0 || scale <= 0) throw std::invalid_argument("Weibull parameters must be > 0");
        auto distribution = std::weibull_distribution<T>(shape, scale);
        _fill(size, data, distribution);
    }

    template<typename T>
    inline static T lognormal(double m = 0.0, double s = 1.0) {
        static_assert(std::is_floating_point_v<T>, "Type must be floating point");
        if (s <= 0) throw std::invalid_argument("LogNormal stddev must be > 0");
        return std::lognormal_distribution<T>(m, s)(engine);
    }

    template<typename T>
    inline static void lognormal(size_t size, T *data, double m = 0.0, double s = 1.0) {
        static_assert(std::is_floating_point_v<T>, "Type must be floating point");
        if (s <= 0) throw std::invalid_argument("LogNormal stddev must be > 0");
        auto distribution = std::lognormal_distribution<T>(m, s);
        _fill(size, data, distribution);
    }

    private:
    // Mersenne Twister engine (thre ad-safe)
    static thread_local std::mt19937 engine;

    template<typename T, typename Distribution>
    static inline void _fill(size_t size, T* data, Distribution& distribution) {
        if (size == 0) throw std::invalid_argument("Size must be > 0");
        if (data == nullptr) throw std::invalid_argument("Data pointer cannot be null");

        for (size_t i = 0; i < size; ++i) {
            data[i] = distribution(engine);
        }
    }
};

// Initialize engine with default seed
thread_local std::mt19937 Random::engine(std::random_device{}());

} // namespace Stalker::Mathematics