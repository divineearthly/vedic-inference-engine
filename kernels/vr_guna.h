/*
 * SUPREME ALGORITHM 3: GUNA-WEIGHTING (Sattva, Rajas, Tamas)
 * Sankhya Philosophy: 24 Tattvas → Dynamic Learning States
 * 
 * Sattva (Light): Protected weights — 0.1x learning rate
 * Rajas (Energy): Active learning — 5x learning rate
 * Tamas (Inertia): Pruned if inactive — 0x learning rate
 */
#pragma once
#include <cmath>
#include <cstring>

enum GunaState { SATTVIC = 0, RAJASIC = 1, TAMASIC = 2 };

struct GunaNeuron {
    float weight;
    GunaState guna;
    int inactivity_counter;
    float accuracy_history;
    
    GunaNeuron() : weight(0), guna(RAJASIC), inactivity_counter(0), accuracy_history(0) {}
    
    void update_guna() {
        if (accuracy_history > 0.8f && inactivity_counter < 100) {
            guna = SATTVIC;  // Protected: high accuracy, frequently used
        } else if (inactivity_counter > 500) {
            guna = TAMASIC;  // Prune: not used for 500+ steps
        } else {
            guna = RAJASIC;  // Active learning
        }
    }
    
    float learning_rate_multiplier() {
        switch (guna) {
            case SATTVIC: return 0.1f;   // Slow updates — preserve knowledge
            case RAJASIC: return 5.0f;    // Fast learning — plasticity
            case TAMASIC: return 0.0f;    // Frozen — prune candidate
        }
        return 1.0f;
    }
};

struct GunaLayer {
    GunaNeuron* neurons;
    int size;
    
    GunaLayer(int n) : size(n) {
        neurons = new GunaNeuron[n];
        for (int i = 0; i < n; i++) {
            neurons[i].weight = sinf(i * 1.618f) * 0.01f;
            neurons[i].guna = RAJASIC;
        }
    }
    ~GunaLayer() { delete[] neurons; }
    
    void forward(float* input, float* output) {
        for (int i = 0; i < size; i++) {
            if (neurons[i].guna == TAMASIC) {
                output[i] = 0;  // Pruned neuron — skip computation
                continue;
            }
            output[i] = input[i] * neurons[i].weight;
            neurons[i].inactivity_counter = (input[i] != 0) ? 0 : neurons[i].inactivity_counter + 1;
        }
    }
    
    void update_guna_states() {
        for (int i = 0; i < size; i++) neurons[i].update_guna();
    }
    
    void stats() {
        int s = 0, r = 0, t = 0;
        for (int i = 0; i < size; i++) {
            if (neurons[i].guna == SATTVIC) s++;
            else if (neurons[i].guna == RAJASIC) r++;
            else t++;
        }
        printf("  Guna-Net: Sattva=%d (protected) | Rajas=%d (learning) | Tamas=%d (pruned)\n", s, r, t);
        printf("  Computational savings: %.1f%%\n", 100.0f * t / size);
    }
};
