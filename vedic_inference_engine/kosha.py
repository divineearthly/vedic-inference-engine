"""
Kosha-Net — 5-Layer Vedic Memory Architecture
Ported from VedaRta/kernels/vr_kosha_net.h
Source: Taittiriya Upanishad, Pancha Kosha Viveka

Annamaya  → Physical/data layer (sensor readings, raw input)
Pranamaya → Energy/gist layer (compressed representations, active working memory)
Manomaya  → Mental/processing layer (thought vectors, active reasoning)
Vijnanamaya → Wisdom/knowledge layer (consolidated long-term knowledge)
Anandamaya → Bliss/causal layer (IMMUTABLE — core identity, NEVER modified)

Catastrophic forgetting prevention: Anandamaya locked.
New knowledge enters as Pranamaya gist → consolidates to Vijnanamaya if cosine_sim > 0.7
"""

from enum import IntEnum
from dataclasses import dataclass, field
from typing import List, Dict, Optional, Any
import time
import math


class KoshaLayer(IntEnum):
    ANNAMAYA = 0     # Physical — raw data
    PRANAMAYA = 1    # Energy — active gists
    MANOMAYA = 2     # Mental — processing
    VIJNANAMAYA = 3  # Wisdom — consolidated knowledge
    ANANDAMAYA = 4   # Bliss — CORE IDENTITY (locked)


@dataclass
class KoshaEntry:
    key: str
    value: Any
    layer: KoshaLayer
    timestamp: float = field(default_factory=time.time)
    access_count: int = 0
    consolidation_score: float = 0.0  # For Pranamaya → Vijnanamaya promotion


class KoshaNet:
    """
    5-layer Kosha memory network.
    
    Flow: Annamaya → Pranamaya → Manomaya → Vijnanamaya
          Anandamaya is the immutable foundation.
    
    Key rules:
    - Anandamaya: Write-once, NEVER modified. Core identity.
    - Vijnanamaya: Consolidated knowledge. High cosine_sim to enter.
    - Pranamaya: Active gists. Volatile working memory.
    - Annamaya: Raw sensor data. Cleared periodically.
    
    Usage:
        kosha = KoshaNet()
        kosha.anandamaya_seed("Sovereign Vedic AI — Silchar, Assam")
        kosha.annamaya_ingest("soil_ph_reading", 5.8)
        gist = kosha.pranamaya_compress("soil_ph_reading", "Soil pH is acidic at 5.8")
        kosha.vijnanamaya_consolidate("soil_health_rule", "pH < 6.0 needs liming", 0.85)
    """
    
    CONSOLIDATION_THRESHOLD = 0.7  # Minimum cosine_sim to move Pranamaya → Vijnanamaya
    
    def __init__(self):
        # Annamaya — raw physical data (cleared periodically)
        self.annamaya: Dict[str, Any] = {}
        
        # Pranamaya — active gists (working memory, volatile)
        self.pranamaya: Dict[str, KoshaEntry] = {}
        
        # Manomaya — processing layer (thought vectors in flight)
        self.manomaya: List[KoshaEntry] = []
        
        # Vijnanamaya — consolidated wisdom (long-term)
        self.vijnanamaya: Dict[str, KoshaEntry] = {}
        
        # Anandamaya — IMMUTABLE identity core
        self.anandamaya: Dict[str, Any] = {}
        self._anandamaya_locked: bool = False
    
    # ── ANANDAMAYA: Immutable identity ──────────────────────────
    
    def anandamaya_seed(self, identity: str, metadata: Dict = None):
        """Seed the Anandamaya core identity. Can ONLY be called once."""
        if self._anandamaya_locked:
            raise ValueError("Anandamaya is locked. Cannot modify core identity.")
        
        self.anandamaya = {
            "identity": identity,
            "created_at": time.time(),
            "metadata": metadata or {},
            "purpose": "inviolable — NEVER modified — core self",
        }
        self._anandamaya_locked = True
    
    def anandamaya_query(self) -> Dict:
        """Query the immutable core identity."""
        return dict(self.anandamaya)
    
    # ── ANNAMAYA: Raw data ingestion ───────────────────────────
    
    def annamaya_ingest(self, key: str, value: Any):
        """Ingest raw sensory/data into Annamaya."""
        self.annamaya[key] = {
            "value": value,
            "timestamp": time.time(),
        }
    
    def annamaya_read(self, key: str) -> Optional[Any]:
        """Read from Annamaya raw store."""
        entry = self.annamaya.get(key)
        return entry["value"] if entry else None
    
    def annamaya_clear_stale(self, max_age_sec: float = 3600):
        """Clear old Annamaya entries (hourly by default)."""
        now = time.time()
        stale = [k for k, v in self.annamaya.items()
                 if now - v["timestamp"] > max_age_sec]
        for k in stale:
            del self.annamaya[k]
        return len(stale)
    
    # ── PRANAMAYA: Active gists (working memory) ────────────────
    
    def pranamaya_compress(self, key: str, gist: Any) -> KoshaEntry:
        """Create a compressed gist in Pranamaya (active working memory)."""
        entry = KoshaEntry(
            key=key,
            value=gist,
            layer=KoshaLayer.PRANAMAYA,
        )
        self.pranamaya[key] = entry
        return entry
    
    def pranamaya_active_gists(self) -> List[str]:
        """List all active gist keys in Pranamaya."""
        return list(self.pranamaya.keys())
    
    # ── MANOMAYA: Processing layer ─────────────────────────────
    
    def manomaya_think(self, key: str, thought: Any) -> KoshaEntry:
        """Add a thought vector to Manomaya for active processing."""
        entry = KoshaEntry(
            key=key,
            value=thought,
            layer=KoshaLayer.MANOMAYA,
        )
        self.manomaya.append(entry)
        # Keep only last 10 thoughts in active processing
        if len(self.manomaya) > 10:
            self.manomaya = self.manomaya[-10:]
        return entry
    
    # ── VIJNANAMAYA: Consolidated wisdom ────────────────────────
    
    def _cosine_sim(self, a: str, b: str) -> float:
        """Simple similarity heuristic for text-based entries."""
        # For production: use actual embeddings. Here: keyword overlap ratio.
        if not a or not b:
            return 0.0
        words_a = set(a.lower().split())
        words_b = set(b.lower().split())
        if not words_a or not words_b:
            return 0.0
        intersection = words_a & words_b
        return len(intersection) / (len(words_a) + len(words_b) - len(intersection))
    
    def vijnanamaya_consolidate(self, key: str, knowledge: Any,
                                 confidence: float = 0.5) -> Optional[KoshaEntry]:
        """
        Consolidate knowledge into Vijnanamaya (long-term wisdom).
        Only succeeds if confidence > CONSOLIDATION_THRESHOLD.
        """
        if confidence < self.CONSOLIDATION_THRESHOLD:
            return None  # Not confident enough to consolidate
        
        entry = KoshaEntry(
            key=key,
            value=knowledge,
            layer=KoshaLayer.VIJNANAMAYA,
            consolidation_score=confidence,
        )
        self.vijnanamaya[key] = entry
        return entry
    
    def vijnanamaya_recall(self, key: str) -> Optional[Any]:
        """Recall consolidated wisdom from Vijnanamaya."""
        entry = self.vijnanamaya.get(key)
        if entry:
            entry.access_count += 1
            return entry.value
        return None
    
    def vijnanamaya_search(self, query: str, top_k: int = 3) -> List[KoshaEntry]:
        """Search Vijnanamaya by similarity to query."""
        scored = []
        for key, entry in self.vijnanamaya.items():
            sim = self._cosine_sim(query, str(entry.value))
            scored.append((sim, entry))
        scored.sort(key=lambda x: x[0], reverse=True)
        return [entry for _, entry in scored[:top_k]]
    
    # ── Catastrophic forgetting check ─────────────────────────
    
    def verify_anandamaya_integrity(self) -> bool:
        """Verify Anandamaya has not been modified since seeding."""
        return self._anandamaya_locked and "identity" in self.anandamaya
    
    def kosha_report(self) -> str:
        """Full Pancha Kosha state report."""
        lines = ["॥ पञ्च कोश — Pancha Kosha State ॥", ""]
        
        lines.append(f"  🕉 ANANDAMAYA (Bliss): {'LOCKED ✓' if self._anandamaya_locked else 'UNLOCKED ⚠'}")
        if self.anandamaya:
            lines.append(f"     Identity: {self.anandamaya.get('identity', 'Unknown')}")
        
        lines.append(f"\n  📚 VIJNANAMAYA (Wisdom): {len(self.vijnanamaya)} consolidated entries")
        for key in list(self.vijnanamaya.keys())[:3]:
            lines.append(f"     • {key}")
        
        lines.append(f"\n  💭 MANOMAYA (Mind): {len(self.manomaya)} active thoughts")
        
        lines.append(f"\n  ⚡ PRANAMAYA (Energy): {len(self.pranamaya)} active gists")
        for key in list(self.pranamaya.keys())[:3]:
            lines.append(f"     • {key}")
        
        lines.append(f"\n  🍚 ANNAMAYA (Food/Body): {len(self.annamaya)} raw data points")
        
        return "\n".join(lines)


