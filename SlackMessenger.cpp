#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include "SlackMessenger.hpp"

SlackMessenger::SlackMessenger(const std::string& apiUrl, const std::string& apiToken)
    : slackApiUrl(apiUrl), slackApiToken(apiToken) {}
    
void SlackMessenger::sendMessageToSlack(const std::string& payload) {
    CURL* curl = curl_easy_init();
    CURLcode res;
    if(curl) {
        //curl opt
        struct curl_slist* headers = NULL;
        //header Setup
        headers = curl_slist_append(headers, ("Authorization: Bearer " + slackApiToken).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        //set slack api url, header, content(payload) 
        curl_easy_setopt(curl, CURLOPT_URL, slackApiUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
        
        //HTTP POST
        res = curl_easy_perform(curl);
        // error
        if (res != CURLE_OK) {
            std::cerr << "cURL failed: " << curl_easy_strerror(res) << std::endl;
        }
        //free and cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}
//read JsonFile Payload(information of channel, text, botname)
std::string SlackMessenger::readJsonFilename(const std::string& filename) {
    std::ifstream file(filename);
    //error
    if(!file.is_open()) {
        std::cerr << "FAILED TO OPEN " <<filename <<std::endl; 
        return "";
    }

    std::string payload((std::istreambuf_iterator<char>(file)), 
    std::istreambuf_iterator<char>());

    file.close();
    return payload;
}

