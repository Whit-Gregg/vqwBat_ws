#ifndef BQ25820_NODE__BQ25820_NODE_HPP_
#define BQ25820_NODE__BQ25820_NODE_HPP_

// #include "bq25820_node/visibility_control.h"
//#include <sensor_msgs/msg/BatteryState.hpp>
#include <rclcpp/rclcpp.hpp>
#include <string>

namespace bq25820_node
{

    class Bq25820Node
    {
      public:
        Bq25820Node();

        virtual ~Bq25820Node();
    };

}       // namespace bq25820_node

#endif       // BQ25820_NODE__BQ25820_NODE_HPP_
