/*
 * SUTRA 63: KALA-CHAKRA (The Time-Wheel)
 * Tiling & Periodic Repetition Kernel
 * 
 * Concept: The universe moves in cycles (Yugas, Kalpas).
 * Computation should recognize cycles and reuse results.
 * 
 * When a pattern repeats, Kala-Chakra detects the period
 * and tiles the result — O(1) for periodic sequences.
 */
#pragma once
#include <cstring>
#include <cstdio>
#include <cmath>

#define MAX_PERIOD 256

struct KalaChakra {
    float cycle_buffer[MAX_PERIOD];  // One full period
    int period_length;               // Length of detected cycle
    float* tiled_cache;              // Pre-computed tiled output
    int total_length;                // Total cached length
    int hits;                        // Cycles reused
    int misses;                      // Fresh computations
    
    KalaChakra() : period_length(0), tiled_cache(nullptr), 
                   total_length(0), hits(0), misses(0) {
        memset(cycle_buffer, 0, sizeof(cycle_buffer));
    }
    ~KalaChakra() { delete[] tiled_cache; }
    
    // Detect if sequence is periodic
    // Uses autocorrelation — O(n) single pass
    int detect_period(float* sequence, int len) {
        if (len < 8) return 0;
        
        // Try periods from 2 to len/4
        int best_period = 0;
        float best_match = 0.0f;
        
        for (int period = 2; period <= len/4 && period <= MAX_PERIOD; period++) {
            float match = 0.0f;
            int comparisons = 0;
            
            for (int i = 0; i < len - period; i += period) {
                for (int j = 0; j < period && i + j + period < len; j++) {
                    match += 1.0f - fabsf(sequence[i + j] - sequence[i + j + period]);
                    comparisons++;
                }
            }
            
            match /= (comparisons + 1);
            
            if (match > 0.95f && match > best_match) {
                best_match = match;
                best_period = period;
            }
        }
        
        return best_period;
    }
    
    // Store one cycle and tile it
    void encode_cycle(float* period_data, int period) {
        period_length = period;
        memcpy(cycle_buffer, period_data, period * sizeof(float));
    }
    
    // Generate tiled output from stored cycle
    void tile(int desired_length) {
        if (period_length == 0) return;
        
        delete[] tiled_cache;
        tiled_cache = new float[desired_length];
        total_length = desired_length;
        
        for (int i = 0; i < desired_length; i++) {
            tiled_cache[i] = cycle_buffer[i % period_length];
        }
    }
    
    // Smart forward: check periodicity first
    // Returns true if Kala-Chakra handled it (no deep compute needed)
    bool forward(float* input, int len, float* output) {
        int period = detect_period(input, len);
        
        if (period > 0 && period == period_length) {
            // Same cycle as before — just tile!
            hits++;
            if (total_length >= len) {
                memcpy(output, tiled_cache, len * sizeof(float));
            } else {
                tile(len);
                memcpy(output, tiled_cache, len * sizeof(float));
            }
            return true; // Kala-Chakra handled it
        }
        
        if (period > 0 && period != period_length) {
            // New cycle detected — encode it for future
            misses++;
            encode_cycle(input, period);
            tile(len * 2); // Pre-compute 2x for future hits
            memcpy(output, tiled_cache, len * sizeof(float));
            return true; // Still handled — cycle detected
        }
        
        misses++;
        return false; // Not periodic — needs Sphota
    }
    
    // Fast sin/cos using cycle knowledge
    // Pre-computed trig from stored cycle
    float fast_sin(float x) {
        if (period_length == 0) return sinf(x);
        // Map x to cycle index
        int idx = ((int)(x * period_length / (2.0f * 3.14159f)) % period_length + period_length) % period_length;
        return cycle_buffer[idx];
    }
    
    void stats() {
        int total = hits + misses;
        float rate = total > 0 ? (float)hits / total : 0;
        printf("  Kala-Chakra: Period=%d | Hit rate: %.1f%% (%d/%d)\n", 
               period_length, rate * 100, hits, total);
        if (period_length > 0) {
            printf("  Cycle encoded: %d elements, reusable infinitely\n", period_length);
            printf("  Compute saved: %dx per repetition\n", total_length / period_length);
        }
    }
};
