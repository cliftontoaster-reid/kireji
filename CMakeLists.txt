cmake_minimum_required(VERSION 3.16)
project(kireji LANGUAGES CXX)

# -------------------------------
# Semantic version (override via -D on the command line)
# -------------------------------
set(KIREJI_VERSION_MAJOR 0 CACHE STRING "Major version number")
set(KIREJI_VERSION_MINOR 1 CACHE STRING "Minor version number")
set(KIREJI_VERSION_PATCH 0 CACHE STRING "Patch version number")
set(KIREJI_VERSION
	"${KIREJI_VERSION_MAJOR}.${KIREJI_VERSION_MINOR}.${KIREJI_VERSION_PATCH}"
)

# -------------------------------
# C++ standard
# -------------------------------
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -------------------------------
# Feature toggles and build options
# -------------------------------
option(ENABLE_HTTP1_0 "Enable HTTP/1.0 support (default: ON)" ON)
option(ENABLE_HTTP1_1 "Enable HTTP/1.1 support (default: ON)" ON)
option(ENABLE_HTTP2 "Enable HTTP/2 support (default: OFF)" OFF)
option(ENABLE_HTTP3 "Enable HTTP/3 support (default: OFF)" OFF)
option(ENABLE_TLS "Enable TLS support (default: OFF)" OFF)
option(ENABLE_COMPRESSION "Enable compression support (default: OFF)" OFF)
option(ENABLE_FSANITIZE
	"Enable runtime sanitizer instrumentation (default: OFF)"
	OFF
)
set(
	FSANITIZE_TYPES
	"address;undefined"
	CACHE
		STRING
		"Sanitizer types to enable when ENABLE_FSANITIZE is ON (semicolons or commas are accepted, e.g. address;undefined;leak)"
)

option(BUILD_TESTS "Build unit tests (default: OFF)" OFF)
option(BUILD_EXAMPLES "Build example applications (default: OFF)" OFF)
option(BUILD_BENCHMARKS "Build benchmark applications (default: OFF)" OFF)
option(BUILD_DOCS "Build documentation (default: OFF)" OFF)
option(BUILD_SHARED "Build shared library (default: ON)" ON)
option(BUILD_STATIC "Build static library (default: ON)" ON)
option(ENABLE_COVERAGE
	"Enable code coverage instrumentation (default: OFF)"
	OFF
)

if(ENABLE_COVERAGE)
	if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		message(STATUS "Code coverage enabled: adding --coverage flags")
		add_compile_options(--coverage -g -O0)
		add_link_options(--coverage)
	else()
		message(WARNING
			"Coverage is not supported with MSVC. ENABLE_COVERAGE will be ignored."
		)
	endif()
endif()

if(ENABLE_FSANITIZE)
	if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		# Allow commas and semicolons in FSANITIZE_TYPES
		string(REPLACE "," ";" FSANITIZE_LIST "${FSANITIZE_TYPES}")
		foreach(SANITIZER ${FSANITIZE_LIST})
			if(SANITIZER)
				message(STATUS "AddressSanitizer: adding -fsanitize=${SANITIZER}")
				add_compile_options(-fsanitize=${SANITIZER} -fno-omit-frame-pointer)
				add_link_options(-fsanitize=${SANITIZER})
			endif()
		endforeach()
	else()
		message(WARNING
			"fsanitize is not supported with MSVC. ENABLE_FSANITIZE will be ignored."
		)
	endif()
endif()

# -------------------------------
# Variables
# -------------------------------
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(TEST_DIR "${CMAKE_SOURCE_DIR}/tests")
set(EXAMPLES_DIR "${CMAKE_SOURCE_DIR}/examples")
set(BENCHMARKS_DIR "${CMAKE_SOURCE_DIR}/benchmarks")
set(DOCS_DIR "${CMAKE_SOURCE_DIR}/docs")

# -------------------------------
# Source files
# -------------------------------
file(GLOB_RECURSE KIREJI_SOURCES
	"${SRC_DIR}/*.cpp"
	"${SRC_DIR}/*.cc"
	"${SRC_DIR}/*.cxx"
)
file(GLOB_RECURSE KIREJI_HEADERS "${INCLUDE_DIR}/*.h" "${INCLUDE_DIR}/*.hpp")

# Verify that sources were found (warn if empty)
if(NOT KIREJI_SOURCES)
	message(WARNING "No source files found in ${SRC_DIR}")
endif()

# -------------------------------
# Compile definitions for features
# -------------------------------
set(KIREJI_COMPILE_DEFS)

if(ENABLE_HTTP1_0)
	list(APPEND KIREJI_COMPILE_DEFS "KIREJI_ENABLE_HTTP1_0")
endif()

if(ENABLE_HTTP1_1)
	list(APPEND KIREJI_COMPILE_DEFS "KIREJI_ENABLE_HTTP1_1")
endif()

if(ENABLE_HTTP2)
	list(APPEND KIREJI_COMPILE_DEFS "KIREJI_ENABLE_HTTP2")
endif()

if(ENABLE_HTTP3)
	list(APPEND KIREJI_COMPILE_DEFS "KIREJI_ENABLE_HTTP3")
endif()

if(ENABLE_TLS)
	list(APPEND KIREJI_COMPILE_DEFS "KIREJI_ENABLE_TLS")
endif()

if(ENABLE_COMPRESSION)
	list(APPEND KIREJI_COMPILE_DEFS "KIREJI_ENABLE_COMPRESSION")
