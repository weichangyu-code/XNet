

include_directories(../src/)
include_directories(../src/common)
include_directories(../src/net)


add_executable(server ./server.cpp)
target_link_libraries(server XNet -lpthread)

add_executable(client ./client.cpp)
target_link_libraries(client XNet -lpthread)

if (UNIX AND NOT APPLE)
    add_executable(testEpoll ./testEpoll.cpp)
    target_link_libraries(testEpoll XNet -lpthread)
endif()


add_executable(testTimer ./testTimer.cpp)
target_link_libraries(testTimer XNet -lpthread)

if (WIN32)
    # Á´½Ó ws2_32 ¿â
    target_link_libraries(server ws2_32)
    target_link_libraries(client ws2_32)
endif()
