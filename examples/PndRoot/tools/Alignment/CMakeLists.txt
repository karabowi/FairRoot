# prepare include and lib paths
Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/tools/Alignment
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${SIMPATH}/lib
# ${Boost_LIBRARY_DIRS}
)

link_directories( ${LINK_DIRECTORIES})

### create MatrixUtils ###

set(SRCS
PndMatrixUtil.cxx
)

set(HEADERS
PndMatrixUtil.h
)

set(LINKDEF  PndMatrixUtilLinkDef.h)
set(LIBRARY_NAME PndMatrixUtil)
set(DEPENDENCIES Base Geom Core RIO)

# Set json.hpp path
include_directories(${json_SOURCE_DIR}/include)

Set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES}
${json_SOURCE_DIR}/include
)

PANDA_GENERATE_LIBRARY()