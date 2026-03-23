# ========================================
# Example: Hello Kireji
# ========================================
# This is a template for building a single example
# Place your example source files in src/ and headers in include/

set(EXAMPLE_NAME hello_kireji)
set(EXAMPLE_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(EXAMPLE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Find all source files
file(GLOB_RECURSE EXAMPLE_SOURCES
	"${EXAMPLE_SRC_DIR}/*.c"
	"${EXAMPLE_SRC_DIR}/*.cpp"
	"${EXAMPLE_SRC_DIR}/*.cc"
	"${EXAMPLE_SRC_DIR}/*.cxx"
)

if(NOT EXAMPLE_SOURCES)
	message(WARNING "No source files found in ${EXAMPLE_SRC_DIR}")
	message(STATUS "Add .c or .cpp files to ${EXAMPLE_SRC_DIR}")
	return()
endif()

# Create the example executable
add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCES})

# Link against the kireji library
target_link_libraries(${EXAMPLE_NAME} PRIVATE kireji::kireji)

# Include directories
target_include_directories(${EXAMPLE_NAME}
	PRIVATE
		"${CMAKE_SOURCE_DIR}/include"
		"${EXAMPLE_INCLUDE_DIR}"
)

# Set C++ standard (examples can use newer standards than the library)
set_target_properties(${EXAMPLE_NAME}
	PROPERTIES
		CXX_STANDARD 11
		CXX_STANDARD_REQUIRED ON
)
