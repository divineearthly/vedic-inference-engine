/*
 * SUTRA 72: SVAR-SAMSKARA — Pattern Signatures & Acoustic Authentication
 * 
 * Svar (Sound/Vibration) + Samskara (Impression/Signature)
 * 
 * 1. Acoustic Pest Monitoring: Identifies Stem Borer vs cricket by frequency
 * 2. Farmer Voice Authentication: Voice-print to verify human operators
 * 3. Sensor Spoof Detection: Validates data patterns against known signatures
 * 
 * Every entity in nature has a unique Svar. The Oracle learns to hear them.
 */

#pragma once
#include <cstring>
#include <cstdio>
#include <cmath>
#include <cstdlib>

#define PHI 1.618033988749895f
#define SVAR_WINDOW 256
#define SVAR_FINGERPRINT_SIZE 16

// ================================================================
// ACOUSTIC SIGNATURE — Frequency fingerprint of any sound
// ================================================================
struct SvarSignature {
    float fingerprint[SVAR_FINGERPRINT_SIZE];  // Unique acoustic ID
    char label[32];                             // What this sound is
    float confidence;                           // How well this signature matches
    int samples_learned;                        // Training examples seen
};

// ================================================================
// PEST SOUND TEMPLATES — Known frequencies of agricultural pests
// ================================================================
struct PestSound {
    const char* pest_name;
    float base_frequency;      // Hz — characteristic frequency
    float frequency_range;     // ±Hz variation
    float pulse_rate;          // Pulses per second
    float amplitude_peak;      // Normalized 0-1
    const char* sound_description;
};

const PestSound PEST_SOUNDS[] = {
    {"Stem Borer Larvae",   2500.0f, 300.0f,  15.0f, 0.7f, "Continuous chewing — high frequency grinding"},
    {"Rice Hispa Adult",    4200.0f, 200.0f,  8.0f,  0.5f, "Intermittent scraping — short bursts"},
    {"Gundhi Bug",          800.0f,  150.0f,  3.0f,  0.4f, "Low frequency tapping — rhythmic"},
    {"Leaf Folder Moth",    3200.0f, 400.0f,  22.0f, 0.6f, "Rapid fluttering — high pulse rate"},
    {"Cricket (Harmless)",  5000.0f, 500.0f,  30.0f, 0.8f, "Musical chirping — regular pattern"},
    {"Aphid Colony",        1200.0f, 200.0f,  50.0f, 0.3f, "Very fine ticking — dense population"},
    {"Grasshopper",         3500.0f, 600.0f,  5.0f,  0.6f, "Sporadic scraping — irregular"},
    {nullptr, 0, 0, 0, 0, nullptr}
};

// ================================================================
// SVAR-SAMSKARA ENGINE
// ================================================================
struct SvarSamskaraEngine {
    SvarSignature known_signatures[32];
    int signature_count;
    
    SvarSamskaraEngine() : signature_count(0) {
        memset(known_signatures, 0, sizeof(known_signatures));
        
        // Seed with known pest sound templates
        for (int i = 0; PEST_SOUNDS[i].pest_name != nullptr; i++) {
            learn_pest_template(&PEST_SOUNDS[i]);
        }
    }
    
    // Convert pest sound template to fingerprint
    void learn_pest_template(const PestSound* pest) {
        if (signature_count >= 32) return;
        
        SvarSignature* sig = &known_signatures[signature_count];
        strncpy(sig->label, pest->pest_name, 31);
        
        // Generate fingerprint from pest characteristics
        float base = pest->base_frequency / 5000.0f;  // Normalize to 0-1
        float range = pest->frequency_range / 1000.0f;
        float pulse = pest->pulse_rate / 50.0f;
        float amp = pest->amplitude_peak;
        
        for (int i = 0; i < SVAR_FINGERPRINT_SIZE; i++) {
            // Each fingerprint element = harmonic of base frequency
            float harmonic = base * (1.0f + i * 0.1f);
            float modulation = sinf(i * pulse * PHI) * range;
            sig->fingerprint[i] = harmonic + modulation * amp;
            // Clamp
            if (sig->fingerprint[i] < 0) sig->fingerprint[i] = 0;
            if (sig->fingerprint[i] > 1.0f) sig->fingerprint[i] = 1.0f;
        }
        
        sig->confidence = 1.0f;
        sig->samples_learned = 100;
        signature_count++;
    }
    
