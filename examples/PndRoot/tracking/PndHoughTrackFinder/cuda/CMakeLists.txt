## @brief Builds the PndHoughTrackFinder GPU version
# @author Anna Alicke <a.alicke@fz-juelich.de>
##

find_package(CUDA)

message(STATUS 
  "-CH- Running with CUDA = "
  ${DO_THE_CUDA_DANCE}
)

# Set include paths (passed to nvcc in form of nvcc -Ipath1 -Ipath2 ...)
# Usually current source dir - with more complex project structures also more
CUDA_INCLUDE_DIRECTORIES(
  ${ROOT_INCLUDE_DIR} 
  ${CMAKE_CURRENT_SOURCE_DIR}
)
list(APPEND CUDA_NVCC_FLAGS ;-arch=compute_75; -code=sm_75 ;-std=c++17; --verbose; -lineinfo; -shared)

# Provide a list of actual .cu files
set(CUDA_SRC
   PndHoughApollonius.cu
)

# The name of the shared library
set(PNDHOUGHTRACKFINDERCUDA "PndHoughTrackFinderCuda")
# Propagate it to parent scope so we can make a dependency there (and don't actually need to know the name string of the dependency)
set(PNDHOUGHTRACKFINDERCUDA ${PNDHOUGHTRACKFINDERCUDA} PARENT_SCOPE)

# Generate the actual library (object)
CUDA_ADD_LIBRARY(${PNDHOUGHTRACKFINDERCUDA} 
  ${CUDA_SRC}
   SHARED # This seems to be, for our case, equivalent with set(BUILD_SHARED_LIBS ON)
)
set_target_properties(${PNDHOUGHTRACKFINDERCUDA} PROPERTIES
  ${FAIRROOT_LIBRARY_PROPERTIES})

