# autonomous-watering
esp32c6 based, solar powered, battery backuped garden water supply system

## todo
The objective of this project is to have an autonomous-as-can-be field water management system. it supports:
- [ ] multiple 24Vdc water valve connections for water distribution
- [ ] solar power supply to avoid grid wiring
- [ ] battery backup for valve power supply in case no sun is shining. watering is done best after heavy sun hours anyway
- [ ] wifi access point for watering configurations and updates
- [ ] optional conductivity sensor to measure earth humidity
- [ ] maybe maybe wifi powered wether data access to avoid watering just before the storm. GPS data relevant?
- [ ] bonus project: support for an electric fence to have sheep stay away from my salad

## Hardware

can be found in [this](./hw) folder. it's a [kicad](https://www.kicad.org) project, so no worries about licenses.
for now i plan to have an [ESP32C6] as the computing base.

## Software

is found in [this](./sw) folder. as it is an embedded project, it fits the hardware described in the [hardware folder](./hw).

