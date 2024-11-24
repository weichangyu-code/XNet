

aux_source_directory(./common COMMON_SRC_LIST)
aux_source_directory(./net NET_SRC_LIST)
aux_source_directory(./ SRC_LIST)

include_directories(./common)
include_directories(./net)
include_directories(./)

# add_definitions(-std=c++11)
# set(CMAKE_CXX_FLAGS -m64)
# set(CMAKE_CXX_FLAGS -pthread)
# add_compile_options(-fPIC)      #这个会影响线程局部变量

add_library(XNet STATIC ${COMMON_SRC_LIST} ${NET_SRC_LIST} ${SRC_LIST})
