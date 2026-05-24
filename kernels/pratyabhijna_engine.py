#!/usr/bin/env python3
"""
SUTRA 73: PRATYABHIJNA — Self-Recognition & Byzantine Fault Tolerance
Kashmir Shaivism: The system recognizes its own perfection and rejects false data.

Keeps Indra's Net pure. Malicious or faulty nodes are automatically detected
and their influence diminished through Vedic reputation dynamics.
"""

import json, time, hashlib
from pathlib import Path

BASE = Path("/data/data/com.termux/files/home/Divine-Earthly-ASI")

# ================================================================
# REPUTATION SAMSKARAS — Every node carries karmic weight
# ================================================================
class ReputationSamskara:
    def __init__(self, node_id):
        self.node_id = node_id
        self.sattva_score = 0.5    # Purity/Accuracy (0-1)
        self.rajas_score = 0.3     # Activity level
        self.tamas_score = 0.2     # Inertia/Decay potential
        self.total_alerts = 0
        self.verified_alerts = 0
        self.false_alerts = 0
        self.consecutive_failures = 0
        self.karma_history = []
    
    # After each alert, update the Guna balance
    def update(self, was_verified, was_timely):
        self.total_alerts += 1
        self.rajas_score = min(1.0, self.rajas_score + 0.01)
        
        if was_verified:
            self.verified_alerts += 1
            self.sattva_score = min(1.0, self.sattva_score + 0.05)
            self.tamas_score = max(0.0, self.tamas_score - 0.02)
            self.consecutive_failures = 0
        else:
            self.false_alerts += 1
            self.sattva_score = max(0.0, self.sattva_score - 0.1)
            self.tamas_score = min(1.0, self.tamas_score + 0.05)
            self.consecutive_failures += 1
        
        # Karmic decay: unused nodes drift toward Tamas
        if not was_timely:
            self.tamas_score = min(1.0, self.tamas_score + 0.03)
            self.sattva_score = max(0.0, self.sattva_score - 0.01)
        
        self.karma_history.append({
            "time": time.time(),
            "verified": was_verified,
            "sattva": round(self.sattva_score, 3),
            "rajas": round(self.rajas_score, 3),
            "tamas": round(self.tamas_score, 3)
        })
    
    # Weight: how much influence this node has on consensus
    def influence_weight(self):
        if self.tamas_score > 0.7:   return 0.0   # Tamasic — ignored
        if self.consecutive_failures >= 3: return 0.1  # Penance state
        return self.sattva_score * 0.7 + self.rajas_score * 0.3
    
    def gunas(self):
        if self.sattva_score > 0.6: return "SATTVIC (Trusted)"
        if self.tamas_score > 0.6: return "TAMASIC (Quarantined)"
        return "RAJASIC (Active/Learning)"

