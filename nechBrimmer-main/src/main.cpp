/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       nikkasouza                                                */
/*    Created:      10/10/2024, 8:15:07 AM                                    */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "vex.h"
#include "Button.h"

using namespace vex;

// A global instance of competition
competition Competition;
brain Brain;

ButtonManager buttonManager;

// define your global instances of motors and other devices here
motor leftMotor = motor(PORT19, ratio36_1, false);

motor rightMotor = motor(PORT11, ratio36_1, true);

controller Controller1 = controller(primary);

inertial inertialSensor = inertial(PORT12);

smartdrive Drivetrain = smartdrive(leftMotor, rightMotor, inertialSensor, 3.25, 9.5, 16, inches, 1);

motor intakeMotor = motor(PORT18, ratio6_1, false);
motor converyorMotor = motor(PORT20, ratio18_1, true);

int converyorSpeed = 100;
bool intakeOn = false;
bool converyorOn = false;

bool leftSide = false;

void auton1Selected() {
  Brain.Screen.clearScreen();
  Brain.Screen.printAt(1, 20, "Left side");

  leftSide = true;

  return;
}

void auton2Selected() {
  Brain.Screen.clearScreen();
  Brain.Screen.printAt(1, 20, "Right side");

  leftSide = false;

  return;
}

void pre_auton(void) {
  leftMotor.setBrake(hold);

  rightMotor.setBrake(hold);

  Drivetrain.setDriveVelocity(100, percent);
  Drivetrain.setTurnVelocity(100, percent);

  inertialSensor.calibrate();

  Button button1(50, 50, 150, 50, color::blue, "Left", auton1Selected);
  Button button2(50, 150, 150, 50, color::red, "Right", auton2Selected);

  buttonManager.addButton(button1);
  buttonManager.addButton(button2);

  buttonManager.drawAllButtons();

  while (true) {
    if (Brain.Screen.pressing()) {
      int touchX = Brain.Screen.xPosition();
      int touchY = Brain.Screen.yPosition();

      buttonManager.handleTouch(touchX, touchY);
    }

    wait(20, msec);
  }

  return;
}

void autonomous(void) {
  Drivetrain.setDriveVelocity(100, percent);
  converyorMotor.setMaxTorque(100, percent);
  converyorMotor.setVelocity(100, percent);

  converyorMotor.spin(forward);
  wait(3, sec);
  converyorMotor.stop();


  intakeMotor.spin(forward, 100, percent);
  wait(1000, msec);
  intakeMotor.stop();

  if (leftSide) {
    Drivetrain.turnFor(90, degrees);
    
    Drivetrain.driveFor(12 * 3, inches, true);
    Drivetrain.turnFor(-90, degrees);
    Drivetrain.setDriveVelocity(-100, percent);

    Drivetrain.driveFor(12, inches, true);
  } else {
    Drivetrain.turnFor(-90, degrees);

    Drivetrain.driveFor(12 * 3, inches, true);
    Drivetrain.turnFor(90, degrees);
    Drivetrain.setDriveVelocity(-100, percent);

    Drivetrain.driveFor(12, inches, true);
  }

  converyorMotor.spin(forward, 100, percent);

  return;
}

void usercontrol(void) {
  // User control code here, inside the loop
  leftMotor.setBrake(hold);

  rightMotor.setBrake(hold);
  
  leftMotor.setMaxTorque(100, percent);
  rightMotor.setMaxTorque(100, percent);

  leftMotor.setVelocity(100, percent);
  rightMotor.setVelocity(100, percent);

  Drivetrain.setDriveVelocity(100, percent);
  Drivetrain.setTurnVelocity(100, percent);

  Controller1.ButtonUp.pressed([]() {
    if (converyorSpeed < 100) {
      converyorSpeed += 5;
    }
  });

  Controller1.ButtonDown.pressed([]() {
    if (converyorSpeed > 0) {
      converyorSpeed -= 5;
    }
  }); 

  // Spin the intake motor when ButtonA is pressed until it is pressed again
  Controller1.ButtonA.pressed([]() {
    if (intakeOn) {
      intakeMotor.stop();
    } else {
      intakeMotor.spin(forward, 100, percent);
    }
    intakeOn = !intakeOn;
  });

  Controller1.ButtonB.pressed([]() {
    if (converyorOn) {
      converyorMotor.stop();
    } else {
      converyorMotor.spin(forward, converyorSpeed, percent);
    }
    converyorOn = !converyorOn;
  });

  while (1) {
    leftMotor.spin(forward, Controller1.Axis3.position(), percent);
    rightMotor.spin(forward, Controller1.Axis2.position(), percent);

    if (Controller1.ButtonR1.pressing()) {
      Drivetrain.drive(forward);
    } else if (Controller1.ButtonL1.pressing()) {
      Drivetrain.drive(reverse);
    } else if (Controller1.Axis3.position() == 0 && Controller1.Axis2.position() == 0) {
      Drivetrain.stop();
    }

    if (Controller1.ButtonR2.pressing() || Controller1.ButtonL2.pressing()) {
      converyorMotor.stop();
      converyorOn = false;
    }

    if (Controller1.ButtonR2.pressing()) {
      converyorMotor.spin(forward, converyorSpeed, percent);
    } else if (Controller1.ButtonL2.pressing()) {
      converyorMotor.spin(reverse, converyorSpeed, percent);
    } else {
      converyorMotor.stop();
    }

    wait(20, msec);
  }
}


int main() {
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  // Run the pre-autonomous function.
  pre_auton();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}
