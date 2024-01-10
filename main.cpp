#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "Customer.h"
#include "QueueList.h"
#include "Register.h"
#include "RegisterList.h"

using namespace std;

// Function Declarations:

// Set mode of the simulation
string getMode();

// Register
void parseRegisterAction(stringstream &lineStream, string mode);
void openRegister(
    stringstream &lineStream,
    string mode);  // register opens (it is upto customers to join)
void closeRegister(stringstream &lineStream,
                   string mode);  // register closes

// Customer
void addCustomer(stringstream &lineStream,
                 string mode);  // customer wants to join
void printfinal();

// Helper functions
bool getInt(stringstream &lineStream, int &iValue);
bool getDouble(stringstream &lineStream, double &dValue);
bool foundMoreArgs(stringstream &lineStream);

// Global variables
RegisterList *registerList;  // holding the list of registers
QueueList *doneList;         // holding the list of customers served
QueueList *singleQueue;      // holding customers in a single virtual queue
double expTimeElapsed;  // time elapsed since the beginning of the simulation

// List of commands:
// To open a register
// register open <ID> <secPerItem> <setupTime> <timeElapsed>
// To close register
// register close <ID> <timeElapsed>
// To add a customer
// customer <items> <timeElapsed>

int main() {
  registerList = new RegisterList();
  doneList = new QueueList();
  singleQueue = new QueueList();
  expTimeElapsed = 0;

  // Set mode by the user
  string mode = getMode();

  string line;
  string command;

  std::cout << "> ";  // Prompt for input
  if (cin.eof()) {
    printfinal();
    delete registerList;
    delete doneList;
    delete singleQueue;
    return 0;
  }

  getline(cin, line);
  if (cin.eof()) {
    printfinal();
    delete registerList;
    delete doneList;
    delete singleQueue;
    return 0;
  }

  while (!cin.eof()) {
    stringstream lineStream(line);
    lineStream >> command;
    // cout << command << endl;
    if (command == "register") {
      parseRegisterAction(lineStream, mode);
    } else if (command == "customer") {
      addCustomer(lineStream, mode);
    } else {
      std::cout << "Invalid operation" << endl;
    }
    std::cout << "> ";  // Prompt for input

    getline(cin, line);
    if (cin.eof()) {
      printfinal();
      delete registerList;
      delete doneList;
      delete singleQueue;
      return 0;
    }
  }

  // You have to make sure all dynamically allocated memory is freed
  // before return 0
  return 0;
}

string getMode() {
  string mode;
  std::cout << "Welcome to ECE 244 Grocery Store Queue Simulation!" << endl;
  std::cout << "Enter \"single\" if you want to simulate a single queue or "
               "\"multiple\" to "
               "simulate multiple queues: \n> ";
  getline(cin, mode);

  if (mode == "single") {
    std::cout << "Simulating a single queue ..." << endl;
  } else if (mode == "multiple") {
    std::cout << "Simulating multiple queues ..." << endl;
  }

  return mode;
}

void addCustomer(stringstream &lineStream, string mode) {
  int items;
  double timeElapsed;
  Customer *inputcustomer;

  double timeforloop = expTimeElapsed;
  if (!getInt(lineStream, items) || !getDouble(lineStream, timeElapsed)) {
    std::cout << "Error: too few arguments." << endl;
    return;
  }
  if (foundMoreArgs(lineStream)) {
    std::cout << "Error: too many arguments." << endl;
    return;
  } else {
    lineStream >> items >> timeElapsed;
    expTimeElapsed += timeElapsed;
    // std::cout << "tester in addcustomer" << expTimeElapsed
    //         << endl;  // tttttttttttttttttttttttttttttttttttttttester
    inputcustomer = new Customer(expTimeElapsed, items);
  }

  // need to first depart the customers that have done
  Register *checkwhichone = nullptr;
  if (registerList->calculateMinDepartTimeRegister(expTimeElapsed) != nullptr) {
    while (registerList->calculateMinDepartTimeRegister(expTimeElapsed)
               ->calculateDepartTime() <= expTimeElapsed) {
      std::cout << "Departed a customer at register ID "
                << registerList->calculateMinDepartTimeRegister(expTimeElapsed)
                       ->get_ID()
                << " at "
                << registerList->calculateMinDepartTimeRegister(expTimeElapsed)
                       ->calculateDepartTime()
                << endl;
      registerList->calculateMinDepartTimeRegister(expTimeElapsed)
          ->departCustomer(doneList);
      if (mode == "single") {
        checkwhichone = registerList->get_free_register();
        while (registerList->get_free_register() &&
               (singleQueue->get_head() != nullptr)) {
          checkwhichone = registerList->get_free_register();
          Customer *waitlist = singleQueue->dequeue();
          checkwhichone->get_queue_list()->enqueue(waitlist);
          std::cout << "Queued a customer with free register "
                    << checkwhichone->get_ID() << endl;
        }
      }
      if (registerList->calculateMinDepartTimeRegister(expTimeElapsed) ==
          nullptr) {
        break;
      }
    }
  }

  // Depending on the mode of the simulation (single or multiple),
  // add the customer to the single queue or to the register with
  // fewest items
  checkwhichone = nullptr;
  if (mode == "single") {
    checkwhichone = registerList->get_free_register();
    if (checkwhichone == nullptr) {
      singleQueue->enqueue(inputcustomer);
      std::cout << "A customer entered" << endl;
      std::cout << "No free registers" << endl;
    } else {
      checkwhichone->get_queue_list()->enqueue(inputcustomer);
      std::cout << "A customer entered" << endl;
      std::cout << "Queued a customer with free register "
                << checkwhichone->get_ID() << endl;
    }
  } else if (mode == "multiple") {
    checkwhichone = registerList->get_min_items_register();
    checkwhichone->get_queue_list()->enqueue(inputcustomer);
    std::cout << "A customer entered" << endl;
    std::cout << "Queued a customer with quickest register "
              << checkwhichone->get_ID() << endl;
  }
}