    // Compute fingerprint from raw audio samples
    void compute_fingerprint(float* audio_samples, int len, float* fingerprint_out) {
        // Simple frequency analysis via zero-crossing and energy
        int zero_crossings = 0;
        float energy = 0;
        float peak = 0;
        
        for (int i = 1; i < len; i++) {
            if ((audio_samples[i-1] < 0 && audio_samples[i] >= 0) ||
                (audio_samples[i-1] >= 0 && audio_samples[i] < 0)) {
                zero_crossings++;
            }
            energy += audio_samples[i] * audio_samples[i];
            if (fabsf(audio_samples[i]) > peak) peak = fabsf(audio_samples[i]);
        }
        
        float avg_energy = energy / len;
        float freq_estimate = (float)zero_crossings / len * 5000.0f;
        
        // Generate fingerprint
        for (int i = 0; i < SVAR_FINGERPRINT_SIZE; i++) {
            fingerprint_out[i] = freq_estimate / 5000.0f * (1.0f + sinf(i * PHI) * 0.1f);
            fingerprint_out[i] += avg_energy * 0.5f * sinf(i * 0.5f);
            if (fingerprint_out[i] < 0) fingerprint_out[i] = 0;
            if (fingerprint_out[i] > 1.0f) fingerprint_out[i] = 1.0f;
        }
    }
    
    // Match a fingerprint against known signatures
    const char* identify_sound(float* fingerprint) {
        float best_match = 0;
        int best_idx = -1;
        
        for (int i = 0; i < signature_count; i++) {
            float similarity = 0;
            for (int j = 0; j < SVAR_FINGERPRINT_SIZE; j++) {
                similarity += 1.0f - fabsf(fingerprint[j] - known_signatures[i].fingerprint[j]);
            }
            similarity /= SVAR_FINGERPRINT_SIZE;
            
            if (similarity > best_match) {
                best_match = similarity;
                best_idx = i;
            }
        }
        
        if (best_match > 0.7f && best_idx >= 0) {
            known_signatures[best_idx].samples_learned++;
            return known_signatures[best_idx].label;
        }
        return "Unknown sound";
    }
    
    // ================================================================
    // FARMER VOICE AUTHENTICATION
    // ================================================================
    struct VoicePrint {
        float samskara[SVAR_FINGERPRINT_SIZE];
        char farmer_name[32];
        float voice_age;  // Tracks vocal changes over time
        int login_count;
    };
    
    VoicePrint registered_farmers[16];
    int farmer_count = 0;
    
    // Register a farmer's voice
    void register_farmer(const char* name, float* voice_samples, int len) {
        if (farmer_count >= 16) return;
        
        VoicePrint* vp = &registered_farmers[farmer_count];
        strncpy(vp->farmer_name, name, 31);
        compute_fingerprint(voice_samples, len, vp->samskara);
        vp->voice_age = 0;
        vp->login_count = 0;
        farmer_count++;
        
        printf("  🎤 VOICE REGISTERED: %s\n", name);
    }
    
    // Authenticate a farmer by voice
    const char* authenticate_farmer(float* voice_samples, int len) {
        float test_fingerprint[SVAR_FINGERPRINT_SIZE];
        compute_fingerprint(voice_samples, len, test_fingerprint);
        
        float best_match = 0;
        int best_idx = -1;
        
        for (int i = 0; i < farmer_count; i++) {
            float similarity = 0;
            for (int j = 0; j < SVAR_FINGERPRINT_SIZE; j++) {
                similarity += 1.0f - fabsf(test_fingerprint[j] - registered_farmers[i].samskara[j]);
            }
            similarity /= SVAR_FINGERPRINT_SIZE;
            
            if (similarity > best_match) {
                best_match = similarity;
                best_idx = i;
            }
        }
        
        if (best_match > 0.8f && best_idx >= 0) {
            registered_farmers[best_idx].login_count++;
            registered_farmers[best_idx].voice_age += 0.01f;
            
            // Update samskara slightly (voice ages)
            for (int j = 0; j < SVAR_FINGERPRINT_SIZE; j++) {
                registered_farmers[best_idx].samskara[j] = 
                    registered_farmers[best_idx].samskara[j] * 0.99f + test_fingerprint[j] * 0.01f;
            }
            
            return registered_farmers[best_idx].farmer_name;
        }
        
        return nullptr;  // Authentication failed
    }
    
    // ================================================================
    // SENSOR SPOOF DETECTION
    // ================================================================
    bool verify_sensor_data(float* data_stream, int len, float* expected_signature) {
        // Compute fingerprint of incoming data
        float data_fingerprint[SVAR_FINGERPRINT_SIZE];
        compute_fingerprint(data_stream, len, data_fingerprint);
        
        // Compare with expected signature
        float similarity = 0;
        for (int i = 0; i < SVAR_FINGERPRINT_SIZE; i++) {
            similarity += 1.0f - fabsf(data_fingerprint[i] - expected_signature[i]);
        }
        similarity /= SVAR_FINGERPRINT_SIZE;
        
        // If similarity drops below threshold, data may be spoofed
        return similarity > 0.75f;
    }
    
    void stats() {
        printf("  🎵 SVAR-SAMSKARA: Pattern Authentication Engine\n");
        printf("     Known sounds: %d (Stem Borer, Hispa, Gundhi Bug...)\n", signature_count);
        printf("     Registered farmers: %d\n", farmer_count);
        printf("     Spoof detection: ACTIVE (threshold 0.75)\n");
    }
};
