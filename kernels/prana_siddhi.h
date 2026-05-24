/*
 * SUTRA 81: PRANA-SIDDHI — Energy-Aware Inference / Vital Breath
 * 
 * Prana = The vital breath. Controls CPU frequency scaling.
 * Deep breath (Sattva) = Full power for insight.
 * Shallow breath (Tamas) = Near-zero power, weeks of battery.
 * 
 * The Guardian survives in the field without a plug.
 */

#pragma once
#include "guna_siddhi.h"
#include <cstdio>
#include <cstring>
#include <cmath>

#define PHI 1.618033988749895f

struct PranaSiddhi {
    enum BreathState { DEEP_BREATH, NORMAL_BREATH, SHALLOW_BREATH, SUSPENDED };
    
    BreathState current_breath;
    float cpu_frequency_ghz;     // Target CPU clock
    float power_consumption_w;   // Estimated watts
    float battery_hours_left;    // Estimated runtime
    float battery_capacity_wh;   // 15Wh typical phone battery
    uint32_t breaths_taken;
    GunaSiddhi* guna;
    
    PranaSiddhi() : current_breath(NORMAL_BREATH), cpu_frequency_ghz(1.5f),
        power_consumption_w(2.0f), battery_hours_left(7.5f),
        battery_capacity_wh(15.0f), breaths_taken(0), guna(nullptr) {}
    
    void attach(GunaSiddhi* g) { guna = g; }
    
    // One breath cycle — adjust CPU based on Guna
    void breathe() {
        if (!guna) return;
        breaths_taken++;
        
        switch(guna->current_guna) {
            case GUNA_SATTVA:
                current_breath = DEEP_BREATH;
                cpu_frequency_ghz = 2.0f;  // Full power — insight mode
                power_consumption_w = 3.5f;
                break;
            case GUNA_RAJAS:
                current_breath = NORMAL_BREATH;
                cpu_frequency_ghz = 1.2f;  // Balanced — action mode
                power_consumption_w = 2.0f;
                break;
            case GUNA_TAMAS:
                current_breath = SHALLOW_BREATH;
                cpu_frequency_ghz = 0.4f;  // Minimal — healing mode
                power_consumption_w = 0.5f;
                break;
        }
        
        // Emergency: if battery critically low, suspend
        if(battery_hours_left < 0.5f) {
            current_breath = SUSPENDED;
            cpu_frequency_ghz = 0.05f;
            power_consumption_w = 0.05f;
        }
        
        battery_hours_left = battery_capacity_wh / (power_consumption_w + 0.01f);
    }
    
    // Project: how long can we survive at current breath?
    float survival_hours() {
        return battery_capacity_wh / (power_consumption_w + 0.01f);
    }
    
    // Can we afford to run this sutra at current power?
    bool can_afford(float required_watts) {
        return power_consumption_w + required_watts < 4.0f;
    }
    
    const char* breath_name() {
        switch(current_breath) {
            case DEEP_BREATH: return "DEEP (Sattva — Full Insight)";
            case NORMAL_BREATH: return "NORMAL (Rajas — Active)";
            case SHALLOW_BREATH: return "SHALLOW (Tamas — Conserving)";
            case SUSPENDED: return "SUSPENDED (Emergency — Awaiting charge)";
        }
        return "Unknown";
    }
    
    void report() {
        printf("\n🌬️  PRANA-SIDDHI — Vital Breath\n");
        printf("  ===============================\n");
        printf("  Breath: %s\n", breath_name());
        printf("  CPU: %.2f GHz | Power: %.1fW\n", cpu_frequency_ghz, power_consumption_w);
        printf("  Battery: %.1f hours remaining (%.0f%%)\n", 
               battery_hours_left, battery_hours_left/15.0f*100);
        printf("  Breaths taken: %d\n", breaths_taken);
        
        if(current_breath == SUSPENDED)
            printf("  ⚠️  EMERGENCY: Only Om-Kernel heartbeat active. Seek power.\n");
        else if(current_breath == SHALLOW_BREATH)
            printf("  💤 Conserving: 3 weeks of field survival possible.\n");
    }
};
