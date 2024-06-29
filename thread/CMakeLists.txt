set(CMAKE_CXX_STANDARD 20)

add_executable(std-thread-basic
    basic.cc)

add_executable(std-thread-mutex
    mutex.cc)

add_executable(std-thread-conditional-variable-basic
    conditional_variable_basic.cc)

add_executable(std-thread-conditional-variable
    conditional_variable_locked_queue.cc)

add_executable(std-thread-future-promise
    future_promise.cc)

add_executable(std-thread-mutex-lock-guard
    mutex_lock_guard.cc)

add_executable(std-thread-shared-lock
    shared_lock.cc)

add_executable(join-thread-cpp14
    join_thread_cpp14.cc)

add_executable(std-thread-waitgroup
    wait_group.cc)

add_executable(std-thread-input-and-echo
    input_and_echo.cc)

add_executable(std-thread-job-system
    job_system.cc)

add_subdirectory(ccia-mpi-example)