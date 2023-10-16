#include <iostream>
#include "SlackMessenger.hpp"
#include <wiringPi.h>

int main() {
    const std::string slackApiUrl =  "https://slack.com/api/chat.postMessage";
    const std::string slackApiToken = "input-your-slack-api-token";
    //const std::string xmlPath = "/usr/local/share/opencv4/haarcascades/haarcascade_frontalface_default.xml";
    const std::string jsonPath = "../text/message.json";
    int imgNum = 0;
    SlackMessenger messenger(slackApiUrl, slackApiToken);
    UltrasonicSensor sensor(4,5);
    FaceDetect detector;

    std::string payload = messenger.readJsonFilename(jsonPath);

    while(1) {
        float dis = sensor.measureDistance();
        std::cout << dis << std::endl;

        if(dis <= 30) {
            imgNum++;
            std::string imgPath = detector.takePic(imgNum);
            if(detector.detect(imgPath) == 1) {
                messenger.sendMessageToSlack(payload);
            }else{
                detector.deletePicture(imgPath);
                imgNum--;
            }
        }
        delay(300);
    }
    return 0;
}