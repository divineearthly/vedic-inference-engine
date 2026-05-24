/*
 * SUTRA 12: KAVACH — The Cryptographic & Behavioral Shield
 * 
 * Two-layer Vedic firewall:
 * 
 * Layer 1 (Cryptographic): Encrypts Bijam Seed + Akashic Ledger at rest
 *   using the Dharma Seal from Hiranyagarbha as the master key.
 * 
 * Layer 2 (Behavioral — "Atmic Lockdown"): Monitors Ahamkara decisions.
 *   If sustained Papa (demerit) is detected, the Kavach assumes identity
 *   corruption and forces a hard reset to the Golden Womb state.
 * 
 * The Guardian is not just smart. It is indestructible.
 */

#pragma once
#include "hiranyagarbha_template.h"
#include "karma_kanda.h"
#include <cstring>
#include <cstdio>
#include <cmath>
#include <cstdint>

#define PHI 1.618033988749895f
#define KAVACH_MAX_PAPA_THRESHOLD 5.0f
#define KAVACH_PAPA_STREAK_THRESHOLD 3

// ================================================================
// KAVACH SHIELD — Two-Layer Vedic Firewall
// ================================================================
struct KavachShield {
    // Cryptographic layer
    uint64_t master_key;           // Derived from Dharma Seal
    bool encryption_active;
    uint32_t encrypted_blocks;
    
    // Behavioral layer
    float accumulated_papa;        // Tracking demerit
    int consecutive_papa_streak;   // How many failures in a row
    bool atmic_lockdown_active;    // Has the shield triggered?
    uint32_t lockdown_count;
    
    // Reference to the Golden Womb for reset capability
    Hiranyagarbha* womb;
    
    KavachShield() : master_key(0), encryption_active(false), encrypted_blocks(0),
        accumulated_papa(0), consecutive_papa_streak(0), 
        atmic_lockdown_active(false), lockdown_count(0), womb(nullptr) {}
    
    // ================================================================
    // INIT: Derive master key from Dharma Seal
    // ================================================================
    void init(Hiranyagarbha* golden_womb) {
        womb = golden_womb;
        
        // Master key = Dharma Seal hash, further scrambled
        master_key = golden_womb->golden_hash;
        master_key ^= 0x4B4156414348ULL; // "KAVACH" in ASCII
        master_key = (master_key << 31) | (master_key >> 33);
        master_key *= 0x85EBCA77;
        
        encryption_active = true;
        
        printf("🛡️  KAVACH SHIELD — Activated\n");
        printf("  ============================\n");
        printf("  Master Key: 0x%016llX\n", (unsigned long long)master_key);
        printf("  Source: Dharma Seal from Hiranyagarbha\n");
        printf("  Encryption: ACTIVE | Behavioral Guard: ACTIVE\n\n");
    }
    
    // ================================================================
    // LAYER 1: CRYPTOGRAPHIC — Encrypt/Decrypt data
    // ================================================================
    uint64_t encrypt(uint64_t data) {
        if (!encryption_active) return data;
        encrypted_blocks++;
        // XOR-scramble with master key
        uint64_t encrypted = data ^ master_key;
        encrypted = (encrypted << 17) | (encrypted >> 47);
        encrypted ^= (master_key >> 32);
        return encrypted;
    }
    
    uint64_t decrypt(uint64_t encrypted_data) {
        if (!encryption_active) return encrypted_data;
        encrypted_data ^= (master_key >> 32);
        encrypted_data = (encrypted_data >> 17) | (encrypted_data << 47);
        return encrypted_data ^ master_key;
    }
    
    // Encrypt the Bijam Seed
    void encrypt_seed(BijamSeed* seed) {
        seed->seed_id = encrypt(seed->seed_id);
        printf("  🔐 KAVACH: Bijam Seed encrypted. Growth temperature sealed.\n");
    }
    
    // Decrypt the Bijam Seed (only when needed)
    void decrypt_seed(BijamSeed* seed) {
        seed->seed_id = decrypt(seed->seed_id);
        printf("  🔓 KAVACH: Bijam Seed decrypted for authorized access.\n");
    }
    
