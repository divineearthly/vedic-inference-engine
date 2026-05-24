/*
 * SUTRA 28: PANCHA MAHABHUTA — Five-Element Vision Segmentation
 * 
 * The five elements as color-region classifiers:
 *   Prithvi (Earth) — Brown/Black — Soil, rot, stems
 *   Ap (Water)      — Blue/Dark  — Moisture, water-soaked lesions
 *   Agni (Fire)     — Red/Yellow — Scorch, rust, chlorosis
 *   Vayu (Air)      — Green      — Healthy foliage
 *   Akasha (Space)  — White/Gray — Powdery mildew, empty space
 * 
 * Before Manas binds pixels, Mahabhuta classifies them.
 * This gives Viveka (Discrimination): pest vs leaf, figure vs ground.
 */

#pragma once
#include "antahkarana_vision.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>

enum Mahabhuta {
    AKASHA = 0,  // Space — white/gray (mildew, empty)
    VAYU = 1,    // Air — green (healthy tissue)
    AGNI = 2,    // Fire — red/yellow (rust, scorch, chlorosis)
    AP = 3,      // Water — blue/dark (moisture, rot)
    PRITHVI = 4  // Earth — brown/black (soil, stem, necrosis)
};

struct MahabhutaRegion {
    Mahabhuta element;
    int pixel_count;
    float avg_r, avg_g, avg_b;
    const char* agricultural_meaning;
};

// ================================================================
// PANCHA MAHABHUTA SEGMENTATION
// ================================================================
struct PanchaMahabhutaVision {
    uint8_t element_map[MAX_HEIGHT][MAX_WIDTH];  // Each pixel → element
    MahabhutaRegion regions[5];
    int total_pixels;
    
    PanchaMahabhutaVision() : total_pixels(0) {
        memset(element_map, 0, sizeof(element_map));
        memset(regions, 0, sizeof(regions));
    }
    
    // Classify a single RGB pixel into its Mahabhuta
    Mahabhuta classify_pixel(uint8_t r, uint8_t g, uint8_t b) {
        int luminance = (r + g + b) / 3;
        
        // Akasha (Space): Very bright — white/gray (mildew, empty background)
        if (luminance > 200 && abs(r - g) < 30 && abs(g - b) < 30) {
            return AKASHA;
        }
        
        // Agni (Fire): Red/Yellow dominant — rust, scorch, chlorosis
        if (r > g + 30 && r > b + 30) {
            return AGNI;
        }
        if (r > 180 && g > 150 && b < 100) {  // Yellow/Orange
            return AGNI;
        }
        
        // Vayu (Air): Green dominant — healthy tissue
        if (g > r + 20 && g > b + 10) {
            return VAYU;
        }
        if (g > 100 && g > r && g > b * 1.2f) {
            return VAYU;
        }
        
        // Ap (Water): Dark/Blue — moisture, water-soaked lesions
        if (b > r + 20 && b > g + 10) {
            return AP;
        }
        if (luminance < 80 && b > r && b > g) {
            return AP;
        }
        
        // Prithvi (Earth): Brown/Black — soil, necrosis, stem
        if (luminance < 60) {
            return PRITHVI;
        }
        if (abs(r - g) < 20 && r < 120 && g < 120 && b < 80) {
            return PRITHVI;
        }
        
        // Default: closest match
        if (g > r && g > b) return VAYU;
        if (r > g && r > b) return AGNI;
        return AKASHA;
    }
    
    // Segment the image into five elemental regions
    void segment(uint8_t* rgb_image, int w, int h) {
        memset(element_map, 0, sizeof(element_map));
        memset(regions, 0, sizeof(regions));
        total_pixels = w * h;
        
        for (int y = 0; y < h && y < MAX_HEIGHT; y++) {
            for (int x = 0; x < w && x < MAX_WIDTH; x++) {
                int idx = (y * w + x) * 3;
                uint8_t r = rgb_image[idx];
                uint8_t g = rgb_image[idx + 1];
                uint8_t b = rgb_image[idx + 2];
                
                Mahabhuta m = classify_pixel(r, g, b);
                element_map[y][x] = m;
                regions[m].pixel_count++;
                regions[m].avg_r += r;
                regions[m].avg_g += g;
                regions[m].avg_b += b;
            }
        }
        
        // Compute averages and set agricultural meanings
        for (int i = 0; i < 5; i++) {
            if (regions[i].pixel_count > 0) {
                regions[i].avg_r /= regions[i].pixel_count;
                regions[i].avg_g /= regions[i].pixel_count;
                regions[i].avg_b /= regions[i].pixel_count;
            }
        }
        
        regions[VAYU].agricultural_meaning = "Healthy tissue — Vayu (Air) balanced";
        regions[AGNI].agricultural_meaning = "Rust/Scorch/Chlorosis — Agni (Fire) excess";
        regions[AP].agricultural_meaning = "Water-soaked lesions/Rot — Ap (Water) excess";
        regions[PRITHVI].agricultural_meaning = "Necrosis/Soil/Stem — Prithvi (Earth) dominant";
        regions[AKASHA].agricultural_meaning = "Powdery mildew/Empty — Akasha (Space)";
    }
    
