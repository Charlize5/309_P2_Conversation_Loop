#include "harness/harness.h"
#include "model/scripted_client.h"
#include <iostream>
#include <fstream>
#include <string>
#include "core/conversation.h"
#include <utility>
#include <stdexcept>



//cinstructor 
Conversation::Conversation()
: data_(nullptr), size_(), capacity_() {}

//destructor
Conversation::~Conversation() {
delete[] data_; }

//copy constructor

Conversation::Conversation(const Conversation& other)
: data_(nullptr), size_(other.size_), capacity_(other.capacity_) {
   

//deep copies are polynomial
    if (capacity_ > 0) {
        data_ = new Message[capacity_];
        for (std::size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    }
}
//copy assignment operator
Conversation& Conversation::operator=(const Conversation& other) {
if (this == &other) return *this;  // Self-assignment check
        // Release current resources
    Message* new_data = nullptr; //copied from lecture slides lol
    if (other.capacity_ > 0) {
        new_data = new Message[other.capacity_];
        for (std::size_t i = 0; i < other.size_; ++i) {
            new_data[i] = other.data_[i];
        }
    }

delete[] data_;  // Release old resources
    data_ = new_data;
    size_ = other.size_;
    capacity_ = other.capacity_;
    return *this; // Return this for the assignments to continue where it left off
      
}
//move constructor
Conversation::Conversation(Conversation&& other) noexcept   
:data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}

//move assignment operator
Conversation& Conversation::operator=(Conversation&& other) noexcept {
    if (this != &other) {
        delete[] data_;  // free current resources
        data_ = other.data_;
        size_ = other.size_;

        capacity_ = other.capacity_;

        other.data_ = nullptr; //nuetralize source
        other.size_ = 0;
        other.capacity_ = 0;
       
    }
    return *this;
}
//autofills for fucntions calls
std::size_t Conversation::size() const noexcept {
    return size_;
}

const Message* Conversation::begin() const noexcept {
    return data_;
}

const Message* Conversation::end() const noexcept {
    return data_ + size_;
}
//doubles if full 
void Conversation::append(Message m) {
    if (size_ == capacity_) {
        std::size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
        Message* new_data = new Message[new_capacity];
        for (std::size_t i = 0; i < size_; ++i) {
            new_data[i] = std::move(data_[i]);
        }
        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
    }
    data_[size_] = std::move(m);
    ++size_;
}

const Message& Conversation::at(std::size_t i) const { //return the messafe at index i
    if (i >= size_) {
        throw std::out_of_range("Index out of bounds"); 
        //spec says handle out of bounds :(
    }
    return data_[i];
}


