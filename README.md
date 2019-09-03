## Library to use AT commands for the Onethinx Core LoRaWAN module

The following AT commands can be used with this AT commands firmware


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
