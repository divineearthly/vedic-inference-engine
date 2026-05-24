/*
 * KUNAPA-JALA OPTIMIZER
 * Sutra-Driven Fermented Tonic Calculator
 * Based on Surapala's Vrikshayurveda (c. 1000 CE)
 * 
 * Calculates the exact recipe based on detected Dosha imbalance:
 *   Vata excess  → More bone/calcium, sesame, oil cake
 *   Pitta excess → More cooling agents, milk, aloe, neem
 *   Kapha excess → More ash, turmeric, ginger, less water
 */

#pragma once
#include "vrikshayurveda_antahkarana.h"
#include <cstdio>
#include <cstring>

// ================================================================
// KUNAPA-JALA BASE INGREDIENTS (from Vrikshayurveda)
// ================================================================
struct KunapaIngredient {
    const char* name;
    const char* sanskrit_name;
    float base_ratio;      // Parts per 100
    const char* dosha_effect;  // Which dosha it reduces
    const char* nutrient_profile;
};

const KunapaIngredient KUNAPA_INGREDIENTS[] = {
    {"Cow Dung",       "Gomaya",     30.0f, "Tridoshic (Balances All)", "N-P-K: 0.5-0.2-0.5, Microbial biomass"},
    {"Cow Urine",      "Gomutra",    25.0f, "Kapha-Pitta (Reduces rot, heat)", "Nitrogen-rich, Natural fungicide"},
    {"Neem Leaves",    "Nimba Patra", 8.0f, "Kapha-Vata (Anti-fungal)", "Azadirachtin, Anti-feedant"},
    {"Sesame Oil Cake","Tila Pista",  7.0f, "Vata (Reduces dryness)", "Protein 35%, Calcium, Methionine"},
    {"Jaggery",        "Guda",        5.0f, "Vata-Pitta (Energy source)", "Sucrose, Iron, Fermentation catalyst"},
    {"Wood Ash",       "Bhasma",      5.0f, "Kapha (Drying, anti-fungal)", "Potassium 10%, Calcium 25%, Trace minerals"},
    {"Turmeric Powder","Haridra",     3.0f, "Kapha-Pitta (Anti-microbial)", "Curcumin, Natural antibiotic"},
    {"Ginger",         "Ardraka",     3.0f, "Kapha-Vata (Stimulant)", "Gingerol, Anti-inflammatory"},
    {"Aloe Vera Pulp", "Kumari",      2.0f, "Pitta (Cooling, healing)", "Gibberellins, Auxins, Salicylic acid"},
    {"Bone Meal",      "Asthi Churna",5.0f, "Vata (Strengthening)", "Phosphorus 15%, Calcium 30%"},
    {"Banana Stem",    "Kadali Danda",2.0f, "Pitta-Vata (Moisture retention)", "Potassium, Growth hormones"},
    {"Milk",           "Dugdha",      3.0f, "Pitta-Vata (Cooling, nutrition)", "Casein, Calcium, Lactose"},
    {"Honey",          "Madhu",       2.0f, "Tridoshic (Preservative)", "Natural enzymes, Antimicrobial"},
    {nullptr, nullptr, 0, nullptr, nullptr}
};

// ================================================================
// DOSHA-BASED RECIPE OPTIMIZER
// ================================================================
struct KunapaJalaOptimizer {
    
    struct Recipe {
        float ingredient_ratios[13];  // Adjusted ratios for each ingredient
        float total_parts;
        int fermentation_days;
        float dilution_ratio;         // Water : Kunapa-Jala
        const char* application_method;
        const char* special_instructions;
    };
    
