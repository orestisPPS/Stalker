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
	_logs.exportToJSON(_logExportPath + "/scale_t_" + type + "_s" + std::to_string(size) + "_u" + std::to_string(Unroll));
		_logs.clear();
	}

};

};