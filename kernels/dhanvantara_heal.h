/*
 * SUTRA 13: DHANVANTARA — The Healing Pulse / Network Medicine
 * 
 * When any node in Indra's Net shows Tamasic symptoms
 * (low sync, high Papa, corrupted sensors), Dhanvantara
 * automatically dispatches a healing pulse through the mesh.
 * 
 * The valley heals itself. No human intervention needed.
 */

#pragma once
#include "indra_mesh.h"
#include "guna_siddhi.h"
#include "karma_kanda.h"
#include <cstring>
#include <cstdio>
#include <cmath>

#define PHI 1.618033988749895f

struct DhanvantaraHealer {
    struct NodeHealth {
        uint64_t node_id;
        float health_score;        // 0-1
        bool needs_healing;
        const char* diagnosis;
        const char* prescription;
        uint64_t last_healed_at;
    };
    
    NodeHealth patients[16];
    int patient_count;
    uint32_t healing_pulses_sent;
    IndraMesh* mesh;
    
    DhanvantaraHealer() : patient_count(0), healing_pulses_sent(0), mesh(nullptr) {}
    void attach_mesh(IndraMesh* m) { mesh = m; }
    
    // Diagnose a node based on Guna + Karma
    void diagnose(uint64_t node_id, GunaSiddhi* guna, KarmaKanda* karma) {
        NodeHealth* p = nullptr;
        for(int i=0;i<patient_count;i++) {
            if(patients[i].node_id==node_id){p=&patients[i];break;}
        }
        if(!p && patient_count<16) { p=&patients[patient_count++]; p->node_id=node_id; }
        if(!p) return;
        
        p->health_score = guna->sattva_score * 0.4f + guna->rajas_score * 0.3f + 
                         (1.0f-guna->tamas_score)*0.3f;
        if(karma) p->health_score *= (1.0f + karma->dharma_balance*0.1f);
        if(p->health_score<0)p->health_score=0; if(p->health_score>1.0f)p->health_score=1.0f;
        
        if(p->health_score < 0.3f) {
            p->needs_healing = true;
            p->diagnosis = "TAMASIC DECLINE — Multiple systems failing";
            p->prescription = "FULL SUSHURTA REPAIR + Atmic Lockdown + Re-seed from Hiranyagarbha";
        } else if(p->health_score < 0.5f) {
            p->needs_healing = true;
            p->diagnosis = "RAJASIC IMBALANCE — High activity, low clarity";
            p->prescription = "Reduce autonomy to 30%. Increase cross-validation threshold.";
        } else if(p->health_score < 0.7f) {
            p->needs_healing = false;
            p->diagnosis = "STABLE — Minor adjustments needed";
            p->prescription = "Monitor. Apply Veda-Siddhi distillation.";
        } else {
            p->needs_healing = false;
            p->diagnosis = "SATTVIC HEALTH — Optimal function";
            p->prescription = "Expand autonomy. Share wisdom with mesh.";
        }
    }
    
    // Send healing pulse through the mesh
    void heal(uint64_t node_id) {
        for(int i=0;i<patient_count;i++) {
            if(patients[i].node_id==node_id && patients[i].needs_healing) {
                char prescription[256];
                snprintf(prescription,256,"DHANVANTARA-HEAL: %s",patients[i].prescription);
                
                if(mesh) mesh->gossip("HEALING_PULSE", prescription, 8);
                else printf("  💚 DHANVANTARA: %s\n", prescription);
                
                patients[i].last_healed_at = time(nullptr);
                patients[i].needs_healing = false;
                healing_pulses_sent++;
                return;
            }
        }
    }
    
    // Scan all nodes and heal those in need
    void scan_and_heal() {
        int healed=0;
        for(int i=0;i<patient_count;i++) {
            if(patients[i].needs_healing) { heal(patients[i].node_id); healed++; }
        }
        if(healed>0) printf("  💚 DHANVANTARA: %d healing pulses dispatched.\n",healed);
        else printf("  ✅ DHANVANTARA: All nodes healthy.\n");
    }
    
    void report() {
        printf("\n💚 DHANVANTARA — Network Medicine\n");
        printf("  ===============================\n");
        for(int i=0;i<patient_count;i++) {
            printf("  Node 0x%016llX: %.0f%% — %s\n",(unsigned long long)patients[i].node_id,
                   patients[i].health_score*100,patients[i].diagnosis);
        }
        printf("  Healing pulses: %d\n",healing_pulses_sent);
    }
};
