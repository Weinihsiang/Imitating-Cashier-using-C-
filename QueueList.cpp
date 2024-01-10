
#include "QueueList.h"

#include "Customer.h"

QueueList::QueueList() { head = nullptr; }

QueueList::QueueList(Customer* customer) { head = customer; }

QueueList::~QueueList() {
  Customer* temp = head;
  Customer* current;
  while (temp != nullptr) {
    current = temp;
    temp = temp->get_next();
    delete current;
  }
}

Customer* QueueList::get_head() { return head; }

void QueueList::enqueue(Customer* customer) {
  // a customer is placed at the end of the queue
  // if the queue is empty, the customer becomes the head
  Customer* temp = head;
  Customer* current = head;
  if (temp == nullptr) {
    head = customer;
  } else {
    while (temp != nullptr) {
      current = temp;
      temp = temp->get_next();
    }
    current->set_next(customer);
  }
}

Customer* QueueList::dequeue() {
  // remove a customer from the head of the queue
  // and return a pointer to it
  Customer* temp = head;
  if (temp != nullptr) {
    head = temp->get_next();
  }
  temp->set_next(nullptr);
  return temp;
}

int QueueList::get_items() {
  // count total number of items each customer in the queue has
  Customer* temp = head;
  int numofitems = 0;
  while (temp != nullptr) {
    numofitems += temp->get_numOfItems();
    temp = temp->get_next();
  }
  return numofitems;
}

bool QueueList::empty_queue() {
  // if the queue is empty, return false
  // if the queue is not empty, delete all customers
  // and return true
  if (head == nullptr) {
    return false;
  } else {
    Customer* temp = head;
    Customer* current;
    while (temp != nullptr) {
      current = temp;
      temp = temp->get_next();
      delete current;
    }
    return true;
  }
}

void QueueList::print() {
  // print customers in a queue
  Customer* temp = head;
  while (temp != nullptr) {
    temp->print();
    temp = temp->get_next();
  }
}
