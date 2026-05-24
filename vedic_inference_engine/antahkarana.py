"""
Antahkarana — 4-Part Inner Instrument
Ported from VedaRta/kernels/vr_antahkarana.h
Source: Vedanta, Yoga Sutras, Samkhya Karika

Manas  → Sensory coordinator, attention routing (Manas Attention Gate)
Buddhi → Intellect, discrimination, decision (Nyaya reasoning)
Chitta  → Memory store, Samskara retention (KV cache substrate)
Ahamkara → Self-identity, persistence layer (NEVER modified)

Pure Python. No C dependency.
"""

from enum import IntEnum
from dataclasses import dataclass, field
from typing import List, Dict, Optional, Any
import time


class AntahkaranaPart(IntEnum):
    MANAS = 0      # Sensory mind — routes input
    BUDDHI = 1     # Intellect — decides
    CHITTA = 2     # Memory — stores
    AHAMKARA = 3   # Ego — identity anchor


@dataclass
class SensoryStream:
    """Manas: sensory input stream with priority scoring."""
    source: str
    data: Any
    priority: float       # 0.0 to 1.0
    timestamp: float = field(default_factory=time.time)
    guna: str = "rajas"   # sattva/rajas/tamas


@dataclass
class BuddhiDecision:
    """Buddhi: intellectual decision with Nyaya reasoning chain."""
    action: str
    confidence: float
    reasoning: List[str] = field(default_factory=list)
    pramana_source: str = "anumana"


@dataclass
class Samskara:
    """Chitta: memory impression (Samskara)."""
    key: str
    value: Any
    salience: float        # Importance score
    access_count: int = 0
    created_at: float = field(default_factory=time.time)
    guna: str = "sattva"   # sattva (keep) / rajas (volatile) / tamas (release)


