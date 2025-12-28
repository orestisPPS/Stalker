#pragma once

#include "Benchmark.h"
#include <Stalker/Mathematics/Vector/VectorMath.h>
#include <cblas.h>
#include <Eigen/Dense>

namespace Benchmarks {

	using namespace Stalker::Mathematics;
	using namespace Stalker::Utility;
	using namespace Stalker::Core::Config;

	class VectorMathBenchmarks : public Benchmark<VectorMathBenchmarks> {
	public:
		VectorMathBenchmarks(const std::string& logExportPath) : Benchmark("Mathematical Operations", logExportPath) {}
	protected:
		friend class Benchmark<VectorMathBenchmarks>;

		void execute() {
			for (size_t mathOpIndex = 0; mathOpIndex < Benchmarks::MathOpNames.size(); ++mathOpIndex) {
				const auto& mathOpName = Benchmarks::MathOpNames[mathOpIndex];
                #define PROCESS_TYPE(T) \
                    for (size_t size : Benchmarks::Sizes) { \
                        if (mathOpName == "scale") { \
                            auto a = this->_createData<T>(size); \
                            this->_forEachUnroll([&](auto unroll) { \
                                constexpr size_t U = decltype(unroll)::value; \
                                this->_testScale<T, U>(size, a); \
                            }); \
                        } \
                        else if (mathOpName == "dot") { \
                            auto a = this->_createData<T>(size); \
                            auto b = this->_createData<T>(size); \
                            this->_forEachUnroll([&](auto unroll) { \
                                constexpr size_t U = decltype(unroll)::value; \
                                this->_testDot<T, U>(size, a, b); \
                            }); \
                        } \
                        else if (mathOpName == "axpy") { \
                            auto x = this->_createData<T>(size); \
                            auto y = this->_createData<T>(size); \
                            this->_forEachUnroll([&](auto unroll) { \
                                constexpr size_t U = decltype(unroll)::value; \
                                this->_testAxpy<T, U>(size, x, y); \
                            }); \
                        } \
                        else if (mathOpName == "sum") { \
                            auto a = this->_createData<T>(size); \
                            this->_forEachUnroll([&](auto unroll) { \
                                constexpr size_t U = decltype(unroll)::value; \
                                this->_testSum<T, U>(size, a); \
                            }); \
                        } \
                    }
                BENCH_FOR_EACH_TYPE(PROCESS_TYPE)
                #undef PROCESS_TYPE
			}
		}
	
	template<typename T, size_t Unroll>
	void _testScale(size_t size, const std::vector<T, Stalker::Memory::AlignedAllocator<T, DefaultAlignment()>>& a) {

		printTitle(std::string("Operation: Scale | Type: ") + typeid(T).name() + " | Unroll: " + std::to_string(Unroll) + " | Size: " + std::to_string(size), "-", T_Color::TOXIC_GREEN);

		auto papiConfig = PAPILogsConfig{};
		_clearAndResetLogs(size, Unroll, papiConfig);

		const std::string type = typeid(T).name();
		auto config = SingleBenchmarkConfig{};
		
        T scalar = static_cast<T>(3.141592653);
		const auto totalBytes = 1 * size * sizeof(T);

		auto allocator = [&]() { 
			auto res = createAlignedVector<T>(size);
			memcpy(res.data(), a.data(), size * sizeof(T));
			return res;
		};

		config.compareOver = {"eigen " + type, "blas " + type};
		#if defined(STALKER_SIMD_AVX2_OK)
		config.name = "stalker avx2 " + type;
		_benchmarkPAPI(
			[&](auto& dst) { 
				VectorMath::scale<T, ExecutionTraitSIMD<T_SIMD::AVX2, true, Unroll>>(size, dst.data(), scalar);
			},
			allocator, config, totalBytes, size
		);
		#endif

		#if defined(STALKER_SIMD_AVX512_OK)
		config.name = "stalker avx512 " + type;
		_benchmarkPAPI(
			[&](auto& dst) {
				VectorMath::scale<T, ExecutionTraitSIMD<T_SIMD::AVX512, true, Unroll>>(size, dst.data(), scalar);
			},
			allocator, config, totalBytes, size
		);
		#endif

		config.compareOver = {};

		#if STALKER_BENCH_EIGEN_AVAILABLE
		{
			config.name = "eigen " + type;
			_benchmarkPAPI(
				[&](auto& dst) {
					Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, 1>> ed(dst.data(), size);
					ed.noalias() = scalar * ed;
				},
				allocator, config, totalBytes, size
			);
		}
		#endif
		#if STALKER_BENCH_OPENBLAS_AVAILABLE
		if (size <= 2147483647) {
			config.name = "blas " + type;
			_benchmarkPAPI(
				[&](auto& dst) {
					if constexpr (std::is_same_v<T, float>) {
						cblas_sscal(static_cast<int>(size), scalar, dst.data(), 1);
					} else if constexpr (std::is_same_v<T, double>) {
						cblas_dscal(static_cast<int>(size), scalar, dst.data(), 1);
					}
				},
				allocator, config, totalBytes, size
			);
		}
		#endif
		_logs.exportToJSON(_logExportPath + "/scale_t_" + type + "_s" + std::to_string(size) + "_" + std::to_string(size) + "_u" + std::to_string(Unroll));
		_logs.clear();
	}

