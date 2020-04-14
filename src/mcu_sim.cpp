#include <chrono>
#include <string>
#include <memory>
#include <filesystem>

#include "sensor_msgs/msg/temperature.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp/clock.hpp"
#include "sensor_msgs/msg/fluid_pressure.hpp"

using namespace std::chrono_literals;

class McuSimPublisher : public rclcpp::Node
{
public:
  McuSimPublisher()
  : Node("mcu")
  {
    clock = std::make_shared<rclcpp::Clock>(RCL_ROS_TIME);
    const auto qos_history_depth = 10;
    publisher_ambient_temperature_ =
      this->create_publisher<sensor_msgs::msg::Temperature>(
      std::string(
        get_name()) + "/sensor/ambient_temperature", qos_history_depth);
    publisher_internal_temperature_ =
      this->create_publisher<sensor_msgs::msg::Temperature>(
      std::string(
        get_name()) + "/sensor/internal_temperature", qos_history_depth);
    publisher_pressure_ =
      this->create_publisher<sensor_msgs::msg::FluidPressure>(
      std::string(
        get_name()) + "/sensor/pressure", qos_history_depth);
  }

  void set_internal_c(float c)
  {
    internal_temperature_.header.stamp = clock->now();
    internal_temperature_.temperature = c;
  }

  void set_ambient_c(float c)
  {
    ambient_temperature_.header.stamp = clock->now();
    ambient_temperature_.temperature = c;
  }

  void set_pressure_pascal(float p)
  {
    pressure_.header.stamp = clock->now();
    pressure_.fluid_pressure = p;
  }

  void publish_values()
  {
    publisher_ambient_temperature_->publish(ambient_temperature_);
    publisher_internal_temperature_->publish(internal_temperature_);
    publisher_pressure_->publish(pressure_);
  }

private:
  rclcpp::Clock::SharedPtr clock;

  sensor_msgs::msg::Temperature internal_temperature_;
  sensor_msgs::msg::Temperature ambient_temperature_;
  sensor_msgs::msg::FluidPressure pressure_;

  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<sensor_msgs::msg::Temperature>::SharedPtr publisher_ambient_temperature_;
  rclcpp::Publisher<sensor_msgs::msg::Temperature>::SharedPtr publisher_internal_temperature_;
  rclcpp::Publisher<sensor_msgs::msg::FluidPressure>::SharedPtr publisher_pressure_;
};

std::shared_ptr<McuSimPublisher> publisher;

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);

  publisher = std::make_shared<McuSimPublisher>();

  auto appname = std::filesystem::path(argv[0]).filename().string();

  std::cout << appname << " starting" << std::endl;
  rclcpp::spin(publisher);

  rclcpp::shutdown();
  std::cout << appname << " shutting down" << std::endl;
  return 0;
}
