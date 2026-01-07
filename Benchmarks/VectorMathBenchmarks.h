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
                            auto papiConfig = PAPILogsConfig{}; \
                            this->_prepareLogs(size, papiConfig); \
                            this->_forEachStorePolicy([&](auto policy) { \
                                constexpr T_SIMDStore Policy = decltype(policy)::value; \
                                this->_forEachUnroll([&](auto unroll) { \
                                    constexpr size_t U = decltype(unroll)::value; \
                                    this->_testScaleStalker<T, U, Policy>(size, a); \
                                }); \
                            }); \
							this->_testScaleBenchmarks<T>(size, a); \
							this->_logs.exportToJSON(this->_logExportPath + "/scale_t_" + typeid(T).name() + "_s" + std::to_string(size)); \
							this->_logs.clear(); \
                        } \
                        else if (mathOpName == "dot") { \
                            auto a = this->_createData<T>(size); \
                            auto b = this->_createData<T>(size); \
                            auto papiConfig = PAPILogsConfig{}; \
                            this->_prepareLogs(size, papiConfig); \
							this->_forEachUnroll([&](auto unroll) { \
								constexpr size_t U = decltype(unroll)::value; \
								this->_testDotStalker<T, U>(size, a, b); \
							}); \
							this->_testDotBenchmarks<T>(size, a, b); \
							this->_logs.exportToJSON(this->_logExportPath + "/dot_t_" + typeid(T).name() + "_s" + std::to_string(size)); \
							this->_logs.clear(); \
                        } \
                        else if (mathOpName == "axpy") { \
                            auto x = this->_createData<T>(size); \
                            auto y = this->_createData<T>(size); \
                            auto papiConfig = PAPILogsConfig{}; \
                            this->_prepareLogs(size, papiConfig); \
                            this->_forEachStorePolicy([&](auto policy) { \
                                constexpr T_SIMDStore Policy = decltype(policy)::value; \
                                this->_forEachUnroll([&](auto unroll) { \
                                    constexpr size_t U = decltype(unroll)::value; \
                                    this->_testAxpyStalker<T, U, Policy>(size, x, y); \
                                }); \
                            }); \
							this->_testAxpyBenchmarks<T>(size, x, y); \
							this->_logs.exportToJSON(this->_logExportPath + "/axpy_t_" + typeid(T).name() + "_s" + std::to_string(size)); \
							this->_logs.clear(); \
                        } \
                        else if (mathOpName == "sum") { \
                            auto a = this->_createData<T>(size); \
                            auto papiConfig = PAPILogsConfig{}; \
                            this->_prepareLogs(size, papiConfig); \
							this->_forEachUnroll([&](auto unroll) { \
								constexpr size_t U = decltype(unroll)::value; \
								this->_testSumStalker<T, U>(size, a); \
							}); \
							this->_testSumBenchmarks<T>(size, a); \
							this->_logs.exportToJSON(this->_logExportPath + "/sum_t_" + typeid(T).name() + "_s" + std::to_string(size)); \
							this->_logs.clear(); \
                        } \
                    }
                BENCH_FOR_EACH_TYPE(PROCESS_TYPE)
                #undef PROCESS_TYPE
			}
		}
	
	template<typename T, size_t Unroll, T_SIMDStore Policy>
	void _testScaleStalker(size_t size, const std::vector<T, Stalker::Memory::AlignedAllocator<T, DefaultAlignment()>>& a) {

		const std::string policyTitle = (Policy == T_SIMDStore::Streamed) ? "Streamed" : "Cached";
		printTitle(std::string("Operation: Scale | Type: ") + typeid(T).name() + " | Unroll: " + std::to_string(Unroll) + " | Store: " + policyTitle + " | Size: " + std::to_string(size), "-", T_Color::TOXIC_GREEN);


		const std::string type = typeid(T).name();
        const std::string policyStr = (Policy == T_SIMDStore::Streamed) ? "_stream" : "_cache";
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
		config.name = "stalker_avx2_u" + std::to_string(Unroll) + policyStr + "_" + type ;
		_benchmarkPAPI(
			[&](auto& dst) { 
				VectorMath::scale<T, ExecutionTraitSIMD<T_SIMD::AVX2, true, Unroll, Policy>>(size, dst.data(), scalar);
			},
			allocator, config, totalBytes, size
		);
		#endif

		#if defined(STALKER_SIMD_AVX512_OK)
		config.name = "stalker_avx512_u" + std::to_string(Unroll) + policyStr + "_" + type ;
		_benchmarkPAPI(
			[&](auto& dst) {
				VectorMath::scale<T, ExecutionTraitSIMD<T_SIMD::AVX512, true, Unroll, Policy>>(size, dst.data(), scalar);
			},
			allocator, config, totalBytes, size
		);
		#endif
	}

	template<typename T>
	void _testScaleBenchmarks(size_t size, const std::vector<T, Stalker::Memory::AlignedAllocator<T, DefaultAlignment()>>& a) {
		const std::string type = typeid(T).name();
		auto config = SingleBenchmarkConfig{};
		
        T scalar = static_cast<T>(3.141592653);
		const auto totalBytes = 1 * size * sizeof(T);

		auto allocator = [&]() { 
			auto res = createAlignedVector<T>(size);
			memcpy(res.data(), a.data(), size * sizeof(T));
			return res;
		};

		#if STALKER_BENCH_EIGEN_AVAILABLE
		{
			config.name = "eigen " + type;
			_benchmarkPAPI(
				[&](auto& dst) {
					Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, 1>, Eigen::Aligned> ed(dst.data(), size);
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
	}

	template<typename T, size_t Unroll>
	void _testDotStalker(size_t size, const std::vector<T, Stalker::Memory::AlignedAllocator<T, DefaultAlignment()>>& a, const std::vector<T, Stalker::Memory::AlignedAllocator<T, DefaultAlignment()>>& b) {

		printTitle(std::string("Operation: Dot | Type: ") + typeid(T).name() + " | Unroll: " + std::to_string(Unroll) + " | Size: " + std::to_string(size), "-", T_Color::TOXIC_GREEN);

		const std::string type = typeid(T).name();
		auto config = SingleBenchmarkConfig{};
		
		auto allocator = [&]() { return T{}; };

		const auto totalBytes = 2 * size * sizeof(T);

		
		#if defined(STALKER_SIMD_AVX2_OK)
		config.name = "stalker_avx2_u" + std::to_string(Unroll) + "_" + type ;
		config.compareOver = { "eigen " + type, "blas " + type };
		_benchmarkPAPI(
			[&](auto& dst) {
				return VectorMath::dot<T, ExecutionTraitSIMD<T_SIMD::AVX2, true, Unroll>>(size, a.data(), b.data());
			},
			allocator, config, totalBytes, size
		);
		#endif

		#if defined(STALKER_SIMD_AVX512_OK)
		config.name = "stalker_avx512_u" + std::to_string(Unroll) + "_" + type ;
		_benchmarkPAPI(
			[&](auto& dst) {
				return VectorMath::dot<T, ExecutionTraitSIMD<T_SIMD::AVX512, true, Unroll>>(size, a.data(), b.data());
			},
			allocator, config, totalBytes, size
		);
		#endif
	}

	template<typename T>
	void _testDotBenchmarks(size_t size, const std::vector<T, Stalker::Memory::AlignedAllocator<T, DefaultAlignment()>>& a, const std::vector<T, Stalker::Memory::AlignedAllocator<T, DefaultAlignment()>>& b) {
		printTitle(std::string("Operation: Dot (Benchmarks) | Type: ") + typeid(T).name() + " | Size: " + std::to_string(size), "-", T_Color::TOXIC_GREEN);

		const std::string type = typeid(T).name();
		auto config = SingleBenchmarkConfig{};
		
		auto allocator = [&]() { return T{}; };

		const auto totalBytes = 2 * size * sizeof(T);

		config.compareOver = {};
		config.name = "std::inner_product " + type;
		_benchmarkPAPI(
			[&](auto& dst) {
				return std::inner_product(a.begin(), a.end(), b.begin(), static_cast<T>(0));
			},
			allocator, config, totalBytes, size
		);

		#if defined(STALKER_BENCH_EIGEN_AVAILABLE)
		Eigen::Map<const Eigen::Matrix<T, Eigen::Dynamic, 1>, Eigen::Aligned> ea(a.data(), size);
		Eigen::Map<const Eigen::Matrix<T, Eigen::Dynamic, 1>, Eigen::Aligned> eb(b.data(), size);
		config.name = "eigen " + type;
		_benchmarkPAPI(
			[&](auto& dst) {
				return ea.dot(eb);
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
						return cblas_sdot(static_cast<int>(size), a.data(), 1, b.data(), 1);
					} else if constexpr (std::is_same_v<T, double>) {
						return cblas_ddot(static_cast<int>(size), a.data(), 1, b.data(), 1);
					}
				},
				allocator, config, totalBytes, size
			);
		}
		#endif
	}

	template<typename T, size_t Unroll, T_SIMDStore Policy>
	void _testAxpyStalker(size_t size, const std::vector<T, Stalker::Memory::AlignedAllocator<T, DefaultAlignment()>>& x, const std::vector<T, Stalker::Memory::AlignedAllocator<T, DefaultAlignment()>>& y) {

		const std::string policyTitle = (Policy == T_SIMDStore::Streamed) ? "Streamed" : "Cached";
		printTitle(std::string("Operation: AXPY (Stalker) | Type: ") + typeid(T).name() + " | Unroll: " + std::to_string(Unroll) + " | Store: " + policyTitle + " | Size: " + std::to_string(size), "-", T_Color::TOXIC_GREEN);


		const std::string type = typeid(T).name();
        const std::string policyStr = (Policy == T_SIMDStore::Streamed) ? "_stream" : "_cache";
		auto config = SingleBenchmarkConfig{};
		
		auto allocator = [&]() { return createAlignedVector<T>(size); };

		T a = static_cast<T>(3.141592653);
		const auto totalBytes = 3 * size * sizeof(T) ;

		#if defined(STALKER_SIMD_AVX2_OK)
		config.name = "stalker_avx2_u" + std::to_string(Unroll) + policyStr + "_" + type ;
		config.compareOver = { "eigen " + type, "blas " + type };
		_benchmarkPAPI(
			[&](auto& dst) {
				VectorMath::axpy<T, ExecutionTraitSIMD<T_SIMD::AVX2, true, Unroll, Policy>>(size, x.data(), y.data(), dst.data(), a);
			},
			allocator, config, totalBytes, size
		);
		#endif
		#if defined(STALKER_SIMD_AVX512_OK)
		config.name = "stalker_avx512_u" + std::to_string(Unroll) + policyStr + "_" + type ;
		config.compareOver = { "eigen " + type, "blas " + type };
		_benchmarkPAPI(
			[&](auto& dst) {
				VectorMath::axpy<T, ExecutionTraitSIMD<T_SIMD::AVX512, true, Unroll, Policy>>(size, x.data(), y.data(), dst.data(), a);
			},
			allocator, config, totalBytes, size
		);
		#endif
	}

	template<typename T>
	void _testAxpyBenchmarks(size_t size, const std::vector<T, Stalker::Memory::AlignedAllocator<T, DefaultAlignment()>>& x, const std::vector<T, Stalker::Memory::AlignedAllocator<T, DefaultAlignment()>>& y) {
		printTitle(std::string("Operation: AXPY (Benchmarks) | Type: ") + typeid(T).name() + " | Size: " + std::to_string(size), "-", T_Color::TOXIC_GREEN);

		const std::string type = typeid(T).name();
		auto config = SingleBenchmarkConfig{};
		
		auto allocator = [&]() { return createAlignedVector<T>(size); };

		T a = static_cast<T>(3.141592653);
		const auto totalBytes = 3 * size * sizeof(T) ;

		#if STALKER_BENCH_EIGEN_AVAILABLE
		Eigen::Map<const Eigen::Matrix<T, Eigen::Dynamic, 1>, Eigen::Aligned> ex(x.data(), size);
		Eigen::Map<const Eigen::Matrix<T, Eigen::Dynamic, 1>, Eigen::Aligned> ey(y.data(), size);
		config.name = "eigen " + type;
		config.compareOver.clear();
		_benchmarkPAPI(
			[&](auto& dst) {
				Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, 1>, Eigen::Aligned> ed(dst.data(), size);
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
	}

	template<typename T, size_t Unroll>
	void _testSumStalker(size_t size, const std::vector<T, Stalker::Memory::AlignedAllocator<T, DefaultAlignment()>>& a) {

		printTitle(std::string("Operation: Sum (Stalker) | Type: ") + typeid(T).name() + " | Unroll: " + std::to_string(Unroll) + " | Size: " + std::to_string(size), "-", T_Color::TOXIC_GREEN);

		const std::string type = typeid(T).name();
		auto config = SingleBenchmarkConfig{};
		
		auto allocator = [&]() { return T{}; }; 

		const auto totalBytes = 1 * size * sizeof(T);

		config.compareOver = { "std::accumulate " + type, "eigen " + type };
		#if defined(STALKER_SIMD_AVX2_OK)
		config.name = "stalker_avx2_u" + std::to_string(Unroll) + "_" + type ;
		_benchmarkPAPI(
			[&](auto& dst) {
				return VectorMath::sum<T, ExecutionTraitSIMD<T_SIMD::AVX2, true, Unroll>>(size, a.data());
			},
			allocator, config, totalBytes, size
		);
		#endif
		
		#if defined(STALKER_SIMD_AVX512_OK)
		config.name = "stalker_avx512_u" + std::to_string(Unroll) + "_" + type ;
		config.compareOver = { "std::accumulate " + type, "eigen " + type };
		_benchmarkPAPI(
			[&](auto& dst) {
				return VectorMath::sum<T, ExecutionTraitSIMD<T_SIMD::AVX512, true, Unroll>>(size, a.data());
			},
			allocator, config, totalBytes, size
		);
		#endif
	}

	template<typename T>
	void _testSumBenchmarks(size_t size, const std::vector<T, Stalker::Memory::AlignedAllocator<T, DefaultAlignment()>>& a) {

		printTitle(std::string("Operation: Sum (Benchmarks) | Type: ") + typeid(T).name() + " | Size: " + std::to_string(size), "-", T_Color::TOXIC_GREEN);


		const std::string type = typeid(T).name();
		auto config = SingleBenchmarkConfig{};
		
		auto allocator = [&]() { return T{}; }; 

		const auto totalBytes = 1 * size * sizeof(T);

		// std::accumulate reference
		config.compareOver = {};
		config.name = "std::accumulate " + type;
		_benchmarkPAPI(
			[&](auto& dst) {
				return std::accumulate(a.begin(), a.end(), static_cast<T>(0));
			},
			allocator, config, totalBytes, size
		);
		#if STALKER_BENCH_EIGEN_AVAILABLE
		{
			Eigen::Map<const Eigen::Matrix<T, Eigen::Dynamic, 1>, Eigen::Aligned> ea(a.data(), size);
			config.name = "eigen " + type;
			_benchmarkPAPI(
				[&](auto& dst) {
					return ea.sum();
				},
				allocator, config, totalBytes, size
			);
		}
		#endif
	}

};

};