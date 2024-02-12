##  Project Description: Cistern Water Level Monitoring with Ultrasonic Sensor and ESP12

The primary objective of this project is to monitor the water level in a cistern using an ultrasonic sensor paired with an ESP12 microcontroller.

### Challenges and Solutions:

- **Power Constraints:**
There is no power source avaliable, batteries had to be used. To minimize power usage, I used the ESP12's deep sleep feature and turned off power to non-essential sensors with a small circuit using Transistors and Mosfets (I know, a bit overkill). Currently, the system operates efficiently with two 3400mAh Panasonic 18650 Li-Ion batteries, lasting over half a year with measurements sent every 30 minutes over WiFi.

- **Firmware Updates:**
Utilizing the barebone ESP12 to reduce power consumption during deep sleep presented challenges in firmware upgrades. Specific GPIOs needed to be bridged, and a serial adapter was required for firmware updates. Over-The-Air updates are the solution but it was not straightforward to implement, because the device is only connected to the internet for a short period of time. To address this, before sending measurements, the device compares its current firmware version with a version hosted on a designated web server. If a newer version is detected, the device proceeds to download the new firmware and initiates a reboot. I simplified firmware updates with a bash script.
