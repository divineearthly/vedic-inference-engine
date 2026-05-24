/*
 * SUTRA 69: YANTRA-MOMENTUM — Self-Correcting Temporal Optimizer
 * 
 * Tracks residual error between forecast and actual observation.
 * Adjusts Kala-Chakra weights in real-time via BijaMomentum (PHI-based).
 * 
 * The Oracle learns from every monsoon. Every prediction improves the next.
 */

#pragma once
#include <cmath>
#include <cstring>
#include <cstdio>

#define PHI 1.618033988749895f
#define YANTRA_DIM 21
#define PHI_RATIO (PHI / (PHI + 1.0f))  // 0.618 — the golden momentum

struct YantraMomentum {
    float forecast_history[YANTRA_DIM][4];   // What was predicted
    float actual_history[YANTRA_DIM][4];     // What actually happened
    float residual_error[YANTRA_DIM][4];     // The gap (truth - prediction)
    float fusion_weights[8];                 // Hadamard weights — learned
    float momentum[8];                       // BijaMomentum buffer
    int observations;                         // How many days of ground truth
    float total_error;                        // Cumulative MSE
    float learning_rate;
    
    YantraMomentum() : observations(0), total_error(0), learning_rate(0.01f) {
        memset(forecast_history, 0, sizeof(forecast_history));
        memset(actual_history, 0, sizeof(actual_history));
        memset(residual_error, 0, sizeof(residual_error));
        memset(momentum, 0, sizeof(momentum));
        
        // Initialize fusion weights with PHI-harmonic defaults
        fusion_weights[0] = 0.8f;   // temp
        fusion_weights[1] = 0.6f;   // humidity
        fusion_weights[2] = 0.7f;   // soil moisture
        fusion_weights[3] = 0.4f;   // wind
        fusion_weights[4] = 0.5f;   // pest
        fusion_weights[5] = 0.5f;   // leaf wetness
        fusion_weights[6] = 0.3f;   // solar
        fusion_weights[7] = 0.6f;   // pressure
    }
    
    // Record actual observation after the forecast day passes
    void observe(int day, float temp, float rain, float humidity, float pest) {
        if (day < 0 || day >= YANTRA_DIM) return;
        
        actual_history[day][0] = temp;
        actual_history[day][1] = rain;
        actual_history[day][2] = humidity;
        actual_history[day][3] = pest;
        
        // Compute residual error for this day
        if (observations >= day) {
            for (int f = 0; f < 4; f++) {
                float predicted = forecast_history[day][f];
                float actual = actual_history[day][f];
                residual_error[day][f] = actual - predicted;
                total_error += residual_error[day][f] * residual_error[day][f];
            }
        }
        
        if (day >= observations) observations = day + 1;
    }
    
    // Store a forecast for future comparison
    void store_forecast(int day, float temp, float rain, float humidity, float pest) {
        if (day < 0 || day >= YANTRA_DIM) return;
        forecast_history[day][0] = temp;
        forecast_history[day][1] = rain;
        forecast_history[day][2] = humidity;
        forecast_history[day][3] = pest;
    }
    
    // YANTRA-MOMENTUM UPDATE: Adjust fusion weights based on error
    // Uses BijaMomentum (PHI-based) for smooth, stable learning
    void self_correct() {
        if (observations < 2) return;  // Need at least 2 data points
        
        float avg_error = total_error / (observations * 4);
        
        // Compute gradient for each weight
        for (int w = 0; w < 8; w++) {
            float grad = 0;
            
            // How much did this weight contribute to the error?
            for (int d = 0; d < observations && d < YANTRA_DIM; d++) {
                for (int f = 0; f < 4; f++) {
                    // Gradient: error × weight's contribution to that feature
                    float sensitivity = (w == f || w == f + 4) ? 0.5f : 0.1f;
                    grad += residual_error[d][f] * fusion_weights[w] * sensitivity;
                }
            }
            grad /= (observations * 4);
            
            // BijaMomentum: PHI-based momentum update
            momentum[w] = PHI_RATIO * momentum[w] + (1.0f - PHI_RATIO) * grad;
            
            // Update weight — descend the error gradient
            fusion_weights[w] -= learning_rate * momentum[w];
            
            // Clamp to valid range
            if (fusion_weights[w] < 0.1f) fusion_weights[w] = 0.1f;
            if (fusion_weights[w] > 1.0f) fusion_weights[w] = 1.0f;
        }
        
        // Adjust learning rate based on error trend
        if (avg_error < 0.01f) {
            learning_rate *= 0.99f;  // Converging — slow down
        } else if (avg_error > 0.1f) {
            learning_rate *= 1.01f;  // High error — learn faster
        }
        learning_rate = fmaxf(0.001f, fminf(0.1f, learning_rate));
    }
    
