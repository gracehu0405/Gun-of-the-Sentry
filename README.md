# Title

## Team
Michael Oduoza, Grace Hu, Steffi Andersen

## Overview
The goal of a project was to develop an **automated nerf gun** that can **track a target** and **capture video.**   The gun can be operated using two modes: “auto” and “interactive.”

Auto mode uses the sensors to track the target, while interactive mode allows the user to control the gun with a keyboard.

A Nerf Dart Zone Titan was used due to its ability to automatically reload darts, and ultrasonic sensors were used to track motion.  An arducam was used to capture the video of the target. 

Below, each component is described in detail:

## Hardware
The nerf gun is mounted on a rotating platform.  3D printed arms hold the gun, and adhesives anchor it in place.  A base made of foam poster board is rotated by a servo motor, powered by 5V.  One raspberry pi is used to control the motor, keyboard, sensors, and arducam.  

--Insert photos--

## Sensors
3 Ultrasonic Module HC-SR04 Sensors measure the distance of objects in front of them.  This value is converted to inches and the shortest distance is calculated.  If there is an object within the threshold of 150 inches, the nerf gun fires. 

- Sensormap.h lays out the GPIO pins associated with each sensor.  Each standard HC-SR04 ultrasonic sensor has one trigger pin and one echo pin. We create two enrties for the echo pin, to be able to independently detect both rising and falling edges on the pin.

- Ultrasound.h initializes each sensor, determines the distance measured by each sensor, the sensor with the closest distance, and the middle sensor on the nerf gun apparatus.

## Arducam

## Firing the Gun

To generate the trigger mechanism, we opened up the nerf gun and observed the hardware.  By wiring the electronic firing system to the raspberry pi, we were able to control the firing of the gun by setting the GPIO pin to high.

- Gun.h includes the functions necessary for rotating and firing the nerf gun.  It rotates the gun until the middle sensor is reading the smallest distance, meaning it is facing the closest target, and then fires one dart.

## Breakdown
Michael: Sensors, interrupts, mode switching, sensor map, trigger mechanism

Grace: Arducam, graphics, building base, 3D printing, calibrating shooting, gun stability

Steffi: Building base, calibrating shooting, servo motor, trigger mechanism, 

## References

--add sources--
