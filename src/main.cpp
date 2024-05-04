#include "main.h"
#include "pros/serial.hpp"
#include <iostream>

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
  std::cout << "**********" << std::endl;
  std::cout << "*   SEND   *" << std::endl;
  std::cout << "**********" << std::endl;
}

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

  const int BUFFER_SIZE = 256; // Increased buffer size for longer IDs
  uint8_t buffer[BUFFER_SIZE];
  int dataIndex = 0;

  while (true) {
    // Define IDs
    const char *ids[] = {"lY", "lX", "rX"};
    // Read input variables
    int analogValues[] = {controller.get_analog(ANALOG_LEFT_Y),
                          controller.get_analog(ANALOG_LEFT_X),
                          controller.get_analog(ANALOG_RIGHT_X)};

    // Store data in the buffer
    for (int i = 0; i < 3; ++i) {
      for (const char *p = ids[i]; *p; ++p) { // Store the ID
        buffer[dataIndex++] = static_cast<uint8_t>(*p);
      }
      buffer[dataIndex++] = '\0'; // Null-terminate the string
      buffer[dataIndex++] =
          static_cast<uint8_t>(analogValues[i]); // Store the value
    }

    // Write the buffer to sendSerial
    sendSerial.write(buffer, dataIndex);

    // Display info (optional)
    std::cout << "Sent data:\n";
    int i = 0;
    while (i < dataIndex) {
      int j = i;
      while (buffer[j] != '\0') { // Read until null byte
        std::cout << static_cast<char>(buffer[j++]);
      }
      std::cout << ": " << static_cast<int>(buffer[j + 1]) << "\n";
      i = j + 2; // Move past the null byte and the data byte
    }

    dataIndex = 0;   // Reset the dataIndex for the next iteration
    pros::delay(10); // Delay to prevent too rapid sending
  }
}

/**
RECEIVE CODE
void opcontrol() {
    pros::Serial receiveSerial(2, 57600);
    pros::Motor motor(20, false);

    int leftY = 0, leftX = 0;
    bool leftY_ready = false, leftX_ready = false;

    const int BUFFER_SIZE = 256;  // Buffer size to handle incoming data
    char buffer[BUFFER_SIZE];
    int bufferIndex = 0;

    while (true) {
        while (receiveSerial.get_read_avail() == 0) {
            pros::delay(1);
        };

        while (receiveSerial.get_read_avail() > 0 && bufferIndex < BUFFER_SIZE - 1) {
            buffer[bufferIndex++] = receiveSerial.read_byte();
            buffer[bufferIndex] = '\0';  // Always null-terminate

            // Check for complete data (ID + value)
            if (bufferIndex > 2 && buffer[bufferIndex - 1] >= 128) {
                char id[3] = { buffer[bufferIndex - 3], buffer[bufferIndex - 2], '\0' };
                int value = static_cast<int>(buffer[bufferIndex - 1]);

                std::cout << value << std::endl;

                //if (value > 127) value -= 256;  // Handle signed byte conversion

                if (strcmp(id, "lY") == 0) {
                    leftY = value;
                    leftY_ready = true;
                } else if (strcmp(id, "lX") == 0) {
                    leftX = value;
                    leftX_ready = true;
                }

                // Process when both are ready
                if (leftY_ready && leftX_ready) {
                    // motor.move(leftX);  // Example action using leftX
                    leftY_ready = leftX_ready = false;  // Reset flags
                }

                bufferIndex = 0;  // Reset buffer for the next ID-value pair
            }
        }
    }
}
*/