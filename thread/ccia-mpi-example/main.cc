#include <thread>
#include <iostream>

#include "atm.h"
#include "bank_machine.h"
#include "interface_machine.h"

int main() {
  BankMachine bank;
  InterfaceMachine interface_hardware;
  Atm machine(bank.get_sender(), interface_hardware.get_sender());
  std::thread bank_thread(&BankMachine::run, &bank);
  std::thread if_thread(&InterfaceMachine::run, &interface_hardware);
  std::thread atm_thread(&Atm::run, &machine);
  messaging::Sender atm_queue(machine.get_sender());
  bool quit_pressed = false;

  while (!quit_pressed) {
	char c = getchar();
	switch (c) {
	  case '0':
	  case '1':
	  case '2':
	  case '3':
	  case '4':
	  case '5':
	  case '6':
	  case '7':
	  case '8':
	  case '9':atm_queue.send(digit_pressed(c));
		break;
	  case 'b':atm_queue.send(balance_pressed());
		break;
	  case 'w':atm_queue.send(withdraw_pressed(50));
		break;
	  case 'c':atm_queue.send(cancel_pressed());
		break;
	  case 'q':quit_pressed = true;
		break;
	  case 'i':atm_queue.send(card_inserted("acc1234"));
		break;
	  default:
		std::cout << "unknown message: " << c << std::endl;
	}
  }

  bank.done();
  machine.done();
  interface_hardware.done();

  atm_thread.join();
  bank_thread.join();
  if_thread.join();
}
