# ========================================
# Kireji Test Suite - Using Google Test
# ========================================

# Attempt to find Google Test system-wide first
find_package(GTest QUIET)

# If Google Test is not found, try to fetch it from GitHub
if(NOT GTest_FOUND)
	message(STATUS "Google Test not found. Fetching from GitHub...")

	include(FetchContent)
	FetchContent_Declare(
		gtest
		GIT_REPOSITORY
		https://github.com/google/googletest.git
		GIT_TAG
		v1.14.0
		GIT_SHALLOW
		TRUE
		GIT_PROGRESS
		TRUE
	)

	# Configure Google Test build options to minimize build time
	set(
		INSTALL_GTEST
		OFF
		CACHE
			BOOL
			"Don't install gtest"
		FORCE
	)
	set(gtest_force_shared_crt ON CACHE BOOL "Use shared CRT" FORCE)

	message(STATUS "Fetching and building Google Test (this may take a moment)...")
	FetchContent_MakeAvailable(gtest)

	message(STATUS "Google Test successfully built and integrated")
else()
	message(STATUS "Found system Google Test: ${GTest_DIR}")
endif()

# ========================================
# Define test source directories
# ========================================
set(TEST_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(TEST_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# ========================================
# Discover and register tests
# ========================================
# Find all test sources in the tests/src directory
file(GLOB_RECURSE TEST_SOURCES
	"${TEST_SRC_DIR}/*.c"
	"${TEST_SRC_DIR}/*.cpp"
	"${TEST_SRC_DIR}/*.cc"
	"${TEST_SRC_DIR}/*.cxx"
)

if(NOT TEST_SOURCES)
	message(WARNING "No test sources found in ${CMAKE_CURRENT_SOURCE_DIR}")
	message(STATUS
		"Create test files (*.c or *.cpp) and add them to CMakeLists.txt"
	)
	return()
endif()

# ========================================
# Create test executable
# ========================================
add_executable(kireji_tests ${TEST_SOURCES})

# Link against the kireji library (use shared library if available, else static)
if(BUILD_SHARED)
	target_link_libraries(kireji_tests PRIVATE kireji::kireji)
else()
	target_link_libraries(kireji_tests PRIVATE kireji::kireji)
endif()

# Link against Google Test
target_link_libraries(kireji_tests PRIVATE GTest::gtest GTest::gtest_main)

# ========================================
# Configure test compilation
# ========================================
target_include_directories(kireji_tests
	PRIVATE
		"${CMAKE_SOURCE_DIR}/include"
		"${TEST_INCLUDE_DIR}"
		"${CMAKE_CURRENT_SOURCE_DIR}"
)

# Use C++11 for tests (required by Google Test)
set_target_properties(kireji_tests
	PROPERTIES
		CXX_STANDARD 11
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS ON
)

# Optional: Add stricter C++ standard if needed
# Uncomment for C++17 or higher:
# set_target_properties(kireji_tests PROPERTIES CXX_STANDARD 17)

# ========================================
# Register tests with CTest
# ========================================
# Google Test automatically registers tests via gtest_discover_tests
gtest_discover_tests(
	kireji_tests
	WORKING_DIRECTORY
	"${CMAKE_BINARY_DIR}"
	DISCOVERY_TIMEOUT
	600
	XML_OUTPUT_DIR
	"${CMAKE_BINARY_DIR}/test-results"
)

# ========================================
# Compiler flags for testing
# ========================================
if(MSVC)
	target_compile_options(kireji_tests PRIVATE /W4 /WX)
else()
	target_compile_options(kireji_tests PRIVATE -Wall -Wextra -Wpedantic)
	target_compile_options(kireji_tests PRIVATE -Werror)
endif()

# ========================================
# Print test configuration summary
# ========================================
message(STATUS "")
message(STATUS "Test configuration:")
message(STATUS "  Test executable: kireji_tests")
message(STATUS "  Test framework: Google Test (GTest)")
message(STATUS "  C++ standard: C++11")
message(STATUS "  Run tests with: ctest or ./kireji_tests")
message(STATUS "    Common flags:")
message(STATUS "      --gtest_filter=<pattern>       Run only matching tests")
message(STATUS "      --gtest_repeat=<count>         Repeat tests N times")
message(STATUS "      --gtest_shuffle                Randomize test order")
message(STATUS "      --gtest_list_tests             List all tests")
message(STATUS "")
