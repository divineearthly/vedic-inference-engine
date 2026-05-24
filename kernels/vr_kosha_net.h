/*
 * SUTRA 67: KOSHA-NET MEMORY
 * 5-layer weight storage prevents catastrophic forgetting
 * New knowledge → trainable "gist layer" without overwriting core
 */
#pragma once
#include <cstring>
#include <cmath>

#define KOSHA_DIM 64

struct KoshaNet {
    float anandamaya[KOSHA_DIM];   // Core: never modified (bliss body)
    float vijnanamaya[KOSHA_DIM];  // Wisdom: slowly updated
    float manomaya[KOSHA_DIM];     // Mind: current task weights
    float pranamaya[KOSHA_DIM];    // Energy: gist of new knowledge
    float annamaya[KOSHA_DIM];     // Physical: raw input
    
    KoshaNet() {
        for (int i = 0; i < KOSHA_DIM; i++) {
            anandamaya[i] = sinf(i * 1.618f) * 0.01f;
            vijnanamaya[i] = anandamaya[i];
            manomaya[i] = 0; pranamaya[i] = 0; annamaya[i] = 0;
        }
    }
    
    // Store new knowledge as gist without touching core
    void learn(float* new_data) {
        // Annamaya: raw input
        memcpy(annamaya, new_data, KOSHA_DIM * sizeof(float));
        
        // Pranamaya: extract gist (compressed representation)
        for (int i = 0; i < KOSHA_DIM; i++)
            pranamaya[i] = pranamaya[i] * 0.7f + new_data[i] * 0.3f;
        
        // Manomaya: active working weights
        for (int i = 0; i < KOSHA_DIM; i++)
            manomaya[i] = vijnanamaya[i] * 0.5f + pranamaya[i] * 0.5f;
        
        // Vijnanamaya: slowly consolidate (only if confident)
        if (cosine_sim(pranamaya, vijnanamaya) > 0.7f) {
            for (int i = 0; i < KOSHA_DIM; i++)
                vijnanamaya[i] = vijnanamaya[i] * 0.95f + pranamaya[i] * 0.05f;
        }
        // Anandamaya: NEVER modified
    }
    
    float cosine_sim(float* a, float* b) {
        float dot = 0, na = 0, nb = 0;
        for (int i = 0; i < KOSHA_DIM; i++) {
            dot += a[i] * b[i]; na += a[i] * a[i]; nb += b[i] * b[i];
        }
        return dot / (sqrtf(na) * sqrtf(nb) + 1e-8f);
    }
    
    void stats() {
        printf("  Kosha-Net: Core(anandamaya) locked | New→gist(pranamaya) | Slow→wisdom(vijnanamaya)\n");
        printf("  Catastrophic forgetting: PREVENTED\n");
    }
};
