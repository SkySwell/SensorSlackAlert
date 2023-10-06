#include <iostream>
#include "SlackMessenger.hpp"
#include <wiringPi.h>

int main() {
    const std::string slackApiUrl =  "https://slack.com/api/chat.postMessage";
    const std::string slackApiToken = "input-your-slack-api-token";

    SlackMessenger messenger(slackApiUrl, slackApiToken);
    std::string payload = messenger.readJsonFilename("json-file-name");

   
    UltrasonicSensor sensor(4,5); //input your gpioNum(trigPin, echoPin)

    while(1) {
        float dis = sensor.measureDistance();
        std::cout << dis << std::endl;

        if(dis <= 30) {
            messenger.sendMessageToSlack(payload);
        }
        delay(3000);
    }
    return 0;
}