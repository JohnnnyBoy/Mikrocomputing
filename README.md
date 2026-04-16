# Mikrocomputing
**ARM Cortex-M4 Embedded Systems Project: From GPIO to Automotive Bus Systems**

Project Mikrocomputing with ARM Cortex-M4 µC to the lecture "Mikrocomputertechnik" on the THI the Technical University of Applied Science of Inolstadt. 

## 📌 Introduction
It was used the ARM Cortex-M4 µC to get into contact with microcomputing with C++, Registers, Interrupts, Peripherals

## 💻 Project Description
This project features a comprehensive implementation of low-level software solutions developed for the ARM Cortex-M4 (STM32F407xx) microcontroller. Created as part of the "Microcomputer Technology" lab at the Technical University of Applied Sciences Ingolstadt (THI), this repository demonstrates the systematic control of complex peripherals and the resolution of electrical engineering challenges using the C programming language.

The primary focus was on register-based programming and high-precision timing, both of which are critical requirements in modern automotive and automation technology. Rather than relying on high-level abstractions, the hardware was initialized "from scratch" to gain a profound understanding of interrupt handling, bus protocols, and signal processing.

## 🛠️ Key Engineering Competencies
**Peripheral Expertise**
Full configuration and management of Hardware Timers (PWM, Input Capture), ADC (Sensor Data Acquisition), and USART.

**HMI & Visualization**
Development of custom drivers for LCD display controllers and keypad matrices utilizing the parallel FSMC (Flexible Static Memory Controller) interface.

**Automotive Communication** 
Implementation of a robust LIN Bus communication protocol to facilitate networking between decentralized electronic control units (ECUs).

**Measurement & Testing**
Verification of software logic through hardware-side analysis using Oscilloscopes and advanced In-Circuit Debugging techniques.


## Explanation Repositories
Project Structure / Modules

**Part 1-2:** Basic I/O & Debugging – Implementation of LED blinking patterns and tactile switch polling.

**Part 3-4:** HMI Interfaces – Custom LCD driver development and keypad matrix integration via parallel interfaces.

**Part 5-6:** Real-Time & Sensors – Interrupt-driven time bases and high-precision temperature sensing.

**Part 7-9:** Communication & Actuators – LIN Bus implementation, ADC data acquisition, and PWM-based throttle valve control.

## 
<img width="650" height="400" alt="ARM_Cortex-M4" src="https://github.com/user-attachments/assets/eeecd3ad-ea99-4f89-95c1-a96deb6a78fa" />


## 📚 References

The development of this project was based on the following official documentation and materials. For copyright reasons, documents from the Technical University of Ingolstadt, STMicroelectronics, and ARM Architecture are not included in this repository.

| ID | Documentdetails |
|:---|:---|
| [1] | **STMicroelectronics**, "UM1472 User manual: Discovery kit for STM32F407/417 lines," *DocID022256 Rev 4*, Jan. 2014. [Online verfügbar](https://www.st.com) |
| [2] | **STMicroelectronics**, "AN2825 Application Note: S-Touch® STMPE811 resistive touchscreen controller advanced features," *Doc ID 15023 Rev 1*, 2011. [Online verfügbar](https://www.st.com) |
| [3] | **ARM Limited**, "Cortex-M4 Revision r0p0 Technical Reference Manual," *ARM DDI 0439B (ID030210)*, 2010. [Online verfügbar](https://www.arm.com) |
| [4] | **STMicroelectronics**, "RM0090 Reference manual: STM32F405xx/07xx, STM32F415xx/17xx, STM32F42xxx and STM32F43xxx advanced ARM-based 32-bit MCUs," *Doc ID 018909 Rev 6*, Feb. 2014. [Online verfügbar](https://www.st.com) |
| [5] | **STMicroelectronics**, "STM32F405xx STM32F407xx, ARM Cortex-M4 32b MCU+FPU, Datasheet - production data," *DocID022152 Rev 4*, Jun. 2013. [Online verfügbar](https://www.st.com) |
| [6] | **ARM Limited**, "Cortex™-M4 Devices Generic User Guide," *ARM DUI 0553A (ID121610)*, 2010. [Online verfügbar](https://www.arm.com) |
| [7] | **Solomon Systech Limited**, "SSD2119 Advance Information: 320 RGB x 240 TFT LCD Driver Integrated Power Circuit," *SSD2119 Rev 1.4*, Jun. 2009. [Online verfügbar](http://www.solomon-systech.com) |
| [8] | **STMicroelectronics**, "PM0214 Programming manual: STM32F3 and STM32F4 Series Cortex®-M4 programming manual," *DocID022708 Rev 4*, Mai 2014. [Online verfügbar](https://www.st.com) |
| [9] | **Embest Technology Co., LTD**, "STM32F4DIS-BB - Rev History & Block Diagram," *Dokumentnr.: STM32F4DIS-BB-001-SH*, Rev V1.0, 24. Okt. 2012. |
| [10] | **Embest Technology Co., LTD**, "STM32F4DIS-LCD - Rev History & Block Diagram," *Dokumentnr.: STM32F4DIS-CAM-001-SH*, Rev V1.0, 24. Okt. 2012. |
| [11] | **STMicroelectronics**, "STMPE811, Advanced resistive touch screen controller with 8-bit GPIO expander," *Doc ID 14489 Rev 2*, Apr. 2009. [Online verfügbar](https://www.st.com) |
| [12] | **Prof. Dr.-Ing. Lorenz Gaul**, "Praktikum Mikrocomputertechnik mit ARM, Cortex-M4 µC, Aufgaben für den Studiengang EIT," *V6.0.0*, TH Ingolstadt, 14. Mär. 2025. |
| [13] | **L. Gaul, U. Margull, G. Passig**, "Praktikum und Übung, MCT mit ARM-Cortex Mikrocontrollern, Installation IDE und Projekterstellung," *V2.6.0*, TH Ingolstadt, 14. Mär. 2025. |
| [14] | **L. Gaul, U. Margull, G. Passig**, "Praktikum Mikrocomputertechnik mit einem ARM Cortex-M4 Controller, Dokumentation," *V4.1.1*, TH Ingolstadt, 14. Mär. 2023. |

---
