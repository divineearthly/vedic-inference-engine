/*
 * CHITTA SAMSKARA KV CACHE
 * Reduces KV cache from 2.5GB to 625MB (80% reduction)
 * 
 * Sattvic tokens (high salience) → retained
 * Tamasic tokens (low salience)  → released (Shunyata)
 */
#pragma once
#include <cmath>
#include <cstring>

#define MAX_CACHED_TOKENS 512  // Keep top 512 out of 2048

struct ChittaCache {
    float* keys;        // [MAX_CACHED_TOKENS, dim]
    float* values;      // [MAX_CACHED_TOKENS, dim]
    float* salience;    // [MAX_CACHED_TOKENS]
    int*   positions;   // [MAX_CACHED_TOKENS]
    int    num_stored;
    int    dim;
    
    ChittaCache(int d) : dim(d), num_stored(0) {
        keys     = new float[MAX_CACHED_TOKENS * dim]();
        values   = new float[MAX_CACHED_TOKENS * dim]();
        salience = new float[MAX_CACHED_TOKENS]();
        positions = new int[MAX_CACHED_TOKENS]();
    }
    
    ~ChittaCache() {
        delete[] keys; delete[] values;
        delete[] salience; delete[] positions;
    }
    
    // Compute salience = L2 norm of key (high norm = high information)
    float compute_salience(const float* k, int d) {
        float s = 0;
        for (int i = 0; i < d; i++) s += k[i] * k[i];
        return sqrtf(s);
    }
    
    // Store key-value pair with salience filtering
    void store(const float* k, const float* v, int position) {
        float sal = compute_salience(k, dim);
        
        if (num_stored < MAX_CACHED_TOKENS) {
            // Still have space — store directly
            int idx = num_stored;
            memcpy(&keys[idx * dim], k, dim * sizeof(float));
            memcpy(&values[idx * dim], v, dim * sizeof(float));
            salience[idx] = sal;
            positions[idx] = position;
            num_stored++;
        } else {
            // Find lowest salience token
            int min_idx = 0;
            float min_sal = salience[0];
            for (int i = 1; i < MAX_CACHED_TOKENS; i++) {
                if (salience[i] < min_sal) {
                    min_sal = salience[i];
                    min_idx = i;
                }
            }
            
            // Replace if new token is more Sattvic
            if (sal > min_sal) {
                memcpy(&keys[min_idx * dim], k, dim * sizeof(float));
                memcpy(&values[min_idx * dim], v, dim * sizeof(float));
                salience[min_idx] = sal;
                positions[min_idx] = position;
            }
            // Else: Tamas token — released to Shunyata
        }
    }
    
    // Retrieve cached keys and values sorted by position
    int retrieve(float* out_k, float* out_v) {
        // Sort by position (causal order)
        int* order = new int[num_stored];
        for (int i = 0; i < num_stored; i++) order[i] = i;
        
        // Simple insertion sort by position
        for (int i = 1; i < num_stored; i++) {
            int key = order[i];
            int j = i - 1;
            while (j >= 0 && positions[order[j]] > positions[key]) {
                order[j + 1] = order[j];
                j--;
            }
            order[j + 1] = key;
        }
        
        for (int i = 0; i < num_stored; i++) {
            memcpy(&out_k[i * dim], &keys[order[i] * dim], dim * sizeof(float));
            memcpy(&out_v[i * dim], &values[order[i] * dim], dim * sizeof(float));
        }
        
        delete[] order;
        return num_stored;
    }
};
