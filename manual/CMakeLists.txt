cmake_minimum_required(VERSION 3.5)
project(manual)

# OS and compiler checks.
if(WIN32)
    add_definitions(-DWIN32)
    SET(PROJECT_LIBS Ws2_32.lib)
endif()

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)

add_definitions(-DDEBUG -O2)
add_definitions(-Werror=return-type)

file(GLOB HEADERS "*.hpp" ".h")
SET_SOURCE_FILES_PROPERTIES(${HEADERS} PROPERTIES HEADER_FILE_ONLY TRUE)
file(GLOB SRC "*.cpp")
add_executable(manual ${HEADERS} ${SRC})
set_property(TARGET manual PROPERTY CXX_STANDARD 17)
# target_link_options(manual PRIVATE LINKER:-sectcreate,__TEXT,__info_plist,${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)
TARGET_LINK_LIBRARIES(manual ${PROJECT_LIBS})
qt5_use_modules(manual Widgets)
qt5_use_modules(manual Core)
