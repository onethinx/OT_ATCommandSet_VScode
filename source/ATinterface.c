/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include <stdint.h>
#include <stdbool.h>
#include "cycfg.h"
#include "OnethinxCore01.h"
#include "ATinterface.h"
#include "Demokit01.h"

extern volatile coreArguments_t coreArguments;
coreStatus_t coreStatus;
coreInfo_t coreInfo;
errorStatus_t coreErrorStatus;

coreConfiguration_t	coreConfig = {
	.Join.KeysPtr = 			&LoRaWAN_keys,
	.Join.DataRate =			DR_AUTO,
	.Join.Power =				PWR_MAX,
	.Join.MAXTries = 			100,
    .Join.SubBand_1st =     	EU_SUB_BANDS_DEFAULT,
	.Join.SubBand_2nd =     	EU_SUB_BANDS_DEFAULT,
	.TX.Confirmed = 			false,
	.TX.DataRate = 				DR_0,
	.TX.Power = 			    PWR_MAX,
	.TX.FPort = 				1,
	.System.Idle.Mode = 		M0_DeepSleep,
	.System.Idle.BleEcoON =		false,
	.System.Idle.DebugON =		true,
};

sleepConfig_t sleepConfig =
{
	.BleEcoON = false,
	.DebugON = true,
    .sleepMode = modeDeepSleep,
	.sleepCores = coresBoth,
	.wakeUpPin = wakeUpPinLow(false),
	.wakeUpTime = wakeUpTimeOff
};

LoRaWAN_keys_t LoRaWAN_keys = 
{
	.KeyType	= PreStored_key,
	.StoredKeys.KeyIndex = 0
};

uint8_t	cmdIDX = 0;
char	cmdBUF[255];
char	tmpBUF[255];
char	rxBUF[255];
uint8_t	tmpBUFIDX = 0;

// Build AT command list, occurrences of 'partial duplicate commands' should come after the 'full command' eg: "RX" should come after "RX_LENGTH"
const char* const ATcmdList[] = {
	"PING",
	"INFO",
	"STATUS",
	"SET_OTAA",
	"JOIN",
	"TX",
	"RX_LENGTH",
	"RX",
	"SLEEPMODE",
	"HELP",
	0
};

// Enum commands in the same order as the list above
typedef enum
{
    cmd_ping,
    cmd_info,
    cmd_status,
    cmd_set_otaa,
    cmd_join,
    cmd_tx,
    cmd_rxlength,
    cmd_rx,
    cmd_sleepmode,
    cmd_help
} command_t;

typedef enum
{
    lora_start,
    lora_idle,
    lora_joining,
    lora_sending,
} state_lora_t;

state_lora_t loraState = lora_start;

typedef enum
{
    resp_ok,
    resp_info,
    resp_joining,
    resp_sending,
    resp_invalidcmd,
    resp_invalidparam,
    resp_bufferoverflow,
    resp_invalidhex,
    resp_core_radioerror,
    resp_core_macerror,
    resp_core_paramerror,
    resp_core_systemerror,
    resp_core_isbusy
} response_t;

char * uint16toDecimalBuilder(uint16_t val, uint8_t idx)
{
    uint8_t incr = 0;
    if (idx != (uint8_t) -1) tmpBUFIDX = idx;
    if (tmpBUFIDX > 246) return NULL;
    tmpBUF[tmpBUFIDX] = '0';
    while (val >= 10000) val -= 10000, tmpBUF[tmpBUFIDX]++;
    if (tmpBUF[tmpBUFIDX] != '0') incr = 1;
    tmpBUFIDX += incr;
    tmpBUF[tmpBUFIDX] = '0';
    while (val >= 1000) val -= 1000, tmpBUF[tmpBUFIDX]++;
    if (tmpBUF[tmpBUFIDX] != '0') incr = 1;
    tmpBUFIDX += incr;
    tmpBUF[tmpBUFIDX] = '0';
    while (val >= 100) val -= 100, tmpBUF[tmpBUFIDX]++;
    if (tmpBUF[tmpBUFIDX] != '0') incr = 1;
    tmpBUFIDX += incr;
    tmpBUF[tmpBUFIDX] = '0';
    while (val >= 10) val -= 10, tmpBUF[tmpBUFIDX]++;
    if (tmpBUF[tmpBUFIDX] != '0') incr = 1;
    tmpBUFIDX += incr;
    tmpBUF[tmpBUFIDX] = '0' + val;
    tmpBUF[++tmpBUFIDX] = 0;
    return (char *) tmpBUF;
}

