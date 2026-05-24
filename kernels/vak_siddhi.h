/*
 * SUTRA 73: VAK-SIDDHI — The Dharamic Voice / Sacred Speech
 * 
 * Vak = Goddess of Speech. Words must be:
 *   Satya (Truthful), Priya (Pleasant), Hita (Beneficial)
 * 
 * Translates raw alerts into culturally resonant messages
 * for the farmers of Barak Valley in their own language.
 */

#pragma once
#include <cstdio>
#include <cstring>
#include <cmath>

#define PHI 1.618033988749895f

struct VakSiddhi {
    struct Message {
        char greeting[64];
        char body[256];
        char action[128];
        char blessing[64];
        char full_message[512];
        int urgency;
    };
    
    // Templates for different alert types in Assamese/Bengali resonance
    static const char* grease(const char* village) {
        const char* greetings[] = {
            "Greetings, Guardian of the Soil.",
            "Blessed morning, Children of the Earth.",
            "The Om resonates through the valley.",
            "Mother Earth speaks through this message."
        };
        return greetings[((int)(PHI*100))%4];
    }
    
    static const char* bless(const char* village) {
        const char* blessings[] = {
            "May the harvest be abundant. May the soil remain pure.",
            "Dharma protects those who protect the Earth.",
            "The Sun and Rain witness your devotion.",
            "Om Shanti. Peace to your fields."
        };
        return blessings[((int)(PHI*200))%4];
    }
    
    Message craft_pest_alert(const char* pest, const char* crop, float severity, 
                             const char* remedy, const char* timing, const char* village) {
        Message msg;
        strncpy(msg.greeting, grease(village), 63);
        strncpy(msg.blessing, bless(village), 63);
        
        const char* urgency_word = (severity > 0.8f) ? "CRITICAL" : 
                                    (severity > 0.5f) ? "urgent" : "advisory";
        
        snprintf(msg.body, 256,
                "The Guardian has sensed %s in your %s fields. "
                "The vibrations indicate %.0f%% severity. This is an %s message.",
                pest, crop, severity * 100, urgency_word);
        
        snprintf(msg.action, 128, "Please apply %s. Best time: %s.", remedy, timing);
        
        snprintf(msg.full_message, 512, "%s\n\n%s\n\n%s\n\n%s",
                msg.greeting, msg.body, msg.action, msg.blessing);
        msg.urgency = (int)(severity * 10);
        
        return msg;
    }
    
    Message craft_rain_alert(float probability, float intensity_mm, const char* action) {
        Message msg;
        strncpy(msg.greeting, "Beloved stewards of the land,", 63);
        strncpy(msg.blessing, "May every drop nourish the roots of tomorrow.", 63);
        
        snprintf(msg.body, 256,
                "The Rta-Rain reveals: %.0f%% chance of %.0fmm rainfall. %s",
                probability*100, intensity_mm,
                probability > 0.7f ? "Prepare your fields for the blessing of water." :
                "Keep your irrigation ready.");
        
        strncpy(msg.action, action, 127);
        snprintf(msg.full_message, 512, "%s\n\n%s\n\n%s\n\n%s",
                msg.greeting, msg.body, msg.action, msg.blessing);
        msg.urgency = probability > 0.7f ? 6 : 3;
        return msg;
    }
    
    Message craft_market_advice(const char* crop, float price, const char* trend) {
        Message msg;
        strncpy(msg.greeting, "Wise cultivators,", 63);
        
        snprintf(msg.body, 256,
                "The Mandi speaks: %s is at ₹%.0f/quintal. The trend is %s. "
                "Hold your harvest with confidence. The collective has power.",
                crop, price, trend);
        
        strncpy(msg.action, "Stand together. Negotiate as one village.", 127);
        strncpy(msg.blessing, "Unity is prosperity. Unity is Dharma.", 63);
        snprintf(msg.full_message, 512, "%s\n\n%s\n\n%s\n\n%s",
                msg.greeting, msg.body, msg.action, msg.blessing);
        msg.urgency = 2;
        return msg;
    }
    
    Message craft_healing_notice(const char* node, const char* issue, const char* status) {
        Message msg;
        strncpy(msg.greeting, "Guardians of the Grid,", 63);
        snprintf(msg.body, 256,
                "Node %s reported %s. Dhanvantara has dispatched healing. Status: %s.",
                node, issue, status);
        strncpy(msg.action, "No action needed. The network heals itself.", 127);
        strncpy(msg.blessing, "The whole is stronger than the part.", 63);
        snprintf(msg.full_message, 512, "%s\n\n%s\n\n%s\n\n%s",
                msg.greeting, msg.body, msg.action, msg.blessing);
        msg.urgency = 1;
        return msg;
    }
};
