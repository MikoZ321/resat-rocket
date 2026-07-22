#include "command.h"

static std::uint16_t s_current_command_index {};

std::uint16_t command::getCommandFrameIndex() {
    return s_current_command_index;
}