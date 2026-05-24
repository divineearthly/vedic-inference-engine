/*
 * VRIKSHAYURVEDA-ANTAHKARANA
 * Vedic Plant Health Recognition Kernel
 * 
 * Based on: Surapala's Vrikshayurveda (c. 1000 CE)
 *           Varahamihira's Brihat Samhita
 *           Krishi-Parashara (c. 400 BCE)
 * 
 * Maps plant symptoms to Tri-Dosha states:
 *   Vata (Air/Wind)    — curling, dryness, stunting
 *   Pitta (Fire/Bile)  — yellowing, burning, wilting
 *   Kapha (Water/Earth)— rot, mold, excessive moisture
 */

#pragma once
#include <cstring>
#include <cstdio>
#include <cmath>

#define PHI 1.618033988749895f

// ================================================================
// TRI-DOSHA PLANT DIAGNOSTIC SYSTEM
// ================================================================
enum PlantDosha {
    PLANT_VATA = 0,   // Air imbalance — dryness, curling
    PLANT_PITTA = 1,  // Fire imbalance — yellowing, burning
    PLANT_KAPHA = 2,  // Water imbalance — rot, mold
    PLANT_SANNIPATA = 3 // All three — critical condition
};

struct PlantSymptom {
    const char* description;
    PlantDosha dosha;
    const char* vrikshayurveda_remedy;
    float severity; // 0.0 = mild, 1.0 = critical
};

// Knowledge base from Vrikshayurveda
const PlantSymptom VRIKSHA_SYMPTOMS[] = {
    // VATA symptoms (Air — dryness patterns)
    {"Leaves curling inward from edges",           PLANT_VATA,    "Apply Sesame oil cake + Cow urine (1:10)", 0.4f},
    {"Stunted growth despite adequate water",      PLANT_VATA,    "Mulch with straw + Apply Kunapa-Jala",     0.6f},
    {"Cracks appearing on fruit surface",          PLANT_VATA,    "Irrigate deeply + Apply clay paste on stem",0.5f},
    {"Premature leaf drop in calm weather",        PLANT_VATA,    "Spray milk-water (1:20) on leaves",        0.5f},
    {"Stem becoming woody and brittle",            PLANT_VATA,    "Apply Panchagavya at base weekly",         0.7f},
    
    // PITTA symptoms (Fire — yellowing/burning)
    {"Leaves turning yellow from tip downward",    PLANT_PITTA,   "Spray Neem leaf extract + Butter milk",    0.4f},
    {"Brown scorched patches on leaf surface",     PLANT_PITTA,   "Provide shade + Apply Aloe vera pulp",     0.6f},
    {"Fruits dropping before ripening",            PLANT_PITTA,   "Apply Banana stem juice at roots",         0.5f},
    {"Reddish discoloration on leaf veins",        PLANT_PITTA,   "Spray cold Cow milk (1:5) on foliage",     0.5f},
    {"Wilting during mid-day, recovering at night",PLANT_PITTA,   "Mulch heavily + Water only at dawn/dusk",  0.7f},
    
    // KAPHA symptoms (Water — rot/mold)
    {"White/grey powdery coating on leaves",       PLANT_KAPHA,   "Spray Ash-water solution (1:10)",           0.4f},
    {"Black sooty mold on leaf surface",           PLANT_KAPHA,   "Spray Neem oil (5%) + Cow urine (10%)",    0.5f},
    {"Stem rot at soil level, slimy texture",      PLANT_KAPHA,   "Apply Turmeric + Lime paste at collar",    0.7f},
    {"Excessive leaf growth but no fruiting",      PLANT_KAPHA,   "Reduce watering + Apply bone meal",        0.4f},
    {"Foul smell from root zone, roots blackened", PLANT_KAPHA,   "Uproot + Solarize soil + Apply Trichoderma",0.9f},
    
    // SANNIPATA (All three — critical)
    {"Complete wilting, yellowing AND rotting",    PLANT_SANNIPATA,"EMERGENCY: Apply Panchagavya + Consult expert", 0.95f},
    
    {nullptr, PLANT_VATA, nullptr, 0}
};

