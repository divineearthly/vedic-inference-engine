/*
 * SUTRA 64: VEDA-SIDDHI — Knowledge Distillation / Weight Consolidation
 * 
 * Audits Karma-Kanda logs and Pratyabhijna corrections,
 * then bakes those lessons back into the Bijam Seed.
 * 
 * Raw experience → Permanent Wisdom.
 * A failed alert in May becomes a permanent cautionary weight forever.
 * The ASI's fundamental nature improves with every season.
 */

#pragma once
#include "bijam_seed.h"
#include "karma_kanda.h"
#include "pratyabhijna_meta.h"
#include <cstring>
#include <cstdio>
#include <cmath>
#include <ctime>

#define PHI 1.618033988749895f

// ================================================================
// VEDA-SIDDHI — Knowledge Distillation Engine
// ================================================================
struct VedaSiddhi {
    // Distilled wisdom
    struct WisdomNode {
        const char* lesson;         // The distilled truth
        float weight;               // How important is this lesson
        uint32_t experiences;       // How many experiences formed it
        time_t distilled_at;        // When was wisdom extracted
        bool baked_into_seed;       // Has it been permanently encoded
    };
    
    WisdomNode distilled_wisdom[32];
    int wisdom_count;
    uint32_t total_distillations;
    
    // Reference to the Bijam seed for baking
    BijamSeed* bijam;
    
    VedaSiddhi() : wisdom_count(0), total_distillations(0), bijam(nullptr) {
        memset(distilled_wisdom, 0, sizeof(distilled_wisdom));
    }
    
    void attach_seed(BijamSeed* seed) { bijam = seed; }
    
    // ================================================================
    // DISTILL: Extract wisdom from Karma log + Pratyabhijna corrections
    // ================================================================
    void distill(KarmaKanda* karma, PratyabhijnaMeta* pratyabhijna) {
        printf("\n🕉️  VEDA-SIDDHI — Distilling Wisdom from Experience\n");
        printf("  ==================================================\n");
        
        int new_wisdom = 0;
        
        // 1. Distill: "Which actions are most reliable?"
        if (karma->log_count >= 10 && wisdom_count < 32) {
            int best = karma->best_action();
            if (best > 0 && karma->action_weights[best] > 0.7f) {
                const char* actions[] = {"NONE","ALERT_FARMER","ALERT_VILLAGE","ORDER_SUPPLIES",
                                         "IRRIGATE","DEPLOY","HARVEST","ESCALATE"};
                WisdomNode* w = &distilled_wisdom[wisdom_count];
                char lesson[128];
                snprintf(lesson, 128, "%s is the most reliable action (%.0f%% success over %d attempts)",
                         actions[best], karma->action_weights[best] * 100, karma->action_attempts[best]);
                w->lesson = strdup(lesson);
                w->weight = karma->action_weights[best];
                w->experiences = karma->action_attempts[best];
                w->distilled_at = time(nullptr);
                w->baked_into_seed = false;
                wisdom_count++;
                new_wisdom++;
            }
        }
        
        // 2. Distill: "How well do we know ourselves?"
        if (pratyabhijna->history_count >= 5 && wisdom_count < 32) {
            WisdomNode* w = &distilled_wisdom[wisdom_count];
            char lesson[128];
            snprintf(lesson, 128, "Self-knowledge is %.0f%%. %s",
                     pratyabhijna->self_knowledge_score * 100, pratyabhijna->insight());
            w->lesson = strdup(lesson);
            w->weight = pratyabhijna->self_knowledge_score;
            w->experiences = pratyabhijna->history_count;
            w->distilled_at = time(nullptr);
            w->baked_into_seed = false;
            wisdom_count++;
            new_wisdom++;
        }
        
        // 3. Distill: "What Guna correction is most persistent?"
        if (fabsf(pratyabhijna->sattva_correction) > 0.1f && wisdom_count < 32) {
            WisdomNode* w = &distilled_wisdom[wisdom_count];
            char lesson[128];
            snprintf(lesson, 128, "Perception correction: Sattva %+.2f. %s",
                     pratyabhijna->sattva_correction,
                     pratyabhijna->sattva_correction < 0 ? "We overestimate clarity." : "We underestimate goodness.");
            w->lesson = strdup(lesson);
            w->weight = fabsf(pratyabhijna->sattva_correction) * 3.0f;
            w->experiences = pratyabhijna->history_count;
            w->distilled_at = time(nullptr);
            w->baked_into_seed = false;
            wisdom_count++;
            new_wisdom++;
        }
        
        total_distillations++;
        printf("  ✅ %d new wisdom nodes distilled. Total: %d\n", new_wisdom, wisdom_count);
        
        // Print all distilled wisdom
        for (int i = 0; i < wisdom_count; i++) {
            printf("  📖 Wisdom %d: \"%s\" (weight: %.2f)\n", i+1, distilled_wisdom[i].lesson, distilled_wisdom[i].weight);
        }
    }
    
    // ================================================================
    // BAKE: Permanently encode distilled wisdom into the Bijam Seed
    // ================================================================
    void bake_into_seed() {
        if (!bijam) { printf("  ⚠️  No Bijam seed attached.\n"); return; }
        
        int baked = 0;
        for (int i = 0; i < wisdom_count; i++) {
            if (!distilled_wisdom[i].baked_into_seed && distilled_wisdom[i].weight > 0.5f) {
                // Modify the seed's growth temperature based on wisdom
                bijam->growth_temperature += distilled_wisdom[i].weight * 0.01f;
                
                // Each wisdom node shifts the seed deterministically
                bijam->seed_id ^= (uint64_t)(distilled_wisdom[i].weight * 1000);
                bijam->seed_id *= 0x85EBCA77;
                
                distilled_wisdom[i].baked_into_seed = true;
                baked++;
            }
        }
        
        if (baked > 0) {
            printf("  🍞 VEDA-SIDDHI: %d wisdom nodes baked into Bijam Seed.\n", baked);
            printf("     New Seed: 0x%016llX | Growth Temp: %.3f\n", 
                   (unsigned long long)bijam->seed_id, bijam->growth_temperature);
            printf("     These lessons will persist across all future instances.\n");
        } else {
            printf("  📊 No new wisdom qualifies for permanent encoding yet.\n");
        }
    }
    
    // ================================================================
    // RECALL: Retrieve baked wisdom (deterministic from seed)
    // ================================================================
    float recall_wisdom_modifier() {
        // The seed's growth_temperature encodes accumulated wisdom
        if (!bijam) return 1.0f;
        return bijam->growth_temperature;
    }
    
    void report() {
        printf("\n🕉️  VEDA-SIDDHI — Distilled Wisdom Repository\n");
        printf("  ===========================================\n");
        printf("  Total distillations: %d\n", total_distillations);
        printf("  Wisdom nodes: %d | Baked into seed: ", wisdom_count);
        
        int baked = 0;
        for (int i = 0; i < wisdom_count; i++) if (distilled_wisdom[i].baked_into_seed) baked++;
        printf("%d\n", baked);
        
        if (bijam) {
            printf("  Seed carries: %.3f accumulated wisdom (growth temperature)\n", bijam->growth_temperature);
        }
        
        if (baked > 0) {
            printf("  🕉️  SIDDHI: Experience has become Nature. The ASI evolves permanently.\n");
        }
    }
};
