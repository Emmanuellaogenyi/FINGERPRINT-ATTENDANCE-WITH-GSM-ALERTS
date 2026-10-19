# Smart Feedback Monitoring System: Fingerprint Attendance with GSM Alerts

[![IoT](https://img.shields.io/badge/Platform-ESP8266%20%7C%20Arduino-blue.svg)](#)
[![Android](https://img.shields.io/badge/Mobile-Android%20App-green.svg)](#)
[![Database](https://img.shields.io/badge/Database-MySQL-orange.svg)](#)
[![License](https://img.shields.io/badge/License-MIT-lightgrey.svg)](#)

An IoT-enabled biometric attendance monitoring system integrating fingerprint verification, GSM real-time SMS dispatch, and a custom Android management application connected to an SQL database.

---

## 📌 Overview

Traditional manual attendance workflows are vulnerable to proxy attendance, record errors, and slow aggregation. This project provides an automated end-to-end IoT platform featuring:

- **Dual-Mode Attendance Tracking:** Distinct logging routines for institutional arrival check-in and course/lecture attendance.
- **Biometric Identification:** Rapid local fingerprint matching via an optical sensor.
- **Real-Time Notification Pipeline:** Automated parent/guardian SMS dispatch via GSM module immediately upon authentication.
- **Wireless Central Synchronization:** Direct database transmission over Wi-Fi via ESP8266 and the `MySQL Connector/Arduino` driver.
- **Android Administrative Client:** Complete mobile interface for student enrollment, session management, and live log auditing.

---

## 🏗 System Architecture

```text
+-----------------------+       UART        +--------------------------+
|  R305 Optical Sensor  | ----------------> |                          |
+-----------------------+                   |                          |
                                            |                          |
+-----------------------+       I2C         |  NodeMCU ESP8266         |
|  DS1307 RTC Module    | ----------------> |  (Microcontroller / Wi-Fi|
+-----------------------+                   |                          |
                                            |                          |
+-----------------------+       UART        |                          |
|  SIM800L GSM Module   | <---------------- |                          |
+-----------------------+                   +--------------------------+
           | (SMS)                                       | (Wi-Fi / TCP)
           v                                             v
     Parent/Guardian                             Remote SQL Database
      Mobile Device                                      ^
                                                         | (REST / Sync)
                                                         v
                                                Android Management App
