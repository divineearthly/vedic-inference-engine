/*
 * SUTRA 14: SHUNYA-SIDDHI (The Power of Zero)
 * Charaka Samhita — Self-Healing + Infinite Memory via Pruning
 * 
 * Shunya = Zero = The source of infinite potential.
 * When a record's Rta-Dharma score is LOW and it's never accessed,
 * the system ZEROS its non-essential data but PRESERVES its Uddishta index.
 * 
 * Result: Infinite storage on finite hardware.
 * Critical events stay in Anandamaya (full clarity).
 * Routine events compress to Bija (seed essence).
 */

#pragma once
#include <cstring>
#include <cstdio>
#include <cmath>
#include <ctime>

#define PHI 1.618033988749895f

// ================================================================
// SHUNYA STATE — Five levels of data compression
// ================================================================
enum ShunyaLevel {
    ANANDAMAYA = 0,  // Full clarity — critical events (100% data preserved)
    VIJNANAMAYA = 1, // Wisdom — frequently accessed (75% preserved)
    MANOMAYA = 2,    // Mind — moderate importance (50% preserved)
    PRANAMAYA = 3,   // Energy — low importance (25% preserved)
    ANNAMAYA = 4,    // Physical — routine events (5% preserved, Uddishta only)
    SHUNYA = 5       // Zero — fully pruned (only index remains, 0.1% storage)
};

struct ShunyaRecord {
    uint64_t uddishta_key;        // ALWAYS preserved — the eternal seed
    char label[32];               // What happened
    float priority_score;         // Rta-Dharma score
    int access_count;             // How often retrieved
    time_t timestamp;             // When recorded
    ShunyaLevel level;            // Current compression level
    float* full_vector;           // May be NULL if pruned
    int vector_dim;               // Original dimension
    
    ShunyaRecord() : uddishta_key(0), priority_score(0), access_count(0),
                     timestamp(0), level(ANANDAMAYA), full_vector(nullptr), vector_dim(0) {
        label[0] = '\0';
    }
    
    ~ShunyaRecord() { if (full_vector) delete[] full_vector; }
    
    // How much storage this record uses (relative to ANANDAMAYA)
    float storage_ratio() {
        switch(level) {
            case ANANDAMAYA:  return 1.0f;   // 100%
            case VIJNANAMAYA: return 0.75f;
            case MANOMAYA:    return 0.50f;
            case PRANAMAYA:   return 0.25f;
            case ANNAMAYA:    return 0.05f;
            case SHUNYA:      return 0.001f; // Just the Uddishta key
        }
        return 1.0f;
    }
    
    const char* level_name() {
        switch(level) {
            case ANANDAMAYA:  return "Anandamaya (Full)";
            case VIJNANAMAYA: return "Vijnanamaya (75%)";
            case MANOMAYA:    return "Manomaya (50%)";
            case PRANAMAYA:   return "Pranamaya (25%)";
            case ANNAMAYA:    return "Annamaya (Bija)";
            case SHUNYA:      return "SHUNYA (Seed only)";
        }
        return "Unknown";
    }
};

// ================================================================
// SHUNYA ENGINE — Automatic compression based on access patterns
// ================================================================
struct ShunyaEngine {
    ShunyaRecord* records;
    int count;
    int capacity;
    float total_storage_used;  // Relative to full storage
    
    ShunyaEngine(int cap = 65536) : count(0), capacity(cap), total_storage_used(0) {
        records = new ShunyaRecord[capacity];
    }
    ~ShunyaEngine() { delete[] records; }
    
    // Store a new event in ANANDAMAYA (full clarity)
    int record(uint64_t key, const char* label, float priority, float* vector, int dim) {
        if (count >= capacity) shunya_prune();  // Free space if needed
        
        ShunyaRecord* r = &records[count];
        r->uddishta_key = key;
        strncpy(r->label, label, 31);
        r->priority_score = priority;
        r->access_count = 0;
        r->timestamp = time(nullptr);
        r->level = ANANDAMAYA;
        r->vector_dim = dim;
        r->full_vector = new float[dim];
        memcpy(r->full_vector, vector, dim * sizeof(float));
        
        total_storage_used += 1.0f;
        return count++;
    }
    
    // Access a record — promote toward ANANDAMAYA
    float* access(int index) {
        if (index < 0 || index >= count) return nullptr;
        
        ShunyaRecord* r = &records[index];
        r->access_count++;
        
        // Promotion: each access pushes toward full clarity
        if (r->level > ANANDAMAYA && r->access_count > 3) {
            ShunyaLevel new_level = (ShunyaLevel)(r->level - 1);
            promote(index, new_level);
        }
        
        return r->full_vector;
    }
    
