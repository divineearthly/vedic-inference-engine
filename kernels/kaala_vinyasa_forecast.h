/*
 * SUTRA 70: KAALA-VINYASA — Temporal Causality Forecasting
 * 
 * Hadamard Product Fusion: Entangles current state with historical cycles.
 * Predicts rainfall, pest emergence, and harvest windows.
 * 
 * Vrishti-Vijnana (Rainfall Science) from Krishi-Parashara:
 * Moon through Nakshatras → predict rain 15 days ahead.
 * Combined with modern sensor data for pinpoint accuracy.
 */

#pragma once
#include <cstring>
#include <cstdio>
#include <cmath>
#include <ctime>

#define PHI 1.618033988749895f
#define FORECAST_DAYS 21

// ================================================================
// NAKSHATRA RAINFALL PREDICTION (Krishi-Parashara)
// ================================================================
struct NakshatraForecast {
    const char* nakshatra;
    int start_day;       // Approximate day of solar year
    float rain_probability;
    const char* wind_effect;
    const char* farming_action;
};

const NakshatraForecast NAKSHATRA_CALENDAR[] = {
    {"Rohini",      135, 0.85f, "Southwest wind brings moisture", 
     "BEST SOWING WINDOW — All crops"},
    {"Mrigashira",  150, 0.70f, "Gentle rain expected", 
     "Transplant rice seedlings"},
    {"Ardra",       165, 0.90f, "Heavy rain, possible storm", 
     "Delay sowing — protect seedlings"},
    {"Punarvasu",   180, 0.75f, "Renewal rains", 
     "Good for transplantation"},
    {"Pushya",      195, 0.60f, "Nourishing light rain", 
     "Apply fertilizer — maximum absorption"},
    {"Ashlesha",    210, 0.80f, "Humid, pest-friendly", 
     "PEST ALERT — Monitor for Borers"},
    {"Magha",       225, 0.50f, "Drying trend begins", 
     "Harvest window opening"},
    {"Purva Phalguni", 240, 0.35f, "Dry, warm", 
     "Good for harvesting and storage"},
    {"Hasta",       270, 0.55f, "Return of moisture", 
     "Prepare for winter crops"},
    {"Chitra",      285, 0.45f, "Variable conditions", 
     "Monitor soil moisture closely"},
    {"Swati",       300, 0.65f, "Good rainfall if wind favorable", 
     "Sow Mustard, winter vegetables"},
    {nullptr, 0, 0, nullptr, nullptr}
};

// ================================================================
// KAALA-VINYASA: Hadamard Product Fusion Forecaster
// ================================================================
struct KaalaVinyasa {
    float historical_patterns[FORECAST_DAYS][8];  // 8 features × 21 days
    float current_state[8];                       // Today's sensor readings
    float forecast[FORECAST_DAYS][4];             // Predicted: temp, rain, humidity, pest_risk
    
    KaalaVinyasa() {
        memset(historical_patterns, 0, sizeof(historical_patterns));
        memset(current_state, 0, sizeof(current_state));
        memset(forecast, 0, sizeof(forecast));
    }
    
    // Load historical data from Kala-Chakra cycles
    void load_cycle_data(float* cycle, int period) {
        for (int d = 0; d < FORECAST_DAYS && d < period; d++) {
            for (int f = 0; f < 8; f++) {
                historical_patterns[d][f] = cycle[d * 8 + f % 8];
            }
        }
    }
    
    // Set current sensor state: [temp, humidity, soil_moisture, wind_speed,
    //                            pest_count, leaf_wetness, solar_radiation, pressure]
    void set_current(float temp, float humidity, float soil_moisture, float wind,
                     float pest_count, float leaf_wetness, float solar, float pressure) {
        current_state[0] = temp;
        current_state[1] = humidity;
        current_state[2] = soil_moisture;
        current_state[3] = wind;
        current_state[4] = pest_count;
        current_state[5] = leaf_wetness;
        current_state[6] = solar;
        current_state[7] = pressure;
    }
    
