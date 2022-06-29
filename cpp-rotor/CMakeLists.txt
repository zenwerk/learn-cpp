set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

if (WIN32)
    set(BOOST_ROOT_DIR "C:\\Boost") # b2.exe link=static,shared toolset=msvc -j8 install
    set(BOOST_INCLUDEDIR "${BOOST_ROOT_DIR}\\include\\boost-1_79")
    set(Boost_INCLUDE_DIR "C:\\Boost\\include\\boost-1_79")
    set(BOOST_LIBRARYDIRS "${BOOST_ROOT_DIR}\\lib")
    set(Boost_COMPILER "-vc142")
endif ()

message("## Boost_INCLUDE_DIR: ${Boost_INCLUDE_DIR}")  # デバッグ用：解決したパスを表示
message("## Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")  # デバッグ用：解決したパスを表示

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
#set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_DEBUG ON)
set(Boost_ARCHITECTURE "-x64")

find_package(Boost ${Boost_VERSION_STRING} REQUIRED COMPONENTS
    date_time
    system
    thread
    regex
    )

set(BUILD_BOOST_ASIO ON CACHE BOOL "with asio") # pick options you need
FetchContent_Declare(
    rotor
    GIT_REPOSITORY https://github.com/basiliscos/cpp-rotor.git
    GIT_TAG v0.24
    GIT_SUBMODULES ""
    BUILD_BOOST_ASIO ON
    )
FetchContent_MakeAvailable(rotor)

add_executable(rotor-helloloopless
    hello_loopless.cc
    )

target_include_directories(rotor-helloloopless PUBLIC ${rotor_SOURCE_DIR}/include)
target_link_libraries(rotor-helloloopless rotor::asio)
