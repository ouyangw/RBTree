# Do basic setup.
# Following cache variable are defined:
#   ${PROJNAME}_CHECK_CXXSTANDARD
#   ${PROJNAME}_CXXSTANDARD
#   ${PROJNAME}_WARNING_FLAG
# ${PROJNAME} is the all upper case project name.
#

if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "build type" FORCE)
  # set (CMAKE_BUILD_TYPE RelWithDebInfo)
endif (NOT CMAKE_BUILD_TYPE)

string (TOUPPER ${CMAKE_PROJECT_NAME} PROJNAME)

option (${PROJNAME}_CHECK_CXXSTANDARD "check higher c++ standard" ON)
set (${PROJNAME}_CXXSTANDARD 98 CACHE STRING "c++ standard")
if (${PROJNAME}_CHECK_CXXSTANDARD)
  include (CheckCXXStandard)
  CHECK_CXX_STANDARD (${PROJNAME}_CXXSTANDARD)
  set (${PROJNAME}_CXXSTANDARD ${${PROJNAME}_CXXSTANDARD} CACHE STRING
    "c++ standard" FORCE)
elseif (NOT ${PROJNAME}_CXXSTANDARD)
  set (${PROJNAME}_CXXSTANDARD 98 CACHE STRING "c++ standard" FORCE)
endif (${PROJNAME}_CHECK_CXXSTANDARD)

# set (${PROJNAME}_WARNING_FLAG -pedantic -Wall -Wextra -Wcast-align
#   -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2
#   -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs
#   -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls
#   -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel
#   -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused
#   CACHE STRING "some warning flag")

# customized (reduced) for release
set (${PROJNAME}_WARNING_FLAG -Wall -Wextra CACHE STRING "some warning flag")