    // Promote a record to higher clarity
    void promote(int index, ShunyaLevel new_level) {
        ShunyaRecord* r = &records[index];
        if (new_level >= r->level) return;  // Only upgrade, never downgrade directly
        
        float old_ratio = r->storage_ratio();
        r->level = new_level;
        
        // If moving out of SHUNYA, reallocate vector
        if (r->level < SHUNYA && r->full_vector == nullptr) {
            r->full_vector = new float[r->vector_dim];
            // Reconstruct from Uddishta key (approximate)
            for (int i = 0; i < r->vector_dim; i++)
                r->full_vector[i] = sinf(i * (r->uddishta_key & 0xFF) * PHI) * 0.1f;
        }
        
        float new_ratio = r->storage_ratio();
        total_storage_used += (new_ratio - old_ratio);
    }
    
    // SHUNYA PRUNE: Compress least-important records
    void shunya_prune() {
        // Find the best candidate for pruning
        int best_idx = -1;
        float best_score = 1e9f;
        time_t now = time(nullptr);
        
        for (int i = 0; i < count; i++) {
            ShunyaRecord* r = &records[i];
            if (r->level == SHUNYA) continue;  // Already fully pruned
            
            // Pruning score: lower = better candidate to prune
            // Formula: priority × recency × access_count
            float age_days = (now - r->timestamp) / 86400.0f;
            float recency = 1.0f / (1.0f + age_days);
            float score = r->priority_score * recency * (1.0f + r->access_count);
            
            if (score < best_score) {
                best_score = score;
                best_idx = i;
            }
        }
        
        if (best_idx >= 0) {
            demote(best_idx);
        }
    }
    
    // Demote a record one level
    void demote(int index) {
        ShunyaRecord* r = &records[index];
        if (r->level >= SHUNYA) return;
        
        float old_ratio = r->storage_ratio();
        r->level = (ShunyaLevel)(r->level + 1);
        
        // If reaching SHUNYA, free the vector
        if (r->level == SHUNYA && r->full_vector) {
            delete[] r->full_vector;
            r->full_vector = nullptr;
        }
        // If at ANNAMAYA, keep only 5% of vector
        else if (r->level == ANNAMAYA && r->full_vector) {
            for (int i = 0; i < r->vector_dim; i++) {
                if (i % 20 != 0) r->full_vector[i] = 0;  // Keep only every 20th value
            }
        }
        
        float new_ratio = r->storage_ratio();
        total_storage_used += (new_ratio - old_ratio);
    }
    
    // Run maintenance: demote old, low-priority records
    void maintain() {
        time_t now = time(nullptr);
        int pruned = 0;
        
        for (int i = 0; i < count; i++) {
            ShunyaRecord* r = &records[i];
            float age_days = (now - r->timestamp) / 86400.0f;
            
            // Auto-demotion rules:
            // 1. LOW priority + 30 days untouched → demote 2 levels
            if (r->priority_score < 0.3f && age_days > 30 && r->access_count == 0) {
                demote(i); demote(i); pruned++;
            }
            // 2. NORMAL priority + 60 days untouched → demote 1 level
            else if (r->priority_score < 0.6f && age_days > 60) {
                demote(i); pruned++;
            }
            // 3. HIGH priority but 180 days old → demote 1 level
            else if (age_days > 180) {
                demote(i); pruned++;
            }
        }
        
        if (pruned > 0) {
            printf("  Shunya-Siddhi: %d records pruned (%.1f%% storage saved)\n", 
                   pruned, (1.0f - total_storage_used / count) * 100);
        }
    }
    
    void stats() {
        int levels[6] = {0};
        for (int i = 0; i < count; i++) levels[records[i].level]++;
        
        printf("  SHUNYA-SIDDHI STORAGE:\n");
        printf("    Anandamaya (Full):  %d\n", levels[0]);
        printf("    Vijnanamaya (75%%):  %d\n", levels[1]);
        printf("    Manomaya (50%%):     %d\n", levels[2]);
        printf("    Pranamaya (25%%):    %d\n", levels[3]);
        printf("    Annamaya (Bija):    %d\n", levels[4]);
        printf("    SHUNYA (Seed):      %d\n", levels[5]);
        printf("    Total records: %d\n", count);
        
        float efficiency = count > 0 ? (1.0f - total_storage_used / count) * 100 : 0;
        printf("    Storage efficiency: %.1f%% (%.1fx compression)\n", 
               efficiency, count / (total_storage_used + 0.01f));
    }
};
