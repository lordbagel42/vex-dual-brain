#include "main.h"
#include "pros/apix.h"
#include "pros/serial.hpp"
#include <bitset>
#include <iostream>

lv_obj_t *myButton;
lv_obj_t *myButtonLabel;
lv_obj_t *myLabel;

lv_style_t myButtonStyleREL; // released style
lv_style_t myButtonStylePR;  // pressed style

void sendSerial() {
  pros::Controller controller(pros::E_CONTROLLER_MASTER);
  pros::Serial sendSerial(1, 57600);
  uint8_t buffer[8];

  while (true) {
    // Read controller values
    int leftY = controller.get_analog(ANALOG_LEFT_Y);
    int leftX = controller.get_analog(ANALOG_LEFT_X);
    int rightY = controller.get_analog(ANALOG_RIGHT_Y);
    int rightX = controller.get_analog(ANALOG_RIGHT_X);
    int randomNum = rand() % 65536;

    // Controller is 8 bit, pad start with zeros
    buffer[0] = 0; // High byte (unused)
    buffer[1] = static_cast<uint8_t>(leftY);
    buffer[2] = 0;
    buffer[3] = static_cast<uint8_t>(leftX);
    buffer[4] = 0;
    buffer[5] = static_cast<uint8_t>(rightY);
    buffer[6] = 0;
    buffer[7] = static_cast<uint8_t>(rightX);

    // Write the buffer to sendSerial
    sendSerial.write(buffer, sizeof(buffer));

    // Display stuff for debug
    std::cout << "Sent: ";
    for (int i = 0; i < 8; ++i) {
      std::cout << std::bitset<8>(buffer[i]) << " ";
    }
    std::cout << std::endl;

    // Screen stuff
    pros::screen::print(TEXT_MEDIUM, 0, "leftY: %d    ", leftY);
    pros::screen::print(TEXT_MEDIUM, 1, "leftX: %d    ", leftX);
    pros::screen::print(TEXT_MEDIUM, 2, "rightY: %d    ", rightY);
    pros::screen::print(TEXT_MEDIUM, 3, "rightX: %d    ", rightX);

    pros::delay(1); // Don't cook the CPU
  }
}

void receiveSerial() {
  // RECEIVE
  pros::Motor motor(20, false);
  uint8_t buffer[8];

  pros::Serial receiveSerial(1, 57600);

  motor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

  while (true) {
    // Ensure there's data available to read
    while (receiveSerial.get_read_avail() <
           8) { // Checking for 8 bytes (4 * 16 bits)
      pros::delay(1);
    }

    // Prepare buffer
    receiveSerial.read(buffer, 8); // Read 8 bytes in

    // Only use the second byte of each pair (low byte)
    int8_t leftY = static_cast<int8_t>(buffer[1]);
    int8_t leftX = static_cast<int8_t>(buffer[3]);
    int8_t rightY = static_cast<int8_t>(buffer[5]);
    int8_t rightX = static_cast<int8_t>(buffer[7]);

    // Display debug data
    std::cout << "Received Data:\n";
    std::cout << "leftY: " << static_cast<int>(leftY) << "\n";
    std::cout << "leftX: " << static_cast<int>(leftX) << "\n";
    std::cout << "rightY: " << static_cast<int>(rightY) << "\n";
    std::cout << "rightX: " << static_cast<int>(rightX) << "\n";

    motor.move(leftY);

    // Screen stuff
    pros::screen::print(TEXT_MEDIUM, 0, "leftY: %d    ", leftY);
    pros::screen::print(TEXT_MEDIUM, 1, "leftX: %d    ", leftX);
    pros::screen::print(TEXT_MEDIUM, 2, "rightY: %d    ", rightY);
    pros::screen::print(TEXT_MEDIUM, 3, "rightX: %d    ", rightX);

    // CPU cooking is bad
    pros::delay(1);
  }
}

static lv_res_t send_btn_click_action(lv_obj_t *btn) {
  // Action for SEND button
  // Add your code here
  lv_obj_clean(lv_scr_act()); // Clear the screen
  pros::Task sendSerialTask(sendSerial);
  return LV_RES_OK;
}

static lv_res_t receive_btn_click_action(lv_obj_t *btn) {
  // Action for RECEIVE button
  // Add your code here
  lv_obj_clean(lv_scr_act()); // Clear the screen
  pros::Task receiveSerialTask(receiveSerial);
  return LV_RES_OK;
}

void initialize() {
  lv_style_copy(&myButtonStyleREL, &lv_style_plain);
  myButtonStyleREL.body.main_color = LV_COLOR_MAKE(150, 0, 0);
  myButtonStyleREL.body.grad_color = LV_COLOR_MAKE(0, 0, 150);
  myButtonStyleREL.body.radius = 0;
  myButtonStyleREL.text.color = LV_COLOR_MAKE(255, 255, 255);

  lv_style_copy(&myButtonStylePR, &lv_style_plain);
  myButtonStylePR.body.main_color = LV_COLOR_MAKE(255, 0, 0);
  myButtonStylePR.body.grad_color = LV_COLOR_MAKE(0, 0, 255);
  myButtonStylePR.body.radius = 0;
  myButtonStylePR.text.color = LV_COLOR_MAKE(255, 255, 255);

  // Create SEND button
  lv_obj_t *sendButton = lv_btn_create(lv_scr_act(), NULL);
  lv_obj_set_free_num(sendButton, 0);
  lv_btn_set_action(sendButton, LV_BTN_ACTION_CLICK, send_btn_click_action);
  lv_btn_set_style(sendButton, LV_BTN_STYLE_REL, &myButtonStyleREL);
  lv_btn_set_style(sendButton, LV_BTN_STYLE_PR, &myButtonStylePR);
  lv_obj_set_size(sendButton, LV_HOR_RES / 2, LV_VER_RES);
  lv_obj_align(sendButton, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

  lv_obj_t *sendButtonLabel = lv_label_create(sendButton, NULL);
  lv_label_set_text(sendButtonLabel, "SEND");

  // Create RECEIVE button
  lv_obj_t *receiveButton = lv_btn_create(lv_scr_act(), NULL);
  lv_obj_set_free_num(receiveButton, 1);
  lv_btn_set_action(receiveButton, LV_BTN_ACTION_CLICK,
                    receive_btn_click_action);
  lv_btn_set_style(receiveButton, LV_BTN_STYLE_REL, &myButtonStyleREL);
  lv_btn_set_style(receiveButton, LV_BTN_STYLE_PR, &myButtonStylePR);
  lv_obj_set_size(receiveButton, LV_HOR_RES / 2, LV_VER_RES);
  lv_obj_align(receiveButton, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);

  lv_obj_t *receiveButtonLabel = lv_label_create(receiveButton, NULL);
  lv_label_set_text(receiveButtonLabel, "RECEIVE");

  // myLabel = lv_label_create(lv_scr_act(), NULL);
  // lv_label_set_text(myLabel, "Button has not been clicked yet");
  // lv_obj_align(myLabel, NULL, LV_ALIGN_IN_LEFT_MID, 10, 0);
}

void disabled() {}

void competition_initialize() {}

void autonomous() {}

void serialSelector() {}

void opcontrol() {
  while (true) {
    pros::delay(20);
  }
}