	template<typename T, size_t Unroll>
	void _testDot(size_t size, const std::vector<T, Stalker::Memory::AlignedAllocator<T, DefaultAlignment()>>& a, const std::vector<T, Stalker::Memory::AlignedAllocator<T, DefaultAlignment()>>& b) {

		printTitle(std::string("Operation: Dot | Type: ") + typeid(T).name() + " | Unroll: " + std::to_string(Unroll) + " | Size: " + std::to_string(size), "-", T_Color::TOXIC_GREEN);

		auto papiConfig = PAPILogsConfig{};
		_clearAndResetLogs(size, Unroll, papiConfig);

		const std::string type = typeid(T).name();
		auto config = SingleBenchmarkConfig{};
		
		auto allocator = [&]() { return T{}; };

		const auto totalBytes = 2 * size * sizeof(T);

		
		#if defined(STALKER_SIMD_AVX2_OK)
		config.name = "stalker avx2 " + type;
		config.compareOver = { "eigen " + type, "blas " + type };
		_benchmarkPAPI(
			[&](auto& dst) {
				dst = VectorMath::dot<T, ExecutionTraitSIMD<T_SIMD::AVX2, true, Unroll>>(size, a.data(), b.data());
				std::cout << dst << std::endl;
			},
			allocator, config, totalBytes, size
		);
		#endif

		#if defined(STALKER_SIMD_AVX512_OK)
		config.name = "stalker avx512 " + type;
		_benchmarkPAPI(
			[&](auto& dst) {
				dst = VectorMath::dot<T, ExecutionTraitSIMD<T_SIMD::AVX512, true, Unroll>>(size, a.data(), b.data());
				std::cout << dst << std::endl;
			},
			allocator, config, totalBytes, size
		);
		#endif
		
		config.compareOver = { };

		#if defined(STALKER_BENCH_EIGEN_AVAILABLE)
		Eigen::Map<const Eigen::Matrix<T, Eigen::Dynamic, 1>> ea(a.data(), size);
		Eigen::Map<const Eigen::Matrix<T, Eigen::Dynamic, 1>> eb(b.data(), size);
		config.name = "eigen " + type;
		_benchmarkPAPI(
			[&](auto& dst) {
				dst = ea.dot(eb);
				std::cout << dst << std::endl;
			},
			allocator, config, totalBytes, size
		);
		#endif

		#if STALKER_BENCH_OPENBLAS_AVAILABLE
		if (size <= 2147483647) {
			config.name = "blas " + type;
			_benchmarkPAPI(
				[&](auto& dst) {
					if constexpr (std::is_same_v<T, float>) {
						dst = cblas_sdot(static_cast<int>(size), a.data(), 1, b.data(), 1);
					} else if constexpr (std::is_same_v<T, double>) {
						dst = cblas_ddot(static_cast<int>(size), a.data(), 1, b.data(), 1);
					}
					std::cout << dst << std::endl;
				},
				allocator, config, totalBytes, size
			);
		}
		#endif
		_logs.exportToJSON(_logExportPath + "/dot_t_" + type + "_s" + std::to_string(size) + "_" + std::to_string(size) + "_u" + std::to_string(Unroll));
		_logs.clear();
	}

