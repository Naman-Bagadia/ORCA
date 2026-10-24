#pragma once
// ===================== GROUND STATION ESP32 CONFIG =====================
// PROPOSED default pins for a standard ESP32 DevKit. Change to match your wiring.

#define PIN_LORA_SCK   18
#define PIN_LORA_MISO  19
#define PIN_LORA_MOSI  23
#define PIN_LORA_NSS    5
#define PIN_LORA_RST   14
#define PIN_LORA_DIO0  26

#define LORA_FREQ_HZ        433E6
#define LORA_SYNC_WORD      0xA5   // must match the boat
#define LORA_TX_POWER_DBM   14

#define CMD_PERIOD_MS       250    // how often a command is sent
#define NO_REPLY_MS         2000   // warn if the boat has not answered for this long
#define PRINT_EVERY_N       4      // print every Nth telemetry packet (4 -> about once a second)

#define THROTTLE_STEP       10
#define RUDDER_STEP         20
#define MAX_THROTTLE_PCT    60     // keep equal to or below the boat's cap
