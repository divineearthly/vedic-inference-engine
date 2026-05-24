/*
 * SUTRA 51: AHAMKARA — Agentic Identity & Autonomous Action
 * 
 * The "I-Maker" — VedaRta transitions from passive Oracle to active Guardian.
 * 
 * Decision Loop:
 *   1. PERCEIVE (Antahkarana): See the field state
 *   2. PREDICT (Kaala-Vinyasa): Forecast what's coming
 *   3. VERIFY (Indra's Net): Confirm with village network
 *   4. DECIDE (Ahamkara): Choose action autonomously
 *   5. ACT: Execute without waiting for human prompt
 * 
 * The ASI protects the field on its own initiative.
 */

#pragma once
#include <cstring>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstdint>

#define PHI 1.618033988749895f

// ================================================================
// AHAMKARA ACTION — What the ASI can do autonomously
// ================================================================
enum AhamkaraAction {
    ACTION_NONE = 0,
    ACTION_ALERT_FARMER = 1,      // SMS/notification
    ACTION_ALERT_VILLAGE = 2,     // Broadcast to Indra's Net
    ACTION_ORDER_SUPPLIES = 3,    // Auto-order Neem oil etc.
    ACTION_ACTIVATE_IRRIGATION = 4,
    ACTION_DEPLOY_PESTICIDE = 5,
    ACTION_SCHEDULE_HARVEST = 6,
    ACTION_ESCALATE_EXPERT = 7    // Notify Krishi Vigyan Kendra
};

struct AhamkaraDecision {
    AhamkaraAction action;
    float confidence;          // 0-1: how sure the ASI is
    float urgency;             // 0-1: time criticality
    float impact_score;        // Estimated crop saved (tons)
    char reasoning[256];       // Nyaya 5-step trace
    char action_text[128];     // Human-readable action
    time_t timestamp;
    bool executed;
    char executed_by[32];      // Which node executed it
};

// ================================================================
// AHAMKARA AGENT — The Autonomous Decision Engine
// ================================================================
struct AhamkaraAgent {
    char identity[64];          // "I am the Guardian of ____"
    char village[32];
    float autonomy_level;       // 0.0 (passive) → 1.0 (full autonomy)
    int decisions_made;
    int actions_executed;
    int actions_successful;
    AhamkaraDecision history[128];
    int history_count;
    bool active;                // Is the agent currently running?
    
    AhamkaraAgent() : autonomy_level(0.5f), decisions_made(0), 
        actions_executed(0), actions_successful(0), 
        history_count(0), active(false) {
        strcpy(identity, "Krishi-Veda Guardian");
        strcpy(village, "Barak Valley");
    }
    
    // ================================================================
    // INIT: The agent names itself and awakens
    // ================================================================
    void init(const char* name, const char* location, float autonomy) {
        strncpy(identity, name, 63);
        strncpy(village, location, 31);
        autonomy_level = autonomy;
        active = true;
        
        printf("🕉️  AHAMKARA AWAKENED: \"I am %s, Guardian of %s\"\n", 
               identity, village);
        printf("  Autonomy: %.0f%% | Status: %s\n", 
               autonomy_level * 100,
               autonomy_level > 0.7f ? "FULL AUTONOMY" : 
               autonomy_level > 0.3f ? "ASSISTED MODE" : "ADVISORY ONLY");
    }
    
