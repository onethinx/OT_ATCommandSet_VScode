| ﻿Command             | Response | Description                          |
|---------------------|----------|--------------------------------------|
| AT                  | OK       | Ping                                 |
| AT+INFO             | [info]   | Show the info of the module          |
| AT+INIT             | OK       | Initialize Stack                     |
| AT+STATUS           | [status] | Show the status of the module        |
| AT+SET_OTAA [keys]  | OK       | Set OTAA keys                        |
| AT+JOIN             | OK       | Join the LoRaWAN network             |
| AT+TX [data]        | OK       |                                      |
| AT+RX_LENGTH        | [length] | Show amount of data in the RX buffer |
| AT+RX               | [data]   | Show received data                   |
| AT+SLEEPMODE [mode] | OK       | Set sleepmode                        |
