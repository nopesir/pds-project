//
// Created by giuseppe on 8/3/20.
//

#include "email.h"


#include <utility>

bool email::sendEmail(const std::string& email, const std::string& uri) {
    CURL *curl;
    CURLcode result = CURLE_OK;
    struct curl_slist *recipients = nullptr;
    struct upload_status upload_ctx{};
    bool res = true;
    upload_ctx.lines_read = 0;
    curl = curl_easy_init();

    if(curl) {
        //Source credentials
        curl_easy_setopt(curl, CURLOPT_USERNAME, "editort.noanswer@gmail.com");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "testpassword0_");

        //SMTP server address and port
        curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.gmail.com:587");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

        //set Receiver
        std::string conc = "To: " + email + "\r\n";
        payload_text[0] = conc.c_str();

        //set URI to send
        std::string msgURI = "You have been invited to contribute on a file. Please insert this uri in the app to start modifying the document: " + uri;
        payload_text[4] = msgURI.c_str();

        //Destination email address
        recipients = curl_slist_append(recipients, email.c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        result = curl_easy_perform(curl);
        if(result != CURLE_OK) {
            std::cout << "Failed to send email." << std::endl;
            res=false;
        }
        if(res) {
            curl_slist_free_all(recipients);
            curl_easy_cleanup(curl);

        }
    }
    return res;

}

size_t email::payload_source(void *ptr, size_t size, size_t nmemb, void *userp) {
    auto *upload_ctx = (struct upload_status *)userp;
    const char *data;

    if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
        return 0;
    }

    data = payload_text[upload_ctx->lines_read];
    if(data) {
        size_t len = strlen(data);
        memcpy(ptr, data, len);
        upload_ctx->lines_read++;
        return len;
    }
    return 0;
}

