/*
 * SUTRA 53: BUDDHI — Intellect & Decision Optimization
 * 
 * While Ahamkara chooses WHAT to do, Buddhi determines HOW to do it.
 * 
 * Given "ORDER_SUPPLIES", Buddhi optimizes:
 *   - Which supplier? (Vedalytics market analysis)
 *   - How much? (Kaala-Vinyasa pest forecast × field area)
 *   - Which route? (Indra's Net village connectivity)
 *   - When? (Kala-Chakra optimal timing window)
 * 
 * Buddhi = Discrimination. The intellect that separates optimal from merely good.
 */

#pragma once
#include <cstring>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <ctime>

#define PHI 1.618033988749895f

// ================================================================
// BUDDHI OPTION — A possible way to execute an action
// ================================================================
struct BuddhiOption {
    char description[128];
    float cost;           // Rupees or resource units
    float time_hours;     // How long to execute
    float effectiveness;  // 0-1: how well it solves the problem
    float risk;           // 0-1: chance of failure
    float dharma_score;   // 0-1: ethical/environmental alignment
    float composite_score; // Final weighted score
};

// ================================================================
// BUDDHI SUPPLIER — A source of agricultural supplies
// ================================================================
struct BuddhiSupplier {
    char name[64];
    char location[32];
    float neem_oil_price_per_liter;
    float delivery_time_hours;
    float reliability;       // 0-1: historical on-time rate
    float organic_certified; // 0-1: how organic
    int distance_km;
};

const BuddhiSupplier KNOWN_SUPPLIERS[] = {
    {"Silchar Agro Center",      "Silchar",      180.0f, 2.0f,  0.95f, 0.9f,  15},
    {"Cachar Organic Collective","Lakhipur",     150.0f, 4.0f,  0.85f, 1.0f,  25},
    {"Barak Valley Farmers Coop","Udharbond",    160.0f, 3.0f,  0.90f, 0.95f, 20},
    {"Assam Agri Supply",        "Karimganj",    140.0f, 6.0f,  0.75f, 0.7f,  45},
    {"Local Neem Processor",     "Sonai",        120.0f, 1.5f,  0.92f, 1.0f,  8},
    {"", "", 0, 0, 0, 0, 0}
};

// ================================================================
// BUDDHI INTELLECT — The Decision Optimizer
// ================================================================
struct BuddhiIntellect {
    BuddhiOption current_options[16];
    int option_count;
    float optimization_level;  // 0-1: how deeply to optimize
    
    BuddhiIntellect() : option_count(0), optimization_level(0.7f) {}
    
    // Generate options for "ORDER_SUPPLIES" action
    void generate_supply_options(float pest_severity, float area_hectares, 
                                  int farmers_affected, const char* nearest_village) {
        option_count = 0;
        
        // Calculate how much Neem oil is needed
        float liters_needed = area_hectares * 5.0f * pest_severity;  // 5L/ha base
        
        for (int i = 0; KNOWN_SUPPLIERS[i].name[0] != 0 && option_count < 16; i++) {
            const BuddhiSupplier* s = &KNOWN_SUPPLIERS[i];
            
            BuddhiOption opt;
            snprintf(opt.description, 128, "%s: %.0fL Neem oil @ ₹%.0f/L = ₹%.0f",
                     s->name, liters_needed, s->neem_oil_price_per_liter,
                     liters_needed * s->neem_oil_price_per_liter);
            
            opt.cost = liters_needed * s->neem_oil_price_per_liter;
            opt.time_hours = s->delivery_time_hours;
            opt.effectiveness = s->organic_certified * 0.7f + s->reliability * 0.3f;
            opt.risk = 1.0f - s->reliability;
            
            // Dharma: prefer local, organic, cooperative suppliers
            float dharma = 0;
            if (strstr(s->name, "Organic") || strstr(s->name, "Farmers")) dharma += 0.3f;
            if (s->organic_certified > 0.9f) dharma += 0.3f;
            if (s->distance_km < 20) dharma += 0.2f;  // Local = lower carbon
            opt.dharma_score = fminf(1.0f, dharma + 0.2f);
            
            current_options[option_count++] = opt;
        }
    }
    
