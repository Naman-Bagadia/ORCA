#pragma once
// ORCA radio protocol. Keep this file identical in boat/ and ground-station/.
//
// Ground -> boat:  C,<throttle -100..100>,<rudder -100..100>,<mode 0=manual 1=auto>,<cal 0/1>,<seq>*HH
// Boat -> ground:  T,<mode>,<vision 0=cam fail 1=no oil 2=suspected oil>,<visionX>,<turbRaw>,<turbDelta>,<state 0/1/2>,<seq>*HH
// HH = two hex digits, XOR of every character before the '*'.
// The boat only transmits as a reply to a command, so the two radios never talk over each other.

#include <Arduino.h>

inline uint8_t orcaChecksum(const String &s) {
  uint8_t c = 0;
  for (size_t i = 0; i < s.length(); i++) c ^= (uint8_t)s[i];
  return c;
}

inline String orcaWrap(const String &body) {
  char tail[6];
  snprintf(tail, sizeof(tail), "*%02X", orcaChecksum(body));
  return body + String(tail);
}

// Returns true and fills `body` (without checksum) only if the checksum matches.
inline bool orcaUnwrap(const String &packet, String &body) {
  int star = packet.lastIndexOf('*');
  if (star < 0 || packet.length() != (size_t)star + 3) return false;
  body = packet.substring(0, star);
  uint8_t expected = (uint8_t)strtol(packet.substring(star + 1).c_str(), nullptr, 16);
  return expected == orcaChecksum(body);
}
