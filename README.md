# Arduino Examples

[![Static check](https://github.com/jo3-tech/jm_arduino_examples/actions/workflows/static-check.yaml/badge.svg)](https://github.com/jo3-tech/jm_arduino_examples/actions/workflows/static-check.yaml) [![Build examples](https://github.com/jo3-tech/jm_arduino_examples/actions/workflows/build-examples.yaml/badge.svg)](https://github.com/jo3-tech/jm_arduino_examples/actions/workflows/build-examples.yaml)

Useful examples and algorithm implementations for the Arduino platform.

This repository is a collection of examples, usually written during feasibility studies for the implementation of a library or project.

## accelerate-stepper-motor

The following examples were created during the development of the [MT-arduino-stepper-driver](https://github.com/Morgritech/MT-arduino-stepper-driver) library. All 3 algorithms were implemented in the library as user-selectable options. A simple [wiring diagram](documentation/accelerate-stepper-motor-wiring-diagram.png), and a [schematic](documentation/accelerate-stepper-motor-schematic.png) showing the setup for testing the algorithms can be found in the "documentation" folder.

### accelerate-stepper-motor-austin-05-algorithm

An [example](accelerate-stepper-motor-austin-05-algorithm/accelerate-stepper-motor-austin-05-algorithm.ino) to illustrate how to accelerate a stepper motor using the algorithm presented in: Austin, D. (2005, January). Generate stepper-motor speed profiles in real time. Embedded Systems Programming.

### accelerate-stepper-motor-eiderman-04-algorithm

An [example](accelerate-stepper-motor-eiderman-04-algorithm/accelerate-stepper-motor-eiderman-04-algorithm.ino) to illustrate how to accelerate a stepper motor using the algorithm presented in: Eiderman, A. (2004). Real Time Stepper Motor Linear Ramping Just by Addition and Multiplication. Retrieved from [HWML](http://hwml.com/LeibRamp.htm).

### accelerate-stepper-motor-morgridge-24-algorithm

An [example](accelerate-stepper-motor-morgridge-24-algorithm/accelerate-stepper-motor-morgridge-24-algorithm.ino) to illustrate how to accelerate a stepper motor using an algorithm developed based on the equations; "v = u + at" and "s = ut + 1/2at<sup>2</sup>" which are simplified based on the conditions under which the algorithm is implemented on a microcontroller. The algorithm is described in the report bundled with the [MT-arduino-stepper-driver](https://github.com/Morgritech/MT-arduino-stepper-driver) library.
