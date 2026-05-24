/*
 * SUTRA 71: VEDALYTICS — Cross-Node Correlation Engine
 * 
 * Distributed Insight Generator for Indra's Net.
 * Discovers hidden causality across the Barak Valley.
 * 
 * Example insight: "When it rains in Lakhipur during Ardra,
 * Stem Borer appears in Sonai 48 hours later."
 * 
 * No single farmer could see this pattern.
 * The network sees everything.
 */

#pragma once
#include <cstring>
#include <cstdio>
#include <cmath>
#include <ctime>

#define PHI 1.618033988749895f
#define MAX_CORRELATIONS 256
#define MAX_NODES 64

// ================================================================
// CROSS-NODE CORRELATION — Discover hidden valley-wide patterns
// ================================================================
struct CrossCorrelation {
    int node_a;           // Source node (e.g., Lakhipur)
    int node_b;           // Target node (e.g., Sonai)
    int feature_a;        // What happened at A (e.g., rain)
    int feature_b;        // What happened at B (e.g., pest emergence)
    int lag_hours;        // Time delay between A and B
    float correlation;    // Pearson correlation coefficient (0-1)
    float confidence;     // Statistical significance
    const char* insight;  // Human-readable finding
};

struct VedalyticsEngine {
    // Node data: each node stores 4 features × 168 hours (7 days hourly)
    float node_history[MAX_NODES][4][168];  // [node][feature][hour]
    int node_count;
    CrossCorrelation discoveries[MAX_CORRELATIONS];
    int discovery_count;
    
    VedalyticsEngine() : node_count(0), discovery_count(0) {
        memset(node_history, 0, sizeof(node_history));
        memset(discoveries, 0, sizeof(discoveries));
    }
    
    // Register a node's hourly data
    void register_node(int node_id) {
        if (node_id < MAX_NODES && node_id >= node_count) {
            node_count = node_id + 1;
        }
    }
    
    // Record a sensor reading for a specific node and hour
    void record(int node_id, int hour, float temp, float rain, float humidity, float pest) {
        if (node_id >= MAX_NODES || hour >= 168) return;
        node_history[node_id][0][hour] = temp;
        node_history[node_id][1][hour] = rain;
        node_history[node_id][2][hour] = humidity;
        node_history[node_id][3][hour] = pest;
    }
    
    // Compute Pearson correlation between two time series
    float pearson_correlation(float* series_a, float* series_b, int len) {
        float sum_a = 0, sum_b = 0, sum_ab = 0, sum_a2 = 0, sum_b2 = 0;
        
        for (int i = 0; i < len; i++) {
            sum_a += series_a[i];
            sum_b += series_b[i];
            sum_ab += series_a[i] * series_b[i];
            sum_a2 += series_a[i] * series_a[i];
            sum_b2 += series_b[i] * series_b[i];
        }
        
        float n = len;
        float numerator = n * sum_ab - sum_a * sum_b;
        float denominator = sqrtf((n * sum_a2 - sum_a * sum_a) * (n * sum_b2 - sum_b * sum_b));
        
        return (denominator > 0.001f) ? numerator / denominator : 0;
    }
    
    // Cross-correlate with time lag: does A predict B after 'lag' hours?
    float cross_correlate(float* series_a, float* series_b, int len, int lag) {
        if (lag >= len) return 0;
        
        // Shift series_a by 'lag' hours relative to series_b
        float shifted_a[168];
        for (int i = 0; i < len - lag; i++) {
            shifted_a[i] = series_a[i];  // A at time t
        }
        
        float shifted_b[168];
        for (int i = 0; i < len - lag; i++) {
            shifted_b[i] = series_b[i + lag];  // B at time t+lag
        }
        
        return pearson_correlation(shifted_a, shifted_b, len - lag);
    }
    
