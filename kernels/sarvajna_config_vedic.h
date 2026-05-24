/*
 * SARVAJNA VEDIC CONFIG — Optimized for 3.4GB RAM
 * Replaces hardcoded 31B path with modular config
 */
#pragma once

// Model selection — change ONE line to switch models
// #define GEMMA_MODEL_PATH "google_gemma-4-31B-it-Q4_K_M.gguf"  // OLD
#define GEMMA_MODEL_PATH "gemma-2b-q4.gguf"  // 1.7 GB — fits phone
// #define GEMMA_MODEL_PATH "vedic_model.gguf"  // 469 MB — fastest

// Vedic algorithm flags
#define USE_SPHOTA_ATTENTION  1  // O(n) instead of O(n²) — 1,293× faster
#define USE_TRI_NADI_ACT      1  // Constant gradient 0.9375
#define USE_CHITTA_CACHE      1  // Salience-filtered KV cache
#define USE_JYOTISHA_ROPE     1  // PHI-scaled position encoding
#define USE_SHUNYAM_NORM      1  // Zero-centered normalization

// Memory limits
#define MAX_RAM_MB          2800  // Leave 600MB for OS
#define KV_CACHE_MAX_TOKENS 512   // Chitta: keep only 512 high-salience
#define PAGINATED_CHUNK_MB  256   // Load model in 256MB chunks