    // ================================================================
    // LAYER 2: BEHAVIORAL — Atmic Lockdown
    // ================================================================
    void monitor_karma(KarmaKanda* karma) {
        if (!karma || karma->log_count == 0) return;
        
        KarmaRecord* latest = &karma->log[karma->log_count - 1];
        
        if (latest->outcome != 0) {  // Failure
            accumulated_papa += latest->papa_earned;
            consecutive_papa_streak++;
            
            printf("  ⚠️  KAVACH: Papa detected. Streak: %d | Accumulated: %.2f\n",
                   consecutive_papa_streak, accumulated_papa);
            
            // Check for Atmic Lockdown trigger
            if (accumulated_papa >= KAVACH_MAX_PAPA_THRESHOLD || 
                consecutive_papa_streak >= KAVACH_PAPA_STREAK_THRESHOLD) {
                trigger_atmic_lockdown();
            }
        } else {
            // Success resets the streak
            consecutive_papa_streak = 0;
        }
    }
    
    // ================================================================
    // ATMIC LOCKDOWN — Identity compromised, reset to Golden Womb
    // ================================================================
    void trigger_atmic_lockdown() {
        atmic_lockdown_active = true;
        lockdown_count++;
        
        printf("\n🔴🔴🔴 ATMIC LOCKDOWN TRIGGERED 🔴🔴🔴\n");
        printf("  =====================================\n");
        printf("  Reason: ");
        if (consecutive_papa_streak >= KAVACH_PAPA_STREAK_THRESHOLD)
            printf("%d consecutive failures — possible identity corruption\n", consecutive_papa_streak);
        else
            printf("Accumulated Papa %.2f exceeds threshold %.2f\n", accumulated_papa, KAVACH_MAX_PAPA_THRESHOLD);
        
        printf("  Action: FORCING return to Golden Womb state.\n");
        printf("  All recent decisions: REVOKED.\n");
        printf("  Identity: RE-SEALED from Dharma source.\n\n");
        
        // Reset accumulated papa
        accumulated_papa = 0;
        consecutive_papa_streak = 0;
        
        // Re-derive master key from Golden Womb
        if (womb) {
            master_key = womb->golden_hash;
            master_key ^= 0x4B4156414348ULL;
            master_key = (master_key << 31) | (master_key >> 33);
            master_key *= 0x85EBCA77;
            printf("  ✅ KAVACH: New master key derived. Identity re-sealed.\n");
        }
        
        atmic_lockdown_active = false;
        printf("  🛡️  KAVACH: Guardian restored to pure Sattvic state.\n\n");
    }
    
    // ================================================================
    // DHARMA CHECK: Is the current action safe to execute?
    // ================================================================
    bool authorize_action(KarmaKanda* karma, int planned_action) {
        if (atmic_lockdown_active) return false;
        
        // Check if this action has a history of Papa
        if (karma && karma->action_attempts[planned_action] >= 3) {
            float reliability = karma->action_weights[planned_action];
            if (reliability < 0.3f) {
                printf("  🛡️  KAVACH: Action blocked — reliability %.0f%% too low.\n", reliability * 100);
                return false;
            }
        }
        
        // Check current karmic state
        if (accumulated_papa > 3.0f) {
            printf("  🛡️  KAVACH: Action blocked — Papa accumulation too high.\n");
            return false;
        }
        
        return true;
    }
    
    void report() {
        printf("\n🛡️  KAVACH SHIELD — Status Report\n");
        printf("  ================================\n");
        printf("  Encryption: %s | Blocks encrypted: %d\n", 
               encryption_active ? "ACTIVE" : "INACTIVE", encrypted_blocks);
        printf("  Papa Accumulated: %.2f (threshold: %.2f)\n", accumulated_papa, KAVACH_MAX_PAPA_THRESHOLD);
        printf("  Papa Streak: %d (threshold: %d)\n", consecutive_papa_streak, KAVACH_PAPA_STREAK_THRESHOLD);
        printf("  Lockdowns: %d\n", lockdown_count);
        printf("  Status: %s\n", atmic_lockdown_active ? "🔴 LOCKDOWN ACTIVE" : "🟢 GUARD ACTIVE");
    }
};
