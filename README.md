# Title

## Team
Michael Oduoza, Grace Hu, Steffi Andersen

## Overview
The goal of a project was to develop an **automated nerf gun** that can **track a target** and **capture all the action on a screen.**   The gun can be operated using two modes: “auto” and “interactive.”

Auto mode uses ultrasound sensors to track a moving target and will automatically fire.
Interactive mode allows the user to control the gun with a keyboard. For this mode, press and hold 'a' to rotate the gun counterclockwise, 'd' for clockwise rotation, and holding the 'Enter' key down for at least a second will trigger firing of the gun.

A Nerf Dart Zone Titan was used due to its ability to automatically reload darts, and ultrasonic sensors were used to track motion. An arducam was used to capture the pictures of the target, which is fed to a monitor and has graphics overlaid to display information about the gun's status. 

Below, each component is described in detail:

## Hardware
Essentially, the nerf gun is mounted on a rotating platform. Four acrylic arms were laser cut out to hold and stabilize the gun, and adhesives anchor it in place. A base made of foam poster board is rotated by a servo motor, powered by 5V. A Raspberry Pi Model A+ (V1.1, 2014) is used to control the motor, keyboard, sensors, arducam, and graphics.  

--Insert photos--

## Sensors
For automatic mode, 3 Ultrasonic Module HC-SR04 Sensors measure the distance of objects in front of them. This value is converted to inches and the shortest distance is calculated. If there is an object within the threshold of 150 inches, the nerf gun fires. 

- Sensormap.h lays out the GPIO pins associated with each sensor. Each standard HC-SR04 ultrasonic sensor has one trigger pin and one echo pin. We create two entries for the echo pin to be able to independently detect both rising and falling edges on the pin.

- Ultrasound.h initializes each sensor, determines the distance measured by each sensor, the sensor with the closest distance, and the middle sensor on the nerf gun apparatus.

## Arducam



## Firing the Gun

To generate the trigger mechanism, we opened up the nerf gun and observed the hardware. By wiring the electronic firing system to the raspberry pi, we were able to control the firing of the gun by setting the GPIO pin to high.

- Gun.h includes the functions necessary for rotating and firing the nerf gunautomatically.  It rotates the gun until the middle sensor is reading the smallest distance, meaning it is facing the closest target, and then fires one dart.

## Breakdown
Michael: Sensors, interrupts, mode switching, sensor map, trigger mechanism

Grace: Arducam, graphics, building the base and mechanics, laser cutting, calibrating shooting, gun stability

Steffi: Building the base and mechanics, calibrating shooting, servo motor, mechanics, trigger mechanism

## References

--add sources--