	template<typename T, size_t Unroll>
	void _testAxpy(size_t size, const std::vector<T, Stalker::Memory::AlignedAllocator<T, DefaultAlignment()>>& x, const std::vector<T, Stalker::Memory::AlignedAllocator<T, DefaultAlignment()>>& y) {

		printTitle(std::string("Operation: AXPY | Type: ") + typeid(T).name() + " | Unroll: " + std::to_string(Unroll) + " | Size: " + std::to_string(size), "-", T_Color::TOXIC_GREEN);

		auto papiConfig = PAPILogsConfig{};
		_clearAndResetLogs(size, Unroll, papiConfig);

		const std::string type = typeid(T).name();
		auto config = SingleBenchmarkConfig{};
		
		auto allocator = [&]() { return createAlignedVector<T>(size); };

		T a = static_cast<T>(3.141592653);
		const auto totalBytes = 3 * size * sizeof(T) ;

			
		#if defined(STALKER_SIMD_AVX2_OK)
		config.name = "stalker avx2 " + type;
		config.compareOver = { "eigen " + type, "blas " + type };
		_benchmarkPAPI(
			[&](auto& dst) {
				VectorMath::axpy<T, ExecutionTraitSIMD<T_SIMD::AVX2, true, Unroll>>(size, x.data(), y.data(), dst.data(), a);
			},
			allocator, config, totalBytes, size
		);
		#endif
		#if defined(STALKER_SIMD_AVX512_OK)
		config.name = "stalker avx512 " + type;
		config.compareOver = { "eigen " + type, "blas " + type };
		_benchmarkPAPI(
			[&](auto& dst) {
				VectorMath::axpy<T, ExecutionTraitSIMD<T_SIMD::AVX512, true, Unroll>>(size, x.data(), y.data(), dst.data(), a);
			},
			allocator, config, totalBytes, size
		);
		#endif

		#if STALKER_BENCH_EIGEN_AVAILABLE
		Eigen::Map<const Eigen::Matrix<T, Eigen::Dynamic, 1>> ex(x.data(), size);
		Eigen::Map<const Eigen::Matrix<T, Eigen::Dynamic, 1>> ey(y.data(), size);
		config.name = "eigen " + type;
		config.compareOver.clear();
		_benchmarkPAPI(
			[&](auto& dst) {
				Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, 1>> ed(dst.data(), size);
				ed.noalias() = a * ex + ey;
			},
			allocator, config, totalBytes, size
		);
		#endif

		#if STALKER_BENCH_OPENBLAS_AVAILABLE
		if (size <= 2147483647) {
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
				allocator, config, totalBytes, size
			);
		}
		#endif

		_logs.exportToJSON(_logExportPath + "/axpy_t_" + type + "_s" + std::to_string(size) + "_" + std::to_string(size) + "_u" + std::to_string(Unroll));
		_logs.clear();
	}

	template<typename T, size_t Unroll>
	void _testSum(size_t size, const std::vector<T, Stalker::Memory::AlignedAllocator<T, DefaultAlignment()>>& a) {

		printTitle(std::string("Operation: Sum | Type: ") + typeid(T).name() + " | Unroll: " + std::to_string(Unroll) + " | Size: " + std::to_string(size), "-", T_Color::TOXIC_GREEN);

		auto papiConfig = PAPILogsConfig{};
		_clearAndResetLogs(size, Unroll, papiConfig);

		const std::string type = typeid(T).name();
		auto config = SingleBenchmarkConfig{};
		
		auto allocator = [&]() { return T{}; }; 

		const auto totalBytes = 1 * size * sizeof(T);

		config.compareOver = { "std::accumulate " + type, "eigen " + type };
		#if defined(STALKER_SIMD_AVX2_OK)
		config.name = "stalker avx2 " + type;
		_benchmarkPAPI(
			[&](auto& dst) {
				dst = VectorMath::sum<T, ExecutionTraitSIMD<T_SIMD::AVX2, true, Unroll>>(size, a.data());
			},
			allocator, config, totalBytes, size
		);
		#endif
		
		#if defined(STALKER_SIMD_AVX512_OK)
		config.name = "stalker avx512 " + type;
		config.compareOver = { "std::accumulate " + type, "eigen " + type };
		_benchmarkPAPI(
			[&](auto& dst) {
				dst = VectorMath::sum<T, ExecutionTraitSIMD<T_SIMD::AVX512, true, Unroll>>(size, a.data());
			},
			allocator, config, totalBytes, size
		);
		#endif

		// std::accumulate reference
		config.compareOver = {};
		config.name = "std::accumulate " + type;
		_benchmarkPAPI(
			[&](auto& dst) {
				dst = std::accumulate(a.begin(), a.end(), static_cast<T>(0));
			},
			allocator, config, totalBytes, size
		);
		#if STALKER_BENCH_EIGEN_AVAILABLE
		{
			Eigen::Map<const Eigen::Matrix<T, Eigen::Dynamic, 1>> ea(a.data(), size);
			config.name = "eigen " + type;
			_benchmarkPAPI(
				[&](auto& dst) {
					dst = ea.sum();
				},
				allocator, config, totalBytes, size
			);
		}
		#endif

		_logs.exportToJSON(_logExportPath + "/sum_t_" + type + "_s" + std::to_string(size) + "_" + std::to_string(size) + "_u" + std::to_string(Unroll));
		_logs.clear();
	}

};

};