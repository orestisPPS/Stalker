#pragma once

#include "Benchmark.h"
#include <Stalker/Memory/Allocators.h>
#include <Stalker/Mathematics/Operations/MathOperations.h>
#include <Stalker/Mathematics/Random.h>
#include <Stalker/Utility/Logs.h>
#include <cblas.h>
#include <Eigen/Dense>

#include <array>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cstring>

namespace Benchmarks {

	using namespace Stalker::Mathematics;
	using namespace Stalker::Utility;
	using namespace Stalker::Core::Config;

	class MathematicsBenchmarks : public Benchmark<MathematicsBenchmarks> {
	public:
		MathematicsBenchmarks(const std::string& logExportPath) : Benchmark("Mathematical Operations", logExportPath) {}
	protected:
		friend class Benchmark<MathematicsBenchmarks>;

		template<size_t Unroll>
		void _run(size_t size) {
			for (size_t memOpIndex = 0; memOpIndex < Benchmarks::MathOpNames.size(); ++memOpIndex) {
				const auto& memOpName = Benchmarks::MathOpNames[memOpIndex];
				if (memOpName == "scale") {
					#define CALL(T) this->_testScale<T, Unroll>(size);
					BENCH_FOR_EACH_TYPE(CALL)   
					#undef CALL
				}
				else if (memOpName == "dot") {
					#define CALL(T) this->_testDot<T, Unroll>(size);
					BENCH_FOR_EACH_TYPE(CALL)   
					#undef CALL
				}
				else if (memOpName == "axpy") {
					#define CALL(T) this->_testAxpy<T, Unroll>(size);
					BENCH_FOR_EACH_TYPE(CALL)   
					#undef CALL
				}
				else if (memOpName == "sum") {
					#define CALL(T) this->_testSum<T, Unroll>(size);
					BENCH_FOR_EACH_TYPE(CALL)   
					#undef CALL
				}
			}
		}
	
	template<typename T, size_t Unroll>
	void _testScale(size_t size) {

		printTitle(std::string("Operation: Scale | Type: ") + typeid(T).name() + " | Unroll: " + std::to_string(Unroll) + " | Size: " + std::to_string(size), "-", T_Color::TOXIC_GREEN);

		auto papiConfig = PAPILogsConfig{};
		_clearAndResetLogs(size, Unroll, papiConfig);

		const std::string type = typeid(T).name();
		auto config = SingleBenchmarkConfig{};
		
		auto allocator = [&]() { return createAlignedVector<T>(size); };

		auto a = allocator();
        T scalar = static_cast<T>(3.141592653);
        Stalker::Mathematics::Random::uniform<T>(size, a.data(), 0, 100);
		const auto totalBytes = 1 * size * sizeof(T);

		config.compareOver = { "blas " + type};
		config.name = "stalker avx2 " + type;
		_benchmarkPAPI(
			[&](auto& dst) {
				MathOperations::scale<T, ExecutionTraitSIMD<T_SIMD::AVX2, Unroll>>(size, a.data(), dst.data(), scalar);
			},
			allocator, config, totalBytes
		);
		

		config.name = "stalker avx512 " + type;
		_benchmarkPAPI(
			[&](auto& dst) {
				MathOperations::scale<T, ExecutionTraitSIMD<T_SIMD::AVX512, Unroll>>(size, a.data(), dst.data(), scalar);
			},
			allocator, config, totalBytes
		);
		
		config.compareOver = {};
		config.name = "blas " + type;
		_benchmarkPAPI(
			[&](auto& dst) {
				if constexpr (std::is_same_v<T, float>) {
					cblas_sscal(static_cast<int>(size), scalar, dst.data(), 1);
				} else if constexpr (std::is_same_v<T, double>) {
					cblas_dscal(static_cast<int>(size), scalar, dst.data(), 1);
				}
			},
			allocator, config, totalBytes
		);
		_logs.exportToJSON(_logExportPath + "/scale_t_" + type + "_s" + std::to_string(size) + "_" + _getSizeType(size) + "_u" + std::to_string(Unroll));
		_logs.clear();
	}