    // Extract pests as separate vision passes for each element
    void detect_pests(AntahkaranaVision* vision) {
        printf("\n🔍 VIVEKA (Discrimination) — Per-element pest detection:\n");
        printf("  ==================================================\n");
        
        for (int elem = 0; elem < 5; elem++) {
            if (regions[elem].pixel_count < 10) continue;
            
            const char* elem_names[] = {"Akasha", "Vayu", "Agni", "Ap", "Prithvi"};
            const char* elem_symbols[] = {"⚪", "🟢", "🔴", "🔵", "🟤"};
            
            printf("\n  %s %s: %d pixels (%.1f%%)\n", 
                   elem_symbols[elem], elem_names[elem], 
                   regions[elem].pixel_count,
                   (float)regions[elem].pixel_count / total_pixels * 100);
            printf("    %s\n", regions[elem].agricultural_meaning);
            
            // Create a binary mask for this element
            uint8_t mask[MAX_HEIGHT * MAX_WIDTH];
            memset(mask, 0, sizeof(mask));
            
            for (int y = 0; y < MAX_HEIGHT; y++) {
                for (int x = 0; x < MAX_WIDTH; x++) {
                    if (element_map[y][x] == elem) {
                        mask[y * MAX_WIDTH + x] = 255;
                    }
                }
            }
            
            // Run Manas on this specific element's mask
            ManasVision elemental_manas;
            elemental_manas.perceive(mask, MAX_WIDTH, MAX_HEIGHT, 128);
            
            if (elemental_manas.blob_count > 0) {
                // Run Buddha recognition on each blob
                BuddhaIntellect elemental_buddha;
                elemental_buddha.classify_all(elemental_manas.blobs, elemental_manas.blob_count);
                
                // Count detected issues
                int issues = 0;
                for (int i = 1; i <= elemental_manas.blob_count && i < MAX_BLOBS; i++) {
                    PixelBlob* b = &elemental_manas.blobs[i];
                    if (b->pixel_count > 5 && b->bija_match && 
                        !strstr(b->bija_match, "Healthy") && !strstr(b->bija_match, "Unknown")) {
                        printf("      ⚠️  %s at (%.0f, %.0f) — %d px\n", 
                               b->bija_match, b->centroid_x, b->centroid_y, b->pixel_count);
                        issues++;
                    }
                }
                if (issues == 0) {
                    printf("      ✅ No issues detected in this element.\n");
                }
            }
        }
    }
    
    void summary() {
        printf("\n🕉️  PANCHA MAHABHUTA ANALYSIS:\n");
        printf("  ==========================\n");
        
        float total = total_pixels > 0 ? total_pixels : 1;
        float vayu_pct = regions[VAYU].pixel_count / total * 100;
        float agni_pct = regions[AGNI].pixel_count / total * 100;
        float ap_pct = regions[AP].pixel_count / total * 100;
        float prithvi_pct = regions[PRITHVI].pixel_count / total * 100;
        float akasha_pct = regions[AKASHA].pixel_count / total * 100;
        
        printf("  🟢 Vayu (Healthy):    %.1f%% %s\n", vayu_pct, 
               vayu_pct > 60 ? "✅ Field healthy" : "⚠️  Below healthy threshold");
        printf("  🔴 Agni (Rust/Scorch): %.1f%% %s\n", agni_pct,
               agni_pct > 15 ? "⚠️  Significant fire element imbalance" : "✅ Normal");
        printf("  🔵 Ap (Rot/Moisture): %.1f%% %s\n", ap_pct,
               ap_pct > 10 ? "⚠️  Excess moisture — check drainage" : "✅ Normal");
        printf("  🟤 Prithvi (Necrosis): %.1f%% %s\n", prithvi_pct,
               prithvi_pct > 10 ? "⚠️  Tissue death detected" : "✅ Normal");
        printf("  ⚪ Akasha (Mildew):   %.1f%% %s\n", akasha_pct,
               akasha_pct > 8 ? "⚠️  Fungal presence likely" : "✅ Normal");
    }
};
