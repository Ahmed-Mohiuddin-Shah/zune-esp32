# Zune-ESP32 Project

# !!! THIS IS NOT FINAL !!!

## !!! WORK IN PROGRESS !!!

## Overview

This project aims to create a Zune HD-like device using an ESP32-S3 microcontroller with a 3.5-inch touch screen. The device will include various features such as an IMU, rotary encoder, camera (via USB 1.1), I2S speaker and microphone, headphone jack, and an SD card for storing programs and files.

## Features

- **ESP32-S3 WROOM-1 (N8R2) Microcontroller**
- **3.5-inch Touch Screen**
- **IMU (Inertial Measurement Unit)**
- **Rotary Encoder**
- **Camera (via USB 1.1)**
- **I2S Speaker and Microphone**
- **Headphone Jack**
- **SD Card Slot**

## Development Goals

1. **Library and SDK System**: Develop a library and SDK to define memory limits so that programs can fit within the ESP32 memory constraints.
2. **PlatformIO Integration**: Use PlatformIO for development to streamline the process of compiling and uploading code to the ESP32.
3. **Cross-Compilation**: Implement a system where the custom library can detect the build target and compile an executable (.exe) for Windows using GCC, and use the appropriate functions or API when compiling and uploading through PlatformIO to the ESP32.

## Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/)
- [GCC for Windows](https://gcc.gnu.org/)
- ESP32-S3 Development Board
- 3.5-inch Touch Screen
- Additional peripherals (IMU, rotary encoder, camera, I2S speaker and microphone, headphone jack, SD card)

### Installation

1. Clone the repository:
   ```sh
   git clone https://github.com/yourusername/zune-esp32.git
   ```
2. Open the project in PlatformIO.

### Building and Uploading

- To compile and upload code to the ESP32:
  ```sh
  platformio run --target upload
  ```
- To compile an executable for Windows:
  ```sh
  gcc -o your_program.exe your_program.c
  ```

## Contributing

Contributions are welcome! Please fork the repository and create a pull request with your changes.

## License

This project is licensed under the MIT License.

## Contact

For any questions or suggestions, please open an issue or contact the project maintainer.
