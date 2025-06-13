#include "NodeHandler.h"
#include "Power.pb.h"
#include <string>
#include <iostream>
std::mutex mutex_;

std::string robotModeToString(power_msg::ROBOTMODE mode) 
{
    switch (mode) {
        case power_msg::REST_MODE:        return "REST";
        case power_msg::SET_ZERO:         return "SET_ZERO";
        case power_msg::HALL_CALIBRATE:   return "HALL_CALIBRATE";
        case power_msg::MOTOR_MODE:       return "MOTOR";
        default:                          return "? — UNKNOWN";
    }
}

void cb(power_msg::PowerStateStamped m) 
{
    
    mutex_.lock();
    std::cout << "header : " << m.header().seq() << "  ";
    std::cout << "stamp : " << m.header().stamp().sec() << "." << m.header().stamp().usec() << "\n";
    std::cout << "digital : " << m.digital() << "  ";
    std::cout << "signal : " << m.signal() << "  ";
    std::cout << "power : " << m.power() << "  ";
    std::cout << "robot_mode : " << robotModeToString(m.robot_mode()) << "\n";

    double v_array[12] = 
    {
        m.v_0(), m.v_1(), m.v_2(), m.v_3(),
        m.v_4(), m.v_5(), m.v_6(), m.v_7(),
        m.v_8(), m.v_9(), m.v_10(), m.v_11()
    };

    double i_array[12] = 
    {
        m.i_0(), m.i_1(), m.i_2(), m.i_3(),
        m.i_4(), m.i_5(), m.i_6(), m.i_7(),
        m.i_8(), m.i_9(), m.i_10(), m.i_11()
    };

    for(int idx = 0; idx < 12; ++idx) {
        std::cout << "v[" << idx << "] : " << v_array[idx] << "  "
                  << "i[" << idx << "] : " << i_array[idx] << "\n";
    }
    mutex_.unlock();
}

int main() {
    core::NodeHandler nh;
    core::Rate rate(1000);
    core::Subscriber<power_msg::PowerStateStamped> &sub = nh.subscribe<power_msg::PowerStateStamped>("power/state", 1000, cb);
    while (1)
    {
        core::spinOnce();
        std::cout << rate.sleep() << " << sleep" << "\n";
    }
    return 0;
}