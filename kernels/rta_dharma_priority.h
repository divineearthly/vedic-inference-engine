/*
 * SUTRA 66: RTA-DHARMA PRIORITY ENGINE
 * Wired into Indra's Net for ethical alert routing
 * 
 * Rta (Cosmic Order): Systemic impact — how many farmers affected
 * Dharma (Duty): Context-sensitive priority — crop importance, season
 * Ahimsa (Non-harm): Minimize resource waste on false alarms
 */

#pragma once
#include <cstring>
#include <cstdio>
#include <cmath>
#include <ctime>

#define PHI 1.618033988749895f

struct RtaDharmaPriority {
    // Crop importance weights (Dharma — food security)
    static float crop_importance(const char* crop) {
        if (strstr(crop, "Sali Rice") || strstr(crop, "Boro Rice")) return 1.0f;  // Staple
        if (strstr(crop, "Ahu Rice"))  return 0.9f;
        if (strstr(crop, "Mustard"))   return 0.7f;  // Oil crop
        if (strstr(crop, "Brinjal") || strstr(crop, "Vegetables")) return 0.5f;
        return 0.6f;
    }
    
    // Season multiplier (Rta — cosmic timing)
    static float season_urgency(int month) {
        if (month >= 6 && month <= 9)  return 1.0f;  // Monsoon — peak pest season
        if (month >= 10 && month <= 11) return 0.9f; // Harvest — protect yield
        if (month >= 3 && month <= 5)  return 0.7f;  // Pre-monsoon
        return 0.5f;  // Winter — lower pest pressure
    }
    
    // Severity multiplier (actual threat level)
    static float severity_weight(float severity) {
        if (severity > 0.8f) return 1.5f;  // Critical — escalate immediately
        if (severity > 0.5f) return 1.0f;  // Moderate
        return 0.5f;  // Minor
    }
    
    // Calculate Rta-Dharma Priority Score (0.0 - 2.0)
    // Above 1.0 = Must act. Below 0.5 = Can defer.
    static float calculate(const char* crop, float severity, int month, int farmers_affected) {
        float dharma = crop_importance(crop);
        float rta = season_urgency(month);
        float threat = severity_weight(severity);
        float scale = 1.0f + (farmers_affected - 1) * 0.1f;  // +10% per additional farmer
        
        // Rta-Dharma entanglement: cosmic order × duty × threat × scale
        return dharma * rta * threat * scale;
    }
    
    // Priority routing decision
    static const char* route(float priority_score) {
        if (priority_score > 1.2f) return "🔴 IMMEDIATE: Broadcast to ALL nodes, SMS alert";
        if (priority_score > 0.8f) return "🟡 HIGH: Notify all Sali/Boro rice farmers";
        if (priority_score > 0.4f) return "🟢 NORMAL: Add to daily briefing";
        return "⚪ LOW: Log for monthly review";
    }
    
    // Ahimsa check: is this alert worth the network disruption?
    static bool worth_broadcasting(float priority_score) {
        return priority_score > 0.3f;  // Don't disturb the network for trivial alerts
    }
};

// ================================================================
// INTEGRATED ALERT WITH RTA-DHARMA PRIORITY
// ================================================================
struct DharmaAlert {
    char alert_id[32];
    char node_id[32];
    char crop[64];
    char pest[64];
    float severity;
    int month;
    int farmers_affected;
    float priority_score;
    const char* routing;
    const char* remedy;
    bool broadcast;
    
    void evaluate() {
        priority_score = RtaDharmaPriority::calculate(crop, severity, month, farmers_affected);
        routing = RtaDharmaPriority::route(priority_score);
        broadcast = RtaDharmaPriority::worth_broadcasting(priority_score);
    }
    
    void print() {
        printf("  🌾 %s | %s\n", crop, pest);
        printf("    Severity: %.0f%% | Farmers: %d | Month: %d\n", severity*100, farmers_affected, month);
        printf("    Rta-Dharma Score: %.2f → %s\n", priority_score, routing);
        printf("    Broadcast: %s\n", broadcast ? "YES ✅" : "NO (Ahimsa — network preserved)");
        if (broadcast) printf("    Remedy: %s\n", remedy);
    }
};
