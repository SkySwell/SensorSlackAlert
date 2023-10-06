#ifndef SLACKMESSENGER_HPP
#define SLACKMESSENGER_HPP

#include <string>


class SlackMessenger {
public:
    SlackMessenger(const std::string& apiUrl, const std::string& apiToken);
    void sendMessageToSlack(const std::string& payload);
    std::string readJsonFilename(const std::string& filename);

private:
    const std::string slackApiUrl;
    const std::string slackApiToken;
};

class UltrasonicSensor {
private:
    int TrigPin;
    int EchoPin;

public:
    UltrasonicSensor(int trigPin, int echoPin);
    float measureDistance();
};

#endif