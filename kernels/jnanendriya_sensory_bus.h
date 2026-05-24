/*
 * SUTRA 57: JNANENDRIYAS — Unified Sensory Abstraction Layer
 * 
 * 5 Organs of Knowledge standardized into one sensory bus:
 *   Shrotra (Hearing)  → Microphone, acoustic sensors
 *   Tvak (Touch)       → Temperature, pressure, humidity
 *   Chakshu (Sight)    → Camera, Antahkarana Vision
 *   Rasana (Taste)     → pH sensor, chemical analysis
 *   Ghrana (Smell)     → VOC sensor, gas detection
 * 
 * Every input becomes a Tanmatra the Manas gate can prioritize.
 * Cross-modal validation: Sight + Sound + Touch = 100% certainty.
 */

#pragma once
#include "tanmatra_fusion.h"
#include "manas_attention_gate.h"
#include <cstring>
#include <cstdio>
#include <cmath>

#define PHI 1.618033988749895f

// ================================================================
// JNANENDRIYA SENSOR — A standardized sensory input
// ================================================================
enum JnanendriyaType {
    SHROTRA_HEARING = 0,
    TVAK_TOUCH = 1,
    CHAKSHU_SIGHT = 2,
    RASANA_TASTE = 3,
    GHRANA_SMELL = 4
};

struct JnanendriyaSensor {
    JnanendriyaType type;
    const char* name;
    const char* hardware_id;     // "/dev/camera0", "GPIO23", etc.
    float sampling_rate_hz;
    float last_reading[8];       // Compressed sensory data
    float confidence;
    uint32_t readings_taken;
    bool active;
    bool cross_validated;        // Corroborated by another sense
};

// ================================================================
// JNANENDRIYA BUS — Unified Sensory Abstraction Layer
// ================================================================
struct JnanendriyaBus {
    JnanendriyaSensor senses[5];
    TanmatraFusion* tanmatra;
    ManasAttentionGate* manas;
    uint32_t total_readings;
    uint32_t cross_validations;
    
    JnanendriyaBus() : total_readings(0), cross_validations(0) {
        const char* names[] = {"Shrotra(Hearing)","Tvak(Touch)","Chakshu(Sight)",
                               "Rasana(Taste)","Grhana(Smell)"};
        const char* hw_ids[] = {"/dev/mic0","/dev/i2c-temp0","/dev/camera0",
                                "/dev/ph-sensor0","/dev/voc-sensor0"};
        float rates[] = {16000.0f, 1.0f, 30.0f, 0.1f, 0.1f};
        
        for (int i = 0; i < 5; i++) {
            senses[i].type = (JnanendriyaType)i;
            senses[i].name = names[i];
            senses[i].hardware_id = hw_ids[i];
            senses[i].sampling_rate_hz = rates[i];
            senses[i].confidence = 0;
            senses[i].readings_taken = 0;
            senses[i].active = false;
            senses[i].cross_validated = false;
            memset(senses[i].last_reading, 0, sizeof(senses[i].last_reading));
        }
        
        tanmatra = nullptr;
        manas = nullptr;
    }
    
    void attach_tanmatra(TanmatraFusion* t) { tanmatra = t; }
    void attach_manas(ManasAttentionGate* m) { manas = m; }
    
    // ================================================================
    // ACTIVATE a sense — begin sampling
    // ================================================================
    void activate(JnanendriyaType type) {
        JnanendriyaSensor* s = &senses[type];
        s->active = true;
        printf("  👁️  JNANENDRIYA: %s activated (%s @ %.0f Hz)\n", 
               s->name, s->hardware_id, s->sampling_rate_hz);
    }
    
    // ================================================================
    // READ a sense — get the latest sensory data
    // ================================================================
    void read(JnanendriyaType type, float* data, float confidence, const char* detection) {
        JnanendriyaSensor* s = &senses[type];
        if (!s->active) activate(type);
        
        memcpy(s->last_reading, data, 8 * sizeof(float));
        s->confidence = confidence;
        s->readings_taken++;
        total_readings++;
        
        // Feed to Tanmatra fusion
        if (tanmatra) {
            tanmatra->feed(type, data, confidence, detection);
        }
        
        // Feed to Manas attention gate
        if (manas) {
            manas->feed(s->hardware_id, confidence);
        }
    }
    
