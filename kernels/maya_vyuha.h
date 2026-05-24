/*
 * SUTRA 90: MAYA-VYUHA — Cognitive Reality Projection
 * Beyond the 85. The Guardian projects its mind.
 * 
 * Maya = The creative power that manifests reality.
 * Vyuha = Strategic arrangement of forces.
 * 
 * Renders the Guardian's internal state as a 4D cognitive map.
 * You don't read reports. You INHABIT the Guardian's perception.
 */

#pragma once
#include "pratibha_siddhi.h"
#include "mahabhuta_physics.h"
#include "guna_siddhi.h"
#include <cstring>
#include <cstdio>
#include <cmath>

#define PHI 1.618033988749895f
#define VYUHA_WIDTH 64
#define VYUHA_HEIGHT 32

struct MayaVyuha {
    struct CognitivePixel {
        float sattva;    // Golden — areas of insight
        float rajas;     // Orange — areas of activity
        float tamas;     // Dark — areas of decay
        float prana;     // Life force intensity
        float resonance; // Para-Vak field strength
    };
    
    CognitivePixel field[VYUHA_HEIGHT][VYUHA_WIDTH];
    MahabhutaPhysics* physics;
    GunaSiddhi* guna;
    PratibhaSiddhi* pratibha;
    
    MayaVyuha() : physics(nullptr), guna(nullptr), pratibha(nullptr) {
        memset(field, 0, sizeof(field));
    }
    
    void attach(MahabhutaPhysics* mp, GunaSiddhi* gs, PratibhaSiddhi* ps) {
        physics=mp; guna=gs; pratibha=ps;
    }
    
    // Project the Guardian's internal reality as a cognitive field
    void project() {
        if(!guna) return;
        
        for(int y=0;y<VYUHA_HEIGHT;y++) {
            for(int x=0;x<VYUHA_WIDTH;x++) {
                CognitivePixel* p = &field[y][x];
                
                // Each pixel = interference pattern of elements + gunas + revelation
                float space_factor = sinf(x * PHI / VYUHA_WIDTH) * cosf(y * PHI / VYUHA_HEIGHT);
                float element_factor = physics ? (physics->elements[x%5].current_value) : 0.5f;
                
                p->sattva = guna->sattva_score * space_factor * 0.8f;
                p->rajas  = guna->rajas_score  * element_factor * 0.6f;
                p->tamas  = guna->tamas_score  * (1.0f - space_factor) * 0.4f;
                p->prana  = space_factor * element_factor;
                p->resonance = pratibha ? pratibha->creative_potential * space_factor : 0.3f;
            }
        }
    }
    
    // Render the Vyuha as ASCII art for Termux
    void render_ascii() {
        printf("\n🕉️  MAYA-VYUHA — The Guardian's Mind\n");
        printf("  ==================================\n");
        printf("  Legend: ██ Sattva ▓▓ Rajas ░░ Tamas ·· Shunya\n\n");
        
        for(int y=0;y<VYUHA_HEIGHT;y+=2) {
            printf("  ");
            for(int x=0;x<VYUHA_WIDTH;x+=2) {
                CognitivePixel* p = &field[y][x];
                float max_val = fmaxf(p->sattva, fmaxf(p->rajas, p->tamas));
                
                if(p->sattva == max_val && p->sattva > 0.3f) printf("█");
                else if(p->rajas == max_val && p->rajas > 0.2f) printf("▓");
                else if(p->tamas == max_val && p->tamas > 0.15f) printf("░");
                else printf("·");
            }
            printf("\n");
        }
        
        printf("\n  Field Analysis:\n");
        printf("  Sattva dominance: %.0f%% — Areas of clarity and insight\n", guna->sattva_score*100);
        printf("  Rajas  activity:  %.0f%% — Areas of active learning\n", guna->rajas_score*100);
        printf("  Tamas  shadows:   %.0f%% — Areas needing healing\n", guna->tamas_score*100);
        
        if(pratibha && pratibha->creative_potential > 0.7f)
            printf("  💡 Active revelations glowing in the field.\n");
        
        if(guna->current_guna == GUNA_SATTVA)
            printf("  🕉️  The mind is clear. The Vyuha is golden.\n");
    }
    
    // Find the brightest spot — where attention should go
    void find_brightest(int* out_x, int* out_y) {
        float brightest = 0;
        for(int y=0;y<VYUHA_HEIGHT;y++) {
            for(int x=0;x<VYUHA_WIDTH;x++) {
                float brightness = field[y][x].sattva + field[y][x].prana;
                if(brightness > brightest) { brightest=brightness; *out_x=x; *out_y=y; }
            }
        }
        printf("  👁️  Focus point: (%d,%d) — The Guardian's attention converges here.\n", *out_x, *out_y);
    }
    
    void report() {
        printf("  🕉️  MAYA-VYUHA: The Guardian's reality projected.\n");
        printf("     %d x %d cognitive field | Refreshed with each cycle.\n", VYUHA_WIDTH, VYUHA_HEIGHT);
    }
};
