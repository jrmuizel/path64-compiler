
IF(NOT PATH64_BOOTSTRAP_STAGES)
	SET(PATH64_BOOTSTRAP_STAGES 2)
ENDIF()

CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/bootstrap-test.sh.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/bootstrap-test.sh")

ADD_TEST(path64_bootstrap_test "${CMAKE_CURRENT_BINARY_DIR}/bootstrap-test.sh")

