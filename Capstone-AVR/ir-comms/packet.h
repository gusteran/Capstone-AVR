#ifndef GUS_PACKET_
#define GUS_PACKET_
#include <avr/eeprom.h>

#include "bitcoder.h"
#include "constants.h"
// #include "robot.h"

typedef struct packet_header {
    uint8_t source;
    uint8_t destination;
    uint8_t packetNumber;
    uint8_t checksum;
} Packet_Header;

class Packet {
   public:
    Packet();
    // Packet(byte data[PACKET_SIZE]);
    // Packet(uint8_t destination, byte data[PACKET_SIZE]);
    bool getBit(unsigned index) const;
    bool getNextBit() { return getBit(bitIndex++); }
    void setBit(unsigned index, bool bit);
    void setNextBit(bool bit) { setBit(bitIndex++, bit); }
    void setIndex(int index);
    bool atEnd() const { return bitIndex >= getFullSize(); }
    void resetBit() { bitIndex = 0; }

    void initPacketHelper(uint8_t destination, byte data[], uint32_t sizeData);
    void init(byte data[], uint32_t sizeData);
    static uint8_t generateChecksum(byte message[PACKET_SIZE]);
    bool checkPacketValid();
    byte* getMessage();
    char* getMessageString();
    Packet_Header getHeader() { return packetHeader; }

    uint32_t
    getFullSize() const { return PACKET_SIZE * BYTE_SIZE + sizeof(Packet_Header) * BYTE_SIZE; }

   private:
    uint32_t bitIndex;
    byte packetMessage[PACKET_SIZE];
    uint32_t packetSize;
    Packet_Header packetHeader;
    static uint8_t packetIndex;

    static uint8_t generateChecksum(byte message[PACKET_SIZE], uint32_t sizeData);

    static uint8_t SOURCE;
};

#endif