endif()

# Add version to compile definitions
list(APPEND KIREJI_COMPILE_DEFS "KIREJI_VERSION=\"${KIREJI_VERSION}\"")

# -------------------------------
# Add library target
# -------------------------------
# Ensure at least one library type is enabled
if(NOT BUILD_SHARED AND NOT BUILD_STATIC)
	message(FATAL_ERROR "At least one of BUILD_SHARED or BUILD_STATIC must be ON")
endif()

# Create shared library if enabled
if(BUILD_SHARED)
	add_library(kireji SHARED ${KIREJI_SOURCES} ${KIREJI_HEADERS})
	target_include_directories(kireji
		PUBLIC
			$<BUILD_INTERFACE:${INCLUDE_DIR}>
			$<INSTALL_INTERFACE:include>
		PRIVATE ${SRC_DIR}
	)
	target_compile_definitions(kireji PRIVATE ${KIREJI_COMPILE_DEFS})
	set_target_properties(kireji
		PROPERTIES
			VERSION ${KIREJI_VERSION}
			SOVERSION ${KIREJI_VERSION_MAJOR}
			CXX_STANDARD 98
			CXX_STANDARD_REQUIRED ON
			CXX_EXTENSIONS OFF
	)
endif()

# Create static library if enabled
if(BUILD_STATIC)
	add_library(kireji_static STATIC ${KIREJI_SOURCES} ${KIREJI_HEADERS})
	target_include_directories(kireji_static
		PUBLIC
			$<BUILD_INTERFACE:${INCLUDE_DIR}>
			$<INSTALL_INTERFACE:include>
		PRIVATE ${SRC_DIR}
	)
	target_compile_definitions(kireji_static PRIVATE ${KIREJI_COMPILE_DEFS})
	set_target_properties(kireji_static
		PROPERTIES
			VERSION ${KIREJI_VERSION}
			CXX_STANDARD 98
			CXX_STANDARD_REQUIRED ON
			CXX_EXTENSIONS OFF
	)

	set_target_properties(kireji_static PROPERTIES OUTPUT_NAME kireji)
endif()

# Create alias for the primary library (shared if available, else static)
if(BUILD_SHARED)
	add_library(kireji::kireji ALIAS kireji)
else()
	add_library(kireji::kireji ALIAS kireji_static)
endif()

# -------------------------------
# Optional: Install targets
# -------------------------------
install(
	TARGETS
		kireji
		kireji_static
	EXPORT kireji-targets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)
install(FILES ${KIREJI_HEADERS} DESTINATION include/kireji)
install(EXPORT kireji-targets
	FILE kireji-targets.cmake
	NAMESPACE kireji::
	DESTINATION lib/cmake/kireji
)

# -------------------------------
# Subdirectories (tests, examples, benchmarks, docs)
# -------------------------------
if(BUILD_TESTS AND EXISTS "${TEST_DIR}/CMakeLists.txt")
	message(STATUS "Building tests")
	enable_testing()
	add_subdirectory(tests)
endif()

if(BUILD_EXAMPLES)
	# Dynamically add all example subdirectories
	file(GLOB EXAMPLE_DIRS "${EXAMPLES_DIR}/*")
	foreach(EXAMPLE_DIR ${EXAMPLE_DIRS})
		if(IS_DIRECTORY "${EXAMPLE_DIR}")
			get_filename_component(EXAMPLE_NAME "${EXAMPLE_DIR}" NAME)
			if(EXISTS "${EXAMPLE_DIR}/CMakeLists.txt")
				message(STATUS "Building example: ${EXAMPLE_NAME}")
				add_subdirectory("${EXAMPLE_DIR}" "examples/${EXAMPLE_NAME}")
			else()
				message(STATUS "Skipping ${EXAMPLE_NAME}: no CMakeLists.txt found")
			endif()
		endif()
	endforeach()
endif()

if(BUILD_BENCHMARKS AND EXISTS "${BENCHMARKS_DIR}/CMakeLists.txt")
	message(STATUS "Building benchmarks")
	add_subdirectory(benchmarks)
endif()

if(BUILD_DOCS AND EXISTS "${DOCS_DIR}/CMakeLists.txt")
	message(STATUS "Building documentation")
	add_subdirectory(docs)
endif()

# Print build configuration summary
message(STATUS "")
message(STATUS "Kireji ${KIREJI_VERSION} build configuration:")
message(STATUS "  C++ Standard: 98")
message(STATUS "  Build Shared Library: ${BUILD_SHARED}")
message(STATUS "  Build Static Library: ${BUILD_STATIC}")
message(STATUS "  HTTP/1.0 Support: ${ENABLE_HTTP1_0}")
message(STATUS "  HTTP/1.1 Support: ${ENABLE_HTTP1_1}")
message(STATUS "  HTTP/2 Support: ${ENABLE_HTTP2}")
message(STATUS "  HTTP/3 Support: ${ENABLE_HTTP3}")
message(STATUS "  TLS Support: ${ENABLE_TLS}")
message(STATUS "  Compression Support: ${ENABLE_COMPRESSION}")
message(STATUS "  Build Tests: ${BUILD_TESTS}")
message(STATUS "  Build Examples: ${BUILD_EXAMPLES}")
message(STATUS "  Build Benchmarks: ${BUILD_BENCHMARKS}")
message(STATUS "  Build Documentation: ${BUILD_DOCS}")
message(STATUS "")
