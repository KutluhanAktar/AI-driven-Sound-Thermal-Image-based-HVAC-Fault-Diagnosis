# AI-driven-Sound-Thermal-Image-based-HVAC-Fault-Diagnosis
Identify the faulty components via anomalous sound detection and diagnose ensuing cooling malfunctions via thermal visual anomaly detection.

**You can inspect the step-by-step project tutorial on [Hackster](https://www.hackster.io/kutluhan-aktar/ai-driven-sound-thermal-image-based-hvac-fault-diagnosis-27084c).**

## Description

After perusing recent research papers on detecting component failures to automate HVAC maintenance, I noticed that there are no practical applications focusing on identifying component abnormalities of intricate water-based HVAC systems to diagnose consecutive thermal cooling malfunctions before instigating hazardous effects on both production quality and the environment. Hence, I decided to build a versatile multi-model AIoT device to detect anomalous sound emanating from cooling fans via a neural network model and to diagnose consecutive thermal cooling malfunctions based on specifically produced thermal images via a visual anomaly detection model. In addition to AI-driven features, I decided to develop a capable and feature-rich web application (dashboard) to improve user experience and make data transfer easier between development boards.

As I started to work on developing my AI-powered device features, I realized that no available open-source data sets were fulfilling the purpose of multi-model HVAC malfunction diagnosis. Thus, since I did not have the resources to collect data from an industrial-level HVAC system, I decided to build a simplified HVAC system simulating the required component failures for data collection and in-field model testing. I got heavily inspired by PC (computer) water cooling systems while designing my simplified HVAC system. Similar to a closed-loop PC water cooling design, I built my system by utilizing a water pump, plastic tubings, an aluminum radiator, and aluminum blocks. As for the coolant reservoir, I decided to design a custom one and print the parts with my 3D printer. Nonetheless, since I decided to produce a precise thermal image by scanning cooling components, I still needed an additional mechanism to move a thermal camera on the targeted components — aluminum blocks. Thus, I decided to design a fully 3D-printable CNC router with the thermal camera container head to position the thermal camera, providing an automatic homing sequence. My custom CNC router is controlled by Arduino Nano and consists of a 28BYJ-48 stepper motor, GT2 pulleys, a timing belt, and gear clamps. While producing thermal images and running the visual anomaly detection model, I simply added an aquarium heater to the closed-water loop in order to instantiate aluminum block cooling malfunctions.

As mentioned earlier, to provide full-fledged AIoT features with seamless integration and simplify complex data transfer procedures between development boards while constructing separate data sets and running multiple models, I decided to develop a versatile web application (dashboard) from scratch. To briefly summarize, the web dashboard can receive audio buffers via HTTP POST requests, save audio samples by given classes, communicate with the Particle Cloud to obtain variables or make Particle boards register them, produce thermal images from thermal imaging buffers to store image samples, and run the visual anomaly detection model on the generated thermal images. In the following tutorial, you can inspect all web dashboard features in detail.

Since this is a multi-model AI-oriented project, I needed to construct two different data sets and train two separate machine learning models in order to build a capable device. First, I focused on constructing a valid audio data set for detecting anomalous sound originating from cooling fans. Since XIAO ESP32C6 is a compact and high-performance IoT development board providing 512KB SRAM and 4 MB Flash, I decided to utilize XIAO ESP32C6 to collect audio samples and run my neural network model for anomalous sound detection. To generate fast and accurate audio samples (buffers), I decided to use a Fermion I2S MEMS microphone. Also, I connected an SSD1306 OLED display and four control buttons to program a feature-rich on-device user interface. After collecting an audio sample, XIAO ESP32C6 transfers it to the web dashboard for data collection. As mentioned earlier, I designed my custom CNC router based on Arduino Nano due to its operating voltage. To provide seamless device operations, XIAO ESP32C6 communicates with Arduino Nano to move the thermal camera container head.

After completing constructing my audio data set, I built my neural network model (Audio MFE) with Edge Impulse to detect sound-based cooling fan abnormalities. Audio MFE models employ a non-linear scale in the frequency domain, called Mel-scale, and perform well on audio data, mostly for non-voice recognition. Since Edge Impulse is nearly compatible with all microcontrollers and development boards, I have not encountered any issues while uploading and running my Audio MFE model on XIAO ESP32C6. As labels, I simply differentiated the collected audio samples by the cooling fan failure presence:

- normal
- defective

After training and testing my neural network model (Audio MFE), I deployed the model as an Arduino library and uploaded it to XIAO ESP32C6. Therefore, the device is capable of detecting anomalous sound emanating from the cooling fans by running the neural network model onboard without any additional procedures or latency.

Since I wanted to employ the secure and reliable Particle Cloud as a proxy to transfer thermal imaging (scan) buffers to the web dashboard, I decided to utilize Photon 2, which is a feature-packed IoT development board optimized for cloud prototyping. To collect accurate thermal imaging buffers, I employed an MLX90641 thermal imaging camera producing 16x12 IR arrays (buffers) with fully calibrated 110° FOV (field-of-view). Also, I connected an ST7735 TFT display and an analog joystick to program a secondary on-device user interface. Even though I managed to create a snapshot (preview) image from the collected thermal scan buffers, Photon 2 is not suitable for generating thermal images, saving image samples, and running a demanding visual anomaly detection model simultaneously due to memory limitations. Therefore, after registering the collected thermal scan buffers to the Particle Cloud, I utilized the web dashboard to obtain the registered buffers via the Particle Cloud API, produce thermal image samples, and run the visual anomaly detection model.

Considering the requirements of producing accurate thermal images and running a visual anomaly detection model, I decided to host my web application (dashboard) on a LattePanda Mu (x86 Compute Module). Combined with its Lite Carrier board, LattePanda Mu is a promising single-board computer featuring an Intel N100 quad-core processor with 64 GB onboard storage.

After completing constructing my thermal image data set, I built my visual anomaly detection model with Edge Impulse to diagnose ensuing thermal cooling malfunctions after applying anomalous sound detection to the water-based HVAC system. Since analyzing cooling anomalies based on thermal images of HVAC system components is a complicated task, I decided to employ an advanced and precise machine learning algorithm based on the GMM anomaly detection algorithm and FOMO. Supported by Edge Impulse Enterprise, FOMO-AD is an exceptional algorithm for detecting unanticipated defects by applying unsupervised learning techniques. Since Edge Impulse is nearly compatible with all microcontrollers and development boards, I have not encountered any issues while uploading and running my FOMO-AD model on LattePanda Mu. As labels, I utilized the default classes required by Edge Impulse to enable the F1 score calculation:

- no anomaly
- anomaly

After training and testing my FOMO-AD visual anomaly detection model, I deployed the model as a Linux (x86_64) application (.eim) and uploaded it to LattePanda Mu. Thus, the web dashboard is capable of diagnosing thermal cooling anomalies based on the specifically produced thermal images by running the visual anomaly detection model on the server (LattePanda Mu) without any additional procedures, reduced accuracy, or latency.

In addition to the discussed features, the web dashboard informs the user of the latest system log updates (completed operations) on the home (index) page automatically and sends an SMS to the verified phone number via Twilio so as to notify the user of the latest cooling status.

Considering the complex structure of this device based on a customized water-based HVAC system, I decided to design two unique PCBs after testing the prototype connections via breadboards. Since I wanted my PCB designs to represent the equilibrium of cooling fan failures and thermal (heat) malfunctions, I got inspired by two ancient rival Pokémon — Kyogre and Groudon.

Finally, in addition to the custom CNC router and coolant reservoir parts, I designed a plethora of complementary 3D parts, from unique PCB encasements to radiator mounts, so as to make the device as robust and compact as possible. To print flexible parts handling water pressure, I utilized a color-changing TPU filament.

So, this is my project in a nutshell 😃

## Demonstration

![1](https://github.com/user-attachments/assets/9f640047-55af-40a9-b9cd-c079a08387e8)

![2](https://github.com/user-attachments/assets/6abc5bcc-48a4-4339-a3f3-903b00608e7c)

![3](https://github.com/user-attachments/assets/5d82112b-b690-4f9e-aa49-07a83fe32a57)

![4](https://github.com/user-attachments/assets/97147f9a-23de-45b3-a511-fdaf4fed122d)

![5](https://github.com/user-attachments/assets/c3eb200f-5e64-4136-b8ca-3748f23be372)

![6](https://github.com/user-attachments/assets/89d699b7-8b18-43bc-ab5f-3c949ec3583c)

![7](https://github.com/user-attachments/assets/9d815aec-a6a1-4761-8b27-26f5ebe45fdd)

![8](https://github.com/user-attachments/assets/a848318b-7d01-46b4-819f-4fd069dbd7cb)

![9](https://github.com/user-attachments/assets/e95c2acd-b34f-4dcb-83d7-2b1106890d3d)

![10](https://github.com/user-attachments/assets/41e0ccd4-ac8b-4e0a-9470-5ae7f8d90976)

![11](https://github.com/user-attachments/assets/35b3e5a8-47c0-45a0-b39b-a2358f83182e)

![12](https://github.com/user-attachments/assets/85416a0c-42c3-472c-9c62-b95b7f52f3e9)

![13](https://github.com/user-attachments/assets/85bd40e5-dfa2-4090-a6d4-caef2fd8b3f3)

![14](https://github.com/user-attachments/assets/2020e4b5-ea6b-4ad9-b8d8-7fe663983e57)

![15](https://github.com/user-attachments/assets/4fbc18a5-c7e8-4a17-bafb-b48b9bc4131e)

![16](https://github.com/user-attachments/assets/c9db0a77-d53b-4cf6-8687-17aa1beb685c)

![17](https://github.com/user-attachments/assets/04a0519b-21cf-41bf-b841-1292b3078669)

![18](https://github.com/user-attachments/assets/ff347961-75b3-4721-88b6-a69e8c4eeedb)

![19](https://github.com/user-attachments/assets/666374fd-d9ae-4696-9fc2-e55d14977657)

![20](https://github.com/user-attachments/assets/47f9a84f-37cd-4e21-b025-e2dc26614721)

![21](https://github.com/user-attachments/assets/7dd31795-1ca3-47c6-b6b1-0eb9fb6257d9)

![22](https://github.com/user-attachments/assets/7ca59c89-603c-49a0-b6ab-4f3b5b6540fa)

![23](https://github.com/user-attachments/assets/5985d94b-6a02-49dc-94e0-b20c5cd7078f)

![24](https://github.com/user-attachments/assets/ef9f0881-594b-4d53-9f0a-a3cb235e6cd0)

![25](https://github.com/user-attachments/assets/4acc3011-e0bb-4a04-9236-955794918269)
