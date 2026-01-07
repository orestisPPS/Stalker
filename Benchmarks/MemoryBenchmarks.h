#pragma once

#include "Benchmark.h"
#include <Stalker/Memory/Allocators.h>
#include <Stalker/Memory/MemoryOperations.h>
#include <Stalker/Mathematics/Random.h>
#include <Stalker/Utility/Logs.h>
#include <cblas.h>
#include <Eigen/Dense>

#include <array>
#include <vector>
#include <algorithm>

namespace Benchmarks {

	using namespace Stalker::Memory;
	using namespace Stalker::Utility;
	using namespace Stalker::Core::Config;

	class MemoryBenchmarks : public Benchmark<MemoryBenchmarks> {
	public:
		MemoryBenchmarks(const std::string& logExportPath) : Benchmark("Memory Operations", logExportPath) {}
	protected:
		friend class Benchmark<MemoryBenchmarks>;

		void execute() {
			for (size_t memOpIndex = 0; memOpIndex < Benchmarks::MemOpNames.size(); ++memOpIndex) {
				const auto& memOpName = Benchmarks::MemOpNames[memOpIndex];
                #define PROCESS_TYPE(T) \
                    for (size_t size : Benchmarks::Sizes) { \
                        if (memOpName == "copy") { \
                            auto src = this->_createData<T>(size); \
                            auto papiConfig = PAPILogsConfig{}; \
                            this->_prepareLogs(size, papiConfig); \
                            this->_forEachStorePolicy([&](auto policy) { \
                                constexpr T_SIMDStore Policy = decltype(policy)::value; \
                                this->_forEachUnroll([&](auto unroll) { \
                                    constexpr size_t U = decltype(unroll)::value; \
                                    this->_testCopyStalker<T, U, Policy>(size, src); \
                                }); \
                            }); \
							this->_testCopyBenchmarks<T>(size, src); \
							this->_logs.exportToJSON(this->_logExportPath + "/copy_t_" + typeid(T).name() + "_s" + std::to_string(size)); \
							this->_logs.clear(); \
                        } \
                        else if (memOpName == "setValue") { \
                            auto papiConfig = PAPILogsConfig{}; \
                            this->_prepareLogs(size, papiConfig); \
                            this->_forEachStorePolicy([&](auto policy) { \
                                constexpr T_SIMDStore Policy = decltype(policy)::value; \
                                this->_forEachUnroll([&](auto unroll) { \
                                    constexpr size_t U = decltype(unroll)::value; \
                                    this->_testSetValueStalker<T, U, Policy>(size); \
                                }); \
                            }); \
							this->_testSetValueBenchmarks<T>(size); \
							this->_logs.exportToJSON(this->_logExportPath + "/set_value_t_" + typeid(T).name() + "_s" + std::to_string(size)); \
							this->_logs.clear(); \
                        } \
                        else if (memOpName == "setZero") { \
                            auto papiConfig = PAPILogsConfig{}; \
                            this->_prepareLogs(size, papiConfig); \
                            this->_forEachStorePolicy([&](auto policy) { \
                                constexpr T_SIMDStore Policy = decltype(policy)::value; \
                                this->_forEachUnroll([&](auto unroll) { \
                                    constexpr size_t U = decltype(unroll)::value; \
                                    this->_testSetZeroStalker<T, U, Policy>(size); \
                                }); \
                            }); \
							this->_testSetZeroBenchmarks<T>(size); \
							this->_logs.exportToJSON(this->_logExportPath + "/set_zero_t_" + typeid(T).name() + "_s" + std::to_string(size)); \
							this->_logs.clear(); \
                        } \
                    }
                BENCH_FOR_EACH_TYPE(PROCESS_TYPE)
                #undef PROCESS_TYPE
			}
		}
	
