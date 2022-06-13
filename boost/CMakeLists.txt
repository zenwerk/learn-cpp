set(CMAKE_CXX_STANDARD 17)

if (WIN32)
    set(BOOST_ROOT_DIR "C:\\Boost") # b2.exe link=static,shared toolset=msvc -j8 install
    set(BOOST_INCLUDEDIR "${Boost_INCLUDE_DIR}")
    set(BOOST_LIBRARYDIR "${BOOST_ROOT_DIR}\\lib")
endif ()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
#set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_ARCHITECTURE "-x64")
set(Boost_DEBUG ON)
set(Boost_COMPILER "-vc142")

#message("## Boost_INCLUDE_DIR: ${Boost_INCLUDE_DIR}")  # デバッグ用：解決したパスを表示
#message("## Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")  # デバッグ用：解決したパスを表示

find_package(Boost ${Boost_VERSION} REQUIRED COMPONENTS system thread date_time)
find_package(Threads REQUIRED)


add_executable(boost-asio-example
    asio.cc)

target_link_libraries(boost-asio-example
    PRIVATE
    Boost::thread
    Boost::system
    Boost::date_time
    )