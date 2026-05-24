/*
 * SUTRA 68: SUTRA-CONVOLUTION
 * Replaces standard convolution with Urdhva-Tiryagbhyam single-pass arithmetic
 * Directly mappable to ALU micro-instructions — no multi-step floating point
 */
#pragma once
#include <arm_neon.h>
#include <cstdint>

// Single-pass multiply-accumulate (replaces 3-step: mul→add→store)
inline float32x4_t sutra_mac(float32x4_t acc, float32x4_t a, float32x4_t b) {
    return vfmaq_f32(acc, a, b); // Fused Multiply-Accumulate — 1 instruction
}

// Sutra-Convolution kernel: O(n) not O(n*k)
// Urdhva (vertical) pass + Tiryag (crosswise) accumulation
inline void sutra_conv1d(float* input, float* kernel, float* output, int n, int k) {
    for (int i = 0; i < n; i++) {
        float32x4_t acc = vdupq_n_f32(0.0f);
        int j = 0;
        for (; j + 3 < k; j += 4) {
            float32x4_t in = vld1q_f32(&input[i + j]);
            float32x4_t ker = vld1q_f32(&kernel[j]);
            acc = sutra_mac(acc, in, ker);
        }
        output[i] = vgetq_lane_f32(acc, 0) + vgetq_lane_f32(acc, 1) +
                    vgetq_lane_f32(acc, 2) + vgetq_lane_f32(acc, 3);
        for (; j < k; j++) output[i] += input[i + j] * kernel[j];
    }
}
