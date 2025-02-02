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
motor leftMotor = motor(PORT6, ratio18_1, false);

motor rightMotor = motor(PORT9, ratio18_1, true);

controller Controller1 = controller(primary);

// inertial inertialSensor = inertial(PORT12);

drivetrain Drivetrain = drivetrain(leftMotor, rightMotor, 25.13, 43.5, 23, distanceUnits::cm, 1);

motor intakeMotor = motor(PORT7, ratio6_1, true);
motor converyorMotor = motor(PORT10, ratio18_1, true);

pneumatics portA = pneumatics(Brain.ThreeWirePort.G);
pneumatics portB = pneumatics(Brain.ThreeWirePort.H);

int converyorSpeed = 80;
bool intakeOn = false;
bool converyorOn = false;
bool extendClamp = false;

bool leftSide = false;
bool blueTeam = false;

void auton1Selected() {
  Brain.Screen.setFillColor(color::orange);
  Brain.Screen.printAt(5, 20, "Left side");
  leftSide = true;
  return;
}

void auton2Selected() {
  Brain.Screen.setFillColor(color::green);
  Brain.Screen.printAt(5, 20, "Right side");
  leftSide = false;
  return;
}

void blueTeamSelected() {
  Brain.Screen.setFillColor(color::blue);
  Brain.Screen.printAt(5, 40, "Blue Team");
  blueTeam = true;
  return;
}

void redTeamSelected() {
  Brain.Screen.setFillColor(color::red);
  Brain.Screen.printAt(5, 40, "Red Team");
  blueTeam = false;
  return;
}

void pre_auton(void) {
  intakeMotor.setMaxTorque(100, percent);

  Button button1(50, 50, 150, 50, color::orange, "Left", auton1Selected);
  Button button2(50, 150, 150, 50, color::green, "Right", auton2Selected);
  Button button3(250, 50, 150, 50, color::blue, "Blue Team", blueTeamSelected);
  Button button4(250, 150, 150, 50, color::red, "Red Team", redTeamSelected);

  buttonManager.addButton(button1);
  buttonManager.addButton(button2);
  buttonManager.addButton(button3);
  buttonManager.addButton(button4);

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

void clamp() {
  if (extendClamp) {
    portA.open();
    portB.close();
  } else {
    portA.close();
    portB.open();
  }
  extendClamp = !extendClamp;
}

void autonomous(void) {
  Drivetrain.setDriveVelocity(100, percent);
  Drivetrain.setTurnVelocity(100, percent);

  // converyorMotor.setMaxTorque(100, percent);
  converyorMotor.setVelocity(85, percent);

  Drivetrain.driveFor(reverse, 850, mm, true);
  Drivetrain.turnFor(5, degrees, true);
  Drivetrain.turnFor(-10, degrees, true);
  Drivetrain.turnFor(5, degrees, true);
  Drivetrain.driveFor(reverse, 100, mm, true);

  converyorMotor.spin(reverse);
  vex::wait(1.5, sec);
  converyorMotor.stop();

  Drivetrain.driveFor(5, mm, true);

  vex::wait(100, msec);
  portA.open();
  portB.close();

  intakeMotor.spin(forward, 100, percent);

  int turnAngle = 80 * (leftSide ? 1 : -1); // negative if left side, simplifies code imo

  Drivetrain.turnFor(turnAngle, degrees, true);

  Drivetrain.driveFor(600, mm, true);

  vex::wait(100, msec);
  portB.open();
  portA.close();

  converyorMotor.spin(reverse);
  vex::wait(2, sec);
  converyorMotor.stop();
  intakeMotor.stop();

  return;
}

void usercontrol(void) {
  // User control code here, inside the loop
  leftMotor.setBrake(hold);
  rightMotor.setBrake(hold);
  intakeMotor.setMaxTorque(100, percent);

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

  Controller1.ButtonA.pressed([]() {
    if (intakeOn) {
      intakeMotor.stop();
    } else {
      intakeMotor.spin(forward, 100, percent);
    }
    intakeOn = !intakeOn;
  });

  Controller1.ButtonX.pressed([]() {
    clamp();
  });

  while (1) {
    leftMotor.spin(forward, Controller1.Axis3.position(), percent);
    rightMotor.spin(forward, Controller1.Axis2.position(), percent);

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
