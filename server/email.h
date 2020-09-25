//
// Created by giuseppe on 8/3/20.
//

#ifndef PDS_PROJECT_EMAIL_H
#define PDS_PROJECT_EMAIL_H

#include <curl/curl.h>
#include <iostream>
#include <cstring>
#include <iomanip>
#include <ctime>
#include <sstream>

static const char *payload_text[] = {
        "To: \r\n", //this will be substituted with the correct email
        "From: EditoRT \r\n",
        "Subject: Invitation to contribute\r\n",
        "\r\n", /* empty line to divide headers from body, see RFC5322 */
        "The body of the message starts here.\r\n", //this will be substituted with the URI of the file
        "\r\n",
        "Enjoy!\r\n",
        "TEAM EditoRT.\r\n",
        nullptr
};

struct upload_status {
    int lines_read;
};

class email {
public:
    static bool sendEmail(const std::string& email, const std::string& uri);
private:
    static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp);
};

#endif //PDS_PROJECT_EMAIL_H
