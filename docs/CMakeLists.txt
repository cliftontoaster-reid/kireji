# ========================================
# Documentation Build Configuration
# ========================================

# Check if Doxygen is installed
find_package(Doxygen OPTIONAL_COMPONENTS dot)

if(NOT DOXYGEN_FOUND)
	message(WARNING "Doxygen not found. Documentation generation disabled.")
	message(STATUS "Install Doxygen to enable: apt-get install doxygen graphviz")
	return()
endif()

message(STATUS "Found Doxygen: ${DOXYGEN_EXECUTABLE}")

# ========================================
# Configure Doxygen template
# ========================================
set(DOXYGEN_INPUT_DIR "${CMAKE_SOURCE_DIR}/include")
set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(DOXYGEN_INDEX_FILE "${DOXYGEN_OUTPUT_DIR}/html/index.html")

# Configure the Doxygen.in template with CMake variables
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/Doxygen.in"
	"${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
	@ONLY
)

# ========================================
# Create Doxygen build target
# ========================================
add_custom_target(doxygen ALL
	COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
	COMMENT "Generating Doxygen documentation"
	DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
)

# ========================================
# Create docs target (wraps doxygen)
# ========================================
add_custom_target(docs DEPENDS doxygen)

# ========================================
# Install targets
# ========================================
install(
	DIRECTORY "${DOXYGEN_OUTPUT_DIR}/html/"
	DESTINATION "share/doc/kireji"
	OPTIONAL
)

install(
	DIRECTORY "${DOXYGEN_OUTPUT_DIR}/man/"
	DESTINATION "share/man"
	OPTIONAL
)

message(STATUS
	"Doxygen documentation will be generated to: ${DOXYGEN_OUTPUT_DIR}"
)
