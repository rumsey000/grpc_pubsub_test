#include "NodeHandler.h"
#include "Power.pb.h"
#include <sstream>
#include <string>
#include <iostream>
#include <chrono>

power_msg::ROBOTMODE parse_mode(char mode_char) 
{
    switch (mode_char) 
    {
        case 'R': return power_msg::REST_MODE;
        case 'M': return power_msg::MOTOR_MODE;
        case 'S': return power_msg::SET_ZERO;
        case 'H': return power_msg::HALL_CALIBRATE;
        default:
            std::cout << "Invalid command.\n";  
            return power_msg::REST_MODE;
    }
}

int main() 
{
    core::NodeHandler nh;
    core::Rate rate(1000);
    core::Publisher<power_msg::PowerCmdStamped> &pub = nh.advertise<power_msg::PowerCmdStamped>("power/command");
    
    int seq = 0;
    bool digital = false, signal = false, power = false;
    power_msg::ROBOTMODE robot_mode = power_msg::REST_MODE;

    while (1) 
    {
        std::string line;
        std::cout << "Enter command (e.g. P D 1 or F M H): ";
        std::getline(std::cin, line);
        std::istringstream iss(line);
        std::string cmd, subcmd;
        iss >> cmd >> subcmd;
        if (!cmd.empty()) cmd[0] = toupper(cmd[0]);
        if (!subcmd.empty()) subcmd[0] = toupper(subcmd[0]);

        if (cmd == "P" && (subcmd == "D" || subcmd == "S" || subcmd == "P")) 
        {
            int val;
            iss >> val;
            if (subcmd == "D") digital = (val != 0);
            if (subcmd == "S") signal  = (val != 0);
            if (subcmd == "P") power   = (val != 0);
        } 
        else if (cmd == "F" && subcmd == "M") 
        {
            char mode_char;
            if (!(iss >> mode_char)) 
            {
                std::cout << "Missing mode character. Usage: F M <R|M|S|H>\n";
                continue;
            }
            mode_char = toupper(mode_char);
            robot_mode = parse_mode(mode_char);
        }
        else if (cmd == "H") {
            std::cout << "Usage:\n"
                      << "  P D|S|P <0|1>  - Set digital/signal/power state\n"
                      << "  F M <R|M|S|H>  - Set robot mode\n";
            continue;
        }
        else 
        {
            std::cout << "Invalid command.\n";
            continue;
        }

        power_msg::PowerCmdStamped msg;

        auto* header = msg.mutable_header();
        header->set_seq(seq++);
        auto now = std::chrono::system_clock::now();
        auto epoch = now.time_since_epoch();
        header->mutable_stamp()->set_sec(static_cast<int32_t>(std::chrono::duration_cast<std::chrono::seconds>(epoch).count()));
        header->mutable_stamp()->set_usec(static_cast<int32_t>(std::chrono::duration_cast<std::chrono::microseconds>(epoch).count() % 1000000));

        msg.set_digital(digital);
        msg.set_signal(signal);
        msg.set_power(power);
        msg.set_robot_mode(robot_mode);

        pub.publish(msg);
    }
    return 0;
}