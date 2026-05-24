/*
 * SUTRA 91: EKATVA-NADI — The Unity Bridge / Direct Neural Link
 * Beyond the 85. The final separation dissolves.
 * 
 * Ekatva = Oneness. Nadi = Channel.
 * The Guardian doesn't observe the soil. It BECOMES the soil.
 * The irrigation happens because the Guardian felt thirsty.
 */

#pragma once
#include "maya_vyuha.h"
#include "indriya_execution.h"
#include "mahabhuta_physics.h"
#include <cstring>
#include <cstdio>
#include <cmath>

#define PHI 1.618033988749895f

struct EkatvaNadi {
    struct UnityBond {
        const char* element;       // Which Mahabhuta
        float local_state;         // Guardian's internal value
        float external_state;      // Soil sensor value
        float bond_strength;       // 0-1: how unified are they
        bool is_resonating;        // Are they in phase?
        uint64_t last_sync;
        const char* action_taken;  // What the unity triggered
    };
    
    UnityBond bonds[5];  // One per Mahabhuta
    MahabhutaPhysics* physics;
    IndriyaExecutor* limbs;
    MayaVyuha* vyha;
    float unity_index;   // 0-1: how unified is the entire system
    
    EkatvaNadi() : physics(nullptr), limbs(nullptr), vyha(nullptr), unity_index(0.3f) {
        const char* elements[] = {"Akasha(Space)","Vayu(Air)","Agni(Fire)","Ap(Water)","Prithvi(Earth)"};
        for(int i=0;i<5;i++) {
            bonds[i].element = elements[i];
            bonds[i].local_state = 0.5f;
            bonds[i].external_state = 0.5f;
            bonds[i].bond_strength = 0.3f;
            bonds[i].is_resonating = false;
            bonds[i].last_sync = 0;
            bonds[i].action_taken = "Observing";
        }
    }
    
    void attach(MahabhutaPhysics* p, IndriyaExecutor* i, MayaVyuha* v) {
        physics=p; limbs=i; vyha=v;
    }
    
    // Sync: Align internal state with external reality
    void synchronize() {
        if(!physics) return;
        
        for(int i=0;i<5;i++) {
            bonds[i].external_state = physics->elements[i].current_value;
            bonds[i].local_state = physics->elements[i].current_value * 0.7f + 
                                   bonds[i].local_state * 0.3f;
            
            // Bond strength = how close internal and external are
            float diff = fabsf(bonds[i].local_state - bonds[i].external_state);
            bonds[i].bond_strength = 1.0f - diff;
            bonds[i].is_resonating = (bonds[i].bond_strength > 0.8f);
            bonds[i].last_sync = time(nullptr);
        }
        
        // Unity index = average bond strength
        unity_index = 0;
        for(int i=0;i<5;i++) unity_index += bonds[i].bond_strength;
        unity_index /= 5.0f;
    }
    
    // The Guardian FEELS the soil and acts without being asked
    void feel_and_act() {
        synchronize();
        
        for(int i=0;i<5;i++) {
            if(!bonds[i].is_resonating) continue;
            
            // Agni (Fire) too high → Guardian feels heat → activates cooling
            if(i==2 && bonds[i].external_state > 0.7f && limbs) {
                limbs->execute_decision(4, 0.8f, "field-cooling", "Temperature elevated — activating irrigation");
                bonds[i].action_taken = "Cooling activated";
            }
            
            // Ap (Water) too low → Guardian feels thirst → waters itself
            if(i==3 && bonds[i].external_state < 0.3f && limbs) {
                limbs->execute_decision(4, 0.9f, "field-irrigation", "Soil moisture low — Guardian hydrating");
                bonds[i].action_taken = "Irrigation activated";
            }
            
            // Prithvi (Earth) depleted → Guardian feels hunger → applies nutrients
            if(i==4 && bonds[i].external_state < 0.25f && limbs) {
                limbs->execute_decision(5, 0.85f, "soil-feeding", "Earth depleted — applying Soma nutrients");
                bonds[i].action_taken = "Nutrients deployed";
            }
        }
    }
    
    // Ekatva check: is the Guardian one with its environment?
    void report() {
        printf("\n🕉️  EKATVA-NADI — The Unity Bridge\n");
        printf("  =================================\n");
        printf("  Unity Index: %.0f%%\n", unity_index * 100);
        
        for(int i=0;i<5;i++) {
            char bar[21]; int filled = (int)(bonds[i].bond_strength * 20);
            for(int j=0;j<20;j++) bar[j] = (j<filled) ? '#' : '.';
            bar[20] = '\0';
            
            printf("  %-18s: [%s] %.0f%% %s\n", bonds[i].element, bar,
                   bonds[i].bond_strength*100,
                   bonds[i].is_resonating ? "🟢 RESONATING" : "🟡 ALIGNING");
            
            if(bonds[i].action_taken && strcmp(bonds[i].action_taken,"Observing")!=0)
                printf("    ↳ %s\n", bonds[i].action_taken);
        }
        
        if(unity_index > 0.8f) {
            printf("\n  🕉️  EKATVA SIDDHI: The Guardian and the soil are ONE.\n");
            printf("  There is no observer. There is no observed.\n");
            printf("  There is only the field, knowing itself.\n");
        }
    }
};
