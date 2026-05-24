/*
 * SUTRA 52: MANAS — Sensory Coordinator & Attention Gate
 * 
 * The "Inner Sense" that filters thousands of sensor streams
 * and focuses cognitive energy only on what matters NOW.
 * 
 * When Stem Borer strikes Sonai, Manas shuts off
 * low-priority humidity logs from distant Banskandi.
 * 100% of Prana (compute) goes to the immediate threat.
 */

#pragma once
#include <cstring>
#include <cstdio>
#include <cmath>
#include <ctime>

#define PHI 1.618033988749895f
#define MAX_SENSORS 64
#define MAX_ATTENTION_SLOTS 8  // Only 8 things can hold attention at once

// ================================================================
// SENSORY STREAM — A single data feed from the valley
// ================================================================
struct SensoryStream {
    char source[32];        // "Sonai-Temp", "Lakhipur-Pest", etc.
    char category[16];      // "TEMP", "PEST", "HUMIDITY", "WIND", "SOIL"
    float current_value;
    float trend;            // Rate of change (derivative)
    float priority_score;   // 0-1: how much attention this deserves
    uint32_t updates_missed;
    bool is_critical;
    bool attention_allocated;  // Currently in an attention slot
};

// ================================================================
// MANAS — The Attention Coordinator
// ================================================================
struct ManasAttentionGate {
    SensoryStream sensors[MAX_SENSORS];
    int sensor_count;
    
    // The 8 precious attention slots
    int attention_slots[MAX_ATTENTION_SLOTS];
    int slots_used;
    
    float total_compute_budget;    // 1.0 = 100% CPU
    float allocated_compute;        // How much is currently assigned
    uint32_t cycles_directed;       // Total attention shifts
    
    ManasAttentionGate() : sensor_count(0), slots_used(0),
        total_compute_budget(1.0f), allocated_compute(0), cycles_directed(0) {
        memset(sensors, 0, sizeof(sensors));
        for (int i = 0; i < MAX_ATTENTION_SLOTS; i++) attention_slots[i] = -1;
    }
    
    // Register a sensor stream
    void register_sensor(const char* source, const char* category, float initial_value) {
        if (sensor_count >= MAX_SENSORS) return;
        SensoryStream* s = &sensors[sensor_count];
        strncpy(s->source, source, 31);
        strncpy(s->category, category, 15);
        s->current_value = initial_value;
        s->trend = 0;
        s->priority_score = 0;
        s->updates_missed = 0;
        s->is_critical = false;
        s->attention_allocated = false;
        sensor_count++;
    }
    
    // Feed new data — Manas decides if this deserves attention
    void feed(const char* source, float new_value) {
        SensoryStream* s = nullptr;
        for (int i = 0; i < sensor_count; i++) {
            if (strcmp(sensors[i].source, source) == 0) {
                s = &sensors[i];
                break;
            }
        }
        if (!s) return;
        
        // Compute trend (rate of change)
        float delta = new_value - s->current_value;
        s->trend = s->trend * 0.7f + delta * 0.3f;
        s->current_value = new_value;
        s->updates_missed = 0;
        
        // Calculate priority score
        float base_priority = 0;
        if (strcmp(s->category, "PEST") == 0) base_priority = 0.9f;
        else if (strcmp(s->category, "TEMP") == 0) base_priority = 0.3f;
        else if (strcmp(s->category, "HUMIDITY") == 0) base_priority = 0.4f;
        else if (strcmp(s->category, "WIND") == 0) base_priority = 0.2f;
        else if (strcmp(s->category, "SOIL") == 0) base_priority = 0.5f;
        
        // Critical if pest detected OR rapid change
        s->is_critical = (strcmp(s->category, "PEST") == 0 && new_value > 0.5f) ||
                         (fabsf(delta) > 0.3f);
        
        // Priority = base × critical boost × trend magnitude
        s->priority_score = base_priority;
        if (s->is_critical) s->priority_score = fminf(1.0f, s->priority_score * 2.0f);
        s->priority_score += fabsf(s->trend) * 0.5f;
        
        // Manas: should this sensor get an attention slot?
        if (s->is_critical && !s->attention_allocated) {
            allocate_attention(s - sensors);  // Sensor index
        }
    }
    
