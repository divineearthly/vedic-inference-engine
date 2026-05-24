/*
 * SUTRA 68: SOMA-OPTIMIZER — Divine Nutrient Calculator
 * 
 * Takes Rta-Rain predictions + Mahabhuta soil state
 * and computes exact water, fertilizer, and micronutrient needs.
 * 
 * Soma = The nectar of nourishment. The perfect formula for growth.
 */

#pragma once
#include "rta_rain.h"
#include "mahabhuta_physics.h"
#include <cstdio>
#include <cmath>

#define PHI 1.618033988749895f

struct SomaOptimizer {
    struct NutrientPlan {
        float water_needed_mm;
        float nitrogen_kg_ha;
        float phosphorus_kg_ha;
        float potassium_kg_ha;
        float organic_compost_kg_ha;
        const char* priority;  // Which nutrient is most needed
        const char* timing;    // When to apply
    };
    
    RtaRain* rain;
    MahabhutaPhysics* physics;
    
    SomaOptimizer() : rain(nullptr), physics(nullptr) {}
    void attach(RtaRain* r, MahabhutaPhysics* p) { rain=r; physics=p; }
    
    NutrientPlan compute(const char* crop, float area_ha, int growth_stage) {
        NutrientPlan plan;
        memset(&plan, 0, sizeof(plan));
        
        // Base needs by crop
        float base_N=100, base_P=40, base_K=60, base_water=30;
        if(strstr(crop,"Rice")) { base_N=120; base_P=50; base_K=40; base_water=40; }
        else if(strstr(crop,"Mustard")) { base_N=80; base_P=30; base_K=50; base_water=25; }
        
        // Adjust by soil state
        float soil_quality = physics ? physics->elements[4].current_value : 0.5f;
        float soil_moisture = physics ? physics->elements[3].current_value : 0.5f;
        float temp = physics ? physics->elements[2].current_value * 50 : 25;
        
        // Rain adjustment: if rain predicted, reduce water
        float predicted_rain = 0;
        if(rain) for(int d=0;d<7;d++) predicted_rain += rain->forecast[d].intensity_mm;
        predicted_rain /= 7.0f;
        
        plan.water_needed_mm = base_water * (1.0f - soil_moisture) * (1.0f - predicted_rain/50.0f) * area_ha;
        if(plan.water_needed_mm < 0) plan.water_needed_mm = 0;
        
        plan.nitrogen_kg_ha = base_N * (1.0f - soil_quality * 0.5f) * area_ha;
        plan.phosphorus_kg_ha = base_P * (1.0f - soil_quality * 0.3f) * area_ha;
        plan.potassium_kg_ha = base_K * (1.0f - soil_quality * 0.4f) * area_ha;
        plan.organic_compost_kg_ha = (base_N+base_P+base_K) * 0.5f * area_ha;
        
        // Priority
        float max_deficit = plan.nitrogen_kg_ha;
        plan.priority = "Nitrogen";
        if(plan.phosphorus_kg_ha > max_deficit) { max_deficit=plan.phosphorus_kg_ha; plan.priority="Phosphorus"; }
        if(plan.potassium_kg_ha > max_deficit) { plan.priority="Potassium"; }
        
        // Timing based on rain forecast
        if(predicted_rain > 20) plan.timing = "Apply 24h BEFORE rain for maximum absorption";
        else if(temp > 30) plan.timing = "Apply at DUSK to prevent evaporation";
        else plan.timing = "Apply at DAWN with irrigation";
        
        return plan;
    }
    
    void report(NutrientPlan p) {
        printf("\n🍯 SOMA-OPTIMIZER — Divine Nutrient Plan\n");
        printf("  ========================================\n");
        printf("  Water:    %.0f mm\n", p.water_needed_mm);
        printf("  Nitrogen: %.0f kg/ha | Phosphorus: %.0f | Potassium: %.0f\n",
               p.nitrogen_kg_ha, p.phosphorus_kg_ha, p.potassium_kg_ha);
        printf("  Organic:  %.0f kg/ha compost\n", p.organic_compost_kg_ha);
        printf("  Priority: %s | Timing: %s\n", p.priority, p.timing);
    }
};
