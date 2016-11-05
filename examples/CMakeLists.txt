project (Examples CXX)

function (ADD_EXAMPLE _NAME)
  set (_EXE_NAME ${_NAME}.exe)
  add_executable (${_EXE_NAME} ./${_NAME}/main.cpp)
  set_target_properties (${_EXE_NAME} PROPERTIES
    CXX_STANDARD ${${PROJNAME}_CXXSTANDARD}
    RUNTIME_OUTPUT_DIRECTORY bin/)
  add_dependencies (examples ${_EXE_NAME})
endfunction()

include_directories (${CMAKE_SOURCE_DIR}/RBTree/)
add_custom_target (examples)

ADD_EXAMPLE (print)
