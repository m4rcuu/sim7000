#pragma once

#include <string>
#include <vector>

class SIM7000
{
private:
    std::string port;
    int baudrate;
    bool postInitOn;
    int powerKey;

    int fd;
    std::string lastMessage;
    std::string lastCommand;
    std::string lastInfo;

    void setGPIO();
    void cleanGPIO();

    void simOn();
    void simOff();

public:
    SIM7000(std::string _port = "/dev/serial0", const int _baudrate = 115200, const bool _postInitOn = true, const int _powerKey = 4);
    ~SIM7000();

    void connectToSerial();
    void disconnectSerial();
    const bool isConnected();

    std::string getLastMessage();
    std::string getLastCommand();
    std::string getLastInfo();

    std::string sendMessage(std::string command = "at", const int bufferSize = 128);
    std::vector<std::string> sendListMessages(std::vector<std::string> commands = {"at"}, const int bufferSize = 128);
};