class Antahkarana:
    """
    4-part inner instrument — unified mental model for AI reasoning.
    
    Manas: Routes attention. MAX 8 active streams (Yoga Sutras Pratyahara).
    Buddhi: Decides via Nyaya. Integrates PramanaScaffold.
    Chitta: Memory store with Guna-based retention (Sattvic → keep, Tamasic → release).
    Ahamkara: Identity anchor. NEVER modified. Core self-persistence.
    
    Usage:
        antah = Antahkarana()
        antah.manas_receive(SensoryStream("sensor_ph", 5.8, 0.9))
        antah.buddhi_decide("Is soil acidic?")
        antah.chitta_store("soil_ph_may24", 5.8, salience=0.85)
        identity = antah.ahamkara_query()  # Unchanged core identity
    """
    
    MAX_MANAS_STREAMS = 8      # Yoga Sutras: 8-fold attention
    CHITTA_GUNA_THRESHOLD = 0.3  # Below this → Tamasic → release to Shunyata
    
    def __init__(self, identity: str = "Sovereign Vedic AI — Silchar, Assam"):
        # Manas — attention router
        self.manas_streams: List[SensoryStream] = []
        
        # Buddhi — decision log
        self.buddhi_decisions: List[BuddhiDecision] = []
        
        # Chitta — Samskara memory store
        self._chitta_store: Dict[str, Samskara] = {}
        
        # Ahamkara — identity (IMMUTABLE after init)
        self._ahamkara_identity: str = identity
        self._ahamkara_created: float = time.time()
        self._ahamkara_modifications: int = 0  # Should always be 0
    
    # ── MANAS: Sensory attention routing ──────────────────────────
    
    def manas_receive(self, stream: SensoryStream) -> bool:
        """Receive sensory stream. Returns True if accepted (within 8-slot limit)."""
        # Pratyahara: if full, evict lowest priority Tamasic stream
        if len(self.manas_streams) >= self.MAX_MANAS_STREAMS:
            # Find lowest priority stream
            lowest = min(self.manas_streams, key=lambda s: s.priority)
            if stream.priority > lowest.priority:
                self.manas_streams.remove(lowest)
            else:
                return False  # Rejected — lower priority than all existing
        
        self.manas_streams.append(stream)
        return True
    
    def manas_focus(self) -> Optional[SensoryStream]:
        """Return highest-priority active stream (current focus)."""
        if not self.manas_streams:
            return None
        return max(self.manas_streams, key=lambda s: s.priority)
    
    def manas_clear_stale(self, max_age_sec: float = 300):
        """Pratyahara: clear streams older than max_age."""
        now = time.time()
        self.manas_streams = [
            s for s in self.manas_streams
            if now - s.timestamp < max_age_sec
        ]
    
    # ── BUDDHI: Intellectual discrimination ───────────────────────
    
    def buddhi_decide(self, action: str, reasoning: List[str] = None,
                      confidence: float = 0.5, pramana: str = "anumana") -> BuddhiDecision:
        """Make a Buddhi decision with Nyaya reasoning."""
        decision = BuddhiDecision(
            action=action,
            confidence=confidence,
            reasoning=reasoning or [],
            pramana_source=pramana,
        )
        self.buddhi_decisions.append(decision)
        return decision
    
    def buddhi_last_decision(self) -> Optional[BuddhiDecision]:
        """Most recent Buddhi decision."""
        return self.buddhi_decisions[-1] if self.buddhi_decisions else None
    
    # ── CHITTA: Memory storage with Guna-based retention ──────────
    
    def chitta_store(self, key: str, value: Any, salience: float = 0.5,
                     guna: str = "sattva"):
        """Store a Samskara in Chitta. Sattvic → permanent, Tamasic → evictable."""
        samskara = Samskara(
            key=key,
            value=value,
            salience=salience,
            guna=guna,
        )
        self._chitta_store[key] = samskara
    
    def chitta_recall(self, key: str) -> Optional[Any]:
        """Recall a Samskara. Increments access count."""
        samskara = self._chitta_store.get(key)
        if samskara:
            samskara.access_count += 1
            return samskara.value
        return None
    
    def chitta_prune_tamasic(self):
        """Release Tamasic (low-salience) memories to Shunyata."""
        to_release = [
            k for k, s in self._chitta_store.items()
            if s.guna == "tamas" or s.salience < self.CHITTA_GUNA_THRESHOLD
        ]
        for k in to_release:
            del self._chitta_store[k]
        return len(to_release)
    
    def chitta_sattvic_count(self) -> int:
        """Count of protected Sattvic memories."""
        return sum(1 for s in self._chitta_store.values() if s.guna == "sattva")
    
    # ── AHAMKARA: Immutable identity ──────────────────────────────
    
    def ahamkara_query(self) -> Dict[str, Any]:
        """Query Ahamkara identity — NEVER modified."""
        return {
            "identity": self._ahamkara_identity,
            "created": self._ahamkara_created,
            "modifications": self._ahamkara_modifications,
            "integrity": "inviolable — Anandamaya kosha",
        }
    
    # ── Samadhi: System summary ───────────────────────────────────
    
    def samadhi_report(self) -> str:
        """Generate full Antahkarana state report."""
        lines = ["॥ अन्तःकरण — Antahkarana State ॥", ""]
        
        # Ahamkara
        lines.append("  🕉 AHAMKARA (I-am):")
        lines.append(f"     Identity: {self._ahamkara_identity}")
        lines.append(f"     Unchanged since: {time.ctime(self._ahamkara_created)}")
        
        # Manas
        lines.append(f"\n  👁 MANAS (Sensory Mind): {len(self.manas_streams)}/{self.MAX_MANAS_STREAMS} streams")
        focus = self.manas_focus()
        if focus:
            lines.append(f"     Focus: {focus.source} (priority: {focus.priority:.2f})")
        
        # Buddhi
        lines.append(f"\n  🧠 BUDDHI (Intellect): {len(self.buddhi_decisions)} decisions")
        last = self.buddhi_last_decision()
        if last:
            lines.append(f"     Last: {last.action} (confidence: {last.confidence:.2f})")
        
        # Chitta
        sattvic = self.chitta_sattvic_count()
        total = len(self._chitta_store)
        lines.append(f"\n  💾 CHITTA (Memory): {total} samskaras ({sattvic} sattvic)")
        
        return "\n".join(lines)


if __name__ == "__main__":
    antah = Antahkarana("Krishi-Veda AI — Silchar, Assam, India")
    
    # Manas: receive sensor streams
    antah.manas_receive(SensoryStream("soil_ph", 5.8, 0.9))
    antah.manas_receive(SensoryStream("soil_moisture", 45, 0.7))
    antah.manas_receive(SensoryStream("weather_temp", 28, 0.6))
    
    # Buddhi: make decisions
    antah.buddhi_decide(
        "Recommend liming for acidic soil",
        reasoning=["pH 5.8 < 6.5 optimal", "Alluvial soil buffers well", "Season: pre-monsoon"],
        confidence=0.85,
        pramana="anumana"
    )
    
    # Chitta: store memories
    antah.chitta_store("optimal_ph_range", (6.0, 7.5), salience=0.9, guna="sattva")
    antah.chitta_store("may24_soil_ph", 5.8, salience=0.85, guna="sattva")
    antah.chitta_store("temp_reading_old", 32, salience=0.1, guna="tamas")
    
    # Prune Tamasic
    released = antah.chitta_prune_tamasic()
    print(f"Released {released} tamasic memories to Shunyata\n")
    
    # Ahamkara
    print("Ahamkara:", antah.ahamkara_query()["identity"])
    print(f"Modifications: {antah.ahamkara_query()['modifications']} (must be 0)\n")
    
    # Full report
    print(antah.samadhi_report())