char * uint32toHexBuilder(uint32_t val, uint8_t idx)
{
    if (idx != (uint8_t) -1) tmpBUFIDX = idx;
    if (tmpBUFIDX > 246) return NULL;
    tmpBUF[tmpBUFIDX] = (val >> 28) & 0x0F;
    tmpBUF[tmpBUFIDX] += (tmpBUF[tmpBUFIDX] > 9)? 55 : '0';
    tmpBUF[++tmpBUFIDX] = (val >> 24) & 0x0F;
    tmpBUF[tmpBUFIDX] += (tmpBUF[tmpBUFIDX] > 9)? 55 : '0';
    tmpBUF[++tmpBUFIDX] = (val >> 20) & 0x0F;
    tmpBUF[tmpBUFIDX] += (tmpBUF[tmpBUFIDX] > 9)? 55 : '0';
    tmpBUF[++tmpBUFIDX] = (val >> 16) & 0x0F;
    tmpBUF[tmpBUFIDX] += (tmpBUF[tmpBUFIDX] > 9)? 55 : '0';
    tmpBUF[++tmpBUFIDX] = (val >> 12) & 0x0F;
    tmpBUF[tmpBUFIDX] += (tmpBUF[tmpBUFIDX] > 9)? 55 : '0';
    tmpBUF[++tmpBUFIDX] = (val >> 8) & 0x0F;
    tmpBUF[tmpBUFIDX] += (tmpBUF[tmpBUFIDX] > 9)? 55 : '0';
    tmpBUF[++tmpBUFIDX] = (val >> 4) & 0x0F;
    tmpBUF[tmpBUFIDX] += (tmpBUF[tmpBUFIDX] > 9)? 55 : '0';
    tmpBUF[++tmpBUFIDX] = (val >> 0) & 0x0F;
    tmpBUF[tmpBUFIDX] += (tmpBUF[tmpBUFIDX] > 9)? 55 : '0';
    tmpBUF[++tmpBUFIDX] = 0;
    return (char *) tmpBUF;
}

char * bytestoHexBuilder(uint8_t * val, uint8_t length, uint8_t idx)
{
    if (idx != (uint8_t) -1) tmpBUFIDX = idx;
    if (((length << 1) + (uint16_t) tmpBUFIDX) > 254) return NULL;
    for (int8_t cnt = 0; cnt < length; cnt++, tmpBUFIDX++)
    {
        tmpBUF[tmpBUFIDX] = (val[cnt] >> 4) & 0x0F;
        tmpBUF[tmpBUFIDX] += (tmpBUF[tmpBUFIDX] > 9)? 55 : '0';
        tmpBUF[++tmpBUFIDX] = (val[cnt] >> 0) & 0x0F;
        tmpBUF[tmpBUFIDX] += (tmpBUF[tmpBUFIDX] > 9)? 55 : '0';
    }
    tmpBUF[tmpBUFIDX] = 0;
    return (char *) tmpBUF;
}

char * stringBuilder(char* string, uint8_t idx)
{
    if (idx != (uint8_t) -1) tmpBUFIDX = idx;
    for (; tmpBUFIDX < 255;)
    {
        if (* string == 0) break;
        tmpBUF[tmpBUFIDX++] = * string++;
    }
    tmpBUF[tmpBUFIDX] = 0;
    return (char *) tmpBUF;
}

