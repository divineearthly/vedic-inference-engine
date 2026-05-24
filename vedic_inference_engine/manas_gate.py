"""
Manas Attention Gate — 8-Slot Sensory Coordinator
Ported from VedaRta/kernels/manas_attention_gate.h
Source: Yoga Sutras — Manas as sensory coordinator, Pratyahara

8 attention slots (Yoga Sutras).
Priority scoring with PHI-dampened compute allocation.
PEST (Pest Emergency Sensor Triage): crisis evicts low-priority streams.
"""

from dataclasses import dataclass, field
from typing import List, Dict, Optional, Any
import time
import math

PHI = 1.618033988749895
MAX_SLOTS = 8  # Yoga Sutras: 8-fold attention


@dataclass
class SensoryPriority:
    """Sensor stream with Vedic priority scoring."""
    sensor_id: str
    source: str           # e.g., "soil_ph", "weather_temp", "pest_alert"
    value: Any
    priority: float       # 0.0 to 1.0
    urgency: str = "normal"  # "normal", "urgent", "crisis"
    timestamp: float = field(default_factory=time.time)
    guna: str = "rajas"   # sattva/rajas/tamas
    
    def compute_weight(self) -> float:
        """PHI-dampened priority weight."""
        urgency_mult = {"normal": 1.0, "urgent": 2.0, "crisis": 5.0}
        base = self.priority * urgency_mult.get(self.urgency, 1.0)
        # PHI dampening: prevent any single stream from dominating
        return base / (1.0 + PHI * base)


class ManasGate:
    """
    Manas Attention Gate — 8-slot sensory coordinator.
    
    Routes sensor inputs by priority. Crisis (PEST) evicts lowest.
    PHI-dampening prevents attention monopolization.
    
    Usage:
        gate = ManasGate()
        gate.attend("soil_ph", 5.8, priority=0.9, source="sensor")
        gate.attend("weather_temp", 28, priority=0.6)
        gate.attend("pest_alert", "aphids", priority=0.95, urgency="crisis")
        focus = gate.current_focus()
        print(f"Attention on: {focus.source} — {focus.value}")
    """
    
    def __init__(self, max_slots: int = MAX_SLOTS):
        self.max_slots = max_slots
        self.slots: List[SensoryPriority] = []
        self._eviction_count: int = 0
        self._crisis_count: int = 0
    
    def attend(self, sensor_id: str, value: Any, priority: float = 0.5,
               source: str = "sensor", urgency: str = "normal",
               guna: str = "rajas") -> bool:
        """
        Attend to a sensor stream. Returns True if accepted.
        Crisis evicts lowest-priority non-crisis stream.
        """
        stream = SensoryPriority(
            sensor_id=sensor_id,
            source=source,
            value=value,
            priority=min(1.0, max(0.0, priority)),
            urgency=urgency,
            guna=guna,
        )
        
        if urgency == "crisis":
            self._crisis_count += 1
        
        # If slots available, accept
        if len(self.slots) < self.max_slots:
            self.slots.append(stream)
            return True
        
        # Find lowest priority slot to potentially evict
        lowest = min(self.slots, key=lambda s: s.compute_weight())
        
        # Crisis always evicts non-crisis
        if urgency == "crisis" and lowest.urgency != "crisis":
            self.slots.remove(lowest)
            self.slots.append(stream)
            self._eviction_count += 1
            return True
        
        # Higher priority evicts lower
        if stream.compute_weight() > lowest.compute_weight():
            self.slots.remove(lowest)
            self.slots.append(stream)
            self._eviction_count += 1
            return True
        
        return False  # Rejected — lower priority than all slots
    
    def current_focus(self) -> Optional[SensoryPriority]:
        """Highest-weight stream currently in attention."""
        if not self.slots:
            return None
        return max(self.slots, key=lambda s: s.compute_weight())
    
    def get_crisis_streams(self) -> List[SensoryPriority]:
        """Get all crisis-level streams (PEST triage)."""
        return [s for s in self.slots if s.urgency == "crisis"]
    
    def pratyahara(self, max_age_sec: float = 300):
        """Pratyahara: withdraw attention from stale streams."""
        now = time.time()
        self.slots = [s for s in self.slots 
                      if now - s.timestamp < max_age_sec]
    
    def get_all_values(self) -> Dict[str, Any]:
        """Get all active sensor values keyed by sensor_id."""
        return {s.sensor_id: s.value for s in self.slots}
    
    def stats(self) -> Dict:
        """Gate statistics."""
        return {
            "active_slots": len(self.slots),
            "max_slots": self.max_slots,
            "evictions": self._eviction_count,
            "crises_handled": self._crisis_count,
            "current_focus": self.current_focus().source if self.current_focus() else "none",
            "crisis_active": len(self.get_crisis_streams()) > 0,
        }
    
    def clear(self):
        """Clear all attention slots."""
        self.slots.clear()


if __name__ == "__main__":
    gate = ManasGate()
    
    # Normal sensor streams
    gate.attend("soil_ph", 5.8, priority=0.7, source="soil_sensor")
    gate.attend("soil_moisture", 45, priority=0.6, source="soil_sensor")
    gate.attend("weather_temp", 28, priority=0.5, source="weather_api")
    gate.attend("weather_humidity", 80, priority=0.4, source="weather_api")
    
    # Crisis — PEST alert
    gate.attend("pest_aphids", "Aphid infestation detected", 
                priority=0.95, urgency="crisis", source="pest_monitor")
    
    # Low priority — gets rejected (8 slots full, lower than all)
    accepted = gate.attend("ambient_light", 500, priority=0.1, source="env_sensor")
    
    print("॥ Manas Attention Gate ॥")
    print(f"  Active slots: {gate.stats()['active_slots']}/{gate.max_slots}")
    print(f"  Current focus: {gate.current_focus().source} — {gate.current_focus().value}")
    print(f"  Crisis active: {gate.stats()['crisis_active']}")
    print(f"  Low-priority accepted: {accepted}")
    print(f"  Evictions: {gate.stats()['evictions']}")
    print()
    
    crisis_streams = gate.get_crisis_streams()
    print(f"  PEST Triage — {len(crisis_streams)} crisis streams:")
    for s in crisis_streams:
        print(f"    ⚠ {s.source}: {s.value} (weight: {s.compute_weight():.3f})")
    
    print(f"\n  All sensor values:")
    for sid, val in gate.get_all_values().items():
        print(f"    {sid}: {val}")
