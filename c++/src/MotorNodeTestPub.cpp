#include "NodeHandler.h"
#include "Motor.pb.h"
#include <sstream>
#include <string>
#include <iostream>
#include <chrono>

motor_msg::LegCmd leg_a, leg_b, leg_c, leg_d;

motor_msg::MotorCmd* getMotor(motor_msg::LegCmd& leg, const std::string& motor_name)
{
    if (motor_name == "HIP") return leg.mutable_hip();
    if (motor_name == "STREEING") return leg.mutable_streeing();
    if (motor_name == "HUB") return leg.mutable_hub();
    return nullptr;
}

motor_msg::LegCmd* getModule(char module_char)
{
    switch (toupper(module_char))
    {
        case 'A': return &leg_a;
        case 'B': return &leg_b;
        case 'C': return &leg_c;
        case 'D': return &leg_d;
        default: return nullptr;
    }
}

void printHelp()
{
    std::cout << "Usage:\n";
    std::cout << "  SET <Module A-D> <Motor HIP|STREEING|HUB> <Pos> <Kp> <Ki> <Kd> <Torque>\n";
    std::cout << "  Example: SET A HIP 1.0 20.0 0.1 0.2 3.0\n";
    std::cout << "  Type SEND to publish current config\n";
    std::cout << "  Type SHOW to view current config\n";
    std::cout << "  Type EXIT to quit\n";
}

void printMotorCmd(const std::string& label, const motor_msg::MotorCmd& m)
{
    std::cout << "  " << label << " => pos: " << m.position()
              << ", kp: " << m.kp()
              << ", ki: " << m.ki()
              << ", kd: " << m.kd()
              << ", torque: " << m.torque() << "\n";
}

void showCurrentConfig()
{
    std::cout << "\n--- Current Command Configuration ---\n";
    std::cout << "[Module A]\n";
    printMotorCmd("HIP", leg_a.hip());
    printMotorCmd("STREEING", leg_a.streeing());
    printMotorCmd("HUB", leg_a.hub());

    std::cout << "[Module B]\n";
    printMotorCmd("HIP", leg_b.hip());
    printMotorCmd("STREEING", leg_b.streeing());
    printMotorCmd("HUB", leg_b.hub());

    std::cout << "[Module C]\n";
    printMotorCmd("HIP", leg_c.hip());
    printMotorCmd("STREEING", leg_c.streeing());
    printMotorCmd("HUB", leg_c.hub());

    std::cout << "[Module D]\n";
    printMotorCmd("HIP", leg_d.hip());
    printMotorCmd("STREEING", leg_d.streeing());
    printMotorCmd("HUB", leg_d.hub());
    std::cout << "--------------------------------------\n";
}

int main()
{
    core::NodeHandler nh;
    core::Publisher<motor_msg::MotorCmdStamped>& pub = nh.advertise<motor_msg::MotorCmdStamped>("motor/command");

    int seq = 0;
    printHelp();

    while (true)
    {
        std::cout << "\n>> ";
        std::string line;
        std::getline(std::cin, line);
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        for (auto& c : cmd) c = toupper(c);

        if (cmd == "EXIT")
        {
            break;
        }
        else if (cmd == "HELP")
        {
            printHelp();
        }
        else if (cmd == "SHOW")
        {
            showCurrentConfig();
        }
        else if (cmd == "SET")
        {
            char module_char;
            std::string motor_name;
            float pos, kp, ki, kd, torque;

            if (!(iss >> module_char >> motor_name >> pos >> kp >> ki >> kd >> torque))
            {
                std::cout << "Invalid format. Use: SET A HIP 1.0 20.0 0.1 0.2 3.0\n";
                continue;
            }

            std::string motor_upper;
            for (auto c : motor_name) motor_upper += toupper(c);

            auto* leg = getModule(module_char);
            if (!leg)
            {
                std::cout << "Invalid module. Use A, B, C, or D.\n";
                continue;
            }

            auto* motor = getMotor(*leg, motor_upper);
            if (!motor)
            {
                std::cout << "Invalid motor name. Use HIP, STREEING, or HUB.\n";
                continue;
            }

            motor->set_position(pos);
            motor->set_kp(kp);
            motor->set_ki(ki);
            motor->set_kd(kd);
            motor->set_torque(torque);

            std::cout << "Updated " << static_cast<char>(toupper(module_char)) << " " << motor_upper
                      << " to pos=" << pos << ", kp=" << kp << ", ki=" << ki
                      << ", kd=" << kd << ", torque=" << torque << "\n";
        }
        else if (cmd == "SEND")
        {
            motor_msg::MotorCmdStamped msg;

            auto* header = msg.mutable_header();
            header->set_seq(seq++);
            auto now = std::chrono::system_clock::now();
            auto epoch = now.time_since_epoch();
            header->mutable_stamp()->set_sec(static_cast<int32_t>(std::chrono::duration_cast<std::chrono::seconds>(epoch).count()));
            header->mutable_stamp()->set_usec(static_cast<int32_t>(std::chrono::duration_cast<std::chrono::microseconds>(epoch).count() % 1000000));

            *msg.mutable_module_a() = leg_a;
            *msg.mutable_module_b() = leg_b;
            *msg.mutable_module_c() = leg_c;
            *msg.mutable_module_d() = leg_d;

            pub.publish(msg);
            std::cout << "Published MotorCmdStamped.\n";
        }
        else
        {
            std::cout << "Unknown command. Type HELP for usage.\n";
        }

        core::spinOnce();
    }

    std::cout << "Exiting Motor Command Publisher.\n";
    return 0;
}
