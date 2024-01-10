#include "RegisterList.h"

#include <iostream>

#include "Register.h"
using namespace std;

RegisterList::RegisterList() {
  head = nullptr;
  size = 0;
}

RegisterList::~RegisterList() {
  // Delete all registers in the list
  Register* temp = head;
  Register* current = head;
  while (temp != nullptr) {
    current = temp;
    temp = temp->get_next();
    delete current;
  }
}

Register* RegisterList::get_head() { return head; }

int RegisterList::get_size() {
  // return number of registers
  return size;
}

Register* RegisterList::get_min_items_register() {
  int minimum = 2147483647;
  int localnum = 0;
  Register* mim;
  if (head != nullptr) {
    Register* registemp = head;
    while (registemp != nullptr) {
      localnum = 0;
      Customer* temp = registemp->get_queue_list()->get_head();
      while (temp != nullptr) {
        localnum += temp->get_numOfItems();
        temp = temp->get_next();
      }

      if (localnum < minimum) {
        mim = registemp;
        minimum = localnum;
      }

      registemp = registemp->get_next();
    }
  }
  return mim;
  // loop all registers to find the register with least number of items
}

Register* RegisterList::get_free_register() {
  // return the register with no customers
  // if all registers are occupied, return nullptr

  Register* registemp = head;
  Register* nopointer = nullptr;
  while (registemp != nullptr) {
    if (registemp->get_queue_list()->get_head() == nullptr) {
      return registemp;
    }
    registemp = registemp->get_next();
  }
  return nopointer;
}

void RegisterList::enqueue(Register* newRegister) {
  // a register is placed at the end of the queue
  // if the register's list is empty, the register becomes the head
  // Assume the next of the newRegister is set to null
  // You will have to increment size
  if (head == nullptr) {
    head = newRegister;
  } else {
    Register* temp = head;
    Register* current = head;
    while (temp != nullptr) {
      current = temp;
      temp = temp->get_next();
    }
    current->set_next(newRegister);
  }
}

bool RegisterList::foundRegister(int ID) {
  // look for a register with the given ID
  // return true if found, false otherwise
  if (head == nullptr) {
    return false;
  } else {
    Register* temp = head;
    while (temp != nullptr) {
      if (temp->get_ID() == ID) {
        return true;
      }
      temp = temp->get_next();
    }
  }
  return false;
}

Register* RegisterList::dequeue(int ID) {
  // dequeue the register with given ID

  // return the dequeued register
  // return nullptr if register was not found
  Register* findID = nullptr;

  if (head == nullptr) {
    return findID;
  } else {
    Register* temp = head;
    Register* previous = nullptr;
    Register* current = head;
    while (temp != nullptr) {
      current = temp;
      if (current->get_ID() == ID) {
        if (previous == nullptr) {
          head = temp->get_next();
          temp->set_next(nullptr);
          findID = temp;
          return findID;
        } else {
          previous->set_next(current->get_next());
          current->set_next(nullptr);
          findID = current;
          return findID;
        }
      }
      previous = temp;
      temp = temp->get_next();
    }
  }
  return findID;
}

Register* RegisterList::calculateMinDepartTimeRegister(double expTimeElapsed) {
  // return the register with minimum time of departure of its customer
  // if all registers are free, return nullptr
  double minimum = 2147483647;
  double localnum = 0;
  Register* mim = nullptr;
  if (head != nullptr) {
    Register* registemp = head;
    while (registemp != nullptr) {
      localnum = registemp->calculateDepartTime();

      if (localnum < minimum && localnum > 0) {
        mim = registemp;
        minimum = localnum;
      }

      registemp = registemp->get_next();
    }
  }
  return mim;
}

void RegisterList::print() {
  Register* temp = head;
  while (temp != nullptr) {
    temp->print();
    temp = temp->get_next();
  }
}
