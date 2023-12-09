# solderfume

[![PlatformIO CI](https:actions/workflows/main.yml/badge.svg)](https:actions/workflows/main.yml)

This is the start of a project to build a smart solder fume extractor.

It includes code to drive a PWM fan and monitor its RPM. It uses a
particle sensor to decide when to spin up the fan.

For fun this code was built with [Homespan](https://github.com/HomeSpan/HomeSpan) to allow it to talk to
Apple's HomeKit. This is clearly not necessary but would allow the
user to tell Siri to turn the fan on.

The project is built on the Arduino framework for the ESP32 using
[PlatformIO](https://platformio.org) as its build system.

Includes an initial attempt at a case for the fan and filter using [OpenSCAD](https://openscad.org).

## Pairing

Homespan's default pairing code is 466-37-726