void outputResponse(response_t response, uint32_t errorValue)
{
    const char* message = "Error FF: Unknown error";
    switch (response)
    {
        case resp_ok:
            message = "OK";
            break;
        case resp_info:
            coreStatus = LoRaWAN_GetInfo(&coreInfo);
            stringBuilder("--= Onethinx LoRaWAN Core OT-X18 =--\r\n: Type-Option-Stage-Region: ", 0);
            stringBuilder((char[]) { coreInfo.buildType, '-', coreInfo.stackOption, '-', coreInfo.stackStage, '-', 0 }, -1);
            uint16toDecimalBuilder(coreInfo.stackRegion, -1);
            stringBuilder("\r\n: Build: ", -1);
            uint16toDecimalBuilder(coreInfo.buildNumber, -1);
            stringBuilder(" (20", -1);
            uint16toDecimalBuilder(coreInfo.buildYear, -1);
            stringBuilder("-", -1);
            uint16toDecimalBuilder(coreInfo.buildMonth, -1);
            stringBuilder("-", -1);
            uint16toDecimalBuilder(coreInfo.buildDayOfMonth, -1);
            stringBuilder(" ", -1);
            uint16toDecimalBuilder(coreInfo.buildHour, -1);
            stringBuilder(":", -1);
            uint16toDecimalBuilder(coreInfo.buildMinute, -1);
            stringBuilder(":", -1);
            uint16toDecimalBuilder(coreInfo.buildSecond, -1);
            stringBuilder(")\r\n: DevEUI: ", -1);
            bytestoHexBuilder((uint8_t *) &coreInfo.devEUI, 8, -1);
            stringBuilder("\r\n: Stack Version: ", -1);
            uint32toHexBuilder(coreStatus.system.version, -1);
            stringBuilder("\r\n: CMD Version: ", -1);
            uint32toHexBuilder(ATcommVersion, -1);
            message = stringBuilder("\r\nOK", -1);
            break;
        case resp_joining:
            message = "joining...";
            break;
        case resp_sending:
            message = "sending...";
            break;
        case resp_invalidcmd:
            message = "Error 01: AT command not found";
            break;
        case resp_invalidparam:
            message = "Error 02: Invalid parameters";
            break;
        case resp_bufferoverflow:
            message = "Error 03: AT buffer overflow";
            break;
        case resp_invalidhex:
            message = "Error 04: Invalid Hexadecimal value";
            break;
        case resp_core_radioerror:
            message = "Error 05: LoRaWAN radio error 0x";
            break;
        case resp_core_macerror:
            message = "Error 06: LoRaWAN MAC error 0x";
            break;
        case resp_core_paramerror:
            message = "Error 07: LoRaWAN parameter error 0x";
            break;
        case resp_core_systemerror:
            message = "Error 08: LoRaWAN system error 0x";
            break;
        case resp_core_isbusy:
            message = "Error 09: LoRaWAN is busy";
            break;
    }
    Cy_SCB_UART_PutString(UART_HW, message);
    if (errorValue != 0) Cy_SCB_UART_PutString(UART_HW, uint32toHexBuilder(errorValue, 0));
    Cy_SCB_UART_PutString(UART_HW, "\r\n");
}

response_t HEXtoBytes(char * source, char * dest, uint8_t byteSize)
{
    uint8_t cnt;
    for(cnt = 0; cnt < byteSize; cnt++)
    {
        if (*source > 0x60) *source -= 0x20;        // Convert to uppercase
        if (*source >= '0' && *source <= '9') *dest = *source - '0';
        else if (*source >= 'A' && *source <= 'F') *dest = *source - 55;
        else return resp_invalidhex;
        *dest <<= 4; source++;
        if (*source > 0x60) *source -= 0x20;        // Convert to uppercase
        if (*source >= '0' && *source <= '9') *dest += *source - '0';
        else if (*source >= 'A' && *source <= 'F') *dest += *source - 55;
        else return resp_invalidhex;
        source++; dest++;
    }
    return resp_ok;
}

