#include <iostream>
#include "SlackMessenger.hpp"
#include <wiringPi.h>

int main() {
    const std::string slackApiUrl =  "https://slack.com/api/chat.postMessage";
    const std::string slackApiToken = "input-your-slack-api-token";
    const std::string xmlPath = "input-haarcascade-xml-file-path";

    SlackMessenger messenger(slackApiUrl, slackApiToken);
    UltrasonicSensor sensor(4,5);
    FaceDetect detector(xmlPath);

    std::string payload = messenger.readJsonFilename("message.json");

    while(1) {
        float dis = sensor.measureDistance();
        std::cout << dis << std::endl;

        if(dis <= 30) {
            //std::cout << detector.detect(detector.takePic()) << std::endl;
            if(detector.detect(detector.takePic()) == 1) {
                messenger.sendMessageToSlack(payload);
            }
        }
        delay(3000);
    }
    return 0;
}