# CHECK_CXX_STANDARD (STANDARD)
#
# Check for the highest working c++ standard
# The variable is set to the standard number
# which can be used to set CXX_STANDARD variable
#

macro (CHECK_CXX_STANDARD _Result)

  include (CheckCXXCompilerFlag)

  CHECK_CXX_COMPILER_FLAG (-std=c++14 CXX14)
  if (CXX14)
    set (${_Result} 14)
  else ()
    CHECK_CXX_COMPILER_FLAG (-std=c++11 CXX11)
    if (CXX11)
      set (${_Result} 11)
    else ()
      set (${_Result} 98)
    endif ()
  endif ()

endmacro (CHECK_CXX_STANDARD)
