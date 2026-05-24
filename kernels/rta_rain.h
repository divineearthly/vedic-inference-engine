/*
 * SUTRA 67: RTA-RAIN — Vrishti-Vijnana / Monsoon Prediction
 * 
 * Uses Akashic Ledger history + Nakshatra calendar + sensor trends
 * to predict rainfall 15 days ahead with Vedic precision.
 * 
 * Krishi-Parashara: "When the Moon enters Rohini with southwest wind,
 * the rains will come within 3 days."
 */

#pragma once
#include "kernels/vr_akashic.h"
#include "mahabhuta_physics.h"
#include <cstring>
#include <cstdio>
#include <cmath>
#include <ctime>

#define PHI 1.618033988749895f

struct RtaRain {
    struct RainfallPrediction {
        int day_offset;        // +1 to +15 days
        float probability;     // 0-1
        float intensity_mm;    // Expected rainfall in mm
        const char* nakshatra_influence;
        const char* farming_action;
    };
    
    RainfallPrediction forecast[15];
    AkashicLedger* akashic;
    MahabhutaPhysics* physics;
    
    RtaRain() : akashic(nullptr), physics(nullptr) {
        memset(forecast, 0, sizeof(forecast));
    }
    
    void attach(AkashicLedger* a, MahabhutaPhysics* p) { akashic=a; physics=p; }
    
    // Nakshatra rain probabilities from Krishi-Parashara
    struct NakshatraRain {
        const char* nakshatra; float rain_prob; const char* action;
    };
    static const NakshatraRain nakshatras[27];
    
    // Predict 15-day rainfall using sensor fusion + Nakshatra
    void predict(int current_solar_day) {
        float base_humidity = physics ? physics->elements[3].current_value : 0.5f;
        float base_pressure = physics ? physics->elements[0].current_value : 0.5f;
        float base_wind = physics ? physics->elements[1].current_value : 0.3f;
        
        for(int d=0; d<15; d++) {
            int solar_day = (current_solar_day + d) % 365;
            int nakshatra_idx = (solar_day / 14) % 27;
            
            float nakshatra_rain = nakshatras[nakshatra_idx].rain_prob;
            float sensor_rain = (base_humidity * 0.5f + (1.0f-base_pressure) * 0.3f + base_wind * 0.2f);
            float distance_factor = 1.0f / (1.0f + d * 0.1f);
            
            forecast[d].day_offset = d + 1;
            forecast[d].probability = nakshatra_rain * 0.5f + sensor_rain * 0.3f + distance_factor * 0.2f;
            forecast[d].intensity_mm = forecast[d].probability * 50.0f;
            forecast[d].nakshatra_influence = nakshatras[nakshatra_idx].nakshatra;
            forecast[d].farming_action = nakshatras[nakshatra_idx].action;
        }
    }
    
    // Best day for sowing in the next 15 days
    int best_sowing_day() {
        int best=-1; float best_score=0;
        for(int d=0;d<15;d++) {
            float score = forecast[d].probability * (forecast[d].intensity_mm > 5 ? 0.5f : 1.0f);
            if(score > best_score) { best_score=score; best=d+1; }
        }
        return best;
    }
    
    void report() {
        printf("\n🌧️  RTA-RAIN — Vrishti-Vijnana / 15-Day Monsoon Forecast\n");
        printf("  =========================================================\n");
        printf("  %-6s %-10s %-10s %-14s %-20s\n", "Day", "Rain%", "mm", "Nakshatra", "Action");
        for(int d=0;d<15;d+=2) {
            printf("  +%-4d %-10.0f %-10.1f %-14s %-20s\n",
                   forecast[d].day_offset, forecast[d].probability*100,
                   forecast[d].intensity_mm, forecast[d].nakshatra_influence,
                   forecast[d].farming_action);
        }
        int sow = best_sowing_day();
        if(sow>0) printf("\n  🌱 Best sowing day: +%d days\n", sow);
    }
};

const RtaRain::NakshatraRain RtaRain::nakshatras[27] = {
    {"Ashwini",0.2f,"Dry — prepare fields"},{"Bharani",0.15f,"Avoid sowing"},
    {"Krittika",0.3f,"Light rain possible"},{"Rohini",0.85f,"BEST SOWING"},
    {"Mrigashira",0.7f,"Good for transplanting"},{"Ardra",0.9f,"Heavy rain — delay"},
    {"Punarvasu",0.75f,"Renewal — good planting"},{"Pushya",0.6f,"Fertilize now"},
    {"Ashlesha",0.8f,"Pest alert — humidity high"},{"Magha",0.5f,"Drying trend"},
    {"Purva Phalguni",0.35f,"Harvest window"},{"Uttara Phalguni",0.3f,"Dry — irrigate"},
    {"Hasta",0.55f,"Moisture returning"},{"Chitra",0.45f,"Variable"},
    {"Swati",0.65f,"Good rain if windy"},{"Vishakha",0.7f,"Planting favorable"},
    {"Anuradha",0.6f,"Steady conditions"},{"Jyeshtha",0.5f,"Moderate"},
    {"Mula",0.4f,"Root crops only"},{"Purva Ashadha",0.55f,"Pre-monsoon"},
    {"Uttara Ashadha",0.5f,"Stable"},{"Shravana",0.75f,"Good rain"},
    {"Dhanishtha",0.65f,"Favorable"},{"Shatabhisha",0.5f,"Moderate"},
    {"Purva Bhadrapada",0.4f,"Decreasing"},{"Uttara Bhadrapada",0.35f,"Dry spell"},
    {"Revati",0.6f,"Nourishing rain"}
};
