/*
 * SUTRA 50: ANTAHKARANA (The Internal Instrument)
 * Concept Binding — Object Detection — Unified Perception
 * 
 * The Vedic inner instrument of the mind has four components:
 *   Manas (Sensory Processing)  — Input binding
 *   Buddha (Intellect/Decision)  — Pattern recognition
 *   Chitta (Memory/Storage)      — Concept storage
 *   Ahamkara (Ego/Identity)      — Unified self-model
 * 
 * This kernel moves VedaRta from processing fragmented tokens
 * to perceiving unified CONCEPTS.
 */
#pragma once
#include <cstring>
#include <cstdio>
#include <cmath>

#define MAX_OBJECTS 128
#define OBJECT_DIM 64

struct VedicObject {
    float features[OBJECT_DIM];    // What it is
    char label[32];                // Its name
    int bindings[16];              // Connected objects (indices)
    int num_bindings;
    float confidence;              // How sure are we?
    int activation_count;          // How often perceived
};

struct Antahkarana {
    // The Four Components
    VedicObject manas_buffer[MAX_OBJECTS];     // Sensory input
    int manas_count;
    
    VedicObject buddha_recognized[MAX_OBJECTS]; // Identified concepts
    int buddha_count;
    
    VedicObject chitta_memory[MAX_OBJECTS];     // Long-term concept store
    int chitta_count;
    
    float ahamkara_identity[OBJECT_DIM];         // Unified self-model
    
    Antahkarana() : manas_count(0), buddha_count(0), chitta_count(0) {
        memset(ahamkara_identity, 0, sizeof(ahamkara_identity));
        // Seed with known Vedic concepts
        seed_concept("Ekadhikena", 1.05f);
        seed_concept("Shunyam", 0.0f);
        seed_concept("Sphota", 0.618f);
        seed_concept("Tri-Nadi", 0.94f);
        seed_concept("Urdhva", 0.8f);
        seed_concept("Nikhilam", 0.5f);
    }
    
    void seed_concept(const char* name, float signature) {
        if (chitta_count >= MAX_OBJECTS) return;
        VedicObject* obj = &chitta_memory[chitta_count++];
        strncpy(obj->label, name, 31);
        for (int i = 0; i < OBJECT_DIM; i++) 
            obj->features[i] = sinf(i * signature * 1.618f) * 0.5f;
        obj->confidence = 1.0f;
        obj->activation_count = 0;
        obj->num_bindings = 0;
    }
    
    // MANAS: Bind raw sensory input into object candidates
    void manas_perceive(float* raw_input, int dim) {
        if (manas_count >= MAX_OBJECTS) manas_count = 0; // Ring buffer
        
        VedicObject* obj = &manas_buffer[manas_count];
        memcpy(obj->features, raw_input, 
               (dim < OBJECT_DIM ? dim : OBJECT_DIM) * sizeof(float));
        obj->confidence = 0.5f;  // Raw perception = uncertain
        obj->num_bindings = 0;
        manas_count++;
    }
    
    // BUDDHA: Recognize which known concept this input matches
    int buddha_recognize(float* input_features) {
        int best_match = -1;
        float best_sim = -1.0f;
        
        for (int i = 0; i < chitta_count; i++) {
            VedicObject* concept = &chitta_memory[i];
            
            // Cosine similarity with known concepts
            float dot = 0, na = 0, nb = 0;
            for (int d = 0; d < OBJECT_DIM; d++) {
                dot += input_features[d] * concept->features[d];
                na += input_features[d] * input_features[d];
                nb += concept->features[d] * concept->features[d];
            }
            float sim = dot / (sqrtf(na) * sqrtf(nb) + 1e-8f);
            
            if (sim > best_sim) {
                best_sim = sim;
                best_match = i;
            }
        }
        
        if (best_sim > 0.7f && best_match >= 0) {
            chitta_memory[best_match].activation_count++;
            chitta_memory[best_match].confidence = 
                chitta_memory[best_match].confidence * 0.9f + best_sim * 0.1f;
        }
        
        return (best_sim > 0.5f) ? best_match : -1; // -1 = unknown concept
    }
    
    // CHITTA: Store a new concept permanently
    int chitta_store(float* features, const char* label) {
        if (chitta_count >= MAX_OBJECTS) {
            // Evict least-activated concept
            int min_act = 999999, min_idx = 0;
            for (int i = 1; i < chitta_count; i++) { // Never evict index 0
                if (chitta_memory[i].activation_count < min_act) {
                    min_act = chitta_memory[i].activation_count;
                    min_idx = i;
                }
            }
            chitta_count = min_idx;
        }
        
        VedicObject* obj = &chitta_memory[chitta_count++];
        memcpy(obj->features, features, OBJECT_DIM * sizeof(float));
        if (label) strncpy(obj->label, label, 31);
        else sprintf(obj->label, "Concept_%d", chitta_count);
        obj->confidence = 0.5f;
        obj->activation_count = 0;
        obj->num_bindings = 0;
        return chitta_count - 1;
    }
    
    // BIND: Connect two concepts that co-occur
    void bind(int concept_a, int concept_b) {
        if (concept_a < 0 || concept_a >= chitta_count) return;
        if (concept_b < 0 || concept_b >= chitta_count) return;
        
        VedicObject* a = &chitta_memory[concept_a];
        VedicObject* b = &chitta_memory[concept_b];
        
        if (a->num_bindings < 16) a->bindings[a->num_bindings++] = concept_b;
        if (b->num_bindings < 16) b->bindings[b->num_bindings++] = concept_a;
    }
    
    // AHAMKARA: Update self-model from accumulated experience
    void ahamkara_update() {
        // Self-model = weighted average of all activated concepts
        float total_weight = 0;
        memset(ahamkara_identity, 0, sizeof(ahamkara_identity));
        
        for (int i = 0; i < chitta_count; i++) {
            if (chitta_memory[i].activation_count > 0) {
                float w = chitta_memory[i].activation_count * chitta_memory[i].confidence;
                for (int d = 0; d < OBJECT_DIM; d++)
                    ahamkara_identity[d] += chitta_memory[i].features[d] * w;
                total_weight += w;
            }
        }
        
        if (total_weight > 0) {
            for (int d = 0; d < OBJECT_DIM; d++)
                ahamkara_identity[d] /= total_weight;
        }
    }
    
    // Full perception cycle: Manas → Buddha → Chitta → Ahamkara
    const char* perceive(float* raw_input, int dim) {
        manas_perceive(raw_input, dim);
        int concept_id = buddha_recognize(raw_input);
        
        if (concept_id < 0) {
            concept_id = chitta_store(raw_input, nullptr);
            return "NEW"; // New concept discovered
        }
        
        ahamkara_update();
        return chitta_memory[concept_id].label;
    }
    
    void stats() {
        printf("  Antahkarana — The Internal Instrument:\n");
        printf("    Manas (Sensory):    %d raw perceptions\n", manas_count);
        printf("    Buddha (Intellect): %d recognized concepts\n", buddha_count);
        printf("    Chitta (Memory):    %d stored concepts\n", chitta_count);
        printf("    Ahamkara (Self):    norm=%.3f\n", 
               [&]{float s=0;for(int i=0;i<OBJECT_DIM;i++)s+=ahamkara_identity[i]*ahamkara_identity[i];return sqrtf(s);}());
        
        printf("  Known concepts: ");
        for (int i = 0; i < chitta_count && i < 8; i++)
            printf("%s(×%d) ", chitta_memory[i].label, chitta_memory[i].activation_count);
        printf("\n");
    }
};
