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
                            this->_forEachUnroll([&](auto unroll) { \
                                constexpr size_t U = decltype(unroll)::value; \
                                this->_testCopy<T, U>(size, src); \
                            }); \
                        } \
                        else if (memOpName == "setValue") { \
                            this->_forEachUnroll([&](auto unroll) { \
                                constexpr size_t U = decltype(unroll)::value; \
                                this->_testSetValue<T, U>(size); \
                            }); \
                        } \
                    }
                BENCH_FOR_EACH_TYPE(PROCESS_TYPE)
                #undef PROCESS_TYPE
			}
		}
	
		template<typename T, size_t Unroll>
		void _testCopy(size_t size, const std::vector<T, Stalker::Memory::AlignedAllocator<T, DefaultAlignment()>>& src) {

			printTitle(std::string("Operation: Copy | Type: ") + typeid(T).name() + " | Unroll: " + std::to_string(Unroll) + " | Size: " + std::to_string(size), "-", T_Color::TOXIC_GREEN);
			
			auto papiConfig = PAPILogsConfig{};
			_clearAndResetLogs(size, Unroll, papiConfig);

			const std::string type = typeid(T).name();
			auto config = SingleBenchmarkConfig{};
			
			auto allocator = [&]() { return createAlignedVector<T>(size); };
			// auto src = this->_createData<T>(size); // Hoisted
			const auto totalBytes = 2 * size * sizeof(T);

			#if defined(STALKER_SIMD_AVX2_OK)
			config.name = "stalker avx2 " + type;
			config.compareOver = { "memcpy " + type, "blas " + type, "eigen " + type };
			_benchmarkPAPI(
				[&](auto& dst) {
					MemoryOperations::copy<T, ExecutionTraitSIMD<T_SIMD::AVX2, Unroll>>(size, dst.data(), src.data());
				},
				allocator, config, totalBytes, size
			);
			#endif
			
			#if defined(STALKER_SIMD_AVX512_OK)
			config.name = "stalker avx512 " + type;
			config.compareOver = { "memcpy " + type, "blas " + type, "eigen " + type };
			_benchmarkPAPI(
					[&](auto& dst) {
							MemoryOperations::copy<T, ExecutionTraitSIMD<T_SIMD::AVX512, Unroll>>(size, dst.data(), src.data());
					},
					allocator, config, totalBytes, size
			);
			#endif

			config.compareOver.clear();
			config.name = "std::memcpy " + type;
			_benchmarkPAPI(
					[&](auto& dst) {
							MemoryOperations::copy<T, ExecutionTraitScalar<true>>(size, dst.data(), src.data());
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
			Eigen::Map<const Eigen::Matrix<T, Eigen::Dynamic, 1>> eigenData(src.data(), static_cast<int>(size));
			_benchmarkPAPI(
				[&](auto& dst) {
					Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, 1>> dstMap(dst.data(), static_cast<int>(size));
					dstMap = eigenData; // assignment timing only
				},
				eigenAllocator, config, totalBytes, size
			);
			#endif

			_logs.exportToJSON(_logExportPath + "/copy_t_" + type + "_s" + std::to_string(size) + "_" + std::to_string(size) + "_u" + std::to_string(Unroll));
			_logs.clear();
		}

		template<typename T, size_t Unroll>
		void _testSetValue(size_t size) {

			printTitle(std::string("Operation: SetValue | Type: ") + typeid(T).name() + " | Unroll: " + std::to_string(Unroll) + " | Size: " + std::to_string(size), "-", T_Color::TOXIC_GREEN);
			
			auto papiConfig = PAPILogsConfig{};
			_clearAndResetLogs(size, Unroll, papiConfig);

			const std::string type = typeid(T).name();
			auto config = SingleBenchmarkConfig{};
			
			auto allocator = [&]() { return createAlignedVector<T>(size); };
			T value = static_cast<T>(3.14159265358979323846);
			const auto totalBytes = 1 * size * sizeof(T);

			#if defined(STALKER_SIMD_AVX2_OK)
			config.name = "stalker avx2 " + type;
			config.compareOver = { "std::fill " + type, "eigen " + type };
			_benchmarkPAPI(
				[&](auto& dst) {
					MemoryOperations::setValue<T, ExecutionTraitSIMD<T_SIMD::AVX2, Unroll>>(size, dst.data(), value);
				},
					allocator, config, totalBytes, size
				);
				#endif
				
			#if defined(STALKER_SIMD_AVX512_OK)
			config.name = "stalker avx512 " + type;
			config.compareOver = { "std::fill " + type, "eigen " + type };
			_benchmarkPAPI(
					[&](auto& dst) {
							MemoryOperations::setValue<T, ExecutionTraitSIMD<T_SIMD::AVX512, Unroll>>(size, dst.data(), value);
					},
					allocator, config, totalBytes, size
			);
			#endif

			config.compareOver.clear();
			config.name = "std::fill " + type;
			_benchmarkPAPI(
					[&](auto& dst) {
							MemoryOperations::setValue<T, ExecutionTraitScalar<true>>(size, dst.data(), value);
					},
					allocator, config, totalBytes, size
			);

			#if STALKER_BENCH_EIGEN_AVAILABLE

			// Eigen baseline for setValue: allocator returns owning vector, lambda only sets values.
			auto eigenAllocator = [&]() { return createAlignedVector<T>(size); };

			config.name = "eigen " + type;
			_benchmarkPAPI(
				[&](auto& dst) {
					Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, 1>> dstMap(dst.data(), static_cast<int>(size));
					dstMap.setConstant(value);
				},
				eigenAllocator, config, totalBytes, size
			);
			#endif

			_logs.exportToJSON(_logExportPath + "/set_value_t_" + type + "_s" + std::to_string(size) + "_" + std::to_string(size) + "_u" + std::to_string(Unroll));
			_logs.clear();
		}
	};

};