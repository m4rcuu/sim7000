#include "inc/sim7000.hpp"

#include <iostream>

int main()
{
    std::vector<std::string> messages = {"at",
                                         "at+cmnb=1",
                                         "at+cnmp=38",
                                         "at+cgatt=1",
                                         "at+cstt=\"pllab.pl\"",
                                         "at+ciicr",
                                         "at+cifsr",
                                         "at+cnact=1",
                                         "at+smconf=\"URL\",172.20.39.40,1883",
                                         "at+smconf=\"KEEPTIME\",60",
                                         "at+smconn",
                                         "at+smsub=\"home/livingroom/temperature\",1",
                                         "at+smpub=\"home/livingroom/temperature\",\"2\",1,1",
                                         "21"};

    SIM7000 sim = SIM7000();
    std::vector<std::string> outputMessages = sim.sendListMessages(messages);

    for (std::string msg : outputMessages)
    {
        std::cout << msg;
    }

    return 0;
}