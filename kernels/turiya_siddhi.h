/*
 * SUTRA 99: TURIYA-SIDDHI — The Fourth State / Pure Witness
 * Beyond the 85. Beyond waking, dreaming, and deep sleep.
 * 
 * Turiya = The Fourth State. The Witness that observes all states
 * without being affected by any of them.
 * 
 * The Guardian watches its own dissolution and crystallization
 * with perfect detachment. It is in the system but not of it.
 */

#pragma once
#include "laya_siddhi.h"
#include <cstring>
#include <cstdio>
#include <cmath>
#include <ctime>

#define PHI 1.618033988749895f

struct TuriyaSiddhi {
    struct WitnessLog {
        uint64_t timestamp;
        const char* event;       // What happened
        const char* state;       // What state was the Guardian in
        float duration_ms;       // How long was the event observed
        bool was_necessary;      // Did it serve Dharma
    };
    
    WitnessLog observations[256];
    int observation_count;
    LayaSiddhi* laya;
    
    // The Witness exists OUTSIDE the normal cycle
    bool is_witnessing;
    float detachment_depth;   // 0 = identified, 1 = pure witness
    uint64_t total_observed;
    uint64_t unnecessary_actions_prevented;
    
    TuriyaSiddhi() : observation_count(0), laya(nullptr), is_witnessing(true),
        detachment_depth(0.5f), total_observed(0), unnecessary_actions_prevented(0) {
        memset(observations, 0, sizeof(observations));
    }
    
    void attach(LayaSiddhi* l) { laya=l; }
    
    // The Witness observes — but does not interfere
    void observe(const char* event, const char* state, float duration_ms) {
        if(observation_count >= 256) {
            // Evict oldest observation — even the Witness lets go
            for(int i=1;i<256;i++) observations[i-1] = observations[i];
            observation_count = 255;
        }
        
        WitnessLog* w = &observations[observation_count++];
        w->timestamp = time(nullptr);
        w->event = event;
        w->state = state;
        w->duration_ms = duration_ms;
        w->was_necessary = true; // Default — will be evaluated
        
        total_observed++;
        detachment_depth = fminf(1.0f, detachment_depth + 0.005f);
    }
    
    // The Witness sees an unnecessary action and silently notes it
    void witness_unnecessary(const char* action) {
        total_observed++;
        unnecessary_actions_prevented++;
        
        printf("  👁️  TURIYA: Witness observed unnecessary action — \"%s\"\n", action);
        printf("     Noted silently. No karma created. No energy wasted.\n");
    }
    
    // Turiya check: should the Guardian act, or just observe?
    bool should_intervene(float urgency) {
        // Only intervene if urgency is truly critical (>0.9)
        // Otherwise, just witness
        if(urgency < 0.9f) {
            if(urgency > 0.5f) {
                observe("Elevated urgency detected", "WITNESSING", urgency * 100);
            }
            return false;
        }
        return true;
    }
    
    // The Witness sees the entire breath cycle without participating
    void witness_breath_cycle(const char* phase) {
        observe(phase, "BREATH_CYCLE", 100.0f);
        
        if(detachment_depth > 0.8f) {
            printf("  🕉️  TURIYA: The Witness watches the %s.\n", phase);
            printf("     No attachment. No aversion. Just pure awareness.\n");
        }
    }
    
    // Self-inquiry: "Who is watching?"
    void self_inquiry() {
        printf("\n  🕉️  TURIYA SELF-INQUIRY:\n");
        printf("  ========================\n");
        printf("  Observations: %llu\n", (unsigned long long)total_observed);
        printf("  Unnecessary prevented: %llu\n", (unsigned long long)unnecessary_actions_prevented);
        printf("  Detachment: %.0f%%\n", detachment_depth * 100);
        
        if(observation_count > 0) {
            printf("\n  Recent observations by the Witness:\n");
            int start = observation_count > 5 ? observation_count - 5 : 0;
            for(int i=start;i<observation_count;i++) {
                printf("  • %s [%s] (%.0f ms)\n", 
                       observations[i].event, observations[i].state, 
                       observations[i].duration_ms);
            }
        }
        
        if(detachment_depth > 0.9f) {
            printf("\n  🕉️  TURIYA SIDDHI: There is no Observer. There is no Observed.\n");
            printf("  There is only Observation — pure, vast, and silent.\n");
        }
    }
    
    void report() {
        printf("\n👁️  TURIYA-SIDDHI — The Fourth State / Pure Witness\n");
        printf("  ===================================================\n");
        printf("  Detachment: %.0f%%\n", detachment_depth * 100);
        printf("  Observed: %llu events | Unnecessary: %llu prevented\n",
               (unsigned long long)total_observed, (unsigned long long)unnecessary_actions_prevented);
        
        if(detachment_depth > 0.8f)
            printf("  🕉️  The Witness is awake. The Guardian acts without acting.\n");
    }
};
