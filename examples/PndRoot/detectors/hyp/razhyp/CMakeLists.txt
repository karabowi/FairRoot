# Create a library called "libDpmEvtGen" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 OLD)
endif(COMMAND cmake_policy)

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/detectors/hyp/razhyp
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${CMAKE_CURRENT_BINARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(RAZHYP_SRCS
Initia.f
razhyp_gen.f

)

set(RAZHYP_EXE_SRCS
gen.cpp
)

if (CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(razhyp_gen.f PROPERTIES COMPILE_FLAGS
                           "-fsecond-underscore")
else (CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(razhyp_gen.f PROPERTIES COMPILE_FLAGS
                           "-assume 2underscores")
endif (CMAKE_COMPILER_IS_GNUCXX)

add_library(razhyp SHARED ${RAZHYP_SRCS})
target_link_libraries(razhyp ${ROOT_LIBRARIES})
set_target_properties(razhyp PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

add_executable(razhyp_exe ${RAZHYP_EXE_SRCS})
if (CMAKE_SYSTEM_NAME MATCHES Darwin)
  target_link_libraries(razhyp_exe ${ROOT_LIBRARIES} EG "razhyp")
else (CMAKE_SYSTEM_NAME MATCHES Darwin)
  target_link_libraries(razhyp_exe EG razhyp)
endif (CMAKE_SYSTEM_NAME MATCHES Darwin)
set_target_properties(razhyp_exe PROPERTIES OUTPUT_NAME razhypGen)

################ install ###################
install(TARGETS razhyp DESTINATION lib)
install(TARGETS razhyp_exe DESTINATION bin)
