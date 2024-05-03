#include "main.h"
#include "pros/link.h"
#include "pros/serial.hpp"
#include <bitset>
#include <iostream>

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
  static bool pressed = false;
  pressed = !pressed;
  if (pressed) {
    pros::lcd::set_text(2, "I was pressed!");
  } else {
    pros::lcd::clear_line(2);
  }
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}

/**
SEND CODE
 */
void opcontrol() {
  pros::Controller controller(pros::E_CONTROLLER_MASTER);
  pros::Serial sendSerial(1, 57600);

  while (true) {
    int leftY = controller.get_analog(ANALOG_LEFT_Y);
    // Convert leftY to a buffer
    uint8_t buffer = static_cast<uint8_t>(leftY);

    // Write the buffer to sendSerial
    sendSerial.write(&buffer, 1);

    // Display info
    std::cout << "Sent: " << std::bitset<8>(buffer) << std::endl;
    pros::delay(1);
  }
}

/** RECEIVE CODE
void opcontrol() {
  pros::Serial receiveSerial(2, 57600);
  pros::Motor motor(20, false);

  while (true) {
    while (receiveSerial.get_read_avail() == 0) {
      int delay = 1;
      pros::delay(delay);
    };

    std::bitset<8> readData(receiveSerial.read_byte());
    std::string readDataString = readData.to_string();
    int receivedNumber = std::stoi(readDataString, nullptr, 2);
    if (receivedNumber > 127) {
      receivedNumber -= 256;
    }
    std::cout << "Received Data: " << readDataString << "\n";
    std::cout << "Received Number: " << receivedNumber << "\n";

    motor = receivedNumber;

    pros::screen::print(TEXT_MEDIUM, 0, "Received: %d", readDataString);
    pros::screen::print(TEXT_MEDIUM, 1, "leftY: %d", receivedNumber);
  }
}
*/