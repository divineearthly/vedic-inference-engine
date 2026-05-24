/*
 * SUTRA 9: INDRA-NET MESH — Zero-Config P2P Mesh Protocol
 * 
 * Every jewel reflects every other jewel. No cloud. No ISP.
 * Uses WiFi Direct + Bluetooth + LoRa simulation for ad-hoc routing.
 * 
 * If a farmer in Sonai detects a pest, the alert HOPS from phone to phone
 * until the entire Barak Valley knows. Even if mobile networks are down.
 */

#pragma once
#include "om_kernel.h"
#include <cstring>
#include <cstdio>
#include <cmath>
#include <cstdint>

#define PHI 1.618033988749895f
#define MESH_MAX_HOPS 8
#define MESH_MAX_PEERS 16
#define MESH_TTL 32  // Time-to-live for gossip packets

// ================================================================
// MESH PACKET — A message hopping through the valley
// ================================================================
struct MeshPacket {
    uint64_t packet_id;
    uint64_t origin_node;       // Who sent it first
    uint64_t om_timestamp;      // OM-network synchronized time
    int priority;               // 0-10: urgency
    int ttl;                    // Hops remaining
    int hop_count;              // How far it's traveled
    char alert_type[32];        // "PEST", "RAIN", "MARKET", "DHARMA"
    char payload[256];          // The message
    uint64_t relay_path[MESH_MAX_HOPS];  // Trace of nodes it passed through
};

// ================================================================
// MESH PEER — A neighboring device
// ================================================================
struct MeshPeer {
    uint64_t node_id;
    char device_name[32];
    float signal_strength;      // 0-1
    int hop_distance;           // How many hops away
    uint64_t last_seen_om_pulse;
    uint32_t packets_relayed;
    bool active;
    enum { WIFI_DIRECT, BLUETOOTH, LORA } link_type;
};

// ================================================================
// INDRA MESH — The Autonomous P2P Grid
// ================================================================
struct IndraMesh {
    MeshPeer peers[MESH_MAX_PEERS];
    int peer_count;
    OmKernel* om;               // Sync heartbeat
    
    // Gossip routing table
    uint64_t seen_packets[256];  // Prevent loops
    int seen_count;
    
    uint32_t packets_sent;
    uint32_t packets_relayed;
    uint32_t packets_received;
    
    IndraMesh() : peer_count(0), om(nullptr), seen_count(0),
        packets_sent(0), packets_relayed(0), packets_received(0) {
        memset(peers, 0, sizeof(peers));
        memset(seen_packets, 0, sizeof(seen_packets));
    }
    
    void attach_om(OmKernel* o) { om = o; }
    
    // ================================================================
    // DISCOVER: Scan for nearby VedaRta devices
    // ================================================================
    void discover_peers() {
        printf("  🔍 INDRA-MESH: Scanning for nearby Guardians...\n");
        
        // In production: actual WiFi Direct / Bluetooth scan
        // Here: simulated discovery for the mesh protocol
        
        if (peer_count < MESH_MAX_PEERS) {
            MeshPeer* p = &peers[peer_count];
            p->node_id = 0xBA2AC001ULL + peer_count;
            snprintf(p->device_name, 32, "Guardian-%d", peer_count + 1);
            p->signal_strength = 0.5f + (rand() % 50) / 100.0f;
            p->hop_distance = 1;
            p->active = true;
            p->link_type = MeshPeer::WIFI_DIRECT;
            peer_count++;
        }
        
        printf("  ✅ Found %d peer(s). Mesh active.\n", peer_count);
    }
    
    // ================================================================
    // SEND: Broadcast a packet to all peers (gossip protocol)
    // ================================================================
    MeshPacket create_packet(const char* alert_type, const char* message, int priority) {
        MeshPacket pkt;
        pkt.packet_id = (uint64_t)time(nullptr) * 1000000 + (packets_sent++);
        pkt.origin_node = 0xD15EED001ULL;  // This node's ID
        pkt.om_timestamp = om ? om->timestamp_event() : time(nullptr) * 1000000ULL;
        pkt.priority = priority;
        pkt.ttl = MESH_TTL;
        pkt.hop_count = 0;
        strncpy(pkt.alert_type, alert_type, 31);
        strncpy(pkt.payload, message, 255);
        memset(pkt.relay_path, 0, sizeof(pkt.relay_path));
        pkt.relay_path[0] = pkt.origin_node;
        
        printf("  📡 INDRA-MESH: Broadcasting \"%s\" (priority %d, TTL %d)\n", 
               alert_type, priority, MESH_TTL);
        return pkt;
    }
    
