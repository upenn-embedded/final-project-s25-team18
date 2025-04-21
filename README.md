[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/WXeqVgks)
# final-project-skeleton

* Team Number: 18
* Team Name: Team 18
* Team Members: Jiwanshi Shah & Jat Tse
* GitHub Repository URL: https://github.com/upenn-embedded/final-project-s25-team18
* GitHub Pages Website URL: [for final submission]

## Final Project Proposal 

### 1. Abstract

*In a few sentences, describe your final project.*

- The Smart Snacks Vending Machine is an upgraded vending solution that accepts coins dispenses snacks via a motorized spiral mechanism, and provides inventory tracking and user feedback through an LCD and speech output. We aim to do so by leveraging serial communication (I2C/SPI), timers/interrupts, and power management. 

### 2. Motivation

*What is the problem that you are trying to solve? Why is this project interesting? What is the intended purpose?*

- Modern vending machines are being redesigned for greater convenience and modernization. Unlike traditional coin-operated machines that are still common, newer models now offer contactless payment options, making snack selection and communication more efficient. Accessibility and user feedback are also key features of this design, with speech output (text-to-speech) and LCD screens ensuring clear communication, especially for users with visual impairments. Additionally, inventory is tracked in real time, reducing downtime caused by stock-outs and enabling automated alerts for restocking, which enhances overall efficiency and service reliability.

### 3. System Block Diagram

*Show your high level design, as done in WS1 and WS2. What are the critical components in your system? How do they communicate (I2C?, interrupts, ADC, etc.)? What power regulation do you need?*

<img width="364" alt="image" src="https://github.com/user-attachments/assets/6b20c32a-2479-45ce-92be-4182c7cea9d5" />


  
### 4. Design Sketches

*What will your project look like? Do you have any critical design features? Will you need any special manufacturing techniques to achieve your vision, like power tools, laser cutting, or 3D printing?*