void coreResponse()
{
    coreErrorStatus = LoRaWAN_GetError();
    if (coreErrorStatus.macErrors != mac_OK) return outputResponse(resp_core_macerror, coreErrorStatus.errorValue);
    if (coreErrorStatus.radioErrors != radio_OK) return outputResponse(resp_core_radioerror, coreErrorStatus.errorValue);
    if (coreErrorStatus.systemErrors != system_OK) return outputResponse(resp_core_systemerror, coreErrorStatus.errorValue);
    if (coreErrorStatus.paramErrors != param_OK) return outputResponse(resp_core_paramerror, coreErrorStatus.errorValue);
    outputResponse(resp_ok, 0);
}

void execCommand(command_t command, uint8_t length, uint8_t cmdLength)
{
    response_t retErr;
    if(coreArguments.status.system.isBusy) return outputResponse(resp_core_isbusy, 0);
    switch (command)
    {
        case cmd_ping:
            outputResponse((length != cmdLength)? resp_invalidparam: resp_ok, 0);
            break;
        case cmd_info:
            outputResponse((length != cmdLength)? resp_invalidparam: resp_info, 0);
            break;
        case cmd_status:
            coreResponse();
            break;
        case cmd_set_otaa:
            if ((length - cmdLength) != 0x45) {
                outputResponse(resp_invalidparam, 0);
                return;
            }
            if ((retErr = HEXtoBytes(&cmdBUF[cmdLength + 3], (char *) &LoRaWAN_keys.OTAA_10x.DevEui, 8)) != resp_ok) return outputResponse(retErr, 0);
            if ((retErr = HEXtoBytes(&cmdBUF[cmdLength + 20], (char *) &LoRaWAN_keys.OTAA_10x.AppEui, 8)) != resp_ok) return outputResponse(retErr, 0);
            if ((retErr = HEXtoBytes(&cmdBUF[cmdLength + 37], (char *) &LoRaWAN_keys.OTAA_10x.AppKey, 16)) != resp_ok) return outputResponse(retErr, 0);
            LoRaWAN_keys.KeyType = OTAA_10x_key;
            LoRaWAN_keys.PublicNetwork = (cmdBUF[cmdLength + 1] == '0')? false : true;
            outputResponse(resp_ok, 0);
            break;
        case cmd_join:
            LoRaWAN_Join(false);
            outputResponse(resp_joining, 0);
            loraState = lora_joining;
            break;
        case cmd_tx:
            length = (length - (cmdLength + 1)) >> 1;
            if ((retErr = HEXtoBytes(&cmdBUF[cmdLength + 1], tmpBUF, length)) != resp_ok) return outputResponse(retErr, 0);
            LoRaWAN_Send((uint8_t *) tmpBUF, length, false);
            outputResponse(resp_sending, 0);
            loraState = lora_sending;
            break;
        case cmd_rxlength:
            if (length != cmdLength) {
                outputResponse(resp_invalidparam, 0);
                return;
            }
            coreStatus = LoRaWAN_GetStatus();
            Cy_SCB_UART_PutString(UART_HW, uint16toDecimalBuilder((uint16_t) coreStatus.mac.bytesToRead, 0));
            Cy_SCB_UART_PutString(UART_HW, "\r\n");
            outputResponse(resp_ok, 0);
            break;
        case cmd_rx:
        if (length != cmdLength) {
                outputResponse(resp_invalidparam, 0);
                return;
            }
            coreStatus = LoRaWAN_GetStatus();
            LoRaWAN_GetRXdata((uint8_t *) rxBUF, coreStatus.mac.bytesToRead);
            Cy_SCB_UART_PutString(UART_HW, bytestoHexBuilder((uint8_t *) rxBUF, coreStatus.mac.bytesToRead, 0));
            Cy_SCB_UART_PutString(UART_HW, "\r\n");
            outputResponse(resp_ok, 0);
            break;
        case cmd_sleepmode:
            if ((length - cmdLength) != 2) {
                outputResponse(resp_invalidparam, 0);
                return;
            }
            switch (cmdBUF[cmdLength + 1])
            {
                case '0':
                    sleepConfig.sleepMode = modeHibernate;
                    break;
                case '1':
                    sleepConfig.sleepMode = modeDeepSleep;
                    break;
                case '2':
                    sleepConfig.sleepMode = modeSleep;
                    break;
                default:
                    outputResponse(resp_invalidparam, 0);
                    return;
                    
            }
            outputResponse(resp_ok, 0);
            while (!Cy_SCB_IsTxComplete(UART_HW));                          // Wait till finished sending OK
            Cy_SysClk_ClkPathSetSource(2U, CY_SYSCLK_CLKPATH_IN_IMO);       // Set Clock MUX to IMO because Sleep will disable BLE ECO clock (ALTHF)
            LoRaWAN_Sleep(&sleepConfig);
            Cy_SysClk_ClkPathSetSource(2U, CY_SYSCLK_CLKPATH_IN_ALTHF);     // Restore Clock MUX to BLE ECO clock (ALTHF)
            break;
        case cmd_help:
            outputResponse((length != cmdLength)? resp_invalidparam: resp_ok, 0);
            break;
    }

}