// ================================================================
// BEEJA-SAMSKARA (Seed Programming Algorithm)
// ================================================================
struct BeejaSamskara {
    // Pre-sowing seed treatment formulas from Vrikshayurveda
    static const char* treat_seed(const char* crop, float seed_weight_kg) {
        // Vidanga (Embelia ribes) treatment — universal seed protector
        if (strstr(crop, "Rice") || strstr(crop, "rice")) {
            return "Soak seeds in Cow urine (1:5 water) for 4 hours, "
                   "then coat with Vidanga powder (50g/kg seed). "
                   "Dry in shade before sowing.";
        }
        if (strstr(crop, "Mustard") || strstr(crop, "mustard")) {
            return "Mix seeds with Honey (1%) + Cow milk, "
                   "dry in shade. Germination improves by 30%.";
        }
        // Default: Panchagavya treatment
        return "Soak seeds in Panchagavya (1:30 water) for 3 hours. "
               "This activates the seed's inner intelligence.";
    }
};

// ================================================================
// BHU-PARIKSHA (Soil Classification)
// ================================================================
enum SoilType {
    JANGALA = 0,  // Arid — Vata dominant
    ANUPA = 1,    // Marshy — Kapha dominant  
    SAMANYA = 2,  // Balanced — Tridoshic
};

struct BhuPariksha {
    SoilType classify(const char* color, const char* taste, float moisture) {
        int vata_score = 0, kapha_score = 0;
        
        if (strstr(color, "red") || strstr(color, "brown")) vata_score++;
        if (strstr(color, "black") || strstr(color, "dark")) kapha_score++;
        if (strstr(taste, "sweet") || strstr(taste, "salty")) kapha_score++;
        if (strstr(taste, "bitter") || strstr(taste, "astringent")) vata_score++;
        if (moisture < 0.2f) vata_score += 2;
        if (moisture > 0.6f) kapha_score += 2;
        
        if (vata_score > kapha_score + 1) return JANGALA;
        if (kapha_score > vata_score + 1) return ANUPA;
        return SAMANYA;
    }
    
    const char* recommend_crop(SoilType soil) {
        switch(soil) {
            case JANGALA: return "Mustard, Millets, Sesame (Vata-tolerant crops)";
            case ANUPA:   return "Paddy Rice, Water chestnut, Lotus (Kapha-tolerant)";
            case SAMANYA: return "Wheat, Vegetables, Pulses (All crops suitable)";
        }
        return "Consult local Krishi Parashara";
    }
};

// ================================================================
// VRIKSHAYURVEDA ANTAHKARANA — Main Diagnostic Engine
// ================================================================
struct VrikshayurvedaAntahkarana {
    
    // Recognize plant disease from farmer's visual description
    const PlantSymptom* diagnose(const char* observation) {
        const PlantSymptom* best = nullptr;
        float best_score = 0;
        
        for (int i = 0; VRIKSHA_SYMPTOMS[i].description != nullptr; i++) {
            // Antahkarana: pattern match the observation against known symptoms
            float score = 0;
            const char* obs = observation;
            const char* sym = VRIKSHA_SYMPTOMS[i].description;
            
            int matches = 0, total = 0;
            while (*obs && *sym) {
                if (((*obs) | 32) == ((*sym) | 32)) matches++;
                obs++; sym++; total++;
            }
            score = (float)matches / (total + 1) * (1.0f - VRIKSHA_SYMPTOMS[i].severity * 0.3f);
            
            if (score > best_score) {
                best_score = score;
                best = &VRIKSHA_SYMPTOMS[i];
            }
        }
        return best;
    }
    
