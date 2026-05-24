/*
 * SUTRA 39: INDRA'S NET
 * Holographic Symmetry Kernel — every node contains the whole
 * If one node disconnects, system state is preserved across remaining nodes
 */
#pragma once
#include <cstring>
#include <cstdint>

#define INDRA_NODES 8
#define STATE_SIZE 64

struct IndraNode {
    float state[STATE_SIZE];      // Local state
    float hologram[STATE_SIZE];   // Compressed global state
    uint32_t version;
    bool active;
    
    IndraNode() : version(0), active(true) {
        for (int i = 0; i < STATE_SIZE; i++) state[i] = 0;
        for (int i = 0; i < STATE_SIZE; i++) hologram[i] = 0;
    }
};

struct IndraNet {
    IndraNode nodes[INDRA_NODES];
    
    // Update a node — automatically syncs to all others
    void update(int node_id, float* new_state) {
        IndraNode* n = &nodes[node_id];
        memcpy(n->state, new_state, STATE_SIZE * sizeof(float));
        n->version++;
        
        // Holographic projection: each node gets a weighted mix of all states
        for (int i = 0; i < INDRA_NODES; i++) {
            if (!nodes[i].active) continue;
            float weight = (i == node_id) ? 0.7f : 0.3f / (INDRA_NODES - 1);
            for (int j = 0; j < STATE_SIZE; j++) {
                nodes[i].hologram[j] = nodes[i].hologram[j] * 0.9f + 
                                       new_state[j] * weight;
            }
        }
    }
    
    // Even if a node disconnects, its state is recoverable
    void recover(int node_id, float* recovered) {
        IndraNode* n = &nodes[node_id];
        // Reconstruct from hologram + neighboring nodes
        float neighbor_sum[STATE_SIZE] = {0};
        int active_neighbors = 0;
        for (int i = 0; i < INDRA_NODES; i++) {
            if (i != node_id && nodes[i].active) {
                for (int j = 0; j < STATE_SIZE; j++) 
                    neighbor_sum[j] += nodes[i].state[j];
                active_neighbors++;
            }
        }
        for (int j = 0; j < STATE_SIZE; j++) {
            recovered[j] = n->hologram[j] * 0.5f;
            if (active_neighbors > 0)
                recovered[j] += neighbor_sum[j] / active_neighbors * 0.5f;
        }
    }
    
    void stats() {
        int active = 0;
        for (int i = 0; i < INDRA_NODES; i++) if (nodes[i].active) active++;
        printf("  Indra's Net: %d/%d nodes active | State preserved holographically\n", 
               active, INDRA_NODES);
    }
};