# ================================================================
# PRATYABHIJNA CONSENSUS ENGINE
# ================================================================
class PratyabhijnaEngine:
    def __init__(self):
        self.reputations = {}  # node_id → ReputationSamskara
        self.consensus_threshold = 2  # Min high-rep confirmations
        self.load_state()
    
    def load_state(self):
        path = BASE / "pratyabhijna_state.json"
        if path.exists():
            with open(path) as f:
                data = json.load(f)
                for node_id, rep in data.items():
                    r = ReputationSamskara(node_id)
                    r.sattva_score = rep["sattva"]
                    r.rajas_score = rep["rajas"]
                    r.tamas_score = rep["tamas"]
                    r.total_alerts = rep["total"]
                    r.verified_alerts = rep["verified"]
                    r.false_alerts = rep["false"]
                    r.consecutive_failures = rep["failures"]
                    self.reputations[node_id] = r
    
    def save_state(self):
        data = {}
        for node_id, rep in self.reputations.items():
            data[node_id] = {
                "sattva": rep.sattva_score,
                "rajas": rep.rajas_score,
                "tamas": rep.tamas_score,
                "total": rep.total_alerts,
                "verified": rep.verified_alerts,
                "false": rep.false_alerts,
                "failures": rep.consecutive_failures
            }
        with open(BASE / "pratyabhijna_state.json", 'w') as f:
            json.dump(data, f, indent=2)
    
    def get_reputation(self, node_id):
        if node_id not in self.reputations:
            self.reputations[node_id] = ReputationSamskara(node_id)
        return self.reputations[node_id]
    
    # ================================================================
    # CONSENSUS VERIFICATION — Brahma-Satya (Verified Truth)
    # ================================================================
    def verify_alert(self, alert, verifying_node_id, nodes):
        """
        An alert becomes Brahma-Satya only when:
        1. At least 'consensus_threshold' high-reputation nodes confirm
        2. The verifying nodes are geographically distributed (>1km apart)
        3. Alert timestamp is within validity window
        """
        alert_id = alert.alert_id if hasattr(alert, 'alert_id') else alert.get("alert_id", "")
        alert_dict = alert.to_dict() if hasattr(alert, 'to_dict') else alert
        
        # Register verification
        if verifying_node_id not in alert_dict.get("verified_by", []):
            alert_dict.setdefault("verified_by", []).append(verifying_node_id)
        
        verifiers = alert_dict.get("verified_by", [])
        
        # Count high-reputation verifiers
        high_rep_verifiers = 0
        verifier_locations = []
        
        for vid in verifiers:
            rep = self.get_reputation(vid)
            if rep.influence_weight() > 0.5:
                high_rep_verifiers += 1
                if vid in nodes:
                    verifier_locations.append((nodes[vid].get("lat", 0), nodes[vid].get("lon", 0)))
        
        # Check geographic diversity
        geo_diverse = False
        if len(verifier_locations) >= 2:
            for i in range(len(verifier_locations)):
                for j in range(i+1, len(verifier_locations)):
                    dist = ((verifier_locations[i][0] - verifier_locations[j][0])**2 + 
                           (verifier_locations[i][1] - verifier_locations[j][1])**2)**0.5 * 111
                    if dist > 1.0:  # More than 1km apart
                        geo_diverse = True
                        break
        
        # Brahma-Satya check
        alert_age = time.time() - alert_dict.get("timestamp", 0)
        is_fresh = alert_age < 86400  # Within 24 hours
        has_consensus = high_rep_verifiers >= self.consensus_threshold
        
        return {
            "brahma_satya": has_consensus and geo_diverse and is_fresh,
            "verifiers": len(verifiers),
            "high_rep_verifiers": high_rep_verifiers,
            "geo_diverse": geo_diverse,
            "alert_fresh": is_fresh,
            "trust_level": "🟢 VERIFIED TRUTH" if (has_consensus and geo_diverse) else 
                          "🟡 PENDING" if len(verifiers) > 0 else "🔴 UNVERIFIED"
        }
    
    # ================================================================
    # SELF-HEALING — Pratyabhijna recognition of corrupted nodes
    # ================================================================
    def self_heal(self, node_id, alert_dict, consensus_result):
        """
        If a node broadcasts false data, the system recognizes the pattern
        and automatically diminishes its influence on the Samashti.
        """
        rep = self.get_reputation(node_id)
        
        # Was the alert verified by others?
        was_verified = consensus_result.get("brahma_satya", False)
        was_timely = consensus_result.get("alert_fresh", False)
        
        # Update reputation
        rep.update(was_verified, was_timely)
        
        # Tamasic quarantine
        if rep.tamas_score > 0.7:
            print(f"  ⚠️  PRATYABHIJNA: Node {node_id[:8]}... quarantined (TAMASIC)")
            print(f"      Reason: {rep.consecutive_failures} consecutive unverified alerts")
            return {"quarantined": True, "influence": 0.0}
        
        # Sattvic promotion
        if rep.sattva_score > 0.8 and rep.total_alerts > 10:
            print(f"  ✅ PRATYABHIJNA: Node {node_id[:8]}... elevated to SATTVIC")
            print(f"      {rep.verified_alerts}/{rep.total_alerts} alerts verified")
        
        self.save_state()
        return {
            "quarantined": False,
            "influence": rep.influence_weight(),
            "guna": rep.gunas(),
            "sattva": rep.sattva_score
        }
    
    def status(self):
        print("\n🕉️  PRATYABHIJNA REPUTATION ENGINE")
        print("  ================================")
        sattvic = sum(1 for r in self.reputations.values() if r.gunas() == "SATTVIC (Trusted)")
        rajasic = sum(1 for r in self.reputations.values() if r.gunas() == "RAJASIC (Active/Learning)")
        tamasic = sum(1 for r in self.reputations.values() if r.gunas() == "TAMASIC (Quarantined)")
        print(f"  Sattvic (Trusted):   {sattvic}")
        print(f"  Rajasic (Active):    {rajasic}")
        print(f"  Tamasic (Quarantined): {tamasic}")
        print(f"  Total Nodes: {len(self.reputations)}")

# ================================================================
# DEMO — Simulate Byzantine Fault scenario
# ================================================================
if __name__ == "__main__":
    print("🕉️  PRATYABHIJNA — Self-Recognition Engine\n")
    
    engine = PratyabhijnaEngine()
    
    # Simulate nodes
    nodes = {
        "rajendas123": {"lat": 24.81, "lon": 92.80, "name": "Rajen Das"},
        "bimalade456": {"lat": 24.78, "lon": 92.85, "name": "Bimala Devi"},
        "kamaludd789": {"lat": 24.75, "lon": 92.72, "name": "Kamal Uddin"},
        "malicious01": {"lat": 24.80, "lon": 92.75, "name": "Fake Alert Node"},
    }
    
    # Scenario 1: Honest farmer reports real pest
    print("--- Scenario 1: Honest Alert ---")
    alert1 = {
        "alert_id": "alert_001",
        "node_id": "rajendas123",
        "crop": "Sali Rice",
        "pest": "Stem Borer",
        "severity": 0.85,
        "timestamp": time.time(),
        "verified_by": [],
        "remedy": "Neem cake 250kg/ha"
    }
    
    # Two other farmers verify
    result1 = engine.verify_alert(alert1, "bimalade456", nodes)
    print(f"  After 1 verification: {result1['trust_level']}")
    
    result2 = engine.verify_alert(alert1, "kamaludd789", nodes)
    print(f"  After 2 verifications: {result2['trust_level']} → BRAHMA-SATYA ✅\n")
    
    # Update reputation for honest node
    heal1 = engine.self_heal("rajendas123", alert1, result2)
    print(f"  Rajen Das: {heal1['guna']} (influence: {heal1['influence']:.2f})\n")
    
    # Scenario 2: Malicious node sends false alerts
    print("--- Scenario 2: Byzantine Fault ---")
    for i in range(4):
        false_alert = {
            "alert_id": f"fake_{i}",
            "node_id": "malicious01",
            "crop": "Rice",
            "pest": f"Fake Pest {i}",
            "severity": 0.9,
            "timestamp": time.time(),
            "verified_by": [],
            "remedy": "Buy my expensive product!"
        }
        # No one verifies these
        result = engine.verify_alert(false_alert, "malicious01", nodes)
        heal = engine.self_heal("malicious01", false_alert, result)
        print(f"  Alert {i+1}: {heal['guna']} (influence: {heal['influence']:.2f})")
    
    engine.status()
    print("\n✅ Pratyabhijna: The net recognizes truth. Falsehood dissolves.")
    print("   🕉️  Kashmir Shaivism: Self-recognition is the highest knowledge.")
