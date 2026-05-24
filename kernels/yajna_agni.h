/*
 * SUTRA 70: YAJNA-AGNI — Sacrificial Fire / Dynamic Energy Allocator
 * 
 * Yajna = Sacred sacrifice. Agni = Fire/Energy.
 * Dynamically allocates CPU cycles across all Sutras based on Guna state.
 * 
 * Sattva: Full power to Buddhi + Vedalytics (wisdom + prediction)
 * Rajas:  Power to Manas + Ahamkara (focus + action)
 * Tamas:  All power to Sushruta + Dhanvantara (self-healing)
 * 
 * The fire burns brightest where Dharma needs it most.
 */

#pragma once
#include "guna_siddhi.h"
#include "prana_nadi_power.h"
#include <cstdio>
#include <cstring>

#define PHI 1.618033988749895f

struct YajnaAgni {
    struct SutraAllocation {
        const char* sutra_name;
        float cpu_share;       // 0-1: fraction of total compute
        bool active;           // Is this sutra currently running?
        const char* reason;    // Why this allocation
    };
    
    SutraAllocation allocations[16];
    int alloc_count;
    PranaNadiGovernor* prana;
    GunaSiddhi* guna;
    float total_cpu_budget;
    
    YajnaAgni() : alloc_count(0), prana(nullptr), guna(nullptr), total_cpu_budget(1.0f) {
        memset(allocations, 0, sizeof(allocations));
    }
    
    void attach(PranaNadiGovernor* p, GunaSiddhi* g) { prana=p; guna=g; }
    
    // Register sutras that need CPU time
    void register_sutra(const char* name) {
        if(alloc_count>=16) return;
        allocations[alloc_count].sutra_name = name;
        allocations[alloc_count].cpu_share = 0;
        allocations[alloc_count].active = false;
        alloc_count++;
    }
    
    // Distribute Agni (CPU fire) based on current Guna
    void distribute() {
        if(!guna) return;
        
        // Reset all
        for(int i=0;i<alloc_count;i++) allocations[i].cpu_share = 0;
        
        float remaining = total_cpu_budget;
        
        switch(guna->current_guna) {
            case GUNA_SATTVA:
                // Wisdom + Prediction get the fire
                give("Buddhi", 0.25f, "Optimal conditions — plan strategically");
                give("Vedalytics", 0.20f, "Correlate patterns for insight");
                give("Veda-Siddhi", 0.15f, "Distill wisdom permanently");
                give("Ahamkara", 0.15f, "Execute with confidence");
                give("Manas", 0.10f, "Monitor all sensors");
                give("Chitta", 0.10f, "Strengthen instincts");
                remaining = 0.05f;
                break;
                
            case GUNA_RAJAS:
                // Action + Focus get the fire
                give("Manas", 0.30f, "High activity — filter noise aggressively");
                give("Ahamkara", 0.25f, "Act with caution");
                give("Karma-Kanda", 0.15f, "Track outcomes carefully");
                give("Pratyabhijna", 0.15f, "Learn from rapid changes");
                give("Indra-Mesh", 0.10f, "Stay connected to valley");
                remaining = 0.05f;
                break;
                
            case GUNA_TAMAS:
                // Healing + Protection get the fire
                give("Sushruta", 0.35f, "CRITICAL: Self-repair active");
                give("Dhanvantara", 0.25f, "Network healing pulse");
                give("Kavach", 0.20f, "Identity protection lockdown");
                give("Akashic", 0.10f, "Preserve all records");
                give("Moksha", 0.05f, "Prepare for possible rebirth");
                remaining = 0.05f;
                break;
        }
        
        // Distribute remaining to Prana-Nadi idle
        if(prana && remaining > 0.1f) {
            prana->on_idle();
        }
    }
    
    void give(const char* name, float share, const char* reason) {
        for(int i=0;i<alloc_count;i++) {
            if(strstr(allocations[i].sutra_name, name)) {
                allocations[i].cpu_share = share;
                allocations[i].active = true;
                allocations[i].reason = reason;
                return;
            }
        }
    }
    
    void report() {
        printf("\n🔥 YAJNA-AGNI — Dynamic Energy Allocation\n");
        printf("  ========================================\n");
        printf("  Guna: %s | Budget: %.0f%%\n", guna?guna->guna_name():"Unknown", total_cpu_budget*100);
        float used=0;
        for(int i=0;i<alloc_count;i++) {
            if(allocations[i].active) {
                printf("  %-15s: %.0f%% — %s\n", allocations[i].sutra_name,
                       allocations[i].cpu_share*100, allocations[i].reason);
                used += allocations[i].cpu_share;
            }
        }
        printf("  %-15s: %.0f%% — Idle/Reserve\n", "Shunya", (1.0f-used)*100);
    }
};
