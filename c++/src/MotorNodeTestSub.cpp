#include "NodeHandler.h"
#include "Motor.pb.h"
#include <vector>
#include <string>
#include <iostream>

std::mutex mutex_;

void printMotorState(const std::string& name, const motor_msg::MotorState& state) 
{
    std::cout << name << " : "
              << "position: " << state.position()
              << " velocity: " << state.velocity()
              << " torque: " << state.torque() << std::endl;
}

void printCmdState(const std::string& name, const motor_msg::MotorCmd& cmd) 
{
    std::cout << name << " : "
              << "position: " << cmd.position()
              << " PID: " << cmd.kp() << " " << cmd.ki() << " " << cmd.kd()
              << " torque: " << cmd.torque() << std::endl;
}

void state_cb(motor_msg::MotorStateStamped m) 
{
    mutex_.lock();

    std::cout << "header : " << m.header().seq() << "  ";
    std::cout << "stamp : " << m.header().stamp().sec() << "." << m.header().stamp().usec() << "\n";

    std::vector<std::pair<std::string, const motor_msg::LegState&>> modules = {
        {"module_a", m.module_a()},
        {"module_b", m.module_b()},
        {"module_c", m.module_c()},
        {"module_d", m.module_d()}
    };

    for (const auto& [mod_name, leg_state] : modules) {
        std::cout << "=== " << mod_name << " ===" << std::endl;
        printMotorState("hip",      leg_state.hip());
        printMotorState("streeing", leg_state.streeing());  
        printMotorState("hub",      leg_state.hub());
        std::cout << std::endl;
    }

    mutex_.unlock();
}

void cmd_cb(motor_msg::MotorCmdStamped m) 
{
    mutex_.lock();

    std::cout << "header : " << m.header().seq() << "  ";
    std::cout << "stamp : " << m.header().stamp().sec() << "." << m.header().stamp().usec() << "\n";

    std::vector<std::pair<std::string, const motor_msg::LegCmd&>> modules = {
        {"module_a", m.module_a()},
        {"module_b", m.module_b()},
        {"module_c", m.module_c()},
        {"module_d", m.module_d()}
    };

    for (const auto& [mod_name, leg_cmd] : modules) {
        std::cout << "=== " << mod_name << " ===" << std::endl;
        printCmdState("streeing", leg_cmd.streeing());  
        printCmdState("hip",      leg_cmd.hip());
        printCmdState("hub",      leg_cmd.hub());
        std::cout << std::endl;
    }

    mutex_.unlock();
}

int main() {
    core::NodeHandler nh;
    core::Rate rate(1000);
    core::Subscriber<motor_msg::MotorStateStamped> &state_sub = nh.subscribe<motor_msg::MotorStateStamped>("motor/state", 1000, state_cb);
    core::Subscriber<motor_msg::MotorCmdStamped> &cmd_sub = nh.subscribe<motor_msg::MotorCmdStamped>("motor/command", 1000, cmd_cb);

    while (1) {
        core::spinOnce();
        std::cout << rate.sleep() << " << sleep" << "\n";
    }

    return 0;
}