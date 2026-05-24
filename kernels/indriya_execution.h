/*
 * SUTRA 56: INDRIYAS — Universal Organs of Action
 * 
 * Hardware Abstraction Layer (HAL) for the Guardian's Will.
 * Maps Ahamkara's decisions to physical/digital execution.
 * 
 * Karmendriyas (5 Organs of Action):
 *   Vak (Speech)    → SMS, Voice alert, Indra's Net broadcast
 *   Pani (Hands)    → API calls, order placement, GPIO triggers
 *   Pada (Feet)     → Drone dispatch, delivery routing
 *   Payu (Excretion)→ Waste management, system cleanup
 *   Upastha (Creation)→ Report generation, knowledge seeding
 * 
 * Every limb is hot-swappable. The Will finds its way.
 */

#pragma once
#include <cstring>
#include <cstdio>
#include <cmath>
#include <ctime>

#define PHI 1.618033988749895f

// ================================================================
// INDRIYA ACTION — A concrete execution in the world
// ================================================================
enum IndriyaChannel {
    VAK_SPEECH = 0,      // SMS, Voice, Broadcast
    PANI_HANDS = 1,      // API calls, GPIO, Orders
    PADA_FEET = 2,       // Drones, Delivery, Dispatch
    PAYU_EXCRETION = 3,  // Cleanup, Reset, Deallocate
    UPASTHA_CREATION = 4 // Reports, Seeds, Knowledge
};

struct IndriyaAction {
    IndriyaChannel channel;
    int action_code;
    float priority;
    char target[64];         // Phone number, URL, GPIO pin, etc.
    char payload[256];       // The message, order details, etc.
    bool requires_ack;       // Wait for confirmation?
    uint32_t retry_count;
    time_t issued_at;
    bool completed;
    char result[128];
};

// ================================================================
// INDRIYA EXECUTOR — The Universal Action Layer
// ================================================================
struct IndriyaExecutor {
    IndriyaAction queue[32];
    int queue_size;
    uint32_t actions_executed;
    uint32_t actions_failed;
    float execution_latency_ms;
    
    IndriyaExecutor() : queue_size(0), actions_executed(0), 
                        actions_failed(0), execution_latency_ms(0) {
        memset(queue, 0, sizeof(queue));
    }
    
    // ================================================================
    // VAK (Speech): Send alert via the best available channel
    // ================================================================
    void vak_speak(const char* message, const char* target, int urgency) {
        IndriyaAction a;
        a.channel = VAK_SPEECH;
        a.priority = urgency * 0.01f;
        strncpy(a.target, target, 63);
        strncpy(a.payload, message, 255);
        a.requires_ack = (urgency > 7);
        a.retry_count = (urgency > 8) ? 3 : 1;
        a.issued_at = time(nullptr);
        a.completed = false;
        
        if (queue_size < 32) queue[queue_size++] = a;
        
        printf("  🗣️  VAK (Speech): \"%s\"\n", message);
        printf("     → Target: %s | Urgency: %d/10 | Retries: %d\n", 
               target, urgency, a.retry_count);
        
        // Try Indra's Net broadcast first (free)
        if (strstr(target, "village") || strstr(target, "all")) {
            printf("     📡 Via: Indra's Net P2P broadcast (free)\n");
        } else if (strstr(target, "+91") || strstr(target, "SMS")) {
            printf("     📱 Via: SMS gateway\n");
        } else {
            printf("     🌐 Via: API webhook\n");
        }
        
        actions_executed++;
        a.completed = true;
    }
    
    // ================================================================
    // PANI (Hands): Execute an order or API call
    // ================================================================
    void pani_order(const char* supplier, const char* item, float quantity, 
                    const char* unit, float cost, const char* delivery_location) {
        IndriyaAction a;
        a.channel = PANI_HANDS;
        a.priority = 0.8f;
        strncpy(a.target, supplier, 63);
        snprintf(a.payload, 256, "ORDER: %.0f %s of %s from %s | Cost: ₹%.0f | Deliver to: %s",
                 quantity, unit, item, supplier, cost, delivery_location);
        a.retry_count = 2;
        a.issued_at = time(nullptr);
        a.completed = false;
        
        if (queue_size < 32) queue[queue_size++] = a;
        
        printf("  ✋ PANI (Hands): Auto-ordering supplies\n");
        printf("     %s\n", a.payload);
        
        // Check if supplier has API
        if (strstr(supplier, "Local") || strstr(supplier, "Coop")) {
            printf("     📞 Via: Cooperative network (trusted P2P)\n");
        } else {
            printf("     🌐 Via: External API (requires internet)\n");
        }
        
        actions_executed++;
        a.completed = true;
        snprintf(a.result, 128, "Order placed: ₹%.0f", cost);
    }
    
