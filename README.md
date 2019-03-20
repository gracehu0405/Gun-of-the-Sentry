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

## Sensors
3 Ultrasonic Module HC-SR04 Sensors measure the distance of objects in front of them.  This value is converted to inches and the shortest distance is calculated.  If there is an object within the threshold of 150 inches, the nerf gun fires. 

## Arducam

## Breakdown
Michael: Sensors, interrupts, mode switching, sensor map, trigger mechanism

Grace: Arducam, graphics, building base, calibrating shooting, gun stability

Steffi: Building base, calibrating shooting, servo motor, trigger mechanism