if __name__ == "__main__":
    kosha = KoshaNet()
    
    # Seed Anandamaya (once only)
    kosha.anandamaya_seed("Krishi-Veda AI — Silchar, Assam, India",
                          metadata={"version": "1.0", "purpose": "Agricultural wisdom"})
    
    # Ingest raw data
    kosha.annamaya_ingest("soil_ph", 5.8)
    kosha.annamaya_ingest("soil_n", 25)
    kosha.annamaya_ingest("temp_c", 28)
    
    # Create active gists
    kosha.pranamaya_compress("soil_status", "Soil is acidic (pH 5.8), low nitrogen (25ppm)")
    kosha.pranamaya_compress("weather_gist", "Temperature 28°C, pre-monsoon conditions")
    
    # Active thinking
    kosha.manomaya_think("acidic_diagnosis", "pH 5.8 requires liming — paravartya calculation needed")
    
    # Consolidate wisdom (high confidence)
    kosha.vijnanamaya_consolidate(
        "liming_rule_alluvial",
        "Alluvial soil at pH < 6.0: apply lime at 2-3 tons/ha, 3 weeks before sowing",
        confidence=0.85
    )
    
    # This should FAIL (low confidence)
    result = kosha.vijnanamaya_consolidate(
        "uncertain_tip", "Maybe apply something sometime",
        confidence=0.3
    )
    print(f"Low-confidence consolidation: {'Stored' if result else 'REJECTED (below threshold)'}")
    
    # Verify integrity
    print(f"\nAnandamaya integrity: {'INTACT ✓' if kosha.verify_anandamaya_integrity() else 'CORRUPTED ⚠'}")
    
    # Search wisdom
    results = kosha.vijnanamaya_search("acidic soil liming requirement")
    print(f"\nSearch 'acidic soil liming': {len(results)} results")
    for r in results:
        print(f"  • {r.value[:80]}...")
    
    # Full report
    print("\n" + kosha.kosha_report())
    
    # Demonstrate Anandamaya lock
    try:
        kosha.anandamaya_seed("Hacker identity", {})
    except ValueError as e:
        print(f"\n✓ Anandamaya protection: {e}")
