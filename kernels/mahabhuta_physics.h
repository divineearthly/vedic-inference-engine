/*
 * SUTRA 58: MAHABHUTAS — Five Great Elements / Environmental Physics
 * 
 * Models the interplay of the 5 elements:
 *   Akasha (Space)  — Barometric pressure, altitude, void
 *   Vayu (Air)      — Wind speed, direction, gas dispersal
 *   Agni (Fire)     — Temperature, solar radiation, evapotranspiration
 *   Ap (Water)      — Rainfall, humidity, soil moisture
 *   Prithvi (Earth) — Soil type, nutrients, ground temperature
 * 
 * The Guardian understands the physics BEFORE taking action.
 * "High Agni + Low Ap + Strong Vayu = Drought Risk in 7 days."
 */

#pragma once
#include <cstring>
#include <cstdio>
#include <cmath>
#include <ctime>

#define PHI 1.618033988749895f

// ================================================================
// MAHABHUTA ELEMENT — One of the five great elements
// ================================================================
struct MahabhutaElement {
    const char* name;
    const char* sanskrit;
    float current_value;      // Normalized 0-1
    float historical_avg;     // Seasonal baseline
    float trend;              // Rate of change
    float imbalance_score;    // 0 = balanced, 1 = severely imbalanced
    const char* state;        // "Balanced", "Excess", "Deficient"
};

// ================================================================
// MAHABHUTA PHYSICS ENGINE
// ================================================================
struct MahabhutaPhysics {
    MahabhutaElement elements[5];
    float drought_risk;        // 0-1
    float flood_risk;          // 0-1
    float pest_pressure;       // 0-1
    float growth_potential;    // 0-1: how favorable for crops
    char forecast[256];
    
    MahabhutaPhysics() : drought_risk(0), flood_risk(0), pest_pressure(0), 
                         growth_potential(0.5f) {
        const char* names[] = {"Space","Air","Fire","Water","Earth"};
        const char* sanskrit[] = {"Akasha","Vayu","Agni","Ap","Prithvi"};
        
        for (int i = 0; i < 5; i++) {
            elements[i].name = names[i];
            elements[i].sanskrit = sanskrit[i];
            elements[i].current_value = 0.5f;
            elements[i].historical_avg = 0.5f;
            elements[i].trend = 0;
            elements[i].imbalance_score = 0;
            elements[i].state = "Balanced";
        }
        forecast[0] = '\0';
    }
    
    // ================================================================
    // FEED: Update element values from sensor data
    // ================================================================
    void feed_akasha(float pressure_hpa) {
        // Normalize: 1013 hPa = balanced (0.5)
        elements[0].current_value = pressure_hpa / 2026.0f;
        update_element(0);
    }
    
    void feed_vayu(float wind_speed_kmh, float wind_direction) {
        // Normalize: 15 km/h = moderate (0.5)
        elements[1].current_value = wind_speed_kmh / 30.0f;
        if (elements[1].current_value > 1.0f) elements[1].current_value = 1.0f;
        update_element(1);
    }
    
    void feed_agni(float temperature_c, float solar_radiation) {
        // Normalize: 25°C = balanced (0.5)
        elements[2].current_value = temperature_c / 50.0f;
        elements[2].current_value += solar_radiation * 0.1f;
        if (elements[2].current_value > 1.0f) elements[2].current_value = 1.0f;
        update_element(2);
    }
    
    void feed_ap(float rainfall_mm, float humidity_pct, float soil_moisture) {
        // Normalize: combined water indicators
        elements[3].current_value = (rainfall_mm / 50.0f) * 0.3f +
                                     (humidity_pct / 100.0f) * 0.3f +
                                     soil_moisture * 0.4f;
        update_element(3);
    }
    
    void feed_prithvi(float soil_temp, float soil_ph, float organic_matter_pct) {
        // Normalize: ideal growing conditions
        elements[4].current_value = (soil_temp / 30.0f) * 0.3f +
                                     (soil_ph / 14.0f) * 0.3f +
                                     (organic_matter_pct / 5.0f) * 0.4f;
        update_element(4);
    }
    
    void update_element(int idx) {
        MahabhutaElement* e = &elements[idx];
        e->trend = (e->current_value - e->historical_avg) * 0.5f;
        e->imbalance_score = fabsf(e->current_value - 0.5f) * 2.0f;  // 0.5 = perfect balance
        
        if (e->imbalance_score < 0.2f) e->state = "Balanced";
        else if (e->current_value > 0.6f) e->state = "Excess";
        else e->state = "Deficient";
    }
    