void parseRegisterAction(stringstream &lineStream, string mode) {
  string operation;
  lineStream >> operation;
  if (operation == "open") {
    openRegister(lineStream, mode);
  } else if (operation == "close") {
    closeRegister(lineStream, mode);
  } else {
    std::cout << "Invalid operation" << endl;
  }
}

void openRegister(stringstream &lineStream, string mode) {
  int ID;
  double secPerItem, setupTime, timeElapsed;

  double timeforloop = expTimeElapsed;
  // cout << mode << endl;
  //  convert strings to int and double
  if (!getInt(lineStream, ID) || !getDouble(lineStream, secPerItem) ||
      !getDouble(lineStream, setupTime) ||
      !getDouble(lineStream, timeElapsed)) {
    std::cout << "Error: too few arguments." << endl;
    return;
  }
  if (foundMoreArgs(lineStream)) {
    std::cout << "Error: too many arguments" << endl;
    return;
  } else {
    lineStream >> ID;
    if (registerList->foundRegister(ID)) {
      std::cout << "Error: register " << ID << " is already open" << endl;
    } else {
      lineStream >> secPerItem >> setupTime >> timeElapsed;
      expTimeElapsed += timeElapsed;
      // std::cout << "tester in open register" << expTimeElapsed
      //   << endl;  // tttttttttttttttttttttttttttttttttttttttester
      // cout << "ID" << ID << secPerItem << setupTime << expTimeElapsed <<
      // endl;
    }
  }
  // need to first depart the customers that have done

  Register *checkwhichone = nullptr;

  if (registerList->calculateMinDepartTimeRegister(expTimeElapsed) != nullptr) {
    while (registerList->calculateMinDepartTimeRegister(expTimeElapsed)
               ->calculateDepartTime() <= expTimeElapsed) {
      std::cout << "Departed a customer at register ID "
                << registerList->calculateMinDepartTimeRegister(expTimeElapsed)
                       ->get_ID()
                << " at "
                << registerList->calculateMinDepartTimeRegister(expTimeElapsed)
                       ->calculateDepartTime()
                << endl;
      registerList->calculateMinDepartTimeRegister(expTimeElapsed)
          ->departCustomer(doneList);
      if (mode == "single") {
        checkwhichone = registerList->get_free_register();
        while (registerList->get_free_register() &&
               (singleQueue->get_head() != nullptr)) {
          checkwhichone = registerList->get_free_register();
          Customer *waitlist = singleQueue->dequeue();
          checkwhichone->get_queue_list()->enqueue(waitlist);
          std::cout << "Queued a customer with free register "
                    << checkwhichone->get_ID() << endl;
        }
      }
      if (registerList->calculateMinDepartTimeRegister(expTimeElapsed) ==
          nullptr) {
        break;
      }
    }
  }

  // Check if the register is already open
  if (registerList->foundRegister(ID)) {
  } else {
    Register *anewregister =
        new Register(ID, secPerItem, setupTime, expTimeElapsed);
    registerList->enqueue(anewregister);
    std::cout << "Opened register " << ID << endl;

    if (mode == "single") {
      if (singleQueue->get_head() != nullptr) {
        Customer *gotoregister = singleQueue->dequeue();
        Register *thefreeregister = registerList->get_free_register();
        registerList->get_free_register()->get_queue_list()->enqueue(
            gotoregister);

        std::cout << "Queued a customer with free register "
                  << thefreeregister->get_ID() << endl;
      }
    }
  }
  // If it's open, print an error message
  // Otherwise, open the register

  // If we were simulating a single queue,
  // and there were customers in line, then
  // assign a customer to the new register
}