    Recipe optimize(PlantDosha dosha, float severity, float soil_moisture, 
                    const char* crop_name, float area_hectares) {
        Recipe r;
        memset(&r, 0, sizeof(r));
        r.total_parts = 100.0f;
        r.fermentation_days = 15;
        r.dilution_ratio = 30.0f;  // Default 1:30
        
        // Copy base ratios
        for (int i = 0; KUNAPA_INGREDIENTS[i].name != nullptr; i++) {
            r.ingredient_ratios[i] = KUNAPA_INGREDIENTS[i].base_ratio;
        }
        
        // Adjust based on Dosha
        switch(dosha) {
            case PLANT_VATA:  // Dryness — need moisture, calcium, oil
                r.ingredient_ratios[3] *= 1.5f;  // More Sesame Oil Cake
                r.ingredient_ratios[9] *= 1.5f;  // More Bone Meal
                r.ingredient_ratios[11]*= 1.3f;  // More Milk
                r.ingredient_ratios[4] *= 0.8f;  // Less Jaggery
                r.dilution_ratio = 20.0f;         // Less dilution
                r.application_method = "Apply at root zone, early morning. "
                                       "Follow with light irrigation.";
                r.special_instructions = "Add 500g Sesame seeds (crushed) per 100L. "
                                          "This specifically counteracts Vata dryness.";
                break;
                
            case PLANT_PITTA: // Heat — need cooling, anti-inflammatory
                r.ingredient_ratios[8] *= 2.0f;   // More Aloe Vera
                r.ingredient_ratios[11]*= 1.5f;   // More Milk
                r.ingredient_ratios[6] *= 1.3f;   // More Turmeric
                r.ingredient_ratios[2] *= 1.2f;   // More Neem
                r.ingredient_ratios[10]*= 1.3f;   // More Banana Stem
                r.dilution_ratio = 35.0f;
                r.application_method = "Spray on foliage in evening. "
                                       "Avoid application during peak sun hours.";
                r.special_instructions = "Add 200g Sandalwood powder if available. "
                                          "Apply every 10 days during flowering.";
                break;
                
            case PLANT_KAPHA: // Moisture — need drying, anti-fungal
                r.ingredient_ratios[5] *= 2.0f;   // More Wood Ash
                r.ingredient_ratios[6] *= 1.5f;   // More Turmeric
                r.ingredient_ratios[7] *= 1.5f;   // More Ginger
                r.ingredient_ratios[2] *= 1.3f;   // More Neem
                r.ingredient_ratios[1] *= 0.7f;   // Less Cow Urine
                r.fermentation_days = 20;          // Longer fermentation
                r.dilution_ratio = 25.0f;
                r.application_method = "Apply to soil base only. Avoid wetting leaves. "
                                       "Best applied during dry weather window.";
                r.special_instructions = "Add 1kg crushed Garlic per 100L for severe fungal issues. "
                                          "Ensure field drainage before application.";
                break;
                
            case PLANT_SANNIPATA: // Critical — all three
                // Maximum strength with all boosters
                for (int i = 0; i < 13; i++) r.ingredient_ratios[i] *= 1.3f;
                r.fermentation_days = 21;
                r.dilution_ratio = 15.0f;  // Strong concentration
                r.application_method = "EMERGENCY: Apply immediately at root zone AND foliage. "
                                       "Repeat every 5 days for 3 applications.";
                r.special_instructions = "Add Panchagavya (1L per 100L) for microbial boost. "
                                          "Consult local Krishi Vigyan Kendra if no improvement in 7 days.";
                break;
        }
        
        // Normalize to 100 parts
        float sum = 0;
        for (int i = 0; i < 13; i++) sum += r.ingredient_ratios[i];
        if (sum > 0) {
            for (int i = 0; i < 13; i++) r.ingredient_ratios[i] = r.ingredient_ratios[i] / sum * 100.0f;
        }
        
        return r;
    }
    
