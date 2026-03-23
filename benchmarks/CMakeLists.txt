# ========================================
# Kireji Benchmarks - Using Google Benchmark
# ========================================

# Attempt to find Google Benchmark system-wide first
find_package(benchmark QUIET)

# If Google Benchmark is not found, try to fetch it from GitHub
if(NOT benchmark_FOUND)
	message(STATUS "Google Benchmark not found. Fetching from GitHub...")

	include(FetchContent)
	FetchContent_Declare(
		benchmark
		GIT_REPOSITORY
		https://github.com/google/benchmark.git
		GIT_TAG
		v1.8.3
		GIT_SHALLOW
		TRUE
		GIT_PROGRESS
		TRUE
	)

	# Configure Google Benchmark build options
	set(
		BENCHMARK_ENABLE_TESTING
		OFF
		CACHE
			BOOL
			"Don't build benchmark's tests"
		FORCE
	)
	set(
		BENCHMARK_ENABLE_INSTALL
		OFF
		CACHE
			BOOL
			"Don't install benchmark globally"
		FORCE
	)
	set(
		BENCHMARK_DOWNLOAD_DEPENDENCIES
		OFF
		CACHE
			BOOL
			"Don't download extra deps"
		FORCE
	)

	message(STATUS
		"Fetching and building Google Benchmark (this may take a moment)..."
	)
	FetchContent_MakeAvailable(benchmark)

	message(STATUS "Google Benchmark successfully built and integrated")
else()
	message(STATUS "Found system Google Benchmark: ${benchmark_DIR}")
endif()

# ========================================
# Define benchmark source directories
# ========================================
set(BENCHMARK_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(BENCHMARK_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# ========================================
# Discover and register benchmarks
# ========================================
# Find all benchmark sources in the benchmarks/src directory
file(GLOB_RECURSE BENCHMARK_SOURCES
	"${BENCHMARK_SRC_DIR}/*.c"
	"${BENCHMARK_SRC_DIR}/*.cpp"
	"${BENCHMARK_SRC_DIR}/*.cc"
	"${BENCHMARK_SRC_DIR}/*.cxx"
)

if(NOT BENCHMARK_SOURCES)
	message(WARNING "No benchmark sources found in ${BENCHMARK_SRC_DIR}")
	message(STATUS "Create benchmark files (*.c or *.cpp) in ${BENCHMARK_SRC_DIR}")
	return()
endif()

# ========================================
# Create benchmark executable
# ========================================
add_executable(kireji_benchmarks ${BENCHMARK_SOURCES})

# Link against the kireji library
target_link_libraries(kireji_benchmarks PRIVATE kireji::kireji)

# Link against Google Benchmark
target_link_libraries(kireji_benchmarks PRIVATE benchmark::benchmark)

# ========================================
# Configure benchmark compilation
# ========================================
target_include_directories(kireji_benchmarks
	PRIVATE
		"${CMAKE_SOURCE_DIR}/include"
		"${BENCHMARK_INCLUDE_DIR}"
		"${CMAKE_CURRENT_SOURCE_DIR}"
)

# Use C++11 for benchmarks (Google Benchmark requires C++11)
set_target_properties(kireji_benchmarks
	PROPERTIES
		CXX_STANDARD 11
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS ON
)

# ========================================
# Register benchmarks with CTest
# ========================================
add_test(
	NAME kireji_benchmarks
	COMMAND kireji_benchmarks --benchmark_print_json --benchmark_format=json
	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# ========================================
# Compiler flags for benchmarks
# ========================================
if(MSVC)
	target_compile_options(kireji_benchmarks PRIVATE /W4)
else()
	target_compile_options(kireji_benchmarks PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ========================================
# Print benchmark configuration summary
# ========================================
message(STATUS "")
message(STATUS "Benchmark configuration:")
message(STATUS "  Benchmark executable: kireji_benchmarks")
message(STATUS "  Benchmark framework: Google Benchmark")
message(STATUS "  C++ standard: C++11")
message(STATUS "  Run benchmarks with: ./kireji_benchmarks [--benchmark_*]")
message(STATUS "    Common flags:")
message(STATUS
	"      --benchmark_filter=<regex>     Run only matching benchmarks"
)
message(STATUS
	"      --benchmark_min_time=<float>    Minimum time per benchmark (seconds)"
)
message(STATUS "      --benchmark_repetitions=<int>   Number of repetitions")
message(STATUS "      --benchmark_report_aggregates_only={true|false}")
message(STATUS "")
