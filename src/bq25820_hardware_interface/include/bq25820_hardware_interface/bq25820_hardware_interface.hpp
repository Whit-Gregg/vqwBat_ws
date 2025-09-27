#ifndef BQ25820_HARDWARE_INTERFACE__BQ25820_HARDWARE_INTERFACE_HPP_
#define BQ25820_HARDWARE_INTERFACE__BQ25820_HARDWARE_INTERFACE_HPP_

#include "bq25820_hardware_interface/visibility_control.h"
#include <hardware_interface/hardware_info.hpp>
#include <hardware_interface/sensor_interface.hpp>
#include <hardware_interface/types/hardware_interface_return_values.hpp>

#include "elapsedMillis.h"
#include <rclcpp/rclcpp.hpp>

namespace bq25820_hardware_interface
{

    class Bq25820HardwareInterface : public hardware_interface::SensorInterface
    {
      public:
        Bq25820HardwareInterface();

        virtual ~Bq25820HardwareInterface() = default;

        //-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~
        //-=+~-=+~-=+~-=
        //-=+~-=+~-=+~-=   overrides from SensorInterface
        //-=+~-=+~-=+~-=
        //-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~

        //-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~
        /// Initialization of the hardware interface from data parsed from the robot's URDF.
        /**
         * \param[in] hardware_info structure with data from URDF.
         * \returns CallbackReturn::SUCCESS if required data are provided and can be parsed.
         * \returns CallbackReturn::ERROR if any error happens or data are missing.
         */
        //virtual hardware_interface::CallbackReturn on_init(const hardware_interface::HardwareInfo &hardware_info) override;
        CallbackReturn on_init(const hardware_interface::HardwareComponentInterfaceParams &params) override;

        //-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~
        /**
         * Import the InterfaceDescription for the StateInterfaces from the HardwareInfo.
         * Separate them into the possible types: Sensor and store them.
         */
        // // // // // virtual void import_state_interface_descriptions(const hardware_interface::HardwareInfo &hardware_info);

        //-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~
        /**
         * Default implementation for exporting the StateInterfaces. The StateInterfaces are created
         * according to the InterfaceDescription. The memory accessed by the controllers and hardware is
         * assigned here and resides in the sensor_interface.
         *
         * \return vector of shared pointers to the created and stored StateInterfaces
         */
        virtual std::vector<hardware_interface::StateInterface::ConstSharedPtr> on_export_state_interfaces() override;

        //-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~
        /// Read the current state values from the actuator.
        /**
         * The data readings from the physical hardware has to be updated
         * and reflected accordingly in the exported state interfaces.
         * That is, the data pointed by the interfaces shall be updated.
         *
         * \param[in] time The time at the start of this control loop iteration
         * \param[in] period The measured time taken by the last control loop iteration
         * \return return_type::OK if the read was successful, return_type::ERROR otherwise.
         */
        virtual hardware_interface::return_type read(const rclcpp::Time &time, const rclcpp::Duration &period) override;

        //-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~

        //-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~
        //-=+~-=+~-=+~-=
        //-=+~-=+~-=+~-=   overrides from LifecycleNodeInterface
        //-=+~-=+~-=+~-=
        //-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~

        //-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~
        virtual CallbackReturn on_configure(const rclcpp_lifecycle::State &previous_state) override;

        //-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~
        virtual CallbackReturn on_cleanup(const rclcpp_lifecycle::State &previous_state) override;

        //-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~
        // virtual CallbackReturn 	on_shutdown (const rclcpp_lifecycle::State &previous_state) override { return CallbackReturn::SUCCESS; }

        //-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~
        virtual CallbackReturn on_activate(const rclcpp_lifecycle::State &previous_state) override;

        //-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~
        virtual CallbackReturn on_deactivate(const rclcpp_lifecycle::State &previous_state) override;

        //-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~
        // virtual CallbackReturn 	on_error (const rclcpp_lifecycle::State &previous_state) override { return CallbackReturn::SUCCESS; }



    //-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~-=+~
  private:
    CallbackReturn get_hardware_parameters(const hardware_interface::HardwareInfo &hardware_info);

    hardware_interface::StateInterface::ConstSharedPtr pVoltage_;
    hardware_interface::StateInterface::ConstSharedPtr pCurrent_;
    hardware_interface::StateInterface::ConstSharedPtr pTemperature_;
    hardware_interface::StateInterface::ConstSharedPtr pPercentage;
    hardware_interface::StateInterface::ConstSharedPtr pStatus_;
    hardware_interface::StateInterface::ConstSharedPtr pHealth_;
    hardware_interface::StateInterface::ConstSharedPtr pTechnology_;


    };       // end of class Bq25820HardwareInterface

}       // end of namespace bq25820_hardware_interface

#endif       // BQ25820_HARDWARE_INTERFACE__BQ25820_HARDWARE_INTERFACE_HPP_
