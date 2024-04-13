# CMakeList.txt : CMake project for NetworkMonitor, include source and define
# project specific logic here.
#
# Use find_library to locate the libraries you need
find_library(LIB1 libxml2 PATHS "D:/Programs/PortableGit/vcpkg/installed/x64-windows/lib")
find_library(LIB2 libmicrohttpd-dll PATHS "D:/Programs/PortableGit/vcpkg/installed/x64-windows/lib")
find_library(LIB3 libcurl PATHS "D:/Programs/PortableGit/vcpkg/installed/x64-windows/lib")
find_library(LIB4 cjson PATHS "D:/Programs/PortableGit/vcpkg/installed/x64-windows/lib")
# Add source to this project's executable.
add_executable (NetworkMonitor "NetworkMonitor.cpp" "NetworkMonitor.h" "Tool.cpp" "Tool.h" "Connection.h" "Connection.cpp"  )
#set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O0")
#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET NetworkMonitor PROPERTY CXX_STANDARD 20)
endif()
# Include directories from vcpkg installation
target_include_directories(NetworkMonitor PRIVATE "D:/Programs/PortableGit/vcpkg/installed/x64-windows/include")
link_directories("D:/Programs/PortableGit/vcpkg/installed/x64-windows/lib")
target_link_libraries(NetworkMonitor
    ${LIB1}
    ${LIB2}
    ${LIB3}
    ${LIB4} 
)
# TODO: Add tests and install targets if needed.
