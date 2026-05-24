/*
 * SUTRA 21.12: VILOKANAM (By Mere Observation)
 * The Fast-Path Dispatcher — Instantaneous Recognition
 * 
 * Concept: Skip deep computation for patterns already seen.
 * Uses Uddishta indices as hash keys for O(1) lookup.
 * 
 * "Vilokanam" = intuitive recognition without step-by-step analysis.
 * The system "just knows" because it has seen the pattern before.
 */
#pragma once
#include "vr_uddishta.h"
#include <cstring>
#include <cstdio>

#define VILOKANAM_CACHE_SIZE 4096

struct VilokanamEntry {
    uint64_t uddishta_key;      // Pattern hash
    float response[64];          // Cached output
    int hit_count;               // Frequency of use
    float confidence;            // How reliable is this cache entry
    bool protected_entry;        // Sattvic: never evict
};

struct VilokanamDispatcher {
    VilokanamEntry cache[VILOKANAM_CACHE_SIZE];
    int entries;
    int hits;
    int misses;
    
    VilokanamDispatcher() : entries(0), hits(0), misses(0) {
        memset(cache, 0, sizeof(cache));
    }
    
    // O(1) lookup: Have we seen this pattern before?
    float* observe(uint64_t pattern_key, float* input_embedding) {
        // Fast hash: modulo into cache
        int slot = pattern_key % VILOKANAM_CACHE_SIZE;
        
        // Linear probe (max 4 steps — constant time)
        for (int probe = 0; probe < 4; probe++) {
            int idx = (slot + probe) % VILOKANAM_CACHE_SIZE;
            
            if (cache[idx].uddishta_key == pattern_key) {
                // CACHE HIT — Vilokanam works!
                hits++;
                cache[idx].hit_count++;
                
                // Promote to Sattvic if very frequent
                if (cache[idx].hit_count > 100) {
                    cache[idx].protected_entry = true;
                }
                
                return cache[idx].response;
            }
            
            if (cache[idx].uddishta_key == 0) {
                // Empty slot — first time seeing this pattern
                break;
            }
        }
        
        // CACHE MISS — need deep computation
        misses++;
        return nullptr; // Signal: "Think deeply about this"
    }
    
    // Store result for future Vilokanam
    void learn(uint64_t pattern_key, float* answer) {
        int slot = pattern_key % VILOKANAM_CACHE_SIZE;
        
        // Find first evictable slot
        for (int probe = 0; probe < 4; probe++) {
            int idx = (slot + probe) % VILOKANAM_CACHE_SIZE;
            
            if (cache[idx].uddishta_key == 0 || 
                (!cache[idx].protected_entry && cache[idx].hit_count < 10)) {
                cache[idx].uddishta_key = pattern_key;
                memcpy(cache[idx].response, answer, 64 * sizeof(float));
                cache[idx].hit_count = 1;
                cache[idx].confidence = 0.5f;
                entries++;
                return;
            }
        }
        
        // Cache full — evict least-used non-protected entry
        int min_hits = 999999, min_idx = slot;
        for (int probe = 0; probe < 4; probe++) {
            int idx = (slot + probe) % VILOKANAM_CACHE_SIZE;
            if (!cache[idx].protected_entry && cache[idx].hit_count < min_hits) {
                min_hits = cache[idx].hit_count;
                min_idx = idx;
            }
        }
        cache[min_idx].uddishta_key = pattern_key;
        memcpy(cache[min_idx].response, answer, 64 * sizeof(float));
        cache[min_idx].hit_count = 1;
        cache[min_idx].confidence = 0.5f;
    }
    
    // "Just know" — Vilokanam inference with fallback
    bool just_know(uint64_t key, float* output) {
        float* cached = observe(key, nullptr);
        if (cached) {
            memcpy(output, cached, 64 * sizeof(float));
            return true; // Instant recognition
        }
        return false; // Need deep thought
    }
    
    float hit_rate() {
        int total = hits + misses;
        return total > 0 ? (float)hits / total : 0;
    }
    
    void stats() {
        int protected_count = 0;
        for (int i = 0; i < VILOKANAM_CACHE_SIZE; i++) {
            if (cache[i].protected_entry) protected_count++;
        }
        printf("  Vilokanam Cache: %d/%d entries | %d Sattvic(protected)\n", 
               entries, VILOKANAM_CACHE_SIZE, protected_count);
        printf("  Hit rate: %.1f%% | Hits: %d | Misses: %d\n", 
               hit_rate() * 100, hits, misses);
        if (hit_rate() > 0.5f) {
            printf("  ⚡ VILOKANAM SIDDHI: >50%% instant recognition\n");
        }
    }
    
    // Memory efficiency vs storing full embeddings
    float memory_savings() {
        // Each entry: 8 bytes key + 256 bytes response = 264 bytes
        // Vs full float32 embedding: 1024 bytes
        return 1024.0f / 264.0f; // ~3.9x compression
    }
};
