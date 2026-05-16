# firmware for autonomous watering system

since hardware is using an esp32c6 as a controller and access interface for config and more, we need some code to flash onto it.

## usage

connect your pcb to your computer using a usb c cable.

to set up toolchain use:

```bash
pip install conan
conan config install https://github.com/tomatenkuchen/conan-settings.git
conan 

```

to build use:

```bash
source ~/my/path/to/esp-idf/export.sh
conan build . -pr=esp32c6 -b=missing
```

to flash use (after build):

```bash
conan export-pkg . -pr=esp32c6
```

## why conan

i know conan to be a useful package manager that has most of the software onboard that i need to use for my projects

## general structure

the device needs access to the following resources for the device to work properly:
- rtc for time keeping without network
- adc for earth conductivity measurement
- some gpios for valve switching
