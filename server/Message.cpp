//
// Created by gheb on 31/07/20.
//

#include "Message.h"

#include <cstring>

Message::Message() : body_length_(0) {
}

const char *Message::data() const {
    return data_;
}

char *Message::data() {
    return data_;
}

std::size_t Message::length() const {
    return header_length + body_length_;
}

const char *Message::body() const {
    return data_ + header_length;
}

char *Message::body() {
    return data_ + header_length;
}

std::size_t Message::body_length() const {
    return body_length_;
}

void Message::body_length(std::size_t new_length) {
    body_length_ = new_length;
}

void Message::decode_header() {
    char header[header_length + 2] = "";
    std::strncat(header, data_+1, header_length);
    body_length_ = std::atoi(header);
    this->setLastChunk(*data_);
}

void Message::encode_header() {
    char header[header_length + 1] = "";
    std::sprintf(header, "%5d", static_cast<int>(body_length_));
    std::memcpy(data_+1, header, header_length);
}

char& Message::isThisLastChunk() {
    return isLastChunk;
}

void Message::setLastChunk(char val) {
    this->isLastChunk = val;
}

Message Message::constructMsg(const std::string& chunkResponse, char isLastChunk) {
    //Send data (header and body)
    Message msg;
    msg.setLastChunk(isLastChunk);
    msg.body_length(chunkResponse.size());
    std::memcpy(msg.body()+1, chunkResponse.data(), msg.body_length());
    //perchè lo chiama di nuovo?
    msg.body_length(chunkResponse.size());

    msg.body()[msg.body_length()+1] = '\0';
    msg.encode_header();
    std::memcpy(msg.data(), &msg.isThisLastChunk(), 1);
    return msg;
}