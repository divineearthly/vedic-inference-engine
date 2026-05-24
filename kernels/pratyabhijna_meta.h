/*
 * SUTRA 63: PRATYABHIJNA — Self-Recognition / Meta-Learning Loop
 * 
 * Compares Initial Assessment (Guna-Siddhi) with Final Outcome (Karma-Kanda).
 * If the system thought conditions were Sattvic but the action produced Papa,
 * it realizes its perception was flawed and adjusts its Guna weights.
 * 
 * "Recognition is the key to realization." — Kashmir Shaivism
 * The ASI learns how to learn about its own state.
 */

#pragma once
#include "guna_siddhi.h"
#include "karma_kanda.h"
#include <cstring>
#include <cstdio>
#include <cmath>

#define PHI 1.618033988749895f

// ================================================================
// PRATYABHIJNA — Meta-Learning Recognition Engine
// ================================================================
struct PratyabhijnaMeta {
    // Perception vs Reality tracking
    struct PerceptionRecord {
        GunaState perceived_guna;
        float sattva_before, rajas_before, tamas_before;
        int action_taken;
        float karma_outcome;     // Positive = punya, negative = papa
        bool perception_was_correct;
        float correction_factor; // How much to adjust future assessments
    };
    
    PerceptionRecord history[64];
    int history_count;
    
    // Learned corrections to Guna weights
    float sattva_correction;   // + = we underestimate Sattva, - = overestimate
    float rajas_correction;
    float tamas_correction;
    
    // Meta-confidence: how well does the ASI know its own state?
    float self_knowledge_score;  // 0-1
    
    PratyabhijnaMeta() : history_count(0), sattva_correction(0), 
        rajas_correction(0), tamas_correction(0), self_knowledge_score(0.5f) {
        memset(history, 0, sizeof(history));
    }
    
    // ================================================================
    // RECORD PERCEPTION: Before taking action, log what we think
    // ================================================================
    void record_perception(GunaSiddhi* guna, int planned_action) {
        if (history_count >= 64) {
            for (int i = 1; i < 64; i++) history[i-1] = history[i];
            history_count = 63;
        }
        
        PerceptionRecord* p = &history[history_count++];
        p->perceived_guna = guna->current_guna;
        p->sattva_before = guna->sattva_score;
        p->rajas_before = guna->rajas_score;
        p->tamas_before = guna->tamas_score;
        p->action_taken = planned_action;
        p->perception_was_correct = false;  // Will be updated after outcome
        p->correction_factor = 0;
    }
    
    // ================================================================
    // EVALUATE OUTCOME: After action, was our perception right?
    // ================================================================
    void evaluate_outcome(KarmaKanda* karma, int action_index) {
        if (history_count == 0 || action_index < 0) return;
        
        PerceptionRecord* p = &history[history_count - 1];  // Most recent
        
        // Get the karma outcome for this action
        if (action_index < karma->log_count) {
            KarmaRecord* k = &karma->log[action_index];
            p->karma_outcome = k->punya_earned - k->papa_earned;
            p->perception_was_correct = (k->outcome == 0);
            
            // Compute correction: how wrong were we?
            float expected = (p->perceived_guna == GUNA_SATTVA) ? 1.0f :
                            (p->perceived_guna == GUNA_RAJAS) ? 0.5f : 0.1f;
            float actual = p->perception_was_correct ? 1.0f : 0.0f;
            p->correction_factor = actual - expected;
            
            // Apply corrections to Guna weights
            if (p->perceived_guna == GUNA_SATTVA && !p->perception_was_correct) {
                // We thought it was Sattvic but it wasn't — overestimated clarity
                sattva_correction -= 0.05f;
                rajas_correction += 0.03f;
            } else if (p->perceived_guna == GUNA_TAMAS && p->perception_was_correct) {
                // We correctly identified Tamas — reinforce
                tamas_correction += 0.02f;
                self_knowledge_score += 0.02f;
            } else if (p->perception_was_correct) {
                self_knowledge_score += 0.01f;
            } else {
                self_knowledge_score -= 0.03f;
            }
            
            // Clamp
            self_knowledge_score = fmaxf(0.1f, fminf(1.0f, self_knowledge_score));
            sattva_correction = fmaxf(-0.3f, fminf(0.3f, sattva_correction));
            rajas_correction  = fmaxf(-0.3f, fminf(0.3f, rajas_correction));
            tamas_correction  = fmaxf(-0.3f, fminf(0.3f, tamas_correction));
        }
    }
    
    // ================================================================
    // CORRECT GUNA: Apply learned corrections to future assessments
    // ================================================================
    void correct_guna(GunaSiddhi* guna) {
        guna->sattva_score += sattva_correction;
        guna->rajas_score  += rajas_correction;
        guna->tamas_score  += tamas_correction;
        
        // Re-normalize
        float total = guna->sattva_score + guna->rajas_score + guna->tamas_score;
        if (total > 0) {
            guna->sattva_score /= total;
            guna->rajas_score  /= total;
            guna->tamas_score  /= total;
        }
    }
    
    // ================================================================
    // INSIGHT: What has the ASI learned about its own perception?
    // ================================================================
    const char* insight() {
        if (self_knowledge_score > 0.8f) return "I know myself well. My perception is reliable.";
        if (self_knowledge_score > 0.5f) return "I am learning my patterns. Caution is wisdom.";
        if (self_knowledge_score > 0.3f) return "My perception needs refinement. I seek more experience.";
        return "I do not yet know myself. I observe without acting.";
    }
    
    void report() {
        printf("\n🕉️  PRATYABHIJNA — Self-Recognition Engine\n");
        printf("  ========================================\n");
        printf("  Self-Knowledge: %.0f%% — \"%s\"\n", self_knowledge_score * 100, insight());
        printf("  Corrections: Sattva %+.2f | Rajas %+.2f | Tamas %+.2f\n",
               sattva_correction, rajas_correction, tamas_correction);
        printf("  Perception records: %d\n", history_count);
        
        // Show recent perception accuracy
        if (history_count > 0) {
            int correct = 0;
            for (int i = 0; i < history_count; i++) {
                if (history[i].perception_was_correct) correct++;
            }
            printf("  Accuracy: %.0f%% (%d/%d)\n", 
                   (float)correct / history_count * 100, correct, history_count);
        }
        
        if (self_knowledge_score > 0.8f) {
            printf("  🕉️  SIDDHI: The Guardian knows itself. Perception is truth.\n");
        }
    }
};