	template<typename T, size_t Unroll>
	void _testDot(size_t size) {

		printTitle(std::string("Operation: Dot | Type: ") + typeid(T).name() + " | Unroll: " + std::to_string(Unroll) + " | Size: " + std::to_string(size), "-", T_Color::TOXIC_GREEN);

		auto papiConfig = PAPILogsConfig{};
		_clearAndResetLogs(size, Unroll, papiConfig);

		const std::string type = typeid(T).name();
		auto config = SingleBenchmarkConfig{};
		
		auto allocator = [&]() { return T{}; };

		auto a = createAlignedVector<T>(size);
		auto b = createAlignedVector<T>(size);
		Stalker::Mathematics::Random::uniform<T>(size, a.data(), 0, 10);
		Stalker::Mathematics::Random::uniform<T>(size, b.data(), 0, 10);
		const auto totalBytes = 2 * size * sizeof(T);

			
		config.name = "stalker avx2 " + type;
		config.compareOver = { "eigen " + type, "blas " + type };
		_benchmarkPAPI(
			[&](auto& dst) {
				dst = MathOperations::dot<T, ExecutionTraitSIMD<T_SIMD::AVX2, Unroll>>(size, a.data(), b.data());
			},
			allocator, config, totalBytes
		);
		config.name = "stalker avx512 " + type;
		_benchmarkPAPI(
			[&](auto& dst) {
				dst = MathOperations::dot<T, ExecutionTraitSIMD<T_SIMD::AVX512, Unroll>>(size, a.data(), b.data());
			},
			allocator, config, totalBytes
		);

		Eigen::Map<const Eigen::Matrix<T, Eigen::Dynamic, 1>> ea(a.data(), static_cast<int>(size));
		Eigen::Map<const Eigen::Matrix<T, Eigen::Dynamic, 1>> eb(b.data(), static_cast<int>(size));
		config.name = "eigen " + type;
		config.compareOver.clear();
		_benchmarkPAPI(
			[&](auto& dst) {
				dst = ea.dot(eb);
			},
			allocator, config, totalBytes
		);

		config.name = "blas " + type;
		_benchmarkPAPI(
			[&](auto& dst) {
				if constexpr (std::is_same_v<T, float>) {
					dst = cblas_sdot(static_cast<int>(size), a.data(), 1, b.data(), 1);
				} else if constexpr (std::is_same_v<T, double>) {
					dst = cblas_ddot(static_cast<int>(size), a.data(), 1, b.data(), 1);
				}
			},
			allocator, config, totalBytes
		);

		_logs.exportToJSON(_logExportPath + "/dot_t_" + type + "_s" + std::to_string(size) + "_" + _getSizeType(size) + "_u" + std::to_string(Unroll));
		_logs.clear();
	}

