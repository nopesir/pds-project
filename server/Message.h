//
// Created by gheb on 31/07/20.
//

#ifndef PROJECT_MESSAGE_H
#define PROJECT_MESSAGE_H


#include <string>
#define MAX_CHUNK_LENGTH 65536 //64K

class Message {

public:
    enum { header_length = 5 };
    Message();
    const char* data() const;
    char* data();
    std::size_t length() const;
    const char* body() const;
    char* body();
    std::size_t body_length() const;
    void set_body_length(std::size_t new_length);
    void decode_header();
    void encode_header();
    char& is_last_chunk();
    void set_last_chunk(char val);
    static Message build_msg(const std::string& chunk, char is_last);

private:
    char data_[MAX_CHUNK_LENGTH + header_length + 1];
    std::size_t body_length_;
    char isLastChunk;

};


#endif //PROJECT_MESSAGE_H
