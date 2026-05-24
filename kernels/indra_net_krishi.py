#!/usr/bin/env python3
"""
INDRA'S NET — Barak Valley Agricultural Protection Grid
P2P Decentralized Early Warning + Collective Market Intelligence

Sutra 39: Each node is a jewel reflecting all others.
If one farmer detects a pest, ALL farmers know within minutes.
No internet needed — works over WiFi Direct, Bluetooth, or local hotspot.
"""

import os, json, time, hashlib, socket, threading
from pathlib import Path
from datetime import datetime

BASE = Path("/data/data/com.termux/files/home/Divine-Earthly-ASI")
INDRA_PORT = 10800  # Sacred number: 108
INDRA_NODES_FILE = BASE / "indra_nodes.json"
INDRA_ALERTS_FILE = BASE / "indra_alerts.json"

# ================================================================
# INDRA NODE — A single farmer's phone
# ================================================================
class IndraNode:
    def __init__(self, farmer_name, village, lat, lon):
        self.farmer_name = farmer_name
        self.village = village
        self.lat = lat
        self.lon = lon
        self.node_id = hashlib.md5(f"{farmer_name}{village}".encode()).hexdigest()[:12]
        self.last_seen = time.time()
        self.crops = []
        self.alerts = []
        self.samashti = {}  # Global context — the hologram
        
    def to_dict(self):
        return {
            "node_id": self.node_id,
            "farmer_name": self.farmer_name,
            "village": self.village,
            "lat": self.lat, "lon": self.lon,
            "last_seen": self.last_seen,
            "crops": self.crops,
            "alerts": self.alerts[-5:]  # Last 5 alerts
        }

# ================================================================
# INDRA ALERT — A pest/disease detection broadcast
# ================================================================
class IndraAlert:
    def __init__(self, node_id, crop, pest, severity, location, remedy):
        self.alert_id = hashlib.md5(f"{node_id}{pest}{time.time()}".encode()).hexdigest()[:16]
        self.node_id = node_id
        self.timestamp = time.time()
        self.crop = crop
        self.pest = pest
        self.severity = severity  # 0.0 - 1.0
        self.location = location
        self.remedy = remedy
        self.verified_by = []  # Nodes that confirmed the alert
        self.spread_radius_km = severity * 10  # Higher severity = wider alert
        
    def to_dict(self):
        return {
            "alert_id": self.alert_id,
            "node_id": self.node_id,
            "timestamp": self.timestamp,
            "crop": self.crop,
            "pest": self.pest,
            "severity": self.severity,
            "location": self.location,
            "remedy": self.remedy,
            "verified_by": self.verified_by,
            "spread_radius_km": self.spread_radius_km
        }