    // Hadamard Product Fusion: element-wise multiply current × historical
    // This entangles the present moment with every point in the historical cycle
    void compute_forecast() {
        for (int d = 0; d < FORECAST_DAYS; d++) {
            float dharma_weight = 1.0f;  // Day 0 = highest confidence
            if (d > 7)  dharma_weight = 0.8f;
            if (d > 14) dharma_weight = 0.6f;
            
            // Hadamard product: current_state ⊙ historical_patterns[d]
            float fused[8];
            for (int f = 0; f < 8; f++) {
                fused[f] = current_state[f] * historical_patterns[d][f] * dharma_weight;
            }
            
            // Decode fused vector into forecast features
            forecast[d][0] = fused[0] * 0.8f + fused[6] * 0.2f;  // Temperature
            forecast[d][1] = fused[1] * 0.6f + fused[5] * 0.4f;  // Rain probability
            forecast[d][2] = fused[1] * 0.7f + fused[2] * 0.3f;  // Humidity
            forecast[d][3] = fused[4] * 0.5f + fused[1] * 0.5f;  // Pest risk
            
            // Nakshatra modifier: seasonal rain patterns
            for (int n = 0; NAKSHATRA_CALENDAR[n].nakshatra != nullptr; n++) {
                int day_of_year = (time(nullptr) / 86400) % 365;
                if (abs(day_of_year - NAKSHATRA_CALENDAR[n].start_day) <= d) {
                    forecast[d][1] = forecast[d][1] * 0.5f + 
                                     NAKSHATRA_CALENDAR[n].rain_probability * 0.5f;
                    break;
                }
            }
        }
    }
    
    // Get specific day forecast
    void get_day(int day, float* temp, float* rain, float* humidity, float* pest_risk) {
        if (day < 0 || day >= FORECAST_DAYS) return;
        *temp = forecast[day][0];
        *rain = forecast[day][1];
        *humidity = forecast[day][2];
        *pest_risk = forecast[day][3];
    }
    
    // Predict pest emergence window
    int predict_pest_peak() {
        float max_risk = 0;
        int peak_day = 0;
        for (int d = 0; d < FORECAST_DAYS; d++) {
            if (forecast[d][3] > max_risk) {
                max_risk = forecast[d][3];
                peak_day = d;
            }
        }
        return peak_day;
    }
    
    // Predict best sowing window
    int predict_sowing_window(float min_temp, float max_temp, float min_rain) {
        int best_start = -1, best_length = 0;
        int current_start = -1, current_length = 0;
        
        for (int d = 0; d < FORECAST_DAYS; d++) {
            bool good = (forecast[d][0] >= min_temp && forecast[d][0] <= max_temp && 
                        forecast[d][1] >= min_rain && forecast[d][3] < 0.4f);
            
            if (good) {
                if (current_start < 0) current_start = d;
                current_length++;
            } else {
                if (current_length > best_length) {
                    best_length = current_length;
                    best_start = current_start;
                }
                current_start = -1;
                current_length = 0;
            }
        }
        
        if (current_length > best_length) {
            best_start = current_start;
            best_length = current_length;
        }
        
        return best_start;
    }
    
    void print_forecast() {
        printf("\n🕉️  KAALA-VINYASA FORECAST (21 Days)\n");
        printf("  ================================\n");
        printf("  %-6s %-8s %-8s %-8s %-8s\n", "Day", "Temp°C", "Rain%", "Humid%", "Pest%");
        printf("  ------ -------- -------- -------- --------\n");
        
        for (int d = 0; d < FORECAST_DAYS; d += 3) {
            printf("  +%-3d   %-8.1f %-8.0f %-8.0f %-8.0f\n",
                   d, forecast[d][0] * 35, forecast[d][1] * 100,
                   forecast[d][2] * 100, forecast[d][3] * 100);
        }
        
        int pest_day = predict_pest_peak();
        float pest_risk = forecast[pest_day][3] * 100;
        printf("\n  🐛 PEST PEAK: Day +%d (%.0f%% risk)\n", pest_day, pest_risk);
        if (pest_risk > 60) {
            printf("     ⚠️  CRITICAL: Apply preventive Neem oil by Day +%d\n", 
                   pest_day > 3 ? pest_day - 3 : 0);
        }
        
        int sow_day = predict_sowing_window(20, 32, 0.4f);
        if (sow_day >= 0) {
            printf("\n  🌱 SOWING WINDOW: Day +%d to +%d\n", sow_day, sow_day + 3);
        }
        
        // Nakshatra guidance
        time_t now = time(nullptr);
        int solar_day = (now / 86400) % 365;
        for (int n = 0; NAKSHATRA_CALENDAR[n].nakshatra != nullptr; n++) {
            if (abs(solar_day - NAKSHATRA_CALENDAR[n].start_day) < 7) {
                printf("\n  🌙 NAKSHATRA: %s\n", NAKSHATRA_CALENDAR[n].nakshatra);
                printf("     Rain: %.0f%% | %s\n", 
                       NAKSHATRA_CALENDAR[n].rain_probability * 100,
                       NAKSHATRA_CALENDAR[n].farming_action);
                break;
            }
        }
    }
};