	template<typename T, size_t Unroll>
	void _testAxpy(size_t size) {

		printTitle(std::string("Operation: AXPY | Type: ") + typeid(T).name() + " | Unroll: " + std::to_string(Unroll) + " | Size: " + std::to_string(size), "-", T_Color::TOXIC_GREEN);

		auto papiConfig = PAPILogsConfig{};
		_clearAndResetLogs(size, Unroll, papiConfig);

		const std::string type = typeid(T).name();
		auto config = SingleBenchmarkConfig{};
		
		auto allocator = [&]() { return createAlignedVector<T>(size); };

		auto x = allocator();
		auto y = allocator();
		T a = static_cast<T>(3.141592653);
		Stalker::Mathematics::Random::uniform<T>(size, x.data(), 0, 10);
		Stalker::Mathematics::Random::uniform<T>(size, y.data(), 0, 10);
		const auto totalBytes = 3 * size * sizeof(T);

			
		config.name = "stalker avx2 " + type;
		config.compareOver = { "eigen " + type, "blas " + type };
		_benchmarkPAPI(
			[&](auto& dst) {
				MathOperations::axpy<T, ExecutionTraitSIMD<T_SIMD::AVX2, Unroll>>(size, x.data(), y.data(), dst.data(), a);
			},
			allocator, config, totalBytes
		);
		
		config.name = "stalker avx512 " + type;
		_benchmarkPAPI(
			[&](auto& dst) {
				MathOperations::axpy<T, ExecutionTraitSIMD<T_SIMD::AVX512, Unroll>>(size, x.data(), y.data(), dst.data(), a);
			},
			allocator, config, totalBytes
		);
		Eigen::Map<const Eigen::Matrix<T, Eigen::Dynamic, 1>> ex(x.data(), static_cast<int>(size));
		Eigen::Map<const Eigen::Matrix<T, Eigen::Dynamic, 1>> ey(y.data(), static_cast<int>(size));
		auto eigenAllocator = [&]() { return Eigen::Matrix<T, Eigen::Dynamic, 1>(); };
		config.name = "eigen " + type;
		config.compareOver.clear();
		_benchmarkPAPI(
				[&](auto& dst) {
					dst = a * ex + ey; // full vector result
				},
			eigenAllocator, config, totalBytes
		);

		config.name = "blas " + type;
		_benchmarkPAPI(
				[&](auto& dst) {
					std::memcpy(dst.data(), y.data(), size * sizeof(T));
					if constexpr (std::is_same_v<T, float>) {
						cblas_saxpy(static_cast<int>(size), a, x.data(), 1, dst.data(), 1);
					} else if constexpr (std::is_same_v<T, double>) {
						cblas_daxpy(static_cast<int>(size), a, x.data(), 1, dst.data(), 1);
					}
				},
			allocator, config, totalBytes
		);

		_logs.exportToJSON(_logExportPath + "/axpy_t_" + type + "_s" + std::to_string(size) + "_" + _getSizeType(size) + "_u" + std::to_string(Unroll));
		_logs.clear();
	}

	template<typename T, size_t Unroll>
	void _testSum(size_t size) {

		printTitle(std::string("Operation: Sum | Type: ") + typeid(T).name() + " | Unroll: " + std::to_string(Unroll) + " | Size: " + std::to_string(size), "-", T_Color::TOXIC_GREEN);

		auto papiConfig = PAPILogsConfig{};
		_clearAndResetLogs(size, Unroll, papiConfig);

		const std::string type = typeid(T).name();
		auto config = SingleBenchmarkConfig{};
		
		auto allocator = [&]() { return T{}; };

		auto a = createAlignedVector<T>(size);
        Stalker::Mathematics::Random::uniform<T>(size, a.data(), 0, 10);
		const auto totalBytes = 1 * size * sizeof(T);

		config.compareOver = { "std::accumulate " + type, "eigen " + type };
		config.name = "stalker avx2 " + type;
		_benchmarkPAPI(
			[&](auto& dst) {
				dst = MathOperations::sum<T, ExecutionTraitSIMD<T_SIMD::AVX2, Unroll>>(size, a.data());
				std::cout << dst << std::endl;
			},
			allocator, config, totalBytes
		);
		
		
		config.name = "stalker avx512 " + type;
		_benchmarkPAPI(
			[&](auto& dst) {
				dst = MathOperations::sum<T, ExecutionTraitSIMD<T_SIMD::AVX512, Unroll>>(size, a.data());
				std::cout << dst << std::endl;
			},
			allocator, config, totalBytes
		);
		
		// std::accumulate reference
		config.compareOver = {};
		config.name = "std::accumulate " + type;
		_benchmarkPAPI(
			[&](auto& dst) {
				dst = std::accumulate(a.begin(), a.end(), static_cast<T>(0));
				std::cout << dst << std::endl;
			},
			allocator, config, totalBytes
		);

		{
			Eigen::Map<const Eigen::Matrix<T, Eigen::Dynamic, 1>> ea(a.data(), static_cast<int>(size));
			config.name = "eigen " + type;
			_benchmarkPAPI(
				[&](auto& dst) {
					dst = ea.sum();
					std::cout << dst << std::endl;
				},
				allocator, config, totalBytes
			);
		}

		_logs.exportToJSON(_logExportPath + "/sum_t_" + type + "_s" + std::to_string(size) + "_" + _getSizeType(size) + "_u" + std::to_string(Unroll));
		_logs.clear();
	}

};

};