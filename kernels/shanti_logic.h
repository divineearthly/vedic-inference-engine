/*
 * SUTRA 82: SHANTI-LOGIC — Temporal Smoothing / Calm Inference
 * 
 * Shanti = Peace that surpasses understanding.
 * Prevents jittery decisions. The Guardian doesn't panic.
 * 
 * A single flickering sensor doesn't trigger alarms.
 * Only sustained evidence moves the mind.
 * Like the Barak river in winter — steady, deep, calm.
 */

#pragma once
#include <cstring>
#include <cstdio>
#include <cmath>
#include <ctime>

#define PHI 1.618033988749895f
#define SHANTI_WINDOW 8  // Samples before mind changes

struct ShantiLogic {
    struct SmoothedSignal {
        float history[SHANTI_WINDOW];
        int idx;
        float current_smoothed;
        float variance;
        bool is_stable;
        const char* name;
        int consecutive_confirmations; // How many times in a row the signal agreed
    };
    
    SmoothedSignal signals[8];
    int signal_count;
    float global_calmness;  // 0-1: how calm is the entire system
    int false_alarms_prevented;
    
    ShantiLogic() : signal_count(0), global_calmness(0.5f), false_alarms_prevented(0) {
        memset(signals, 0, sizeof(signals));
    }
    
    void register_signal(const char* name) {
        if(signal_count>=8) return;
        signals[signal_count].name = name;
        signals[signal_count].is_stable = false;
        signal_count++;
    }
    
    // Feed a new raw value — returns the smoothed, calm value
    float observe(const char* name, float raw_value) {
        SmoothedSignal* s = nullptr;
        for(int i=0;i<signal_count;i++) {
            if(strcmp(signals[i].name,name)==0) { s=&signals[i]; break; }
        }
        if(!s) { register_signal(name); s=&signals[signal_count-1]; }
        
        // Add to history
        s->history[s->idx] = raw_value;
        s->idx = (s->idx + 1) % SHANTI_WINDOW;
        
        // Exponential moving average (PHI-weighted for smoothness)
        float avg = 0; float total_w = 0;
        for(int i=0;i<SHANTI_WINDOW;i++) {
            float w = 1.0f / (1.0f + i * 0.5f); // Older = less weight
            avg += s->history[(s->idx - 1 - i + SHANTI_WINDOW) % SHANTI_WINDOW] * w;
            total_w += w;
        }
        s->current_smoothed = (total_w > 0) ? avg / total_w : raw_value;
        
        // Compute variance
        float var = 0;
        for(int i=0;i<SHANTI_WINDOW;i++) {
            float diff = s->history[i] - s->current_smoothed;
            var += diff * diff;
        }
        s->variance = var / SHANTI_WINDOW;
        s->is_stable = (s->variance < 0.05f); // Low variance = stable signal
        
        // Count consecutive confirmations
        if(fabsf(raw_value - s->current_smoothed) < 0.15f) s->consecutive_confirmations++;
        else s->consecutive_confirmations = 0;
        
        // Global calmness = average stability
        global_calmness = 0;
        for(int i=0;i<signal_count;i++) global_calmness += signals[i].is_stable ? 1.0f : 0.0f;
        global_calmness /= (signal_count > 0 ? signal_count : 1);
        
        return s->current_smoothed;
    }
    
    // Shanti-check: is it safe to act on this signal?
    bool is_calm_enough(const char* name, int required_confirmations = 3) {
        for(int i=0;i<signal_count;i++) {
            if(strcmp(signals[i].name,name)==0) {
                bool calm = signals[i].consecutive_confirmations >= required_confirmations;
                if(!calm) false_alarms_prevented++;
                return calm;
            }
        }
        return false; // Unknown signal — don't act
    }
    
    // Wait for calm before acting
    const char* wait_for_calm(const char* name) {
        for(int i=0;i<signal_count;i++) {
            if(strcmp(signals[i].name,name)==0) {
                if(signals[i].is_stable) return "STABLE — Proceed with Dharma.";
                else if(signals[i].consecutive_confirmations >= 2) return "CALMING — Wait briefly.";
                else return "AGITATED — Do not act. Observe.";
            }
        }
        return "UNKNOWN — Silence.";
    }
    
    void report() {
        printf("\n🧘 SHANTI-LOGIC — The Calm Mind\n");
        printf("  ===============================\n");
        printf("  Global Calmness: %.0f%%\n", global_calmness * 100);
        printf("  False alarms prevented: %d\n", false_alarms_prevented);
        for(int i=0;i<signal_count;i++) {
            printf("  %-15s: %.2f (var:%.3f) %s | Confirmations: %d\n",
                   signals[i].name, signals[i].current_smoothed,
                   signals[i].variance,
                   signals[i].is_stable ? "🟢 STABLE" : "🟡 AGITATED",
                   signals[i].consecutive_confirmations);
        }
    }
};
