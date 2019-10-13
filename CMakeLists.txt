cmake_minimum_required(VERSION 3.12)
project(papyrus-vm LANGUAGES CXX)

option(SKYRIM_DIR "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Skyrim")

if (NOT SKYRIM_DIR MATCHES ".*Skyrim")
  message(FATAL_ERROR "Bad Skyrim path: \n${SKYRIM_DIR}")
endif()

file(GLOB src "${CMAKE_CURRENT_SOURCE_DIR}/src/papyrus-vm-lib/*")
add_library(papyrus-vm-lib STATIC ${src})

file(GLOB src "${CMAKE_CURRENT_SOURCE_DIR}/src/papyrus-vm-main/*")
add_executable(papyrus-vm-main ${src})
target_include_directories(papyrus-vm-main PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/papyrus-vm-lib")
target_link_libraries(papyrus-vm-main PRIVATE papyrus-vm-lib)
set(VM_MAIN_PATH ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/papyrus-vm-main.exe)

enable_testing()

function(add_vm_test SCRIPT_NAME)
  string(CONCAT test_name ${SCRIPT_NAME} "_test")
  add_test(NAME ${test_name} COMMAND ${CMAKE_COMMAND}
    -DSCRIPTS_DIR=${CMAKE_CURRENT_SOURCE_DIR}/test_files
    -DSCRIPT_NAME=${SCRIPT_NAME}
    -DOUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}/pex
    -DSKYRIM_DIR=${SKYRIM_DIR}
    -DVM_MAIN_PATH=${VM_MAIN_PATH}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/test_files/RunTest.cmake
  )

  set_tests_properties(${test_name} PROPERTIES
    FAIL_REGULAR_EXPRESSION "(.*No output generated for.*)|(.*failed with.*)"
    TIMEOUT 100
  )
endfunction()

file(GLOB tests "${CMAKE_CURRENT_SOURCE_DIR}/test_files/*")
foreach(test ${tests})
  if (NOT ${test} MATCHES ".*\.cmake")
    get_filename_component(test ${test} NAME)

    string(LENGTH ${test} length)
    math(EXPR length "${length} - 4" OUTPUT_FORMAT DECIMAL)
    string(SUBSTRING ${test} 0 ${length} test)
    message(STATUS "Adding test '${test}'")
    add_vm_test(${test})
  endif()
endforeach()
