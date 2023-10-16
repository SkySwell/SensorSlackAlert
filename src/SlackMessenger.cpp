#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include "SlackMessenger.hpp"
#include <wiringPi.h>
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/shape_predictor.h>
#include <dlib/image_io.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <vector>

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

FaceDetect::FaceDetect(){}

std::string FaceDetect::takePic(int imgNum) {
    std::string photoName = "img" + std::to_string(imgNum) + ".jpg";
    std::string imgPath = "../img/" + photoName;
    std::string command = "/usr/bin/libcamera-still -n -o " + imgPath;
    std::system(command.c_str());
    return imgPath;
}

int FaceDetect::detect(const std::string& imgPath) {

    dlib::frontal_face_detector faceDetector = dlib::get_frontal_face_detector();
    dlib::shape_predictor shapePredictor;
    dlib::deserialize("../detector/shape_predictor_68_face_landmarks.dat") >> shapePredictor;
    cv::Mat img = cv::imread(imgPath);

    while(1) {
        
        dlib::cv_image<dlib::bgr_pixel> dlibImage(img);
        std::vector<dlib::rectangle> faces = faceDetector(dlibImage);

        if(!faces.empty()) {
            std::cout << "face detect!" << std::endl;
            return 1;
        }else {
            std::cout << "not FIND!" << std::endl;
            return 0;
        }
    }
}

void FaceDetect::deletePicture(const std::string& imgPath) {
    std::string command = "rm " + imgPath;
    std::system(command.c_str());
    std::cout << "image Deleted!" << std::endl;
}