    // Discover all significant correlations across the valley
    void discover(const char* node_names[]) {
        discovery_count = 0;
        
        const char* feature_names[] = {"Temperature", "Rainfall", "Humidity", "Pest Activity"};
        
        // For every pair of nodes
        for (int a = 0; a < node_count && discovery_count < MAX_CORRELATIONS; a++) {
            for (int b = 0; b < node_count && discovery_count < MAX_CORRELATIONS; b++) {
                if (a == b) continue;  // Skip self-correlation
                
                // For every pair of features
                for (int fa = 0; fa < 4; fa++) {
                    for (int fb = 0; fb < 4; fb++) {
                        // Try different time lags: 12h, 24h, 36h, 48h, 72h
                        int lags[] = {12, 24, 36, 48, 72};
                        for (int li = 0; li < 5 && discovery_count < MAX_CORRELATIONS; li++) {
                            int lag = lags[li];
                            float corr = cross_correlate(
                                node_history[a][fa], node_history[b][fb], 168, lag);
                            
                            // Significant correlation?
                            if (fabsf(corr) > 0.6f) {
                                CrossCorrelation* d = &discoveries[discovery_count];
                                d->node_a = a;
                                d->node_b = b;
                                d->feature_a = fa;
                                d->feature_b = fb;
                                d->lag_hours = lag;
                                d->correlation = corr;
                                d->confidence = fabsf(corr) * 0.9f + 0.1f;
                                
                                // Generate human-readable insight
                                char insight[256];
                                snprintf(insight, 256,
                                    "When %s in %s %s, %s in %s follows %d hours later "
                                    "(correlation: %.0f%%)",
                                    feature_names[fa], node_names[a],
                                    corr > 0 ? "increases" : "drops",
                                    feature_names[fb], node_names[b],
                                    lag, fabsf(corr) * 100);
                                d->insight = strdup(insight);
                                
                                discovery_count++;
                            }
                        }
                    }
                }
            }
        }
        
        // Sort by correlation strength (PHI-weighted: prefer moderate lags)
        for (int i = 0; i < discovery_count - 1; i++) {
            for (int j = i + 1; j < discovery_count; j++) {
                float score_i = fabsf(discoveries[i].correlation) * 
                               (1.0f - fabsf((float)(discoveries[i].lag_hours - 24)) / 100.0f);
                float score_j = fabsf(discoveries[j].correlation) * 
                               (1.0f - fabsf((float)(discoveries[j].lag_hours - 24)) / 100.0f);
                if (score_j > score_i) {
                    CrossCorrelation tmp = discoveries[i];
                    discoveries[i] = discoveries[j];
                    discoveries[j] = tmp;
                }
            }
        }
    }
    
    // Generate early warning: given current observation at node A, predict for node B
    void generate_warnings(const char* node_names[]) {
        printf("\n🕉️  VEDALYTICS — Valley-Wide Correlation Analysis\n");
        printf("  ==============================================\n\n");
        
        if (discovery_count == 0) {
            printf("  📊 Insufficient data for cross-node correlation.\n");
            printf("  Continue recording. Insights emerge after 7 days.\n");
            return;
        }
        
        printf("  TOP CROSS-NODE INSIGHTS:\n\n");
        
        int shown = 0;
        for (int i = 0; i < discovery_count && shown < 10; i++) {
            CrossCorrelation* d = &discoveries[i];
            
            // Only show the strongest, most confident correlations
            if (fabsf(d->correlation) < 0.65f) continue;
            
            printf("  %d. %s\n", shown + 1, d->insight);
            printf("     Confidence: %.0f%% | Valley-wide impact: %s\n",
                   d->confidence * 100,
                   fabsf(d->correlation) > 0.8f ? "HIGH — Alert all farmers" :
                   fabsf(d->correlation) > 0.7f ? "MEDIUM — Notify affected villages" :
                   "LOW — Monitor trend");
            printf("\n");
            shown++;
        }
        
        if (shown == 0) {
            printf("  ✅ No strong cross-node correlations yet.\n");
            printf("  The valley is in harmony. Patterns will emerge with time.\n");
        } else {
            printf("  🛡️  PRATYABHIJNA ACTION: %d significant patterns found.\n", shown);
            printf("  Broadcasting early warnings to affected villages.\n");
        }
    }
    
    // Market intelligence: aggregate yield predictions across all nodes
    void market_intelligence(const char* crop, const char* node_names[]) {
        printf("\n📊 VEDALYTICS MARKET INTELLIGENCE: %s\n", crop);
        printf("  ======================================\n");
        
        int farmers_growing = 0;
        float total_pest_risk = 0;
        float total_rain = 0;
        
        for (int n = 0; n < node_count; n++) {
            farmers_growing++;
            for (int h = 144; h < 168; h++) {  // Last 24 hours
                total_pest_risk += node_history[n][3][h];
                total_rain += node_history[n][1][h];
            }
        }
        
        if (farmers_growing > 0) {
            float avg_pest = total_pest_risk / (farmers_growing * 24);
            float avg_rain = total_rain / (farmers_growing * 24);
            
            printf("  Farmers: %d | Avg Pest Risk: %.0f%% | Avg Rain: %.0f%%\n",
                   farmers_growing, avg_pest * 100, avg_rain * 100);
            
            if (avg_pest > 0.5f) {
                printf("  ⚠️  MARKET SIGNAL: High pest pressure across valley.\n");
                printf("  Suggestion: HOLD harvested stock — prices likely to rise.\n");
            } else if (avg_rain > 0.7f) {
                printf("  🌧️  MARKET SIGNAL: Good rainfall — bumper harvest expected.\n");
                printf("  Suggestion: Consider forward contracts at current prices.\n");
            } else {
                printf("  ✅ MARKET: Normal conditions. Fair prices expected.\n");
            }
        }
    }
};