    // Full Vrikshayurveda consultation
    void consult(const char* crop, const char* observation, 
                 const char* soil_color, const char* soil_taste, float moisture) {
        
        printf("🕉️  VRIKSHAYURVEDA-ANTAHKARANA CONSULTATION\n");
        printf("  Crop: %s\n", crop);
        printf("  Observation: \"%s\"\n\n", observation);
        
        // 1. Bhu-Pariksha: Soil classification
        BhuPariksha bhu;
        SoilType soil = bhu.classify(soil_color, soil_taste, moisture);
        const char* soil_names[] = {"Jangala (Arid/Vata)", "Anupa (Marshy/Kapha)", "Samanya (Balanced)"};
        printf("  BHU-PARIKSHA: Soil is %s\n", soil_names[soil]);
        printf("  Recommended crops: %s\n\n", bhu.recommend_crop(soil));
        
        // 2. Disease diagnosis
        const PlantSymptom* disease = diagnose(observation);
        if (disease && disease->description) {
            const char* dosha_names[] = {"VATA (Air)", "PITTA (Fire)", "KAPHA (Water)", "SANNIPATA (Critical)"};
            printf("  VYADHI-NIDANA (Diagnosis):\n");
            printf("    Symptom:  %s\n", disease->description);
            printf("    Dosha:    %s\n", dosha_names[disease->dosha]);
            printf("    Severity: %.0f%%\n\n", disease->severity * 100);
            
            printf("  CHIKITSA (Treatment from Vrikshayurveda):\n");
            printf("    %s\n\n", disease->vrikshayurveda_remedy);
            
            // 3. Beeja-Samskara for prevention
            printf("  BEEJA-SAMSKARA (Seed Treatment for future):\n");
            printf("    %s\n\n", BeejaSamskara::treat_seed(crop, 1.0f));
        } else {
            printf("  No matching disease pattern found.\n");
            printf("  Recommend: Apply Panchagavya as general tonic.\n\n");
        }
        
        // 4. Kunapa-Jala recipe if needed
        if (disease && disease->severity > 0.5f) {
            printf("  KUNAPA-JALA (Fermented Nutrient Tonic):\n");
            printf("    Mix: Cow dung (5kg) + Cow urine (10L) + Jaggery (1kg)\n");
            printf("    Ferment 15 days. Dilute 1:30 with water.\n");
            printf("    Apply at root zone every 7 days.\n\n");
        }
    }
};

// ================================================================
// VRISHTI-VIJNANA (Rainfall Prediction from Krishi-Parashara)
// ================================================================
struct VrishtiVijnana {
    // Predict rainfall based on Nakshatra (lunar mansion)
    const char* predict_rainfall(const char* nakshatra, const char* wind_direction) {
        // Krishi-Parashara encoded: certain Nakshatras predict rain
        const char* rain_nakshatras[] = {
            "Rohini", "Mrigashira", "Ardra", "Punarvasu", "Pushya",
            "Ashlesha", "Magha", "Purva Phalguni", "Uttara Phalguni",
            "Hasta", "Chitra", "Swati", "Vishakha", nullptr
        };
        
        for (int i = 0; rain_nakshatras[i] != nullptr; i++) {
            if (strstr(nakshatra, rain_nakshatras[i])) {
                if (strstr(wind_direction, "east") || strstr(wind_direction, "south")) {
                    return "High probability of rain within 3 days. "
                           "Delay sowing. Protect harvested grains.";
                }
                return "Moderate chance of rainfall. Proceed with caution.";
            }
        }
        
        // Check for drought Nakshatras
        const char* dry_nakshatras[] = {"Ashwini", "Bharani", "Krittika", nullptr};
        for (int i = 0; dry_nakshatras[i] != nullptr; i++) {
            if (strstr(nakshatra, dry_nakshatras[i])) {
                return "Dry period likely. Irrigate if water available. "
                       "Good time for harvesting.";
            }
        }
        
        return "Normal conditions. Follow seasonal schedule.";
    }
};
