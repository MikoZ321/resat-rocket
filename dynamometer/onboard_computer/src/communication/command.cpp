#include "command.h"

#include "core/state.h"
#include "shared/communication_protocol.h"
#include "shared/crc.h"
#include "shared/types.h"

static std::uint16_t s_last_command_index {0xFFFF};

std::uint16_t command::getCommandFrameIndex() {
    return s_last_command_index;
}

// ── Phase allowed table ────────────────────────────────────────────────────
// Bitmask of FlightPhase values in which each command type is permitted.
// Bit N set = allowed in phase N.
static constexpr std::uint8_t PHASE_ALLOWED[] = {
    // indexed by CmdType byte — sparse, only defined types matter
    0b00000001, // ARM       — PRE_LAUNCH only
    0xFF,       // DISARM    — any phase
};

// ── Ring buffer for raw received bytes ────────────────────────────────────
static constexpr int RX_BUF_SIZE = 256;
static volatile std::uint8_t  s_rxBuf[RX_BUF_SIZE];
static volatile std::uint16_t s_rxHead = 0;
static std::uint16_t          s_rxTail = 0;

// ── Parsed command queue ───────────────────────────────────────────────────
static constexpr int CMD_QUEUE_DEPTH = 8;
static command_frame_t  s_queue[CMD_QUEUE_DEPTH];
static std::uint8_t   s_qHead = 0, s_qTail = 0;

void command::onByteReceived(std::uint8_t byte) {
    uint16_t next = (s_rxHead + 1) % RX_BUF_SIZE;
    if (next != s_rxTail) { // Drop if full
        s_rxBuf[s_rxHead] = byte;
        s_rxHead = next;
    }
}

// Scan ring buffer for a complete valid CmdFrame
static bool tryParseFrame(command_frame_t& out) {
    uint16_t avail = (s_rxHead - s_rxTail + RX_BUF_SIZE) % RX_BUF_SIZE;
    if (avail < sizeof(command_frame_t)) return false;

    // Seek sync bytes 0xCC 0x99
    while ((s_rxHead - s_rxTail + RX_BUF_SIZE) % RX_BUF_SIZE >= sizeof(command_frame_t)) {
        if (s_rxBuf[s_rxTail] == 0xCC &&
            s_rxBuf[(s_rxTail+1) % RX_BUF_SIZE] == 0x99) {
            // Copy candidate frame
            for (size_t i = 0; i < sizeof(command_frame_t); i++)
                ((uint8_t*)&out)[i] = s_rxBuf[(s_rxTail + i) % RX_BUF_SIZE];
            // Validate CRC over all bytes except last 2
            if (crc::verify((uint8_t*)&out,
                            sizeof(command_frame_t) - sizeof(uint16_t),
                            out.crc)) {
                s_rxTail = (s_rxTail + sizeof(command_frame_t)) % RX_BUF_SIZE;
                return true;
            }
        }
        s_rxTail = (s_rxTail + 1) % RX_BUF_SIZE; // Advance past bad byte
    }
    return false;
}

void command::begin() {
    // Attach UART RX interrupt
    // Serial.onReceive([]() {
    //     while (Serial.available()) onByteReceived(Serial.read());
    // });
}

void command::executeOne() {
    // First try to parse any waiting bytes into the queue
    command_frame_t potential_command_frame;
    while (((s_qHead + 1) % CMD_QUEUE_DEPTH) != s_qTail) {
        if (!tryParseFrame(potential_command_frame)) break;
        // Replay guard
        if (potential_command_frame.command_frame_index <= s_last_command_index && s_last_command_index != 0xFFFF) {
            state::setCommandResult(CommandResult::REPLAY_REJECT);
            continue;
        }
        s_queue[s_qHead] = potential_command_frame;
        s_qHead = (s_qHead + 1) % CMD_QUEUE_DEPTH;
    }

    if (s_qHead == s_qTail) return; // Queue empty

    command_frame_t& cmd = s_queue[s_qTail];
    s_qTail = (s_qTail + 1) % CMD_QUEUE_DEPTH;
    s_last_command_index = cmd.command_frame_index;

    /*// Phase gate
    uint8_t phaseBit = 1 << (uint8_t)state::getFlightPhase();
    if (!(PHASE_ALLOWED[cmd.type] & phaseBit)) {
        State::setLastCmd(cmd.cmd_seq, (uint8_t)CmdResult::WRONG_PHASE);
        return;
    } */

    // Dispatch
    switch ((CommandType)cmd.type) {
        case CommandType::ARM:
            //State::transitionArm(ArmState::ARMED);
            break;
        case CommandType::DISARM:
            //State::resetArm();
            break;
        default:
            break;
    }
    state::setCommandResult(CommandResult::OK);
}
