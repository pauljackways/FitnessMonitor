set(SRC_DIR ../target/src)
set(TIVAWARE_DIR ../target/libs/tivaware)
set(MOCK_DIR mocks)
set(FREERTOS_CONFIG ../target)


cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(FitnessTrackerTests LANGUAGES C)

enable_testing()
add_subdirectory(unity)

add_unity_test(test_example test_example.c)
add_unity_test(test_fff test_fff.c)

add_unity_test(test_adc test_adc.c ${SRC_DIR}/ADC_read.c ${SRC_DIR}/adc_hal_tiva.c)
target_include_directories(test_adc PRIVATE ${SRC_DIR} ${MOCK_DIR} ${TIVAWARE_DIR})

add_unity_test(test_adc_hal test_adc_hal.c ${SRC_DIR}/adc_hal_tiva.c)
target_include_directories(test_adc_hal PRIVATE ${SRC_DIR} ${MOCK_DIR} ${TIVAWARE_DIR})

add_unity_test(test_button_functionality test_button_functionality.c ${SRC_DIR}/button_manager.c ${SRC_DIR}/buttons4.c ${FREERTOS_CONFIG})
target_include_directories(test_button_functionality PRIVATE ${SRC_DIR} ${MOCK_DIR} ${TIVAWARE_DIR})

add_unity_test(test_buttons4 test_buttons4.c ${SRC_DIR}/buttons4.c)
target_include_directories(test_buttons4 PRIVATE ${SRC_DIR} ${MOCK_DIR} ${TIVAWARE_DIR})

add_unity_test(test_circular_buffer test_circular_buffer.c ${SRC_DIR}/circBufT.c)
target_include_directories(test_circular_buffer PRIVATE ${SRC_DIR} ${MOCK_DIR} ${TIVAWARE_DIR})