#if defined(ESP32)

#include "safe_boot.h"

#include <Arduino.h>
#include <rom/rtc.h>

#include <SerialStream.h>
#include <WiFiManager.h>
#include <upload_ota.h>

enum class ResetReason
{
    NO_MEAN = 0,
    POWERON_RESET = 1,           /**<1, Vbat power on reset*/
    SW_RESET = 3,                /**<3, Software reset digital core*/
    OWDT_RESET = 4,              /**<4, Legacy watch dog reset digital core*/
    DEEPSLEEP_RESET = 5,         /**<3, Deep Sleep reset digital core*/
    SDIO_RESET = 6,              /**<6, Reset by SLC module, reset digital core*/
    TG0WDT_SYS_RESET = 7,        /**<7, Timer Group0 Watch dog reset digital core*/
    TG1WDT_SYS_RESET = 8,        /**<8, Timer Group1 Watch dog reset digital core*/
    RTCWDT_SYS_RESET = 9,        /**<9, RTC Watch dog Reset digital core*/
    INTRUSION_RESET = 10,        /**<10, Instrusion tested to reset CPU*/
    TGWDT_CPU_RESET = 11,        /**<11, Time Group reset CPU*/
    SW_CPU_RESET = 12,           /**<12, Software reset CPU*/
    RTCWDT_CPU_RESET = 13,       /**<13, RTC Watch dog Reset CPU*/
    EXT_CPU_RESET = 14,          /**<14, for APP CPU, reseted by PRO CPU*/
    RTCWDT_BROWN_OUT_RESET = 15, /**<15, Reset when the vdd voltage is not stable*/
    RTCWDT_RTC_RESET = 16        /**<16, RTC Watch dog reset digital core and rtc module*/
};

ResetReason getResetReason(int cpu_no)
{
    return (ResetReason)rtc_get_reset_reason(cpu_no);
}

bool hasCrashed(const ResetReason &reason)
{
    return reason == ResetReason::OWDT_RESET ||
           reason == ResetReason::TG0WDT_SYS_RESET ||
           reason == ResetReason::TG1WDT_SYS_RESET ||
           reason == ResetReason::RTCWDT_SYS_RESET ||
           reason == ResetReason::RTCWDT_CPU_RESET;
}

void printResetReason(ResetReason reason)
{
    switch (reason)
    {
    case ResetReason::POWERON_RESET: /**<1, Vbat power on reset*/
        Serial.println("POWERON_RESET");
        break;
    case ResetReason::SW_RESET: /**<3, Software reset digital core*/
        Serial.println("SW_RESET");
        break;
    case ResetReason::OWDT_RESET: /**<4, Legacy watch dog reset digital core*/
        Serial.println("OWDT_RESET");
        break;
    case ResetReason::DEEPSLEEP_RESET: /**<5, Deep Sleep reset digital core*/
        Serial.println("DEEPSLEEP_RESET");
        break;
    case ResetReason::SDIO_RESET: /**<6, Reset by SLC module, reset digital core*/
        Serial.println("SDIO_RESET");
        break;
    case ResetReason::TG0WDT_SYS_RESET: /**<7, Timer Group0 Watch dog reset digital core*/
        Serial.println("TG0WDT_SYS_RESET");
        break;
    case ResetReason::TG1WDT_SYS_RESET: /**<8, Timer Group1 Watch dog reset digital core*/
        Serial.println("TG1WDT_SYS_RESET");
        break;
    case ResetReason::RTCWDT_SYS_RESET: /**<9, RTC Watch dog Reset digital core*/
        Serial.println("RTCWDT_SYS_RESET");
        break;
    case ResetReason::INTRUSION_RESET: /**<10, Instrusion tested to reset CPU*/
        Serial.println("INTRUSION_RESET");
        break;
    case ResetReason::TGWDT_CPU_RESET: /**<11, Time Group reset CPU*/
        Serial.println("TGWDT_CPU_RESET");
        break;
    case ResetReason::SW_CPU_RESET: /**<12, Software reset CPU*/
        Serial.println("SW_CPU_RESET");
        break;
    case ResetReason::RTCWDT_CPU_RESET: /**<13, RTC Watch dog Reset CPU*/
        Serial.println("RTCWDT_CPU_RESET");
        break;
    case ResetReason::EXT_CPU_RESET: /**<14, for APP CPU, reseted by PRO CPU*/
        Serial.println("EXT_CPU_RESET");
        break;
    case ResetReason::RTCWDT_BROWN_OUT_RESET: /**<15, Reset when the vdd voltage is not stable*/
        Serial.println("RTCWDT_BROWN_OUT_RESET");
        break;
    case ResetReason::RTCWDT_RTC_RESET: /**<16, RTC Watch dog reset digital core and rtc module*/
        Serial.println("RTCWDT_RTC_RESET");
        break;
    default:
        Serial.println("NO_MEAN");
    }
}

void safeBoot()
{
    ResetReason reset_cpu0 = getResetReason(0);
    ResetReason reset_cpu1 = getResetReason(1);

    Serial.print("CPU0 reset reason: ");
    printResetReason(reset_cpu0);

    Serial.print("CPU1 reset reason: ");
    printResetReason(reset_cpu1);

    bool crashed = false;

    if (hasCrashed(reset_cpu0) || hasCrashed(reset_cpu1)) // esp crashed! safe mode
    {
        Serial << "ESP CRASHED!"
               << endl
               << "BOOTING IN SAVE MODE"
               << endl;

        crashed = true;
    }

    WiFiManager wm;
    wm.autoConnect();
    setupOTA();

    if (crashed)
    {
        while (1)
        {
            loopOTA();
        }
    }
}

#endif