//
// Created by gheb on 31/07/20.
//

#include "Message.h"

#include <cstring>
#include <iostream>

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

void Message::set_body_length(std::size_t new_length) {
    body_length_ = new_length;
}

void Message::decode_header() {
    char header[header_length + 2] = "";
    std::strncat(header, data_+1, header_length);
    body_length_ = std::atoi(header);
    this->set_last_chunk(*data_);
}

void Message::encode_header() {
    char header[header_length + 1] = "";
    std::sprintf(header, "%5d", static_cast<int>(body_length_));
    std::memcpy(data_+1, header, header_length);
}

char& Message::is_last_chunk() {
    return isLastChunk;
}

void Message::set_last_chunk(char val) {
    this->isLastChunk = val;
}

Message Message::build_msg(const std::string& chunk, char is_last) {
    //Send data (header and body)
    Message msg;
    msg.set_last_chunk(is_last);
    msg.set_body_length(chunk.size());
    std::memcpy(msg.body()+1, chunk.data(), msg.body_length());
    msg.set_body_length(chunk.size());

    msg.body()[msg.body_length()+1] = '\0';
    msg.encode_header();
    std::memcpy(msg.data(), &msg.is_last_chunk(), 1);
    return msg;
}