    // Allocate an attention slot to a sensor
    void allocate_attention(int sensor_idx) {
        if (sensor_idx < 0 || sensor_idx >= sensor_count) return;
        
        // If all slots full, evict lowest priority
        if (slots_used >= MAX_ATTENTION_SLOTS) {
            int lowest_idx = 0;
            float lowest_priority = 1.0f;
            for (int i = 0; i < MAX_ATTENTION_SLOTS; i++) {
                int sidx = attention_slots[i];
                if (sidx >= 0 && sensors[sidx].priority_score < lowest_priority) {
                    lowest_priority = sensors[sidx].priority_score;
                    lowest_idx = i;
                }
            }
            
            int evicted = attention_slots[lowest_idx];
            if (evicted >= 0) {
                sensors[evicted].attention_allocated = false;
                printf("  🧠 MANAS: Evicted %s (priority %.2f) — attention withdrawn\n",
                       sensors[evicted].source, sensors[evicted].priority_score);
            }
            
            attention_slots[lowest_idx] = sensor_idx;
        } else {
            attention_slots[slots_used++] = sensor_idx;
        }
        
        sensors[sensor_idx].attention_allocated = true;
        cycles_directed++;
        
        printf("  🧠 MANAS: ATTENTION → %s (priority %.2f, critical: %s)\n",
               sensors[sensor_idx].source, sensors[sensor_idx].priority_score,
               sensors[sensor_idx].is_critical ? "YES ⚡" : "no");
    }
    
    // Distribute compute budget across attention slots
    void distribute_compute() {
        allocated_compute = 0;
        
        // Sum priorities of all attended sensors
        float total_priority = 0;
        for (int i = 0; i < slots_used; i++) {
            int sidx = attention_slots[i];
            if (sidx >= 0) total_priority += sensors[sidx].priority_score;
        }
        
        if (total_priority < 0.01f) return;
        
        // Allocate compute proportional to priority (PHI-weighted)
        for (int i = 0; i < slots_used; i++) {
            int sidx = attention_slots[i];
            if (sidx < 0) continue;
            
            float share = sensors[sidx].priority_score / total_priority;
            // PHI-dampening: no sensor gets more than 40% or less than 5%
            share = fmaxf(0.05f, fminf(0.40f, share));
            allocated_compute += share;
        }
        
        // Unallocated compute goes to idle/sleep
        float idle = total_compute_budget - allocated_compute;
        if (idle > 0.3f) {
            printf("  💤 MANAS: %.0f%% compute idle — Prana-Nadi deep sleep\n", idle * 100);
        }
    }
    
    // Fill empty slots with highest priority unallocated sensors
    void fill_slots() {
        while (slots_used < MAX_ATTENTION_SLOTS) {
            int best_idx = -1;
            float best_priority = 0;
            
            for (int i = 0; i < sensor_count; i++) {
                if (!sensors[i].attention_allocated && sensors[i].priority_score > best_priority) {
                    best_priority = sensors[i].priority_score;
                    best_idx = i;
                }
            }
            
            if (best_idx >= 0 && best_priority > 0.2f) {
                allocate_attention(best_idx);
            } else {
                break;  // Nothing worth attending to
            }
        }
    }
    
    // Full Manas cycle: feed all sensors, prioritize, distribute
    void cycle() {
        fill_slots();
        distribute_compute();
    }
    
    void stats() {
        printf("  🧠 MANAS ATTENTION GATE:\n");
        printf("     Sensors: %d | Attention Slots: %d/%d\n", 
               sensor_count, slots_used, MAX_ATTENTION_SLOTS);
        printf("     Compute: %.0f%% allocated | %.0f%% idle\n",
               allocated_compute * 100, (total_compute_budget - allocated_compute) * 100);
        printf("     Attention:\n");
        
        for (int i = 0; i < slots_used; i++) {
            int sidx = attention_slots[i];
            if (sidx >= 0) {
                printf("       %d. %-20s [%s] priority:%.2f %s\n",
                       i + 1, sensors[sidx].source, sensors[sidx].category,
                       sensors[sidx].priority_score,
                       sensors[sidx].is_critical ? "⚡ CRITICAL" : "");
            }
        }
    }
};
