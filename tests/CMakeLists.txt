set(SRC_DIR ../target/src)
set(TIVAWARE_DIR ../target/libs/tivaware)
set(MOCK_DIR mocks)

cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(FitnessTrackerTests LANGUAGES C)

enable_testing()
add_subdirectory(unity)

add_unity_test(test_example test_example.c)
add_unity_test(test_fff test_fff.c)

add_unity_test(test_adc test_adc.c ${SRC_DIR}/ADC_read.c)
target_include_directories(test_adc PRIVATE ${SRC_DIR} ${MOCK_DIR} ${TIVAWARE_DIR})