![diagram-front](https://github.com/user-attachments/assets/ab7c29f5-78b7-4a08-af13-406155cf00cd)

![diagram-back](https://github.com/user-attachments/assets/facf7f82-1471-4bdf-b812-2a1f5b2bf431)

We are currently planning to use cardboard to construct the vending machine as it greatly resembles a box, however we will look into the possibility of laser printing the box for a betetr experience. 

### 5. Software Requirements Specification (SRS)

*Formulate key software requirements here. Think deeply on the design: What must your device do? How will you measure this during validation testing? Create 4 to 8 critical system requirements.*



*These must be testable! See the Final Project Manual Appendix for details. Refer to the table below; replace these examples with your own.*

**5.1 Definitions, Abbreviations**

Here, you will define any special terms, acronyms, or abbreviations you plan to use for hardware

- MCU: Microcontroller Unit (ATmega328P or ESP32)

- ISR: Interrupt Service Routine

- RFID: Radio-Frequency Identification – used to lock/unlock the machine for authorized restocking

**5.2 Functionality**

| ID      | Description |
|---------|-------------|
| SRS-01  | **Coin Detection:** The microcontroller will detect coin insertion using an interrupt within 50 ms of the coin passing the sensor. Verified via debug logs and timing using a logic analyzer. |
| SRS-02  | **Coin Value Reading & Balance Update:** After detecting a coin, the system will identify its value and update the user’s balance in under 1 second. Feedback will be given via LCD or buzzer. |
| SRS-03  | **Motor Control for Dispensing:** Upon product selection, the motor will start/stop within ±100 ms to prevent double-dispensing. Timing is verified using an oscilloscope or logic analyzer. |
| SRS-04  | **Inventory Tracking:** An IR sensor will detect when a product is dropped. The internal inventory will be updated accordingly, and a flag will be toggled in the firmware to confirm the drop. |
| SRS-05  | **RFID for Restocking Access:** RFID will be used to unlock the machine for authorized restocking. RFID tap will be confirmed via serial logs and feedback on the LCD. |
| SRS-06  | **User Interface (LCD Display):** The LCD will display real-time balance, product selection, and error messages. The screen will refresh at a minimum rate of 2 Hz to ensure users are kept informed. |


### 6. Hardware Requirements Specification (HRS)

*Formulate key hardware requirements here. Think deeply on the design: What must your device do? How will you measure this during validation testing? Create 4 to 8 critical system requirements.*

*These must be testable! See the Final Project Manual Appendix for details. Refer to the table below; replace these examples with your own.*

**6.1 Definitions, Abbreviations**

Here, you will define any special terms, acronyms, or abbreviations you plan to use for hardware

- **DC Motor**: Spiral coil motor used for product dispensing  
- **IR Sensor**: Infrared photodiode/phototransistor pair used to detect dropped items  
- **Motor Driver**: L298N or ULN2003 driver IC used to interface motors with MCU  
- **LCD**: 16x2 or TFT display module (I2C or SPI protocol)  
- **PSU**: Power Supply Unit (12V adapter)

**6.2 Functionality**

| ID      | Description |
|---------|-------------|
| HRS-01  | The coin acceptor shall reliably accept standard coins (e.g., US quarters) with a success rate of ≥90%. Verified by testing with 10+ insertions to ensure minimal misreads or rejects. |
| HRS-02  | The DC motor shall generate enough torque to rotate the spiral coil and dispense a snack in under 2 seconds. Torque will be measured and motor activation verified using a force gauge or oscilloscope. |
| HRS-03  | The IR sensor shall detect dropped snacks within a 5 cm range. Detection will be tested using a test object and confirmed via firmware flag or serial output. |
| HRS-04  | The LCD (I2C or SPI) shall operate at 5V or 3.3V logic and display text clearly within ±5% of the target contrast. Verified under normal lighting and through voltage inspection. |
| HRS-05  | The power system shall include a 12V PSU and regulators providing stable 5V or 3.3V outputs within ±5% tolerance. Voltage levels will be confirmed with a multimeter or oscilloscope. |
| HRS-06  | The buzzer shall output 65–75 dB at 1 ft distance to clearly alert users to errors or confirmations. Volume tested using a dB meter or smartphone app. |

### 7. Bill of Materials (BOM)

*What major components do you need and why? Try to be as specific as possible. Your Hardware & Software Requirements Specifications should inform your component choices.*

*In addition to this written response, copy the Final Project BOM Google Sheet and fill it out with your critical components (think: processors, sensors, actuators). Include the link to your BOM in this section.*


| Component           | Description |
|---------------------|-------------|
| **Microcontroller** | ATmega328P or ESP32 – Main control unit handling all logic, I/O, and timing operations |
| **Coin Acceptor**   | Detects and validates standard coins (e.g., US quarters) for balance updates |
| **RFID Module**     | Used to unlock the machine for authorized restocking personnel (not for user payment) |
| **DC Motor + Driver** | Motor (e.g., spiral coil type) driven by L298N or ULN2003 IC – used for item dispensing |
| **Infrared Sensor** | IR emitter and photodiode pair used to detect successful item drops into the bin |
| **LCD Display + Keypad** | 16x2 or TFT LCD screen (I2C/SPI) to display balance, feedback, and errors; buttons for user interaction |
| **Buzzer + Speaker** | Outputs 65–75 dB alerts to confirm actions, errors, or other states |
| **Power Supply**    | 12V adapter with 5V/3.3V regulators for stable system power across modules |

 A detailed BOM including part numbers, links, and pricing is maintained here:  
(https://docs.google.com/spreadsheets/d/1iPDKnOJ7NfpnbtSCWD45nYKtOHLzh2dI/edit?usp=sharing&ouid=116562270372585393158&rtpof=true&sd=true)

### 8. Final Demo Goals

*How will you demonstrate your device on demo day? Will it be strapped to a person, mounted on a bicycle, require outdoor space? Think of any physical, temporal, and other constraints that could affect your planning.*

#### 1. Physical Setup
The vending machine will be placed on a tabletop with the coin acceptor, LCD display, and product selection buttons clearly visible and accessible to users.

#### 2. Demonstration Flow
- User inserts a **coin**.
- The **LCD** updates to reflect the new balance.
- The user **presses a button** to select a snack.
- The **motor** activates to dispense the snack.
- The **IR sensor** detects the snack drop and confirms delivery.
- The **LCD** updates the inventory count.
- A **buzzer** or **speech module** provides audible confirmation of success.

#### 3. Error Cases
- If a selected item is **out of stock**, the system will show an **error message** on the LCD and trigger a **buzzer alert**.
- If an **invalid coin** is inserted, the system will reject it and optionally trigger a **beep**.

#### 4. Wi-Fi Demo (Optional)
- The system may optionally demonstrate **real-time or periodic posting** of inventory status to a local dashboard via Wi-Fi for restocking/logging purposes.

### 9. Sprint Planning

*You've got limited time to get this project done! How will you plan your sprint milestones? How will you distribute the work within your team? Review the schedule in the final project manual for exact dates.*

| Milestone      | Functionality Achieved                                                                                    | Distribution of Work                             |
|----------------|-----------------------------------------------------------------------------------------------------------|--------------------------------------------------|
| **Sprint #1**  | - Hardware prototyping: wire up MCU, coin acceptor, and motor driver on breadboard                        | Jiwanshi: Breadboarding, Coin ISR                |
| *(Week 1)*     | - Coin interrupt testing: validate coin insertion triggers an interrupt                                   | Jat: Motor testing                               
|                | - Basic motor control: use ISRs to rotate motor for fixed time                                            |                                                  |
|                | - Integrate IR sensor to detect item drop                                                                 | Jat: IR sensor setup                             |
|                | - Implement LCD with I2C to show balance & item selection                                                 | Jiwanshi: LCD integration & inventory logic      |
| **Sprint #2**  | - Add basic inventory decrement logic after dispense                                                      |                                                  |
|  *(Week 2)*    | - Refine payment system: handle coin values & insufficient fund                                           | Jat: Coin logic & balance update                 |
|                | - Implement RFID-based restock locking system via I2C                                                     | Jiwanshi: RFID + speech module                   |
|                | - Add speech feedback module for user alerts                                                              |                                                  |
|                | - Add basic inventory decrement logic after dispense                                                      |                                                  |
| **MVP Demo**   | - Demonstrate coin-based purchase, dispensing, item tracking, and at least one type of feedback           | Both members present and demo respective parts   |
| **Final Demo** | - Finalize hardware enclosure and tidy up wiring                                                          | Jiwanshi: Hardware polish & wiring               |
|                | - Add error handling: out-of-stock & invalid coin cases                                                   | Jat: Wi-Fi logging + error messages              |
|                | - Enable Wi-Fi-based inventory posting to local dashboard                                                 |                                                  |


**This is the end of the Project Proposal section. The remaining sections will be filled out based on the milestone schedule.**

## Sprint Review #1

### Last week's progress
- Ordered all required parts and completed the drawing for laser cutting.
- Finished Venture Labs training.
- Wrote basic code for the coin acceptor.

### Current state of project
- Continuing work on the coin acceptor.
- Successfully integrated the motor with the Atmega microcontroller and working on the RFID integration.
- Revised the laser-cutting design.
- Received some of the parts needed for final assembly.

### Next week's plan
- Attach the motor to the wheel and spiral to test the dispensing mechanism.
- Perform the final laser cutting of the design.
- Incorporate the slot and slide for the coin acceptor.
- Get the LCD module fully operational with the Atmega.

- drawing:

![PHOTO-2025-03-30-14-33-31](https://github.com/user-attachments/assets/5c67eead-e790-4fe9-ac99-261cab17732d)

<img width="1407" alt="image" src="https://github.com/user-attachments/assets/655de750-6d17-41db-ba5c-76c4d1a1f374" />


## Sprint Review #2

### Last week's progress

### Current state of project

### Next week's plan

## MVP Demo

1. Show a system block diagram & explain the hardware implementation.
   ![IMG_0300](https://github.com/user-attachments/assets/4f164a34-c9f6-4787-8f61-85ecc4746e8a)

3. Explain your firmware implementation, including application logic and critical drivers you've written.
4. Demo your device.
5. Have you achieved some or all of your Software Requirements Specification (SRS)?
   1. Show how you collected data and the outcomes.
| ID      | Description |
|---------|-------------|
| SRS-01  | **Coin Detection:** The microcontroller will detect coin insertion using an interrupt within 20 ms of the coin passing the ultrasonicsensor. Verified via debug logs and timing. We used UART to output diatnce and if a coin was detected and kept a counter to show how many and reset it after every motor movement. |
| SRS-02  | **Coin Value Reading & Balance Update:** After detecting a coin, the system will update the user’s balance in under 1 second until sufficient balance is met. If the balance is not sufficient after 1 minute+ then the LCD will output insufficient balance until more coins are inserted. Feedback is be given via LCD and the motor turning. |
| SRS-03  | **Motor Control for Dispensing:** Upon product selection, the motor will start/stop within ±30s to prevent double-dispensing. We will also be controlling the motor by making it stop once a snack is detected. We verified the motor spinning using oscilloscopes as well as the time of it spinning. |
| SRS-04  | **Inventory Tracking:** A ultrasonic sensor detects when a product is dropped. There are two such set-up with each having a slot in the laser cut part. The internal inventory is updated accordingly. We tested this by using UART to detect the distance and delays.|
| SRS-04  | **User Interface (LCD Display):** The LCD displays real-time balance, product selection, and error messages. |

5. Have you achieved some or all of your Hardware Requirements Specification (HRS)?
   1. Show how you collected data and the outcomes.

| ID      | Description |
|---------|-------------|
| HRS-01  | The coin acceptor reliably accept standard coins (e.g., US quarters) with a success rate of ≥90%. Verified by testing with 10+ insertions to ensure minimal misreads or rejects. Also tested on ramp and confirmed the speed of the coin and the diatnce detection with UART |
| HRS-02  | The DC motor generates enough torque to rotate the spiral coil connected to the wheel and dispense a snack in for 30 seconds. Torque was be measured and motor activation verified using an oscilloscope. We also placed a product in the spiral to ensure that the weight was okay. |
| HRS-03  | The ultrasonic sensors detect dropped snacks within a 5 cm range. Detection was tested using a test object and confirmed via serial output. |
| HRS-04  | The LCD operates at 5V and displays text clearly within ±5% of the target contrast. |
| HRS-05  | The power system shall include a 6V PSU and regulators providing stable 5V and 6V outputs within ±5% tolerance. Voltage levels were confirmed with a multimeter |
| HRS-06  | The speaker is still pending to be integrated. |

6. Show off the remaining elements that will make your project whole: mechanical casework, supporting graphical user interface (GUI), web portal, etc.
   - Attaching all laser cut parts and emebedding all the parts into their respective places. Getting the motor to stop upon snack drop detection. Implementing the speaker and audio feedback and integrating into the whole circuit.
8. What is the riskiest part remaining of your project?
   - The Speaker and Audio Feedback will be the most trickest part as we are still working on it. 

   1. How do you plan to de-risk this?
  - Get started on it as soon as possible and test multiple times before integrating it into the circuit. 
10. What questions or help do you need from the teaching team?
    - NA. We had issues getting both Atmegas to work at the same time but we are figuring it out. 

## Final Project Report

Don't forget to make the GitHub pages public website!
If you’ve never made a GitHub pages website before, you can follow this webpage (though, substitute your final project repository for the GitHub username one in the quickstart guide):  [https://docs.github.com/en/pages/quickstart](https://docs.github.com/en/pages/quickstart)

### 1. Video

[Insert final project video here]

* The video must demonstrate your key functionality.
* The video must be 5 minutes or less.
* Ensure your video link is accessible to the teaching team. Unlisted YouTube videos or Google Drive uploads with SEAS account access work well.
* Points will be removed if the audio quality is poor - say, if you filmed your video in a noisy electrical engineering lab.

### 2. Images

[Insert final project images here]

*Include photos of your device from a few angles. If you have a casework, show both the exterior and interior (where the good EE bits are!).*

### 3. Results

*What were your results? Namely, what was the final solution/design to your problem?*

#### 3.1 Software Requirements Specification (SRS) Results

*Based on your quantified system performance, comment on how you achieved or fell short of your expected requirements.*

*Did your requirements change? If so, why? Failing to meet a requirement is acceptable; understanding the reason why is critical!*

*Validate at least two requirements, showing how you tested and your proof of work (videos, images, logic analyzer/oscilloscope captures, etc.).*

| ID     | Description                                                                                               | Validation Outcome                                                                          |
| ------ | --------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------- |
| SRS-01 | The IMU 3-axis acceleration will be measured with 16-bit depth every 100 milliseconds +/-10 milliseconds. | Confirmed, logged output from the MCU is saved to "validation" folder in GitHub repository. |

#### 3.2 Hardware Requirements Specification (HRS) Results

*Based on your quantified system performance, comment on how you achieved or fell short of your expected requirements.*

*Did your requirements change? If so, why? Failing to meet a requirement is acceptable; understanding the reason why is critical!*

*Validate at least two requirements, showing how you tested and your proof of work (videos, images, logic analyzer/oscilloscope captures, etc.).*

| ID     | Description                                                                                                                        | Validation Outcome                                                                                                      |
| ------ | ---------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------- |
| HRS-01 | A distance sensor shall be used for obstacle detection. The sensor shall detect obstacles at a maximum distance of at least 10 cm. | Confirmed, sensed obstacles up to 15cm. Video in "validation" folder, shows tape measure and logged output to terminal. |
|        |                                                                                                                                    |                                                                                                                         |

### 4. Conclusion

Reflect on your project. Some questions to address:

* What did you learn from it?
* What went well?
* What accomplishments are you proud of?
* What did you learn/gain from this experience?
* Did you have to change your approach?
* What could have been done differently?
* Did you encounter obstacles that you didn’t anticipate?
* What could be a next step for this project?

## References

Fill in your references here as you work on your final project. Describe any libraries used here.
