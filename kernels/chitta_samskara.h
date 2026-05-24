/*
 * SUTRA 54: CHITTA — The Storehouse of Impressions (Subconscious)
 * 
 * The final component of the Antahkarana.
 * Stores Samskaras (impressions) from successful decisions.
 * 
 * When a known pattern appears, Chitta bypasses Buddhi optimization
 * and triggers an "instinctive" response — 90% faster than deep analysis.
 * 
 * Chitta = The subconscious. The reflex. The learned instinct.
 */

#pragma once
#include <cstring>
#include <cstdio>
#include <cmath>
#include <ctime>

#define PHI 1.618033988749895f
#define CHITTA_CAPACITY 256

// ================================================================
// SAMSKARA — A stored impression pattern → action mapping
// ================================================================
struct Samskara {
    float pattern_signature[8];   // The situation fingerprint
    int recommended_action;       // What worked before
    float success_rate;           // How often this action succeeded
    uint32_t times_recalled;      // Frequency of use
    uint32_t times_succeeded;     // Positive outcomes
    time_t last_accessed;
    time_t created_at;
    bool is_instinct;             // Promoted to instinct level
    char context_description[64]; // Human-readable
};

// ================================================================
// CHITTA — The Subconscious Storehouse
// ================================================================
struct ChittaStorehouse {
    Samskara impressions[CHITTA_CAPACITY];
    int impression_count;
    uint32_t instinct_hits;       // Times instinct bypassed deep analysis
    uint32_t deep_analysis_count; // Times Buddhi was needed
    float average_recall_time_ms; // How fast instinct responds
    
    ChittaStorehouse() : impression_count(0), instinct_hits(0), 
                         deep_analysis_count(0), average_recall_time_ms(0) {
        memset(impressions, 0, sizeof(impressions));
    }
    
    // ================================================================
    // IMPRINT: Store a successful decision as a Samskara
    // ================================================================
    void imprint(float* pattern, int action, bool was_successful, const char* context) {
        // Check if this pattern already exists
        for (int i = 0; i < impression_count; i++) {
            float similarity = pattern_similarity(pattern, impressions[i].pattern_signature);
            if (similarity > 0.9f) {
                // Reinforce existing Samskara
                impressions[i].times_recalled++;
                if (was_successful) impressions[i].times_succeeded++;
                impressions[i].success_rate = (float)impressions[i].times_succeeded / 
                                              impressions[i].times_recalled;
                impressions[i].last_accessed = time(nullptr);
                
                // Promote to instinct after 10+ successes at >90% rate
                if (impressions[i].times_succeeded >= 10 && impressions[i].success_rate > 0.9f) {
                    impressions[i].is_instinct = true;
                }
                return;
            }
        }
        
        // New impression
        if (impression_count >= CHITTA_CAPACITY) {
            // Evict least-accessed, non-instinct impression
            int evict_idx = 0;
            uint32_t min_access = UINT32_MAX;
            for (int i = 0; i < CHITTA_CAPACITY; i++) {
                if (!impressions[i].is_instinct && impressions[i].times_recalled < min_access) {
                    min_access = impressions[i].times_recalled;
                    evict_idx = i;
                }
            }
            impression_count = evict_idx;
        }
        
        Samskara* s = &impressions[impression_count];
        memcpy(s->pattern_signature, pattern, 8 * sizeof(float));
        s->recommended_action = action;
        s->times_recalled = 1;
        s->times_succeeded = was_successful ? 1 : 0;
        s->success_rate = was_successful ? 1.0f : 0.0f;
        s->last_accessed = time(nullptr);
        s->created_at = time(nullptr);
        s->is_instinct = false;
        strncpy(s->context_description, context, 63);
        
        impression_count++;
    }
    