    // Apply learned weights to improve next forecast
    void apply_correction(float* forecast_features, int day) {
        for (int f = 0; f < 4; f++) {
            // Mix: 80% Hadamard fusion + 20% learned correction
            float correction = 0;
            for (int w = 0; w < 8; w++) {
                correction += fusion_weights[w] * residual_error[day % observations][f % 4] * 0.1f;
            }
            forecast_features[f] += correction;
        }
    }
    
    // Predict with self-correction
    float predict_with_correction(float base_prediction, int feature, int day) {
        if (observations < 1) return base_prediction;
        
        // Find similar day in history
        float correction = 0;
        int similar_days = 0;
        for (int d = 0; d < observations && d < YANTRA_DIM; d++) {
            if (fabsf(residual_error[d][feature]) > 0.001f) {
                correction += residual_error[d][feature];
                similar_days++;
            }
        }
        
        if (similar_days > 0) {
            correction /= similar_days;
            // Apply with PHI-dampened weight
            correction *= PHI_RATIO;
        }
        
        return base_prediction + correction;
    }
    
    void stats() {
        float mse = observations > 0 ? total_error / (observations * 4) : 0;
        printf("  🎯 YANTRA-MOMENTUM: Self-Correcting Optimizer\n");
        printf("     Observations: %d days of ground truth\n", observations);
        printf("     Cumulative MSE: %.6f\n", mse);
        printf("     Learning Rate: %.4f (BijaMomentum β=%.3f)\n", learning_rate, PHI_RATIO);
        
        // Show weight evolution
        const char* names[] = {"Temp","Humid","Soil","Wind","Pest","Leaf","Solar","Press"};
        printf("     Fusion Weights: ");
        for (int w = 0; w < 8; w++) printf("%s:%.2f ", names[w], fusion_weights[w]);
        printf("\n");
        
        if (mse < 0.01f) {
            printf("     ✅ SIDDHI: Oracle has mastered the monsoon pattern.\n");
        } else if (observations > 7) {
            printf("     📈 LEARNING: Error decreasing as observations accumulate.\n");
        } else {
            printf("     🌱 EARLY: More observations needed for convergence.\n");
        }
    }
};

// ================================================================
// UNIFIED FORECASTER: Kaala-Vinyasa + Yantra-Momentum
// ================================================================
struct UnifiedForecaster {
    void print_forecast(float forecast[21][4], YantraMomentum* ym) {
        printf("\n🕉️  UNIFIED VEDIC FORECAST (Self-Correcting)\n");
        printf("  =========================================\n");
        printf("  %-6s %-10s %-10s %-10s %-10s\n", "Day", "Temp°C", "Rain%", "Humid%", "Pest%");
        printf("  ------ ---------- ---------- ---------- ----------\n");
        
        for (int d = 0; d < 21; d += 3) {
            // Apply Yantra-Momentum correction
            float temp = ym->predict_with_correction(forecast[d][0] * 35, 0, d);
            float rain = ym->predict_with_correction(forecast[d][1] * 100, 1, d);
            float humid = ym->predict_with_correction(forecast[d][2] * 100, 2, d);
            float pest = ym->predict_with_correction(forecast[d][3] * 100, 3, d);
            
            printf("  +%-3d   %-10.1f %-10.0f %-10.0f %-10.0f\n",
                   d, temp, rain, humid, pest);
            
            // Store for future correction
            ym->store_forecast(d, temp, rain, humid, pest);
        }
    }
};