void closeRegister(stringstream &lineStream, string mode) {
  int ID;
  double timeElapsed;
  // convert string to int
  double timeforloop = expTimeElapsed;
  if (!getInt(lineStream, ID) || !getDouble(lineStream, timeElapsed)) {
    std::cout << "Error: too few arguments." << endl;
    return;
  }
  if (foundMoreArgs(lineStream)) {
    std::cout << "Error: too many arguments" << endl;
    return;
  } else {
    lineStream >> ID;
    // cout << ID << registerList->foundRegister(ID) << endl;  // tester
    if (!registerList->foundRegister(ID)) {
      std::cout << "Error: register " << ID << " is not open" << endl;
    } else {  // backtohere
      lineStream >> timeElapsed;
      expTimeElapsed += timeElapsed;
      // std::cout << "tester in close register" << expTimeElapsed
      //<< endl;  // tttttttttttttttttttttttttttttttttttttttester
    }
  }
  Register *checkwhichone = nullptr;

  if (registerList->calculateMinDepartTimeRegister(expTimeElapsed) != nullptr) {
    while (registerList->calculateMinDepartTimeRegister(expTimeElapsed)
               ->calculateDepartTime() <= expTimeElapsed) {
      std::cout << "Departed a customer at register ID "
                << registerList->calculateMinDepartTimeRegister(expTimeElapsed)
                       ->get_ID()
                << " at "
                << registerList->calculateMinDepartTimeRegister(expTimeElapsed)
                       ->calculateDepartTime()
                << endl;
      registerList->calculateMinDepartTimeRegister(expTimeElapsed)
          ->departCustomer(doneList);
      if (mode == "single") {
        checkwhichone = registerList->get_free_register();
        while (registerList->get_free_register() &&
               (singleQueue->get_head() != nullptr)) {
          checkwhichone = registerList->get_free_register();
          Customer *waitlist = singleQueue->dequeue();
          checkwhichone->get_queue_list()->enqueue(waitlist);
          std::cout << "Queued a customer with free register "
                    << checkwhichone->get_ID() << endl;
        }
      }
      if (registerList->calculateMinDepartTimeRegister(expTimeElapsed) ==
          nullptr) {
        break;
      }
    }
  }
  // Check if the register is open
  // If it is open dequeue it and free it's memory
  // Otherwise, print an error message
  if (!registerList->foundRegister(ID)) {
  } else {
    Register *gonnadelete;
    gonnadelete = registerList->dequeue(ID);
    std::cout << "Closed register " << gonnadelete->get_ID() << endl;
    delete gonnadelete;
  }
}

bool getInt(stringstream &lineStream, int &iValue) {
  // Reads an int from the command line
  string command;
  lineStream >> command;
  if (lineStream.fail()) {
    return false;
  }
  iValue = stoi(command);
  return true;
}

bool getDouble(stringstream &lineStream, double &dvalue) {
  // Reads a double from the command line
  string command;
  lineStream >> command;
  if (lineStream.fail()) {
    return false;
  }
  dvalue = stod(command);
  return true;
}

bool foundMoreArgs(stringstream &lineStream) {
  string command;
  lineStream >> command;
  if (lineStream.fail()) {
    return false;
  } else {
    return true;
  }
}

void printfinal() {
  int totalnum = 0;
  double totalwait = 0;
  double averagewait = 0;
  double standarddeviation = 0;
  double maxwait = 0;
  Customer *temp = doneList->get_head();
  while (temp != nullptr) {
    totalnum += 1;
    totalwait += (temp->get_departureTime()) - (temp->get_arrivalTime());
    if ((temp->get_departureTime()) - (temp->get_arrivalTime()) > maxwait) {
      maxwait = (temp->get_departureTime()) - (temp->get_arrivalTime());
    }
    temp = temp->get_next();
  }

  averagewait = totalwait / totalnum;

  temp = doneList->get_head();
  while (temp != nullptr) {
    double difference =
        ((temp->get_departureTime()) - (temp->get_arrivalTime())) - averagewait;
    double square = difference * difference;
    standarddeviation += square;
    temp = temp->get_next();
  }

  standarddeviation = standarddeviation / totalnum;
  standarddeviation = sqrt(standarddeviation);

  std::cout << endl << "Finished at time " << expTimeElapsed << endl;
  std::cout << "Statistics:" << endl;
  std::cout << "  Maximum wait time: " << maxwait << endl;
  std::cout << "  Average wait time: " << averagewait << endl;
  std::cout << "  Standard Deviation of wait time: " << standarddeviation
            << endl;
}
