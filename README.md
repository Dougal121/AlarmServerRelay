# AlarmServerRelay
ESP-based alarm server → relay bridge for TP-Link VIGI NVR systems

AlarmServerRelay provides a simple way to trigger real-world alarms (lights, sirens, buzzers, door strikes, etc.) using an ESP-based relay board connected to a TP-Link VIGI NVR.

Many of the larger VIGI models (e.g., 64-channel) include on-board alarm outputs — but the smaller units (4/8/16-channel) do not. This project fills that gap by acting as a bridge:

The NVR sends an HTTP alarm event to the ESP.

The ESP parses the event and activates one or more relays.

You get physical alarm outputs even on NVR models that don’t include them.

The goal is a fast, reliable, self-contained alarm interface that requires no cloud services, no proprietary hardware, and no subscription — just a tiny ESP module and a relay board.


                   +----------------------------------+
                   |        ESP32 + 2 Relays           |
                   | (relays are built into the board) |
                   |                                   |
                   |   [ Relay 1 Screw Terminals ]     |
                   |     COM     NO     NC             |
                   |                                   |
                   |   [ Relay 2 Screw Terminals ]     |
                   |     COM     NO     NC             |
                   |                                   |
                   |  7-12V -- VIN                     |
                   |   GND  -- GND                     |
                   +----------------------------------+
                                ^
                                |  WiFi
                                |
                     +---------------------+
                     |   TP-Link VIGI NVR  |
                     |  sends HTTP alarms  |
                     +---------------------+