void ATcomm(void)
{
    if (loraState == lora_start)
    {
        //Cy_GPIO_Pin_FastInit(LED_RED_PORT, LED_RED_PIN, CY_GPIO_DM_STRONG_IN_OFF, 1, LED_RED_HSIOM);
        //Cy_GPIO_Pin_FastInit(LED_BLUE_PORT, LED_BLUE_PIN, CY_GPIO_DM_STRONG_IN_OFF, 0, LED_BLUE_HSIOM);
        LoRaWAN_Init(&coreConfig);
        //LED_R_SET(0);
        //LED_B_SET(1);
        outputResponse(resp_info, 0);
        loraState = lora_idle;
    }
    if ((loraState != lora_idle) && (LoRaWAN_GetStatus().system.isBusy == false))
    {
        outputResponse(resp_ok, 0);
        loraState = lora_idle;
    }
    uint32_t cmd;
    if ((cmd = Cy_SCB_UART_Get(UART_HW)) == CY_SCB_UART_RX_NO_DATA) return;
    uint8_t RXbyte = (uint8_t) cmd;
    switch(cmdIDX)
    {
        case 0:
            if (RXbyte == 'A') cmdIDX++;
            break;
        case 1:
            if (RXbyte == 'T') cmdIDX++;
            else cmdIDX = 0;
            break;
        case 2:
            if (RXbyte == '+') cmdIDX++;
            else cmdIDX = 0;
            break;
        case 255:   // Error MAX command size exceeded
            outputResponse(resp_bufferoverflow, 0);
            cmdIDX = 0;
            break;
        default:
            if (RXbyte != '\r' && RXbyte != '\n') cmdBUF[++cmdIDX-4] = RXbyte;
            else 
            {
                uint8_t cnt, cmdPNT = 0; cmdIDX -= 3;
                while (ATcmdList[cmdPNT] != 0) {
                    for (cnt = 0; cnt <= cmdIDX; cnt++) {
                        if (ATcmdList[cmdPNT][cnt] == 0) {
                            execCommand(cmdPNT, cmdIDX, cnt);
                            cmdIDX = 0;
                            return;
                        }
                        if (cmdBUF[cnt] != ATcmdList[cmdPNT][cnt]) break;           // different character, try next cmd
                    }
                    cmdPNT++;
                }
                outputResponse(resp_invalidcmd, 0);
                cmdIDX = 0;
            }
            break;
    }
}
	
/* [] END OF FILE */