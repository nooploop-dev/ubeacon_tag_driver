set(TARGET_NAME test_${LIB_NAME})
add_executable(${TARGET_NAME} ${TARGET_NAME}.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE ${LIB_NAME} Catch2::Catch2WithMain)
add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