    // Buddhi's core function: score and rank all options
    void evaluate_options(float urgency, float budget_limit) {
        for (int i = 0; i < option_count; i++) {
            BuddhiOption* opt = &current_options[i];
            
            // Composite score = weighted sum
            // Cost weight decreases with urgency (spend more when critical)
            float cost_weight = 0.25f * (1.0f - urgency * 0.5f);
            float time_weight = 0.30f * urgency;  // Time matters more when urgent
            float effect_weight = 0.25f;
            float risk_weight = 0.10f;
            float dharma_weight = 0.10f;
            
            // Normalize cost (lower = better)
            float cost_score = (budget_limit > 0) ? 1.0f - (opt->cost / budget_limit) : 0.5f;
            if (cost_score < 0) cost_score = 0;
            
            // Normalize time (lower = better)
            float time_score = 1.0f - (opt->time_hours / 48.0f);  // 48h = worst case
            if (time_score < 0) time_score = 0;
            
            opt->composite_score = 
                cost_score * cost_weight +
                time_score * time_weight +
                opt->effectiveness * effect_weight +
                (1.0f - opt->risk) * risk_weight +
                opt->dharma_score * dharma_weight;
            
            // Over-budget penalty
            if (opt->cost > budget_limit) opt->composite_score *= 0.5f;
        }
        
        // Sort by composite score (highest = best)
        for (int i = 0; i < option_count - 1; i++) {
            for (int j = i + 1; j < option_count; j++) {
                if (current_options[j].composite_score > current_options[i].composite_score) {
                    BuddhiOption tmp = current_options[i];
                    current_options[i] = current_options[j];
                    current_options[j] = tmp;
                }
            }
        }
    }
    
    // Get the best option
    BuddhiOption* best_option() {
        if (option_count == 0) return nullptr;
        return &current_options[0];
    }
    
    // Buddhi's final recommendation with reasoning
    void recommend(float pest_severity, float area_hectares, 
                   int farmers_affected, float urgency, float budget,
                   const char* village) {
        
        printf("  🧠 BUDDHI INTELLECT — Optimizing Action\n");
        printf("  ========================================\n");
        printf("  Context: %.0f%% pest severity, %.1f ha, %d farmers\n",
               pest_severity * 100, area_hectares, farmers_affected);
        printf("  Urgency: %.0f%% | Budget: ₹%.0f\n\n", urgency * 100, budget);
        
        generate_supply_options(pest_severity, area_hectares, farmers_affected, village);
        evaluate_options(urgency, budget);
        
        // Show top 3 options
        printf("  TOP OPTIONS:\n");
        for (int i = 0; i < 3 && i < option_count; i++) {
            BuddhiOption* o = &current_options[i];
            printf("  %d. %s\n", i + 1, o->description);
            printf("     Score: %.3f | Time: %.1fh | Dharma: %.0f%% | Risk: %.0f%%\n",
                   o->composite_score, o->time_hours, o->dharma_score * 100, o->risk * 100);
        }
        
        // Final recommendation
        BuddhiOption* best = best_option();
        if (best) {
            printf("\n  ✅ BUDDHI RECOMMENDS: %s\n", best->description);
            
            if (best->dharma_score > 0.8f) {
                printf("  🕉️  DHARMA ALIGNED: This choice supports local organic farmers.\n");
            }
            if (best->composite_score > 0.8f) {
                printf("  🎯 OPTIMAL: Highest composite score across all factors.\n");
            }
        }
    }
    
    // Buddhi also handles "SCHEDULE_HARVEST" optimization
    void optimize_harvest_schedule(float crop_maturity, float rain_forecast, 
                                    int labor_available, float market_price) {
        printf("  🧠 BUDDHI — Harvest Optimization\n");
        printf("  ================================\n");
        
        // Optimal harvest window
        float maturity_window = crop_maturity * 7.0f;  // Days until ready
        float rain_risk = rain_forecast * 3.0f;         // Days of delay from rain
        
        int best_day = (int)(maturity_window + rain_risk * 0.5f);
        best_day = best_day < 1 ? 1 : (best_day > 14 ? 14 : best_day);
        
        printf("  Crop ready in: %.0f days | Rain risk: %.0f days delay\n", 
               maturity_window, rain_risk);
        printf("  Labor: %d workers | Market: ₹%.0f/ton\n", labor_available, market_price);
        
        if (rain_forecast > 0.6f) {
            printf("  ⚠️  RAIN WARNING: Harvest %d days early to avoid crop loss.\n", best_day);
        } else if (market_price > 2000.0f) {
            printf("  📈 MARKET OPPORTUNITY: Prices high. Harvest at peak maturity.\n");
        } else {
            printf("  ✅ OPTIMAL: Harvest in %d days under normal conditions.\n", best_day);
        }
    }
    
    void stats() {
        printf("  🧠 BUDDHI: Intellect active | Optimization: %.0f%% depth\n", 
               optimization_level * 100);
    }
};
