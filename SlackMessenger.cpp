#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include "SlackMessenger.hpp"
#include <wiringPi.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/objdetect.hpp>

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

//UltrasonicSensor
UltrasonicSensor::UltrasonicSensor(int trigPin, int echoPin)
: TrigPin(trigPin), EchoPin(echoPin) {
    //init pinMode
    wiringPiSetup();
    pinMode(EchoPin, INPUT);
    pinMode(TrigPin, OUTPUT);
}

//
float UltrasonicSensor::measureDistance() {
    struct timeval tv1;
    struct timeval tv2;
    long time1, time2;
    float dis;

    digitalWrite(TrigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(TrigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(TrigPin, LOW);

    while (!(digitalRead(EchoPin) == 1));
    gettimeofday(&tv1, NULL);
    while (!(digitalRead(EchoPin) == 0));
    gettimeofday(&tv2, NULL);

    time1 = tv1.tv_sec * 1000000 + tv1.tv_usec;
    time2 = tv2.tv_sec * 1000000 + tv2.tv_usec;

    dis = (float)(time2 - time1) / 1000000 * 34000 / 2;

    return dis;
    
}

FaceDetect::FaceDetect(const std::string& xmlPath)
: haarcascadeXmlPath(xmlPath) {}

std::string FaceDetect::takePic() {
    //구현 해야함//
    return "IMG.jpg";
}

int FaceDetect::detect(const std::string& imgPath) {

    cv::Mat img = cv::imread(imgPath);
    //Classifier object
    cv::CascadeClassifier faceCascade;
    //load XML to object(faceCascade)
    faceCascade.load(haarcascadeXmlPath);

    //XML file check
    if(faceCascade.empty()) {
        std::cout << "XML file load failed!" << std::endl;
    }

    std::vector<cv::Rect> faces;
    faces.clear();
    faceCascade.detectMultiScale(img, faces, 1.1, 3);

    if(faces.size() > 0) {
        std::cout << "find FACE!" << std::endl;
        return 1;
    }
    else {
        std::cout << "not FIND!" << std::endl;
        return 0;
    }
    //return 0;
}