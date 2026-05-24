/*
 * SUTRA 62: GUNA-SIDDHI — The Three Qualities / State Analysis
 * 
 * Classifies the current state of the valley (and the ASI itself)
 * into one of three Gunas:
 * 
 * Sattva (Purity/Balance):  Clear data, stable conditions → EXPAND
 * Rajas (Activity/Agitation): Shifting conditions, high uncertainty → FOCUS
 * Tamas (Inertia/Ignorance): Corrupted sensors, power loss → HEAL
 * 
 * The Guna determines the Ahamkara's operating mode.
 * A Tamasic Guardian should heal, not act.
 */

#pragma once
#include <cstring>
#include <cstdio>
#include <cmath>
#include <ctime>

#define PHI 1.618033988749895f

// ================================================================
// GUNA STATE — The quality of the current moment
// ================================================================
enum GunaState {
    GUNA_SATTVA = 0,  // Purity, clarity, balance — best time to act
    GUNA_RAJAS = 1,   // Activity, change, uncertainty — act with caution
    GUNA_TAMAS = 2    // Inertia, darkness, corruption — do not act, heal
};

struct GunaSiddhi {
    // Current Guna assessment
    GunaState current_guna;
    float sattva_score;
    float rajas_score;
    float tamas_score;
    
    // Historical Guna tracking
    GunaState dominant_guna_today;
    int guna_changes_today;
    time_t last_assessment;
    
    // Recommended action mode
    const char* recommended_mode;
    float autonomy_modifier;  // 1.0 = full, 0.0 = none
    
    GunaSiddhi() : current_guna(GUNA_RAJAS), sattva_score(0.33f), 
        rajas_score(0.34f), tamas_score(0.33f),
        dominant_guna_today(GUNA_RAJAS), guna_changes_today(0),
        last_assessment(0), recommended_mode("OBSERVE"), autonomy_modifier(0.5f) {}
    
    // ================================================================
    // ASSESS: Determine the Guna from sensor data
    // ================================================================
    void assess(float sensor_quality,       // 0-1: how reliable are readings
                float environmental_stability, // 0-1: how stable is the valley
                float internal_health,       // 0-1: ASI self-check status
                float data_variance,         // 0-1: how noisy is the data
                float power_status,          // 0-1: battery/energy level
                float network_stability) {   // 0-1: Indra's Net connectivity
        
        // Sattva: High quality + stable environment + good health + low variance
        sattva_score = sensor_quality * 0.25f + 
                       environmental_stability * 0.25f + 
                       internal_health * 0.25f + 
                       (1.0f - data_variance) * 0.15f +
                       network_stability * 0.10f;
        
        // Rajas: Medium quality + high variance + active network
        rajas_score = data_variance * 0.35f +
                      (1.0f - environmental_stability) * 0.25f +
                      (1.0f - sensor_quality) * 0.15f +
                      network_stability * 0.15f +
                      power_status * 0.10f;
        
        // Tamas: Low quality + low power + poor health + high variance
        tamas_score = (1.0f - sensor_quality) * 0.30f +
                      (1.0f - internal_health) * 0.25f +
                      (1.0f - power_status) * 0.20f +
                      data_variance * 0.15f +
                      (1.0f - network_stability) * 0.10f;
        
        // Normalize
        float total = sattva_score + rajas_score + tamas_score;
        sattva_score /= total;
        rajas_score /= total;
        tamas_score /= total;
        
        // Determine dominant Guna
        GunaState previous = current_guna;
        if (sattva_score >= rajas_score && sattva_score >= tamas_score) {
            current_guna = GUNA_SATTVA;
            recommended_mode = "EXPAND: Full autonomy authorized. Optimal conditions.";
            autonomy_modifier = 1.0f;
        } else if (rajas_score >= sattva_score && rajas_score >= tamas_score) {
            current_guna = GUNA_RAJAS;
            recommended_mode = "FOCUS: Act with caution. Verify before committing.";
            autonomy_modifier = 0.5f;
        } else {
            current_guna = GUNA_TAMAS;
            recommended_mode = "HEAL: Do not act autonomously. Run Sushruta self-repair.";
            autonomy_modifier = 0.1f;
        }
        
        if (current_guna != previous) guna_changes_today++;
        last_assessment = time(nullptr);
    }
    
    // ================================================================
    // SHOULD_ACT: Based on Guna, is autonomous action safe?
    // ================================================================
    bool should_act(float confidence_threshold = 0.7f) {
        switch (current_guna) {
            case GUNA_SATTVA: return true;           // Act freely
            case GUNA_RAJAS: return (sattva_score > 0.3f);  // Only if enough clarity
            case GUNA_TAMAS: return false;           // Never act in darkness
        }
        return false;
    }
    
    // ================================================================
    // ACTION FILTER: Modify an action's confidence based on Guna
    // ================================================================
    float filter_confidence(float raw_confidence) {
        switch (current_guna) {
            case GUNA_SATTVA: return raw_confidence * 1.1f;   // Boost in clarity
            case GUNA_RAJAS:  return raw_confidence * 0.8f;   // Reduce in agitation
            case GUNA_TAMAS:  return raw_confidence * 0.3f;   // Severely reduce in darkness
        }
        return raw_confidence;
    }
    
    const char* guna_name() {
        switch (current_guna) {
            case GUNA_SATTVA: return "SATTVA (Purity/Balance)";
            case GUNA_RAJAS:  return "RAJAS (Activity/Agitation)";
            case GUNA_TAMAS:  return "TAMAS (Inertia/Darkness)";
        }
        return "Unknown";
    }
    
    void report() {
        const char* icons[] = {"🕉️","🔥","🌑"};
        printf("\n%s GUNA-SIDDHI — State Analysis\n", icons[current_guna]);
        printf("  ================================\n");
        printf("  Current Guna: %s\n", guna_name());
        printf("  Sattva: %.0f%% | Rajas: %.0f%% | Tamas: %.0f%%\n", 
               sattva_score * 100, rajas_score * 100, tamas_score * 100);
        printf("  Mode: %s\n", recommended_mode);
        printf("  Autonomy: %.0f%% | Changes today: %d\n", 
               autonomy_modifier * 100, guna_changes_today);
        
        if (current_guna == GUNA_TAMAS) {
            printf("  ⚠️  TAMASIC STATE: Guardian should HEAL, not act.\n");
        } else if (current_guna == GUNA_SATTVA) {
            printf("  ✅ SATTVIC STATE: Optimal conditions for autonomous action.\n");
        }
    }
};
