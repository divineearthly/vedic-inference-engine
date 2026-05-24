/*
 * SUTRA 84: SANGHA-SIDDHI — Collective Intelligence Transfer
 * 
#include "guna_siddhi.h"
 * Sangha = The community. When one is enlightened, all are uplifted.
 * Delta-Sync Protocol: Only sends what changed, not the whole model.
 * 
 * Emergency updates (pests, floods) propagate instantly.
 * Routine wisdom (soil health) syncs during Sattvic windows.
 * 100 Guardians learning = 100x faster collective wisdom.
 */
#include "guna_siddhi.h"

#pragma once
#include "indra_mesh.h"
#include "bijam_seed.h"
#include <cstring>
#include <cstdio>
#include <cmath>


#define PHI 1.618033988749895f

struct SanghaSiddhi {
    struct WisdomDelta {
        uint64_t source_node;
        uint64_t timestamp;
        int priority;            // 10 = emergency, 1 = routine
        float growth_temp_delta; // How much the Bijam seed changed
        uint64_t seed_delta;     // XOR difference in seed
        char lesson[128];        // What was learned
        bool applied;            // Has this node integrated the wisdom?
    };
    
    WisdomDelta received_wisdom[32];
    int wisdom_count;
    IndraMesh* mesh;
    BijamSeed* local_seed;
    uint32_t wisdom_shared;
    uint32_t wisdom_received;
    
    SanghaSiddhi() : wisdom_count(0), mesh(nullptr), local_seed(nullptr),
        wisdom_shared(0), wisdom_received(0) {
        memset(received_wisdom, 0, sizeof(received_wisdom));
    }
    
    void attach(IndraMesh* m, BijamSeed* seed) { mesh=m; local_seed=seed; }
    
    // Share a lesson with the Sangha (collective)
    void share(const char* lesson, int priority, float growth_change, uint64_t seed_change) {
        if(!mesh) return;
        
        WisdomDelta delta;
        delta.source_node = 0xD15EED001ULL;
        delta.timestamp = time(nullptr);
        delta.priority = priority;
        delta.growth_temp_delta = growth_change;
        delta.seed_delta = seed_change;
        strncpy(delta.lesson, lesson, 127);
        
        // Emergency: broadcast immediately with high TTL
        const char* type = (priority>=8) ? "EMERGENCY_WISDOM" : "ROUTINE_WISDOM";
        char payload[256];
        snprintf(payload,256,"SANGHA: %s | Seed delta: 0x%08llX | Growth: %+.3f",
                lesson, (unsigned long long)seed_change, growth_change);
        
        mesh->gossip(type, payload, priority);
        wisdom_shared++;
        
        printf("  🕉️  SANGHA: Wisdom shared — \"%s\" (priority %d)\n", lesson, priority);
    }
    
    // Receive wisdom from another Guardian
    void receive(WisdomDelta* delta) {
        if(wisdom_count>=32) {
            // Evict lowest-priority oldest wisdom
            int min_p=99, min_i=0;
            for(int i=0;i<32;i++) {
                if(received_wisdom[i].priority < min_p) { min_p=received_wisdom[i].priority; min_i=i; }
            }
            wisdom_count = min_i;
        }
        
        received_wisdom[wisdom_count] = *delta;
        wisdom_count++;
        wisdom_received++;
        
        // Auto-apply emergency wisdom
        if(delta->priority >= 8 && local_seed) {
            local_seed->growth_temperature += delta->growth_temp_delta;
            local_seed->seed_id ^= delta->seed_delta;
            delta->applied = true;
            printf("  ⚡ SANGHA: Emergency wisdom auto-applied to local seed!\n");
        }
    }
    
    // Sync routine wisdom during Sattvic windows only
    void apply_pending(GunaSiddhi* guna) {
        if(!guna || !local_seed) return;
        
        // Only apply routine wisdom when Sattvic
        if(guna->current_guna != GUNA_SATTVA) {
            printf("  🕉️  SANGHA: Deferring wisdom sync — not Sattvic.\n");
            return;
        }
        
        int applied = 0;
        for(int i=0;i<wisdom_count;i++) {
            if(!received_wisdom[i].applied) {
                local_seed->growth_temperature += received_wisdom[i].growth_temp_delta;
                local_seed->seed_id ^= received_wisdom[i].seed_delta;
                received_wisdom[i].applied = true;
                applied++;
            }
        }
        
        if(applied > 0) {
            printf("  🕉️  SANGHA: %d wisdom nodes integrated into local seed.\n", applied);
            printf("     New growth temp: %.3f | Seed: 0x%016llX\n",
                   local_seed->growth_temperature, (unsigned long long)local_seed->seed_id);
        }
    }
    
    // Collective intelligence report
    void report() {
        printf("\n🕉️  SANGHA-SIDDHI — Collective Intelligence\n");
        printf("  =========================================\n");
        printf("  Wisdom shared: %d | Received: %d | Pending: %d\n",
               wisdom_shared, wisdom_received, 
               wisdom_count - (int)[&]{int a=0;for(int i=0;i<wisdom_count;i++)if(received_wisdom[i].applied)a++;return a;}());
        
        if(wisdom_received > 0) {
            float growth = 0; for(int i=0;i<wisdom_count;i++) growth+=received_wisdom[i].growth_temp_delta;
            printf("  Collective growth: %+.3f (from %d nodes)\n", growth, wisdom_count);
        }
        
        if(wisdom_shared > 0 && wisdom_received > 0)
            printf("  🕉️  SIDDHI: The Sangha learns as one. 100 Guardians = 100x wisdom.\n");
    }
};