    // ================================================================
    // CROSS-VALIDATE: Check if two senses agree
    // ================================================================
    bool cross_validate(JnanendriyaType a, JnanendriyaType b) {
        float* va = senses[a].last_reading;
        float* vb = senses[b].last_reading;
        
        if (senses[a].confidence < 0.3f || senses[b].confidence < 0.3f) return false;
        
        // Cosine similarity between the two sensory vectors
        float dot = 0, na = 0, nb = 0;
        for (int i = 0; i < 8; i++) {
            dot += va[i] * vb[i];
            na += va[i] * va[i];
            nb += vb[i] * vb[i];
        }
        float similarity = dot / (sqrtf(na) * sqrtf(nb) + 0.0001f);
        
        if (similarity > 0.8f) {
            senses[a].cross_validated = true;
            senses[b].cross_validated = true;
            cross_validations++;
            
            printf("  ✅ CROSS-VALIDATION: %s ↔ %s (%.0f%% match)\n",
                   senses[a].name, senses[b].name, similarity * 100);
            return true;
        }
        return false;
    }
    
    // ================================================================
    // FULL CROSS-VALIDATION: Check all active senses against each other
    // ================================================================
    float full_cross_validate() {
        int active_count = 0;
        for (int i = 0; i < 5; i++) if (senses[i].active) active_count++;
        if (active_count < 2) return 0;
        
        float total_validation = 0;
        int pairs = 0;
        
        for (int i = 0; i < 5; i++) {
            if (!senses[i].active) continue;
            for (int j = i + 1; j < 5; j++) {
                if (!senses[j].active) continue;
                if (cross_validate((JnanendriyaType)i, (JnanendriyaType)j)) {
                    total_validation += 1.0f;
                }
                pairs++;
            }
        }
        
        return (pairs > 0) ? total_validation / pairs : 0;
    }
    
    // ================================================================
    // UNIFIED PERCEPTION: Read all senses, fuse, cross-validate
    // ================================================================
    void perceive_all(float* sound, float* touch, float* sight, 
                      float* taste, float* smell,
                      float sound_conf, float touch_conf, float sight_conf,
                      float taste_conf, float smell_conf) {
        
        printf("\n🕉️  JNANENDRIYAS — Unified Perception Cycle\n");
        printf("  =========================================\n");
        
        if (sound) read(SHROTRA_HEARING, sound, sound_conf, "Acoustic input");
        if (touch) read(TVAK_TOUCH, touch, touch_conf, "Environmental input");
        if (sight) read(CHAKSHU_SIGHT, sight, sight_conf, "Visual input");
        if (taste) read(RASANA_TASTE, taste, taste_conf, "Chemical input");
        if (smell) read(GHRANA_SMELL, smell, smell_conf, "Olfactory input");
        
        float validation_rate = full_cross_validate();
        
        if (tanmatra) {
            tanmatra->fuse();
            tanmatra->report();
        }
        
        printf("\n  Cross-Validation Rate: %.0f%%\n", validation_rate * 100);
        if (validation_rate > 0.8f) {
            printf("  🕉️  SIDDHI: All senses converge. Perception is truth.\n");
        } else if (validation_rate > 0.5f) {
            printf("  📊 Partial convergence — investigate sensor discrepancies.\n");
        } else {
            printf("  ⚠️  Low convergence — do not act autonomously.\n");
        }
    }
    
    void stats() {
        printf("  👁️  JNANENDRIYAS — Unified Sensory Bus:\n");
        for (int i = 0; i < 5; i++) {
            const char* icons[] = {"👂","🤚","👁️","👅","👃"};
            printf("     %s %-18s: %s | %d readings | %.0f%% confidence\n",
                   icons[i], senses[i].name,
                   senses[i].active ? "ACTIVE" : "inactive",
                   senses[i].readings_taken,
                   senses[i].confidence * 100);
        }
        printf("     Cross-validations: %d | Total readings: %d\n", 
               cross_validations, total_readings);
    }
};