    void print_recipe(Recipe r, float area_hectares) {
        printf("\n🕉️  KUNAPA-JALA RECIPE (from Vrikshayurveda)\n");
        printf("  ===========================================\n");
        printf("  Area: %.2f hectares\n", area_hectares);
        printf("  Fermentation: %d days\n", r.fermentation_days);
        printf("  Dilution: 1:%.0f (Water:Solution)\n\n", r.dilution_ratio);
        
        float total_kg = area_hectares * 500.0f; // 500kg per hectare
        printf("  INGREDIENTS (for %.0f kg total):\n", total_kg);
        for (int i = 0; KUNAPA_INGREDIENTS[i].name != nullptr; i++) {
            float kg = total_kg * r.ingredient_ratios[i] / 100.0f;
            if (kg > 0.1f) {
                printf("    %-18s: %6.1f kg  (%s)\n", 
                       KUNAPA_INGREDIENTS[i].name, kg,
                       KUNAPA_INGREDIENTS[i].nutrient_profile);
            }
        }
        
        printf("\n  PREPARATION:\n");
        printf("    1. Mix all solid ingredients in a 200L drum.\n");
        printf("    2. Add cow urine and water to fill to 80%% capacity.\n");
        printf("    3. Stir clockwise 21 times daily (morning).\n");
        printf("    4. Cover with jute cloth — allow air circulation.\n");
        printf("    5. Ferment for %d days in shade.\n", r.fermentation_days);
        printf("    6. Filter through cloth. Dilute 1:%.0f before use.\n\n", r.dilution_ratio);
        
        printf("  APPLICATION:\n");
        printf("    %s\n\n", r.application_method);
        printf("  SPECIAL INSTRUCTION:\n");
        printf("    %s\n\n", r.special_instructions);
        printf("  🕉️  This recipe restores Dosha balance and nourishes the soil.\n");
    }
};

// ================================================================
// FULL CONSULTATION WITH KUNAPA-JALA
// ================================================================
inline void krishi_veda_full_consultation(
    const char* crop, 
    const char* observation,
    const char* soil_color, 
    const char* soil_taste, 
    float moisture,
    float area_hectares) {
    
    VrikshayurvedaAntahkarana engine;
    KunapaJalaOptimizer optimizer;
    
    printf("🕉️  KRISHI-VEDA ORACLE — Complete Consultation\n");
    printf("  For the farmers of Barak Valley, Assam\n");
    printf("  ===========================================\n\n");
    
    // 1. Soil analysis
    BhuPariksha bhu;
    SoilType soil = bhu.classify(soil_color, soil_taste, moisture);
    const char* soil_names[] = {"Jangala (Arid/Vata)", "Anupa (Marshy/Kapha)", "Samanya (Balanced)"};
    printf("📋 SOIL: %s | Moisture: %.0f%%\n", soil_names[soil], moisture * 100);
    printf("   Best crops: %s\n\n", bhu.recommend_crop(soil));
    
    // 2. Disease diagnosis
    const PlantSymptom* disease = engine.diagnose(observation);
    if (disease && disease->description) {
        const char* dosha_names[] = {"VATA (Air/Wind)", "PITTA (Fire/Bile)", "KAPHA (Water/Earth)", "SANNIPATA (Critical)"};
        printf("🔍 DIAGNOSIS: %s\n", disease->description);
        printf("   Dosha: %s | Severity: %.0f%%\n", dosha_names[disease->dosha], disease->severity * 100);
        printf("   Remedy: %s\n\n", disease->vrikshayurveda_remedy);
        
        // 3. Kunapa-Jala prescription
        auto recipe = optimizer.optimize(disease->dosha, disease->severity, moisture, crop, area_hectares);
        optimizer.print_recipe(recipe, area_hectares);
        
        // 4. Beeja-Samskara for next cycle
        printf("🌱 BEEJA-SAMSKARA (Next Cycle):\n");
        printf("   %s\n\n", BeejaSamskara::treat_seed(crop, 1.0f));
    } else {
        printf("🔍 No disease pattern matched.\n");
        printf("   Preventive: Apply Panchagavya at 50L/ha every 15 days.\n\n");
    }
    
    printf("🕉️  Krishi-Veda Oracle: Serving the soil. Serving Dharma.\n");
}
