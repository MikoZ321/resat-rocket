#include "spi_flash.h"

#include <Arduino.h>
#include <cstddef>
#include <SPIMemory.h>
#include <SPI.h>

#include "config.h"

static SPIFlash s_flash_memory(SPI_FLASH_CS_PIN);

// Each sector is 4096 bytes on the W25Q64
static const std::uint32_t SECTOR_SIZE = 4096;
static const std::uint32_t META_SECTOR = 0; // session header + write ptr
static const std::uint32_t INDEX_SECTOR = 1; // reserved
static const std::uint32_t DATA_START_SECTOR = 2;
static const std::uint32_t DATA_END_SECTOR = 2046;
static const std::uint32_t RECOVERY_SECTOR = 2047;

static const std::uint32_t DATA_START_ADDR = DATA_START_SECTOR * SECTOR_SIZE;
static const std::uint32_t DATA_END_ADDR = DATA_END_SECTOR * SECTOR_SIZE;
static const std::uint32_t DATA_CAPACITY = DATA_END_ADDR - DATA_START_ADDR;
static const std::uint32_t RECOVERY_ADDR = RECOVERY_SECTOR * SECTOR_SIZE;
static const std::uint32_t META_ADDR = META_SECTOR * SECTOR_SIZE;

static const std::uint32_t META_MAGIC = 0xF1A5F1A5UL;
static const std::uint8_t  FW_VERSION = 1;
static const std::uint8_t  FRAME_FMT_VERSION = 1;
static const std::uint32_t JEDEC_W25Q64 = 0xEF4017;

// runtime state
static std::uint32_t s_write_pointer = 0; // byte offset from DATA_START_ADDR
static std::uint32_t s_session_id = 0;
static bool s_is_initialized = false;

static void readMetadata() {
    std::uint32_t magic = 0;
    s_flash_memory.readByteArray(META_ADDR, (std::uint8_t*) &magic, 4);

    if (magic != META_MAGIC) {
        s_write_pointer = 0;
        s_session_id = 0;
        return;
    }

    s_flash_memory.readByteArray(META_ADDR + 4,  (std::uint8_t*)&s_session_id, 4);
    s_flash_memory.readByteArray(META_ADDR + 8,  (std::uint8_t*)&s_write_pointer,  4);
    s_session_id++;
}

static void writeMetadata() {
    s_flash_memory.eraseSector(META_ADDR);

    std::uint32_t magic = META_MAGIC;
    s_flash_memory.writeByteArray(META_ADDR, (std::uint8_t*) &magic, 4);
    s_flash_memory.writeByteArray(META_ADDR + 4, (std::uint8_t*) &s_session_id, 4);
    s_flash_memory.writeByteArray(META_ADDR + 8, (std::uint8_t*) &s_write_pointer, 4);
    s_flash_memory.writeByte(META_ADDR + 12, FW_VERSION);
    s_flash_memory.writeByte(META_ADDR + 13, FRAME_FMT_VERSION);
}

static void appendBytes(const std::uint8_t* data, std::size_t length) {
    if (s_write_pointer + length > DATA_CAPACITY) return;

    std::uint32_t address = DATA_START_ADDR + s_write_pointer;
    s_flash_memory.writeByteArray(address, (std::uint8_t*)data, length);
    s_write_pointer += length;
}

namespace spi_flash {
    bool begin() {
        s_is_initialized = false;

        if (!s_flash_memory.begin()) return false;

        std::uint32_t jedec = s_flash_memory.getJEDECID();
        if (jedec != JEDEC_W25Q64) return false;

        readMetadata();

        s_is_initialized = true;
        return true;
    }

    void eraseFullMemory() {
        for (std::uint32_t sector = DATA_START_SECTOR; sector <= DATA_END_SECTOR; sector++) {
            s_flash_memory.eraseSector(sector * SECTOR_SIZE);
        }
        // Also erase recovery sector
        s_flash_memory.eraseSector(RECOVERY_ADDR);

        // Write fresh session header
        s_write_pointer  = 0;
        s_session_id = 0;
        writeMetadata();
    }

    void writeFullTelemetryFrame(const full_telemetry_frame_t& full_frame) {
        if (!s_is_initialized) return;

        appendBytes((const std::uint8_t*) &full_frame, sizeof(full_frame));
    }

    void writeMiniTelemetryFrame(const mini_telemetry_frame_t& mini_frame) {
        if (!s_is_initialized) return;

        appendBytes((const std::uint8_t*) &mini_frame, sizeof(mini_frame));
    }

    void periodicMaintenance(std::uint32_t slow_tick_number) {
        if (!s_is_initialized) return;


        if (slow_tick_number % WRITE_POINTER_FLUSH_TICKS == 0) {
            writeMetadata();
        }

        if (slow_tick_number % FLASH_MIRROR_TICKS == 0 && s_write_pointer >= SECTOR_SIZE) {
            std::uint32_t mirrorStart = DATA_START_ADDR + s_write_pointer - SECTOR_SIZE;
            std::uint8_t buf[SECTOR_SIZE];
            s_flash_memory.readByteArray(mirrorStart, buf, SECTOR_SIZE);
            s_flash_memory.eraseSector(RECOVERY_ADDR);
            s_flash_memory.writeByteArray(RECOVERY_ADDR, buf, SECTOR_SIZE);
        }
    }

    void dumpToSerial() {
        if (!s_is_initialized) return; 
 
        Serial.print("[Flash] Dumping ");
        Serial.print(s_write_pointer);
        Serial.println(" bytes...");
        const std::uint32_t CHUNK = 256;
        std::uint8_t buf[CHUNK];
        for (std::uint32_t offset = 0; offset < s_write_pointer; offset += CHUNK) {
            std::uint32_t len = min((std::uint32_t)CHUNK, s_write_pointer - offset);
            s_flash_memory.readByteArray(DATA_START_ADDR + offset, buf, len);
            Serial.write(buf, len);
        }
        Serial.println("[Flash] Dump complete.");
    }
}