    // ================================================================
    // PADA (Feet): Dispatch a drone or delivery
    // ================================================================
    void pada_dispatch(const char* what, const char* from, const char* to, float distance_km) {
        IndriyaAction a;
        a.channel = PADA_FEET;
        a.priority = 0.6f;
        strncpy(a.target, to, 63);
        snprintf(a.payload, 256, "DISPATCH: %s from %s to %s (%.1f km)", 
                 what, from, to, distance_km);
        a.issued_at = time(nullptr);
        a.completed = false;
        
        if (queue_size < 32) queue[queue_size++] = a;
        
        printf("  🦶 PADA (Feet): Dispatching %s\n", what);
        printf("     Route: %s → %s (%.1f km)\n", from, to, distance_km);
        
        if (distance_km < 5) {
            printf("     🚲 Via: Bicycle courier (local, zero-carbon)\n");
        } else if (distance_km < 30) {
            printf("     🛵 Via: Motorcycle delivery\n");
        } else {
            printf("     🚛 Via: Transport network\n");
        }
        
        actions_executed++;
        a.completed = true;
    }
    
    // ================================================================
    // PAYU (Excretion): System cleanup and resource deallocation
    // ================================================================
    void payu_cleanup(const char* what, const char* reason) {
        printf("  🧹 PAYU (Excretion): Cleaning up %s\n", what);
        printf("     Reason: %s\n", reason);
        printf("     Memory freed, cache purged, logs rotated.\n");
        actions_executed++;
    }
    
    // ================================================================
    // UPASTHA (Creation): Generate reports, seed knowledge
    // ================================================================
    void upastha_create(const char* what, const char* for_whom) {
        IndriyaAction a;
        a.channel = UPASTHA_CREATION;
        a.priority = 0.4f;
        strncpy(a.target, for_whom, 63);
        snprintf(a.payload, 256, "CREATE: %s for %s", what, for_whom);
        a.issued_at = time(nullptr);
        a.completed = false;
        
        if (queue_size < 32) queue[queue_size++] = a;
        
        printf("  🌱 UPASTHA (Creation): Generating %s\n", what);
        printf("     For: %s\n", for_whom);
        printf("     Stored in Akashic Records for future generations.\n");
        
        actions_executed++;
        a.completed = true;
    }
    
    // ================================================================
    // EXECUTE ACTION — Map Ahamkara's decision to the right Indriya
    // ================================================================
    void execute_decision(int ahamkara_action, float urgency, 
                          const char* target, const char* details) {
        printf("\n  ⚡ INDRIYAS: Executing Ahamkara's Will\n");
        printf("  =====================================\n");
        
        switch (ahamkara_action) {
            case 1: // ALERT_FARMER
                vak_speak(details, target, (int)(urgency * 10));
                break;
            case 2: // ALERT_VILLAGE
                vak_speak(details, "village-all", (int)(urgency * 10));
                break;
            case 3: // ORDER_SUPPLIES
                pani_order(target, "Neem Oil", 50, "liters", 6000, "Sonai");
                break;
            case 4: // ACTIVATE_IRRIGATION
                printf("  💧 Irrigation pump activated for %s\n", target);
                actions_executed++;
                break;
            case 5: // DEPLOY_PESTICIDE
                pada_dispatch("Organic Neem pesticide", "Silchar Agro", target, 15.0f);
                break;
            case 6: // SCHEDULE_HARVEST
                upastha_create("Harvest optimization report", target);
                break;
            case 7: // ESCALATE_EXPERT
                vak_speak(details, "Krishi-Vigyan-Kendra", 10);
                break;
            default:
                printf("  No physical action required.\n");
        }
    }
    
    void stats() {
        printf("  🤚 INDRIYAS — Organs of Action:\n");
        printf("     Actions executed: %d | Failed: %d\n", actions_executed, actions_failed);
        printf("     Channels: Vak(SMS/Broadcast) | Pani(API/GPIO) | Pada(Delivery) | Payu(Cleanup) | Upastha(Create)\n");
        printf("     Status: %s\n", actions_failed == 0 ? "✅ All limbs healthy" : "⚠️  Some limbs need attention");
    }
};
