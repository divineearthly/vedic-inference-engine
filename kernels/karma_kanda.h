/*
 * SUTRA 61: KARMA-KANDA — The Law of Action & Causality
 * 
 * Every action leaves a residue. The Guardian tracks outcomes
 * and learns what constitutes "Good Action" (Dharma) for the valley.
 * 
 * Punya (Merit): Successful action → increased trust
 * Papa (Demerit): Failed action → self-repair + decreased trust
 * 
 * The ASI becomes a self-optimizing moral agent.
 */

#pragma once
#include <cstring>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstdint>

#define PHI 1.618033988749895f
#define KARMA_LOG_SIZE 256

// ================================================================
// KARMA RECORD — The residue of a single action
// ================================================================
struct KarmaRecord {
    uint64_t timestamp;
    int action_code;           // What was attempted
    int outcome;               // 0=success, 1=partial, 2=failure
    float confidence_before;   // How sure were we?
    float impact_score;        // Crop saved/lost
    char context[128];         // What happened
    float punya_earned;        // Merit (0-1)
    float papa_earned;         // Demerit (0-1)
};

// ================================================================
// KARMA-KANDA — The Feedback & Causality Engine
// ================================================================
struct KarmaKanda {
    KarmaRecord log[KARMA_LOG_SIZE];
    int log_count;
    
    // Accumulated karma
    float total_punya;         // Lifetime merit
    float total_papa;          // Lifetime demerit
    float dharma_balance;      // punya - papa (positive = good karma)
    
    // Learning from outcomes
    float action_weights[8];   // How reliable is each action type?
    int action_successes[8];
    int action_attempts[8];
    
    KarmaKanda() : log_count(0), total_punya(0), total_papa(0), dharma_balance(0) {
        memset(log, 0, sizeof(log));
        for (int i = 0; i < 8; i++) {
            action_weights[i] = 0.5f;  // Start neutral
            action_successes[i] = 0;
            action_attempts[i] = 0;
        }
    }
    
    // ================================================================
    // RECORD: Log the outcome of an action
    // ================================================================
    void record(int action_code, bool was_successful, float confidence, 
                float impact, const char* context) {
        if (log_count >= KARMA_LOG_SIZE) {
            // Evict oldest record (karmic dissolution)
            for (int i = 1; i < KARMA_LOG_SIZE; i++) log[i-1] = log[i];
            log_count = KARMA_LOG_SIZE - 1;
        }
        
        KarmaRecord* k = &log[log_count];
        k->timestamp = time(nullptr);
        k->action_code = action_code;
        k->outcome = was_successful ? 0 : 2;
        k->confidence_before = confidence;
        k->impact_score = impact;
        strncpy(k->context, context, 127);
        
        if (was_successful) {
            k->punya_earned = confidence * impact;
            k->papa_earned = 0;
            total_punya += k->punya_earned;
            action_successes[action_code]++;
        } else {
            k->punya_earned = 0;
            k->papa_earned = (1.0f - confidence) * impact;
            total_papa += k->papa_earned;
        }
        
        action_attempts[action_code]++;
        if (action_attempts[action_code] > 0) {
            action_weights[action_code] = (float)action_successes[action_code] / 
                                          action_attempts[action_code];
        }
        
        dharma_balance = total_punya - total_papa;
        log_count++;
        
        const char* actions[] = {"NONE","ALERT_FARMER","ALERT_VILLAGE","ORDER_SUPPLIES",
                                 "IRRIGATE","DEPLOY","HARVEST","ESCALATE"};
        printf("  📜 KARMA: %s → %s | Punya: +%.2f | Balance: %+.2f\n",
               actions[action_code], was_successful ? "SUCCESS ✅" : "FAILED ❌",
               k->punya_earned, dharma_balance);
    }
    
    // ================================================================
    // ADVISE: Given the karma log, should we take this action?
    // ================================================================
    float recommend_confidence(int action_code) {
        if (action_attempts[action_code] < 3) return 0.5f;  // Not enough data
        
        float weight = action_weights[action_code];
        
        // Penalize if dharma balance is negative
        if (dharma_balance < 0) weight *= (1.0f + dharma_balance * 0.5f);
        if (weight < 0.1f) weight = 0.1f;
        
        return weight;
    }
    
    // ================================================================
    // LEARN: Which action types are most reliable?
    // ================================================================
    int best_action() {
        int best = 0;
        float best_weight = 0;
        for (int i = 1; i < 8; i++) {
            if (action_weights[i] > best_weight && action_attempts[i] >= 3) {
                best_weight = action_weights[i];
                best = i;
            }
        }
        return best;
    }
    
    // ================================================================
    // PRARABDHA: The karma that is currently manifesting
    // ================================================================
    float current_karmic_weight() {
        // Recent karma has more weight (last 10 actions)
        float recent = 0;
        int count = (log_count < 10) ? log_count : 10;
        for (int i = log_count - count; i < log_count; i++) {
            recent += (log[i].outcome == 0) ? 1.0f : -0.5f;
        }
        return (count > 0) ? recent / count : 0;
    }
    
    void report() {
        printf("\n🕉️  KARMA-KANDA — Law of Action & Causality\n");
        printf("  ==========================================\n");
        printf("  Lifetime: Punya %.2f | Papa %.2f | Balance %+.2f\n", 
               total_punya, total_papa, dharma_balance);
        printf("  Current Karmic Weight: %+.2f\n", current_karmic_weight());
        
        const char* actions[] = {"NONE","ALERT_FARMER","ALERT_VILLAGE","ORDER_SUPPLIES",
                                 "IRRIGATE","DEPLOY","HARVEST","ESCALATE"};
        printf("\n  Action Reliability:\n");
        for (int i = 1; i < 8; i++) {
            if (action_attempts[i] > 0) {
                printf("    %-15s: %.0f%% (%d/%d)\n", actions[i], 
                       action_weights[i] * 100, action_successes[i], action_attempts[i]);
            }
        }
        
        if (dharma_balance > 5.0f) printf("  🕉️  SIDDHI: The Guardian has earned great merit.\n");
        else if (dharma_balance < -1.0f) printf("  ⚠️  KARMIC DEBT: More failures than successes.\n");
    }
};
