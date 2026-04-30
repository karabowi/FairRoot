## @brief Builds the CA GPU version
# @author Jette Schumann <j.schumann@fz-juelich.de>
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

# # Needed to switch between proper ROOT TVector2 or fake TVector2
# if (ROOT_FOUND)
#   add_definitions(-DTHERE_IS_ROOT)
#   message (STATUS "-CH- There is ROOT")
# endif (ROOT_FOUND)

# Provide a list of actual .cu files
set(CUDA_SRC
   trackletGenerator.cu
)

# Stear compiler flags - eg different compute capabilites
list(APPEND CUDA_NVCC_FLAGS --gpu-architecture sm_75;-std=c++17;-O2;-DVERBOSE)

#SET(CUDA_PROPAGATE_HOST_FLAGS OFF)

# The name of the shared library
set(PNDSTTCACUDA "PndSttCACuda")
# Propagate it to parent scope so we can make a dependency there (and don't actually need to know the name string of the dependency)
set(PNDSTTCACUDA ${PNDSTTCACUDA} PARENT_SCOPE)

# Generate the actual library (object)
CUDA_ADD_LIBRARY(${PNDSTTCACUDA} 
  ${CUDA_SRC}
   SHARED # This seems to be, for our case, equivalent with set(BUILD_SHARED_LIBS ON)
)
set_target_properties(${PNDSTTCACUDA} PROPERTIES
  ${FAIRROOT_LIBRARY_PROPERTIES})