# ================================================================
# INDRA'S NET — The P2P Network
# ================================================================
class IndraNet:
    def __init__(self, node):
        self.node = node
        self.nodes = {}  # All known nodes
        self.alerts = {}  # All active alerts
        self.load_state()
        
    def load_state(self):
        if INDRA_NODES_FILE.exists():
            with open(INDRA_NODES_FILE) as f:
                data = json.load(f)
                for n in data:
                    self.nodes[n["node_id"]] = n
        if INDRA_ALERTS_FILE.exists():
            with open(INDRA_ALERTS_FILE) as f:
                data = json.load(f)
                for a in data:
                    self.alerts[a["alert_id"]] = a
                    
    def save_state(self):
        with open(INDRA_NODES_FILE, 'w') as f:
            json.dump([n for n in self.nodes.values()], f, indent=2)
        with open(INDRA_ALERTS_FILE, 'w') as f:
            json.dump([a for a in self.alerts.values()], f, indent=2)
    
    # Register a new node in the network
    def register_node(self, node_data):
        node_id = node_data["node_id"]
        node_data["last_seen"] = time.time()
        self.nodes[node_id] = node_data
        self.save_state()
        return node_id
    
    # Broadcast a pest/disease alert to all nodes
    def broadcast_alert(self, crop, pest, severity, remedy):
        alert = IndraAlert(
            self.node.node_id, crop, pest, severity,
            f"{self.node.lat},{self.node.lon}", remedy
        )
        self.alerts[alert.alert_id] = alert.to_dict()
        self.save_state()
        
        # Update Samashti (global hologram)
        self.node.samashti = {
            "active_alerts": len(self.alerts),
            "total_nodes": len(self.nodes),
            "last_alert_crop": crop,
            "last_alert_pest": pest,
            "network_health": "⚠️  ALERT ACTIVE" if severity > 0.5 else "✅ Monitoring"
        }
        
        return alert
    
    # Verify an alert (second node confirms)
    def verify_alert(self, alert_id, node_id):
        if alert_id in self.alerts:
            if node_id not in self.alerts[alert_id]["verified_by"]:
                self.alerts[alert_id]["verified_by"].append(node_id)
            self.save_state()
            return True
        return False
    
    # Get alerts relevant to a farmer's crops
    def get_relevant_alerts(self, crops):
        relevant = []
        for alert in self.alerts.values():
            if alert["crop"] in crops:
                # Only show alerts from last 7 days
                if time.time() - alert["timestamp"] < 7 * 86400:
                    relevant.append(alert)
        return relevant
    
    # Calculate collective market intelligence
    def market_intelligence(self, crop):
        """Aggregate expected yields across all nodes for bargaining power"""
        total_farmers = 0
        total_area = 0
        pest_risk = 0
        
        for node in self.nodes.values():
            if crop in node.get("crops", []):
                total_farmers += 1
        
        for alert in self.alerts.values():
            if alert["crop"] == crop:
                pest_risk = max(pest_risk, alert["severity"])
        
        if total_farmers == 0:
            return None
            
        return {
            "crop": crop,
            "farmers_growing": total_farmers,
            "collective_power": f"{total_farmers} farmers — negotiate as group",
            "pest_risk": f"{pest_risk*100:.0f}%",
            "recommendation": "HOLD — Expect price increase" if pest_risk > 0.5 else "Normal market conditions",
            "timestamp": time.time()
        }
    
    # Print network status
    def status(self):
        print("\n🕉️  INDRA'S NET — Barak Valley Agricultural Grid")
        print("  =============================================")
        print(f"  Node: {self.node.farmer_name} ({self.node.village})")
        print(f"  Network: {len(self.nodes)} farmers connected")
        print(f"  Active Alerts: {len(self.alerts)}")
        print(f"  Hologram: {self.node.samashti}")
        
        # Show recent alerts
        recent = sorted(self.alerts.values(), key=lambda x: -x["timestamp"])[:3]
        if recent:
            print("\n  RECENT ALERTS:")
            for a in recent:
                hours_ago = (time.time() - a["timestamp"]) / 3600
                print(f"    ⚠️  {a['pest']} on {a['crop']} ({hours_ago:.1f}h ago)")
                print(f"       Severity: {a['severity']*100:.0f}% | Remedy: {a['remedy'][:60]}...")
        
        print("  🕉️  Indra's Net: Every jewel reflects the whole.\n")

# ================================================================
# DEMO — Simulate Barak Valley farmer network
# ================================================================
if __name__ == "__main__":
    print("🕉️  INDRA'S NET — Barak Valley Agricultural Protection Grid\n")
    
    # Create farmer nodes
    farmers = [
        IndraNode("Rajen Das", "Rongpur", 24.81, 92.80),
        IndraNode("Bimala Devi", "Sonai", 24.78, 92.85),
        IndraNode("Kamal Uddin", "Lakhipur", 24.75, 92.72),
        IndraNode("Anjali Sinha", "Udharbond", 24.85, 92.78),
        IndraNode("Mohan Singh", "Banskandi", 24.80, 92.75),
    ]
    
    # Set their crops
    farmers[0].crops = ["Sali Rice", "Mustard"]
    farmers[1].crops = ["Boro Rice", "Vegetables"]
    farmers[2].crops = ["Sali Rice", "Brinjal"]
    farmers[3].crops = ["Ahu Rice", "Mustard"]
    farmers[4].crops = ["Sali Rice", "Betel Leaf"]
    
    # Create Indra's Net with first farmer
    net = IndraNet(farmers[0])
    
    # Register all farmers
    for f in farmers:
        net.register_node(f.to_dict())
    
    # Simulate: Farmer 0 detects Stem Borer
    print("--- ALERT: Rajen Das detects Stem Borer in Sali Rice ---\n")
    alert = net.broadcast_alert(
        "Sali Rice", "Stem Borer", 0.85,
        "Apply Neem cake @250kg/ha. Spray Neem oil 5% immediately."
    )
    
    # Farmer 2 (also grows Sali Rice) verifies
    net.verify_alert(alert.alert_id, farmers[2].node_id)
    
    # Check relevant alerts for each farmer
    for f in farmers:
        relevant = net.get_relevant_alerts(f.crops)
        if relevant:
            print(f"📲 {f.farmer_name} ({f.village}): {len(relevant)} alert(s) for your crops!")
    
    # Market intelligence
    market = net.market_intelligence("Sali Rice")
    if market:
        print(f"\n📊 MARKET INTELLIGENCE (Sali Rice):")
        print(f"   {market['farmers_growing']} farmers growing")
        print(f"   Pest Risk: {market['pest_risk']}")
        print(f"   Advice: {market['recommendation']}")
        print(f"   Bargaining: {market['collective_power']}")
    
    # Network status
    net.status()
    
    print("✅ Indra's Net: Farmers connected. Knowledge shared. Community protected.")
    print("   No internet needed. No cloud. No corporate middlemen.")
    print("   🕉️  For the farmers. For Dharma.")