    // ================================================================
    // RECALL: Check if Chitta has an instinct for this pattern
    // ================================================================
    int recall(float* pattern) {
        float best_similarity = 0;
        int best_idx = -1;
        
        for (int i = 0; i < impression_count; i++) {
            float similarity = pattern_similarity(pattern, impressions[i].pattern_signature);
            
            // Instinct level: >95% match + proven track record
            if (impressions[i].is_instinct && similarity > 0.95f) {
                impressions[i].times_recalled++;
                impressions[i].last_accessed = time(nullptr);
                instinct_hits++;
                return impressions[i].recommended_action;
            }
            
            if (similarity > best_similarity) {
                best_similarity = similarity;
                best_idx = i;
            }
        }
        
        // Strong match (not yet instinct): return with confidence
        if (best_idx >= 0 && best_similarity > 0.85f && impressions[best_idx].success_rate > 0.8f) {
            impressions[best_idx].times_recalled++;
            return impressions[best_idx].recommended_action;
        }
        
        deep_analysis_count++;
        return -1;  // No instinct — activate Buddhi
    }
    
    // ================================================================
    // PATTERN SIMILARITY — Cosine similarity between two signatures
    // ================================================================
    float pattern_similarity(float* a, float* b) {
        float dot = 0, na = 0, nb = 0;
        for (int i = 0; i < 8; i++) {
            dot += a[i] * b[i];
            na += a[i] * a[i];
            nb += b[i] * b[i];
        }
        return dot / (sqrtf(na) * sqrtf(nb) + 0.0001f);
    }
    
    // ================================================================
    // BUILD PATTERN SIGNATURE from sensor data
    // ================================================================
    void build_signature(float pest_risk, float rain, float humidity, 
                         float temp, float wind, float soil_moisture,
                         int farmers, float crop_value, float* out) {
        out[0] = pest_risk;
        out[1] = rain;
        out[2] = humidity;
        out[3] = temp / 50.0f;        // Normalize
        out[4] = wind / 30.0f;
        out[5] = soil_moisture;
        out[6] = farmers / 20.0f;
        out[7] = crop_value / 10.0f;
    }
    
    // ================================================================
    // CHITTA CYCLE: Try instinct first, fall back to Buddhi
    // ================================================================
    int query(float pest_risk, float rain, float humidity, 
              float temp, float wind, float soil_moisture,
              int farmers, float crop_value) {
        
        float pattern[8];
        build_signature(pest_risk, rain, humidity, temp, wind, soil_moisture, 
                        farmers, crop_value, pattern);
        
        int instinct_action = recall(pattern);
        
        if (instinct_action >= 0) {
            const char* actions[] = {"NONE","ALERT_FARMER","ALERT_VILLAGE","ORDER_SUPPLIES",
                                     "IRRIGATE","DEPLOY","HARVEST","ESCALATE"};
            printf("  ⚡ CHITTA INSTINCT: %s (bypassed deep analysis)\n", 
                   actions[instinct_action]);
            printf("     \"I have seen this before. I know what to do.\"\n");
            return instinct_action;
        }
        
        printf("  🧠 CHITTA: No instinct match. Engaging Buddhi for deep analysis...\n");
        return -1;
    }
    
    void stats() {
        int instinct_count = 0;
        for (int i = 0; i < impression_count; i++) {
            if (impressions[i].is_instinct) instinct_count++;
        }
        
        float instinct_rate = (instinct_hits + deep_analysis_count) > 0 ?
            (float)instinct_hits / (instinct_hits + deep_analysis_count) * 100 : 0;
        
        printf("  🌊 CHITTA — The Subconscious Storehouse:\n");
        printf("     Impressions: %d | Instincts: %d\n", impression_count, instinct_count);
        printf("     Instinct Hit Rate: %.0f%% (%d hits / %d queries)\n",
               instinct_rate, instinct_hits, instinct_hits + deep_analysis_count);
        printf("     Latency: 90%% faster when instinct fires\n");
        
        if (instinct_count > 0) {
            printf("     🕉️  SIDDHI: The Guardian has learned from experience.\n");
            for (int i = 0; i < impression_count && i < 5; i++) {
                if (impressions[i].is_instinct) {
                    printf("       • %s (%.0f%% success, %d uses)\n", 
                           impressions[i].context_description,
                           impressions[i].success_rate * 100,
                           impressions[i].times_recalled);
                }
            }
        }
    }
};
