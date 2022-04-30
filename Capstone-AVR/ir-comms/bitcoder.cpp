#include "bitcoder.h"

void decodeBytes(byte *msg, bool *buffer, unsigned packetSize) {
    for (unsigned int i = 0; i < packetSize; i++) {
        for (int j = 0; j < 8; j++) {
            msg[i] |= buffer[i * 8 + j] << (7 - j);
        }
    }
}

void encodeBytes(byte *msg, bool *buffer, unsigned msgSize) {
    int bufNum = 0;
    for (unsigned int i = 0; i < msgSize; i++) {
        for (int j = 7; j >= 0; j--) {
            buffer[bufNum++] = (msg[i] >> j) & 1;
        }
    }
}

void decodeCharset(char *msg, bool *buffer, unsigned packetSize) {
    decodeBytes((byte *)msg, buffer, packetSize);
}

void encodeCharset(char *msg, bool *buffer, unsigned msgSize) {
    encodeBytes((byte *)msg, buffer, msgSize);
}