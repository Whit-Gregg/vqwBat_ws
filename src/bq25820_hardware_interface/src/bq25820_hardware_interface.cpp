#include "bq25820_hardware_interface/bq25820_hardware_interface.hpp"
#include "bq25820_hardware_interface.hpp"

namespace bq25820_hardware_interface
{

    Bq25820HardwareInterface::Bq25820HardwareInterface() {}

    // Bq25820HardwareInterface::~Bq25820HardwareInterface() {}

    CallbackReturn bq25820_hardware_interface::Bq25820HardwareInterface::on_configure(const rclcpp_lifecycle::State &previous_state)
    {
        //................
        return CallbackReturn();
    }

    CallbackReturn bq25820_hardware_interface::Bq25820HardwareInterface::on_cleanup(const rclcpp_lifecycle::State &previous_state)
    {
        //................
        return CallbackReturn();
    }

    CallbackReturn bq25820_hardware_interface::Bq25820HardwareInterface::on_activate(const rclcpp_lifecycle::State &previous_state)
    {
        //................
        return CallbackReturn();
    }

    CallbackReturn bq25820_hardware_interface::Bq25820HardwareInterface::on_deactivate(const rclcpp_lifecycle::State &previous_state)
    {
        //................
        return CallbackReturn();
    }

}       // end of namespace bq25820_hardware_interface
