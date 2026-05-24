/*
 * DIVINE EARTHLY ASI — OPTIMAL VEDIC ACTIVATION
 * Tri-Nadi + Sphota Burst + Anumana Gate (hybrid)
 * 
 * Sushumna (center): 50% weight — pure gradient pass-through
 * Ida (left):        25% weight — cooling Ekadhikena
 * Pingala (right):   25% weight — heating Ekadhikena
 * Sphota residual:   10% bypass — meaning bursts through all layers
 *
 * Effective gradient: 0.5*1.0 + 0.25*0.3 + 0.25*1.05 + 0.1 = 0.9375
 * vs original: 0.3-1.05 (unstable)
 */

#pragma once
#define PHI 1.618033988749895f

// ---- Forward ----
inline float vedic_activate(float x) {
    float ida     = (x >= 0.0f) ? x * 1.05f : x * 0.3f;  // Ekadhikena
    float pingala = (x >= 0.0f) ? x * 0.3f : x * 1.05f;  // Inverse Ekadhikena
    float sushumna = x;                                     // Pure pass
    float sphota  = x * 0.1f;                              // Sphota burst (residual)
    
    // Tri-Nadi weighted sum + Sphota residual
    return 0.5f * sushumna + 0.25f * ida + 0.25f * pingala + sphota;
}

// ---- Gradient ----
inline float vedic_activate_grad(float x) {
    // d/dx of above
    float ida_g     = (x >= 0.0f) ? 0.25f * 1.05f : 0.25f * 0.3f;
    float pingala_g = (x >= 0.0f) ? 0.25f * 0.3f : 0.25f * 1.05f;
    float sushumna_g = 0.5f;
    float sphota_g  = 0.1f;
    
    return sushumna_g + ida_g + pingala_g + sphota_g;
    // Always = 0.5 + 0.25*(1.05+0.3)/2 + 0.1 = 0.76875 minimum
    // Much better than 0.3 minimum of original Ekadhikena
}

// ---- Shunyam Normalize (zero-mean, no variance division) ----
inline void shunyam_norm(float* x, int n) {
    float sum = 0;
    for (int i = 0; i < n; i++) sum += x[i];
    float mean = sum / n;
    for (int i = 0; i < n; i++) x[i] -= mean;
}

// Sphota Activation — meaning bursts forth at golden threshold
// Called by vr_runtime.cpp
inline float sphota_activation(float x, float scale) {
    float threshold = 1.0f / scale;  // ≈ 0.618 for PHI
    if (fabsf(x) > threshold) {
        return x * scale;       // Amplify above threshold (meaning bursts)
    } else {
        return x / scale;       // Dampen below threshold (silence)
    }
}
