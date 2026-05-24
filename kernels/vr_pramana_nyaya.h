/*
 * SUTRA 65: PRAMANA-NYAYA LOGIC
 * 5-Step Reasoning Scaffold: Every token tagged with epistemic source
 * Pratijna→Hetu→Udaharana→Upanaya→Nigamana
 */
#pragma once
#include <cstring>
#include <cstdio>

enum PramanaSource {
    PRATYAKSHA = 0, // Direct Perception
    ANUMANA    = 1, // Inference
    UPAMANA    = 2, // Comparison
    SHABDA     = 3, // Testimony
    UNGROUNDED = 4  // Hallucination risk!
};

struct PramanaToken {
    char text[64];
    PramanaSource source;
    float confidence;
    char hetu[128];     // Reason
    char udaharana[128]; // Example
};

struct NyayaScaffold {
    PramanaToken tokens[256];
    int count;
    
    NyayaScaffold() : count(0) {}
    
    // Tag a token with its epistemic source
    void tag(const char* text, PramanaSource src, float conf, 
             const char* reason="", const char* example="") {
        if (count >= 256) return;
        PramanaToken* t = &tokens[count++];
        strncpy(t->text, text, 63);
        t->source = src;
        t->confidence = conf;
        strncpy(t->hetu, reason, 127);
        strncpy(t->udaharana, example, 127);
    }
    
    // Hallucination check: flag ungrounded tokens
    int detect_hallucinations() {
        int flagged = 0;
        for (int i = 0; i < count; i++) {
            if (tokens[i].source == UNGROUNDED || tokens[i].confidence < 0.5f) {
                printf("  ⚠️  HALLUCINATION: '%s' (confidence: %.2f)\n", 
                       tokens[i].text, tokens[i].confidence);
                flagged++;
            }
        }
        return flagged;
    }
    
    // Generate 5-step reasoning trace
    void explain() {
        printf("  Nyaya 5-Step Reasoning:\n");
        for (int i = 0; i < count && i < 5; i++) {
            const char* steps[] = {"Pratijna","Hetu","Udaharana","Upanaya","Nigamana"};
            printf("    %d. %s: %s", i+1, steps[i], tokens[i].text);
            if (tokens[i].hetu[0]) printf(" (because: %s)", tokens[i].hetu);
            printf("\n");
        }
    }
};