    // ================================================================
    // RELAY: Forward a packet to all peers except the sender
    // ================================================================
    bool relay_packet(MeshPacket* pkt, uint64_t from_node) {
        // Check if we've already seen this packet (loop prevention)
        for (int i = 0; i < seen_count; i++) {
            if (seen_packets[i] == pkt->packet_id) return false;
        }
        
        // Record that we've seen it
        if (seen_count < 256) seen_packets[seen_count++] = pkt->packet_id;
        
        // Decrement TTL
        if (pkt->ttl <= 0) return false;
        pkt->ttl--;
        pkt->hop_count++;
        
        // Add ourselves to relay path
        if (pkt->hop_count < MESH_MAX_HOPS) {
            pkt->relay_path[pkt->hop_count] = 0xD15EED001ULL;
        }
        
        packets_relayed++;
        
        // Forward to all active peers
        int forwarded = 0;
        for (int i = 0; i < peer_count; i++) {
            if (peers[i].active && peers[i].node_id != from_node) {
                peers[i].packets_relayed++;
                forwarded++;
            }
        }
        
        if (forwarded > 0) {
            printf("  🔄 INDRA-MESH: Relayed packet #%llu to %d peers (hop %d, TTL %d)\n",
                   (unsigned long long)pkt->packet_id, forwarded, pkt->hop_count, pkt->ttl);
        }
        
        return forwarded > 0;
    }
    
    // ================================================================
    // RECEIVE: Process an incoming packet
    // ================================================================
    void receive_packet(MeshPacket* pkt) {
        packets_received++;
        
        printf("  📥 INDRA-MESH: Received \"%s\" from node 0x%016llX (hops: %d)\n",
               pkt->alert_type, (unsigned long long)pkt->origin_node, pkt->hop_count);
        printf("     Message: %s\n", pkt->payload);
        
        // Auto-relay high-priority alerts
        if (pkt->priority >= 7 && pkt->ttl > 0) {
            relay_packet(pkt, pkt->origin_node);
        }
    }
    
    // ================================================================
    // GOSSIP: Spread a message through the entire mesh
    // ================================================================
    void gossip(const char* alert_type, const char* message, int priority) {
        MeshPacket pkt = create_packet(alert_type, message, priority);
        
        // Send to all direct peers
        for (int i = 0; i < peer_count; i++) {
            if (peers[i].active) {
                peers[i].packets_relayed++;
            }
        }
        
        // Simulate relay hops
        MeshPacket relay_pkt = pkt;
        for (int hop = 1; hop < 4 && relay_pkt.ttl > 0; hop++) {
            relay_packet(&relay_pkt, pkt.origin_node);
        }
        
        printf("  🌐 INDRA-MESH: Gossip complete. Estimated reach: %d nodes.\n", 
               peer_count * 3);
    }
    
    void report() {
        printf("\n🌐 INDRA-MESH — P2P Mesh Network\n");
        printf("  ===============================\n");
        printf("  Peers: %d active\n", peer_count);
        printf("  Packets: %d sent | %d relayed | %d received\n", 
               packets_sent, packets_relayed, packets_received);
        printf("  Seen cache: %d unique packets\n", seen_count);
        
        for (int i = 0; i < peer_count; i++) {
            printf("  Peer %d: %s | Signal: %.0f%% | Hops: %d | Relayed: %d\n",
                   i, peers[i].device_name, peers[i].signal_strength * 100,
                   peers[i].hop_distance, peers[i].packets_relayed);
        }
        
        printf("  🕉️  Off-Grid: No internet. No cloud. No ISP. Just the valley.\n");
    }
};