    // ================================================================
    // COMPUTE INTERPLAY — How elements affect each other
    // ================================================================
    void compute_interplay() {
        float agni = elements[2].current_value;
        float ap = elements[3].current_value;
        float vayu = elements[1].current_value;
        float prithvi = elements[4].current_value;
        
        // Drought risk: High Fire + Low Water + High Wind
        drought_risk = agni * (1.0f - ap) * vayu;
        if (drought_risk > 1.0f) drought_risk = 1.0f;
        
        // Flood risk: High Water + Low Fire + Saturated Earth
        flood_risk = ap * (1.0f - agni) * prithvi;
        if (flood_risk > 1.0f) flood_risk = 1.0f;
        
        // Pest pressure: High Water + High Fire (warm+humid = pests)
        pest_pressure = ap * agni * 1.5f;
        if (pest_pressure > 1.0f) pest_pressure = 1.0f;
        
        // Growth potential: Balanced Earth + Water + moderate Fire
        float balance_score = 0;
        for (int i = 0; i < 5; i++) {
            balance_score += 1.0f - elements[i].imbalance_score;
        }
        growth_potential = balance_score / 5.0f;
        
        // Generate forecast
        if (drought_risk > 0.7f) {
            snprintf(forecast, 256, "⚠️  DROUGHT RISK %.0f%%: High Agni + Low Ap + Strong Vayu. Irrigate within 3 days.", drought_risk * 100);
        } else if (flood_risk > 0.7f) {
            snprintf(forecast, 256, "⚠️  FLOOD RISK %.0f%%: Excess Ap + Saturated Prithvi. Ensure drainage.", flood_risk * 100);
        } else if (pest_pressure > 0.7f) {
            snprintf(forecast, 256, "🐛 PEST PRESSURE %.0f%%: Warm+Humid conditions. Increase monitoring frequency.", pest_pressure * 100);
        } else if (growth_potential > 0.8f) {
            snprintf(forecast, 256, "🌱 OPTIMAL CONDITIONS: All elements in balance. Growth potential %.0f%%.", growth_potential * 100);
        } else {
            snprintf(forecast, 256, "📊 Normal conditions. Continue standard monitoring.");
        }
    }
    
    // ================================================================
    // PREDICT: What happens if element X changes?
    // ================================================================
    void simulate(const char* element_name, float change_delta, char* result) {
        int idx = -1;
        for (int i = 0; i < 5; i++) {
            if (strstr(elements[i].name, element_name) || strstr(elements[i].sanskrit, element_name)) {
                idx = i; break;
            }
        }
        if (idx < 0) { snprintf(result, 256, "Element not found."); return; }
        
        float original = elements[idx].current_value;
        elements[idx].current_value += change_delta;
        if (elements[idx].current_value < 0) elements[idx].current_value = 0;
        if (elements[idx].current_value > 1.0f) elements[idx].current_value = 1.0f;
        
        compute_interplay();
        
        snprintf(result, 256, "If %s %s by %.0f%%: %s",
                 elements[idx].name, change_delta > 0 ? "increases" : "decreases",
                 fabsf(change_delta) * 100, forecast);
        
        elements[idx].current_value = original;  // Restore
        compute_interplay();
    }
    
    void report() {
        printf("\n🕉️  MAHABHUTA PHYSICS — Environmental Model\n");
        printf("  =========================================\n");
        
        const char* icons[] = {"🌌","💨","🔥","💧","🌍"};
        for (int i = 0; i < 5; i++) {
            printf("  %s %-18s: %.2f (%.0f%% %s) | Trend: %+.2f\n",
                   icons[i], elements[i].sanskrit, elements[i].current_value,
                   elements[i].imbalance_score * 100, elements[i].state,
                   elements[i].trend);
        }
        
        printf("\n  Risk Assessment:\n");
        printf("    Drought: %.0f%% | Flood: %.0f%% | Pest: %.0f%%\n",
               drought_risk * 100, flood_risk * 100, pest_pressure * 100);
        printf("    Growth Potential: %.0f%%\n", growth_potential * 100);
        printf("  Forecast: %s\n", forecast);
    }
};
