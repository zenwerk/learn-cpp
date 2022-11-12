
add_executable(ccia-mpi-example)

target_sources(ccia-mpi-example
    PUBLIC
    mpi.h
    atm.h
    bank_machine.h
    interface_machine.h
    messages.h
#    dispatcher.h
#    queue.h
#    receiver.h
#    sender.h
#    template_dispatcher.h
    main.cc
    )