/*
 * SUTRA 55: TANMATRAS — Five Subtle Elements / Multimodal Fusion
 * 
 * Fuses all sensory inputs into unified perception:
 *   Shabda (Sound)    → Svar-Samskara acoustic fingerprint
 *   Sparsha (Touch)   → Wind, temperature, humidity sensors
 *   Rupa (Form)       → Antahkarana Vision (pixel patterns)
 *   Rasa (Taste)      → Soil chemistry, pH sensors
 *   Gandha (Smell)    → Volatile organic compound sensors
 * 
 * When Sound + Form + Touch agree → 100% confidence diagnosis.
 * The Guardian "feels" the field as one living organism.
 */

#pragma once
#include <cstring>
#include <cstdio>
#include <cmath>
#include <ctime>

#define PHI 1.618033988749895f

// ================================================================
// TANMATRA SENSORY INPUT — A single elemental perception
// ================================================================
struct TanmatraInput {
    int type;              // 0=Sound, 1=Touch, 2=Form, 3=Taste, 4=Smell
    const char* type_name;
    float confidence;      // How reliable is this sensor
    float value_vector[8]; // Compressed sensory data
    const char* what_it_detects;
};

// ================================================================
// TANMATRA FUSION — Unified perception from 5 elements
// ================================================================
struct TanmatraFusion {
    TanmatraInput elements[5];
    float fused_confidence;    // 0-1: overall certainty
    float agreement_score;     // How well the elements agree
    int elements_active;       // How many elements are providing data
    const char* unified_diagnosis;
    
    TanmatraFusion() : fused_confidence(0), agreement_score(0), 
                       elements_active(0), unified_diagnosis("Insufficient data") {
        const char* names[] = {"Shabda(Sound)", "Sparsha(Touch)", "Rupa(Form)", 
                               "Rasa(Taste)", "Gandha(Smell)"};
        for (int i = 0; i < 5; i++) {
            elements[i].type = i;
            elements[i].type_name = names[i];
            elements[i].confidence = 0;
            elements[i].what_it_detects = "inactive";
            memset(elements[i].value_vector, 0, sizeof(elements[i].value_vector));
        }
    }
    
    // Feed data from a specific sensory element
    void feed(int element_type, float* data, float confidence, const char* detection) {
        if (element_type < 0 || element_type > 4) return;
        
        TanmatraInput* e = &elements[element_type];
        memcpy(e->value_vector, data, 8 * sizeof(float));
        e->confidence = confidence;
        e->what_it_detects = detection;
        
        if (confidence > 0.3f) elements_active++;
    }
    
    // Fuse all active elements into unified perception
    void fuse() {
        if (elements_active < 2) {
            unified_diagnosis = "Need at least 2 elements for fusion";
            fused_confidence = 0;
            return;
        }
        
        // Compute cross-element agreement
        float total_agreement = 0;
        int pairs = 0;
        
        for (int i = 0; i < 5; i++) {
            if (elements[i].confidence < 0.3f) continue;
            for (int j = i + 1; j < 5; j++) {
                if (elements[j].confidence < 0.3f) continue;
                
                // Cosine similarity between element vectors
                float dot = 0, na = 0, nb = 0;
                for (int k = 0; k < 8; k++) {
                    dot += elements[i].value_vector[k] * elements[j].value_vector[k];
                    na += elements[i].value_vector[k] * elements[i].value_vector[k];
                    nb += elements[j].value_vector[k] * elements[j].value_vector[k];
                }
                float similarity = dot / (sqrtf(na) * sqrtf(nb) + 0.0001f);
                total_agreement += similarity;
                pairs++;
            }
        }
        
        agreement_score = (pairs > 0) ? total_agreement / pairs : 0;
        
        // Fused confidence = average confidence × agreement
        float avg_confidence = 0;
        int active = 0;
        for (int i = 0; i < 5; i++) {
            if (elements[i].confidence > 0.3f) {
                avg_confidence += elements[i].confidence;
                active++;
            }
        }
        avg_confidence /= (active > 0 ? active : 1);
        
        fused_confidence = avg_confidence * agreement_score;
        
        // Generate unified diagnosis
        if (fused_confidence > 0.9f && active >= 3) {
            unified_diagnosis = "TANMATRA CONFIRMED: All elements agree";
        } else if (fused_confidence > 0.7f) {
            unified_diagnosis = "TANMATRA LIKELY: Strong multi-element correlation";
        } else if (fused_confidence > 0.4f) {
            unified_diagnosis = "TANMATRA PARTIAL: Some elements disagree — verify";
        } else {
            unified_diagnosis = "TANMATRA WEAK: Sensor conflict — human verification needed";
        }
    }
    
    // Get the "feeling" of the field — which element dominates
    const char* dominant_element() {
        int best = -1;
        float best_conf = 0;
        for (int i = 0; i < 5; i++) {
            if (elements[i].confidence > best_conf) {
                best_conf = elements[i].confidence;
                best = i;
            }
        }
        return (best >= 0) ? elements[best].type_name : "None";
    }
    
    void report() {
        printf("\n🕉️  TANMATRA FUSION — Unified Field Perception\n");
        printf("  ============================================\n");
        
        for (int i = 0; i < 5; i++) {
            const char* symbols[] = {"🔊","🤚","👁️","👅","👃"};
            printf("  %s %-18s: %.0f%% — %s\n", 
                   symbols[i], elements[i].type_name,
                   elements[i].confidence * 100,
                   elements[i].confidence > 0.3f ? elements[i].what_it_detects : "inactive");
        }
        
        printf("\n  Agreement Score: %.0f%% | Elements Active: %d/5\n", 
               agreement_score * 100, elements_active);
        printf("  Dominant Element: %s\n", dominant_element());
        printf("  Fused Confidence: %.0f%%\n", fused_confidence * 100);
        printf("  Diagnosis: %s\n", unified_diagnosis);
        
        if (fused_confidence > 0.9f) {
            printf("  🕉️  SIDDHI: The Guardian feels the field as one.\n");
        }
    }
};