		template<typename T, size_t Unroll, T_SIMDStore Policy>
		void _testCopyStalker(size_t size, const std::vector<T, Stalker::Memory::AlignedAllocator<T, DefaultAlignment()>>& src) {

			const std::string policyTitle = (Policy == T_SIMDStore::Streamed) ? "Streamed" : "Cached";
			printTitle(std::string("Operation: Copy (Stalker) | Type: ") + typeid(T).name() + " | Unroll: " + std::to_string(Unroll) + " | Store: " + policyTitle + " | Size: " + std::to_string(size), "-", T_Color::TOXIC_GREEN);
			
			const std::string type = typeid(T).name();
            const std::string policyStr = (Policy == T_SIMDStore::Streamed) ? "_stream" : "_cache";
			auto config = SingleBenchmarkConfig{};
			
			auto allocator = [&]() { return createAlignedVector<T>(size); };
			const auto totalBytes = 2 * size * sizeof(T);

			config.compareOver = { "std::memcpy " + type, "blas " + type, "eigen " + type };

			#if defined(STALKER_SIMD_AVX2_OK)
			config.name = "stalker_avx2_u" + std::to_string(Unroll) + policyStr + "_" + type ;
			_benchmarkPAPI(
				[&](auto& dst) {
					MemoryOperations::copy<T, ExecutionTraitSIMD<T_SIMD::AVX2, true, Unroll, Policy>>(size, dst.data(), src.data());
				},
				allocator, config, totalBytes, size
			);
			#endif
			
			#if defined(STALKER_SIMD_AVX512_OK)
			config.name = "stalker_avx512_u" + std::to_string(Unroll) + policyStr + "_" + type ;
			_benchmarkPAPI(
				[&](auto& dst) {
						MemoryOperations::copy<T, ExecutionTraitSIMD<T_SIMD::AVX512, true, Unroll, Policy>>(size, dst.data(), src.data());
				},
				allocator, config, totalBytes, size
			);
			#endif
		}

		template<typename T>
		void _testCopyBenchmarks(size_t size, const std::vector<T, Stalker::Memory::AlignedAllocator<T, DefaultAlignment()>>& src) {
			
			printTitle(std::string("Operation: Copy (Benchmarks) | Type: ") + typeid(T).name() + " | Size: " + std::to_string(size), "-", T_Color::TOXIC_GREEN);

			const std::string type = typeid(T).name();
			auto config = SingleBenchmarkConfig{};
			
			auto allocator = [&]() { return createAlignedVector<T>(size); };
			const auto totalBytes = 2 * size * sizeof(T);

			config.compareOver = {};
			config.name = "std::memcpy " + type;
			_benchmarkPAPI(
				[&](auto& dst) {
						std::memcpy(dst.data(), src.data(), size * sizeof(T));
				},
				allocator, config, totalBytes, size
			);

			#if STALKER_BENCH_OPENBLAS_AVAILABLE
			config.name = "blas " + type;
			_benchmarkPAPI(
				[&](auto& dst) {
					if constexpr (std::is_same_v<T, float>) {
							cblas_scopy(static_cast<int>(size), src.data(), 1, dst.data(), 1);
					} else if constexpr (std::is_same_v<T, double>) {
							cblas_dcopy(static_cast<int>(size), src.data(), 1, dst.data(), 1);
					}
				},
				allocator, config, totalBytes, size
			);
			#endif

			#if STALKER_BENCH_EIGEN_AVAILABLE

			auto eigenAllocator = [&]() { return createAlignedVector<T>(size); };

			config.name = "eigen " + type;
			Eigen::Map<const Eigen::Matrix<T, Eigen::Dynamic, 1>, Eigen::Aligned> eigenData(src.data(), static_cast<int>(size));
			_benchmarkPAPI(
				[&](auto& dst) {
					Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, 1>, Eigen::Aligned> dstMap(dst.data(), static_cast<int>(size));
					dstMap = eigenData; // assignment timing only
				},
				eigenAllocator, config, totalBytes, size
			);
			#endif
		}

		template<typename T, size_t Unroll, T_SIMDStore Policy>
		void _testSetValueStalker(size_t size) {

			const std::string policyTitle = (Policy == T_SIMDStore::Streamed) ? "Streamed" : "Cached";
			printTitle(std::string("Operation: SetValue (Stalker) | Type: ") + typeid(T).name() + " | Unroll: " + std::to_string(Unroll) + " | Store: " + policyTitle + " | Size: " + std::to_string(size), "-", T_Color::TOXIC_GREEN);
			
			const std::string type = typeid(T).name();
            const std::string policyStr = (Policy == T_SIMDStore::Streamed) ? "_stream" : "_cache";
			auto config = SingleBenchmarkConfig{};
			
			auto allocator = [&]() { return createAlignedVector<T>(size); };
			T value = static_cast<T>(3.14159265358979323846);
			const auto totalBytes = 1 * size * sizeof(T);

			config.compareOver = { "std::fill " + type, "eigen " + type };

			#if defined(STALKER_SIMD_AVX2_OK)
			config.name = "stalker_avx2_u" + std::to_string(Unroll) + policyStr + "_" + type ;
			_benchmarkPAPI(
				[&](auto& dst) {
					MemoryOperations::setValue<T, ExecutionTraitSIMD<T_SIMD::AVX2, true, Unroll, Policy>>(size, dst.data(), value);
				},
					allocator, config, totalBytes, size
				);
				#endif
				
			#if defined(STALKER_SIMD_AVX512_OK)
			config.name = "stalker_avx512_u" + std::to_string(Unroll) + policyStr + "_" + type ;
			_benchmarkPAPI(
				[&](auto& dst) {
						MemoryOperations::setValue<T, ExecutionTraitSIMD<T_SIMD::AVX512, true, Unroll, Policy>>(size, dst.data(), value);
				},
				allocator, config, totalBytes, size
			);
			#endif
		}