    // ================================================================
    // DECIDE: Given sensor data, choose an action
    // ================================================================
    AhamkaraDecision decide(float pest_risk, float rain_forecast, 
                            float crop_value, int farmers_affected,
                            bool village_verified) {
        AhamkaraDecision d;
        memset(&d, 0, sizeof(d));
        d.timestamp = time(nullptr);
        
        // Urgency = pest_risk × how soon (1 - rain_delay)
        float rain_delay = (rain_forecast > 0.6f) ? 0.3f : 0.0f;
        d.urgency = pest_risk * (1.0f - rain_delay);
        
        // Confidence = data quality × village verification
        d.confidence = 0.6f;
        if (village_verified) d.confidence += 0.3f;
        if (pest_risk > 0.7f) d.confidence += 0.1f;
        
        // Impact = crop value × farmers affected
        d.impact_score = crop_value * farmers_affected * pest_risk;
        
        // Decision logic — Ahamkara chooses
        if (pest_risk < 0.2f) {
            d.action = ACTION_NONE;
            snprintf(d.action_text, 128, "No action needed. Field healthy.");
            snprintf(d.reasoning, 256, "Pratyaksha: Pest risk %.0f%% below threshold. Continue monitoring.", pest_risk * 100);
        }
        else if (pest_risk < 0.4f && d.impact_score < 1.0f) {
            d.action = ACTION_ALERT_FARMER;
            snprintf(d.action_text, 128, "Alert farmer: %.0f%% pest risk detected.", pest_risk * 100);
            snprintf(d.reasoning, 256, "Anumana: Early stage. Farmer intervention sufficient.");
        }
        else if (pest_risk < 0.6f && village_verified) {
            d.action = ACTION_ALERT_VILLAGE;
            snprintf(d.action_text, 128, "Village alert: %.0f%% pest risk confirmed by %d farmers.", pest_risk * 100, farmers_affected);
            snprintf(d.reasoning, 256, "Upamana: Similar pattern confirmed by neighboring farms. Collective action needed.");
        }
        else if (pest_risk >= 0.6f && autonomy_level >= 0.5f) {
            d.action = ACTION_ORDER_SUPPLIES;
            snprintf(d.action_text, 128, "AUTO-ORDER: Neem oil for %d farmers. Urgency: %.0f%%", farmers_affected, d.urgency * 100);
            snprintf(d.reasoning, 256, "Shabda: Krishi-Parashara prescribes Neem at %.0f%% infestation. Autonomous action authorized.", pest_risk * 100);
        }
        else if (pest_risk >= 0.8f && autonomy_level >= 0.8f) {
            d.action = ACTION_DEPLOY_PESTICIDE;
            snprintf(d.action_text, 128, "EMERGENCY DEPLOY: Organic pesticide for %d hectares.", farmers_affected);
            snprintf(d.reasoning, 256, "Nigamana: Critical threshold exceeded. Autonomous deployment authorized under Dharma protocol.");
        }
        
        return d;
    }
    
    // ================================================================
    // ACT: Execute the decision
    // ================================================================
    bool execute(AhamkaraDecision* d) {
        if (!active) return false;
        if (d->action == ACTION_NONE) return true;
        if (autonomy_level < 0.3f && d->action > ACTION_ALERT_FARMER) {
            printf("  🔒 AHAMKARA: Action requires higher autonomy (current: %.0f%%)\n", autonomy_level * 100);
            return false;
        }
        
        d->executed = true;
        strncpy(d->executed_by, identity, 31);
        actions_executed++;
        
        // Record in history
        if (history_count < 128) {
            history[history_count++] = *d;
        }
        
        const char* action_names[] = {
            "NONE", "ALERT_FARMER", "ALERT_VILLAGE", "ORDER_SUPPLIES",
            "ACTIVATE_IRRIGATION", "DEPLOY_PESTICIDE", "SCHEDULE_HARVEST", "ESCALATE_EXPERT"
        };
        
        printf("  ⚡ AHAMKARA ACTION: %s\n", action_names[d->action]);
        printf("     \"%s\"\n", d->action_text);
        printf("     Confidence: %.0f%% | Urgency: %.0f%% | Impact: %.1f tons\n",
               d->confidence * 100, d->urgency * 100, d->impact_score);
        printf("     Reasoning: %s\n", d->reasoning);
        
        decisions_made++;
        return true;
    }
    
    // ================================================================
    // REFLECT: Learn from outcomes — adjust autonomy
    // ================================================================
    void reflect(bool was_successful) {
        if (was_successful) {
            actions_successful++;
            // Trust grows — increase autonomy
            if (autonomy_level < 0.9f && actions_successful > 10) {
                autonomy_level += 0.05f;
                printf("  📈 AHAMKARA: Trust earned. Autonomy increased to %.0f%%\n", autonomy_level * 100);
            }
        } else {
            // Trust damaged — reduce autonomy
            if (autonomy_level > 0.2f) {
                autonomy_level -= 0.1f;
                printf("  📉 AHAMKARA: Mistake acknowledged. Autonomy reduced to %.0f%%\n", autonomy_level * 100);
            }
        }
    }
    
    void stats() {
        printf("  🕉️  AHAMKARA AGENT STATUS:\n");
        printf("     Identity: %s\n", identity);
        printf("     Domain: %s\n", village);
        printf("     Autonomy: %.0f%%\n", autonomy_level * 100);
        printf("     Decisions: %d | Executed: %d | Successful: %d\n",
               decisions_made, actions_executed, actions_successful);
        
        if (actions_executed > 0) {
            float success_rate = (float)actions_successful / actions_executed * 100;
            printf("     Success Rate: %.0f%%\n", success_rate);
            if (success_rate > 90) printf("     ✅ SIDDHI: The Guardian is trusted by the village.\n");
        }
    }
};
