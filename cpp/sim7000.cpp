#include "inc/sim7000.hpp"

#include <wiringPi.h>
#include <wiringSerial.h>

#include <algorithm>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <chrono>
#include <thread>

SIM7000::SIM7000(std::string _port, const int _baudrate, const bool _postInitOn, const int _powerKey) : port(_port),
                                                                                                        baudrate(_baudrate),
                                                                                                        postInitOn(_postInitOn),
                                                                                                        powerKey(_powerKey)
{
    this->fd = 0;
    this->lastMessage = "";
    this->lastCommand = "";
    this->lastInfo = "";

    if (this->postInitOn)
    {
        this->connectToSerial();
        if (this->isConnected())
        {
            this->setGPIO();
            this->simOn();
        }
    }
}

SIM7000::~SIM7000()
{
    this->simOff();
    this->disconnectSerial();
    this->cleanGPIO();
}

void SIM7000::connectToSerial()
{
    this->fd = serialOpen(this->port.c_str(), this->baudrate);
}

void SIM7000::disconnectSerial()
{
    serialClose(this->fd);
    this->fd = 0;
}

const bool SIM7000::isConnected()
{
    if (this->fd > 0)
    {
        return true;
    }

    return false;
}

void SIM7000::setGPIO()
{
    wiringPiSetupGpio();
    pinMode(this->powerKey, OUTPUT);
}

void SIM7000::cleanGPIO()
{
    digitalWrite(this->powerKey, LOW);
}

void SIM7000::simOn()
{
    digitalWrite(this->powerKey, HIGH);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    digitalWrite(this->powerKey, LOW);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    int num = 0;
    while (num < 10 && this->lastInfo != "OK")
    {
        num++;
        this->sendMessage();
        std::cout << "Trying to turn on SIM7000 for " << num << " time/s. Last info: " << this->lastInfo << "!\n";
    }
}

void SIM7000::simOff()
{
    digitalWrite(this->powerKey, HIGH);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    digitalWrite(this->powerKey, LOW);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

std::string SIM7000::getLastMessage()
{
    return this->lastMessage;
}

std::string SIM7000::getLastCommand()
{
    return this->lastCommand;
}

std::string SIM7000::getLastInfo()
{
    return this->lastInfo;
}

std::string SIM7000::sendMessage(std::string command, const int bufferSize)
{
    command += '\r';
    serialFlush(this->fd);
    std::this_thread::sleep_for(std::chrono::milliseconds(1200));
    serialPuts(this->fd, command.c_str());
    std::this_thread::sleep_for(std::chrono::milliseconds(1200));

    char buffer[bufferSize];
    memset(buffer, '\0', bufferSize);
    read(this->fd, buffer, bufferSize);

    std::string bufferString = buffer;
    this->lastMessage = bufferString;

    int numberOfr = 0;
    std::string temporaryCommand = "";
    std::string temporaryInfo = "";
    for (char s : this->lastMessage)
    {
        switch (s)
        {
        case '\r':
            numberOfr++;
            break;
        case '\n':
            break;
        default:
            if (numberOfr < 2)
            {
                temporaryCommand += s;
            }
            else
            {
                temporaryInfo += s;
            }
            break;
        }
    }
    this->lastCommand = temporaryCommand;
    this->lastInfo = temporaryInfo;

    return this->lastMessage;
}

std::vector<std::string> SIM7000::sendListMessages(std::vector<std::string> commands, const int bufferSize)
{
    std::vector<std::string> listMessages;

    for (std::string cmd : commands)
    {
        cmd += '\r';
        serialFlush(this->fd);
        std::this_thread::sleep_for(std::chrono::milliseconds(1200));
        serialPuts(this->fd, cmd.c_str());
        std::this_thread::sleep_for(std::chrono::milliseconds(1200));

        char buffer[bufferSize];
        memset(buffer, '\0', bufferSize);
        read(this->fd, buffer, bufferSize);

        std::string bufferString = buffer;
        this->lastMessage = bufferString;
        listMessages.emplace_back(this->lastMessage);
    }
    int numberOfr = 0;
    std::string temporaryCommand = "";
    std::string temporaryInfo = "";
    for (char s : this->lastMessage)
    {
        switch (s)
        {
        case '\r':
            numberOfr++;
            break;
        case '\n':
            break;
        default:
            if (numberOfr < 2)
            {
                temporaryCommand += s;
            }
            else
            {
                temporaryInfo += s;
            }
            break;
        }
    }
    this->lastCommand = temporaryCommand;
    this->lastInfo = temporaryInfo;

    return listMessages;
}