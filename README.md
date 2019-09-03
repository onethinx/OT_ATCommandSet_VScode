## Library to use AT commands for the Onethinx Core LoRaWAN module

The following AT commands can be used with this AT commands firmware:

| ﻿Command             | Response  | Description                          |
|---------------------|------------|--------------------------------------|
| AT                  | OK         | Ping                                 |
| AT+INFO             | [info]     | Show the info of the module          |
| AT+INIT             | OK / Error | Initialize Stack                     |
| AT+STATUS           | [status]   | Show the status of the module        |
| AT+SET_OTAA [keys]  | OK / Error | Set OTAA keys                        |
| AT+JOIN             | OK / Error | Join the LoRaWAN network             |
| AT+TX [data]        | OK / Error |                                      |
| AT+RX_LENGTH        | [length]   | Show amount of data in the RX buffer |
| AT+RX               | [data]     | Show received data                   |
| AT+SLEEPMODE [mode] | OK / Error | Set sleepmode                        |

## Notes
- All commands are terminated by "\r\n"
- Sleep IO: 
    - low: enter sleep mode
    - high: wake up
- UART settings: 
    - baudrate 19200
    - databits: 8
    - parity: N
    - stopbits: 1
- All data is in hexadecimal format.
- OTAA keys [keys] format:
    - [public network] [DevEUI] [AppEUI] [AppKey]
    - public network: 1 or 0 (1 for public network, 0 for private network)
    - DevEUI: hexadecimal values 8 byte DevEUI (16 characters)
    - AppEUI: hexadecimal values 8 byte AppEUI (16 characters)
    - AppKey: hexadecimal values 16 byte AppKey (32 characters)
- Sleep mode [mode] value:
    - 0: Hibernate
    - 1: Deep Sleep	
    - 2: Sleep
