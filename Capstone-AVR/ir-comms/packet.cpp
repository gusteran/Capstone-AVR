#include "packet.h"

uint8_t Packet::packetIndex = 0;
uint8_t Packet::SOURCE;

Packet::Packet() {
    for (int i = 0; i < PACKET_SIZE; i++) {
        packetMessage[i] = 0;
    }
    packetHeader = {0, 0, 0, 0};
    packetSize = PACKET_SIZE * BYTE_SIZE + sizeof(Packet_Header) * BYTE_SIZE;
    bitIndex = 0;
}

void Packet::init(byte data[PACKET_SIZE], uint32_t sizeData) {
    initPacketHelper(-1, data, sizeData);
}

void Packet::initPacketHelper(uint8_t destination, byte data[], uint32_t sizeData) {
	//TODO, Check that this can read the EE_PROM
    if (SOURCE == 0) SOURCE = eeprom_read_byte(EE_ADDR_ID);
    for (uint16_t i = 0; i < sizeData && i < PACKET_SIZE; i++) {
        packetMessage[i] = (i < sizeData) ? data[i] : 0;
    }
    bitIndex = 0;
    packetSize = sizeData * BYTE_SIZE + sizeof(Packet_Header) * BYTE_SIZE;  // bytes to bits
    uint8_t checksum = generateChecksum(data, sizeData);
    packetHeader = {SOURCE, destination, Packet::packetIndex++, checksum};

    // Serial.print("Transmit debug for size ");
    // Serial.println(size);
    // for (int i = 0; i < size; i++) {
    //     Serial.print((getBit(i)) ? "1 " : "0 ");
    // }
    // Serial.println();
}

bool Packet::getBit(unsigned index) const {
    if (index < 0 || index >= getFullSize()) {
        return false;
    } else if (index < BYTE_SIZE * 1) {
        return (packetHeader.source >> (index % BYTE_SIZE)) & 1;
    } else if (index < BYTE_SIZE * 2) {
        return (packetHeader.destination >> (index % BYTE_SIZE)) & 1;
    } else if (index < BYTE_SIZE * 3) {
        return (packetHeader.packetNumber >> (index % BYTE_SIZE)) & 1;
    } else if (index < BYTE_SIZE * 4) {
        return (packetHeader.checksum >> (index % BYTE_SIZE)) & 1;
    } else {
        index -= BYTE_SIZE * 4;
        return (packetMessage[index / BYTE_SIZE] >> (index % BYTE_SIZE)) & 1;
    }
    byte pack = packetMessage[index / BYTE_SIZE];
    int bit = index % BYTE_SIZE;
    return (pack >> bit) & 1;
}

void Packet::setBit(unsigned index, bool bit) {
    if (index < 0 || index >= getFullSize()) {
		// TODO: Error handling
    } else if (index < BYTE_SIZE * 1) {
        packetHeader.source |= bit << (index % BYTE_SIZE);
    } else if (index < BYTE_SIZE * 2) {
        index -= BYTE_SIZE * 1;
        packetHeader.destination |= bit << (index % BYTE_SIZE);
    } else if (index < BYTE_SIZE * 3) {
        index -= BYTE_SIZE * 2;
        packetHeader.packetNumber |= bit << (index % BYTE_SIZE);
    } else if (index < BYTE_SIZE * 4) {
        index -= BYTE_SIZE * 3;
        packetHeader.checksum |= bit << (index % BYTE_SIZE);
    } else {
        index -= BYTE_SIZE * 4;
        packetMessage[index / BYTE_SIZE] |= bit << (index % BYTE_SIZE);
    }
}

uint8_t Packet::generateChecksum(byte message[PACKET_SIZE]){
    return generateChecksum(message, PACKET_SIZE);
}

uint8_t Packet::generateChecksum(byte message[PACKET_SIZE], uint32_t sizeData) {
    uint8_t sum = 0;
    for (unsigned i = 0; i < PACKET_SIZE * BYTE_SIZE && i < sizeData * BYTE_SIZE; i++) {
        sum += (message[i / BYTE_SIZE] >> (i % BYTE_SIZE)) & 1;
    }
    return sum;
}

bool Packet::checkPacketValid() {
    return packetHeader.checksum == generateChecksum(packetMessage);
}

byte * Packet::getMessage() {
    return packetMessage;
}

char *Packet::getMessageString() {
    return (char *)packetMessage;
}