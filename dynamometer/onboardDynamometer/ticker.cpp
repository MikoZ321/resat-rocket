#include "ticker.h"
#include "config.h"
#include "esp_timer.h"

static volatile bool s_tick_is_ready = false;
static volatile std::uint32_t s_tick_count = 0;
static volatile std::uint32_t s_missed_tick_count = 0;

// Internal interrupt service routine for the hardware timer
// Executed every time the hardware timer triggers
// IRAM_ATTR to ensure that it does not get stuck in flash
static void onTick(void *) {
    if (s_tick_is_ready) s_missed_tick_count++;

    s_tick_is_ready = true;
    s_tick_count++;
}

namespace ticker {
    void begin() {
        esp_timer_handle_t timer_handler;
        esp_timer_create_args_t timer_arguments = {};

        timer_arguments.callback = onTick;
        timer_arguments.name = "tick";
        timer_arguments.dispatch_method = ESP_TIMER_TASK;

        esp_timer_create(&timer_arguments, &timer_handler);
        esp_timer_start_periodic(timer_handler, TICK_FAST_INTERVAL_MS * 1000ULL);
    }

    bool consume() {
        if (!s_tick_is_ready) return false;

        s_tick_is_ready = false;
        return true;
    }

    std::uint32_t getTickCount() {
        return s_tick_count;
    }

    std::uint32_t getMissedTickCount() {
        return s_missed_tick_count;
    }
}