		template<typename T>
		void _testSetValueBenchmarks(size_t size) {

			printTitle(std::string("Operation: SetValue (Benchmarks) | Type: ") + typeid(T).name() + " | Size: " + std::to_string(size), "-", T_Color::TOXIC_GREEN);
			
			const std::string type = typeid(T).name();
			auto config = SingleBenchmarkConfig{};
			
			auto allocator = [&]() { return createAlignedVector<T>(size); };
			T value = static_cast<T>(3.14159265358979323846);
			const auto totalBytes = 1 * size * sizeof(T);

			config.compareOver = {};
			config.name = "std::fill " + type;
			_benchmarkPAPI(
				[&](auto& dst) {
						std::fill(dst.begin(), dst.end(), value);
				},
				allocator, config, totalBytes, size
			);

			#if STALKER_BENCH_EIGEN_AVAILABLE

			// Eigen baseline for setValue: allocator returns owning vector, lambda only sets values.
			auto eigenAllocator = [&]() { return createAlignedVector<T>(size); };

			config.name = "eigen " + type;
			_benchmarkPAPI(
				[&](auto& dst) {
					Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, 1>, Eigen::Aligned> dstMap(dst.data(), static_cast<int>(size));
					dstMap.setConstant(value);
				},
				eigenAllocator, config, totalBytes, size
			);
			#endif
		}
		template<typename T, size_t Unroll, T_SIMDStore Policy>
		void _testSetZeroStalker(size_t size) {

			const std::string policyTitle = (Policy == T_SIMDStore::Streamed) ? "Streamed" : "Cached";
			printTitle(std::string("Operation: SetZero (Stalker) | Type: ") + typeid(T).name() + " | Unroll: " + std::to_string(Unroll) + " | Store: " + policyTitle + " | Size: " + std::to_string(size), "-", T_Color::TOXIC_GREEN);
			
			const std::string type = typeid(T).name();
            const std::string policyStr = (Policy == T_SIMDStore::Streamed) ? "_stream" : "_cache";
			auto config = SingleBenchmarkConfig{};
			
			auto allocator = [&]() { return createAlignedVector<T>(size); };
			const auto totalBytes = 1 * size * sizeof(T);

			config.compareOver = { "std::memset " + type, "eigen " + type };

			#if defined(STALKER_SIMD_AVX2_OK)
			config.name = "stalker_avx2_u" + std::to_string(Unroll) + policyStr + "_" + type ;
			_benchmarkPAPI(
				[&](auto& dst) {
					MemoryOperations::setZero<T, ExecutionTraitSIMD<T_SIMD::AVX2, true, Unroll, Policy>>(size, dst.data());
				},
					allocator, config, totalBytes, size
				);
				#endif
				
			#if defined(STALKER_SIMD_AVX512_OK)
			config.name = "stalker_avx512_u" + std::to_string(Unroll) + policyStr + "_" + type ;
			_benchmarkPAPI(
				[&](auto& dst) {
					MemoryOperations::setZero<T, ExecutionTraitSIMD<T_SIMD::AVX512, true, Unroll, Policy>>(size, dst.data());
				},
				allocator, config, totalBytes, size
			);
			#endif
		}

		template<typename T>
		void _testSetZeroBenchmarks(size_t size) {

			printTitle(std::string("Operation: SetZero (Benchmarks) | Type: ") + typeid(T).name() + " | Size: " + std::to_string(size), "-", T_Color::TOXIC_GREEN);
			
			const std::string type = typeid(T).name();
			auto config = SingleBenchmarkConfig{};
			
			auto allocator = [&]() { return createAlignedVector<T>(size); };
			const auto totalBytes = 1 * size * sizeof(T);

			config.compareOver = {};
			config.name = "std::memset " + type;
			_benchmarkPAPI(
				[&](auto& dst) {
						std::memset(dst.data(), 0, size * sizeof(T));
				},
				allocator, config, totalBytes, size
			);

			#if STALKER_BENCH_EIGEN_AVAILABLE

			auto eigenAllocator = [&]() { return createAlignedVector<T>(size); };

			config.name = "eigen " + type;
			_benchmarkPAPI(
				[&](auto& dst) {
					Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, 1>, Eigen::Aligned> dstMap(dst.data(), static_cast<int>(size));
					dstMap.setZero();
				},
				eigenAllocator, config, totalBytes, size
			);
			#endif
		}
	};

};