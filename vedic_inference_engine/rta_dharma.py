"""Rta-Dharma Priority — ported from VedaRta/kernels/rta_dharma_priority.h
Source: Rigveda (Rta — cosmic order), Dharmashastra, Ahimsa
Ethical routing: output shaped by community impact, not just prediction accuracy.
Pure Python. No C dependency."""

from dataclasses import dataclass
from typing import List


@dataclass
class DharmaPriority:
    """Priority score for a domain action based on Vedic ethics."""
    action: str
    crop_importance: float     # 0.0-1.0 — staple crop weight
    season_urgency: float      # 0.0-1.0 — time-critical factor
    severity_weight: float     # 0.0-1.0 — problem severity
    farmers_affected: int      # number of farmers impacted
    ahimsa_score: float = 1.0  # 0.0-1.0 — non-harm compliance
    
    def compute_score(self) -> float:
        """Rta-Dharma composite priority score.
        Higher score = higher ethical priority for routing."""
        community_factor = min(1.0, self.farmers_affected / 1000.0)
        base = (self.crop_importance * 0.30 +
                self.season_urgency * 0.25 +
                self.severity_weight * 0.25 +
                community_factor * 0.20)
        return base * self.ahimsa_score


class RtaDharmaRouter:
    """Routes actions by Dharma priority. Higher priority = served first.
    
    Usage:
        router = RtaDharmaRouter()
        router.add("Irrigation alert", crop=0.9, season=0.8, severity=0.7, farmers=500)
        router.add("Fertilizer tip", crop=0.5, season=0.3, severity=0.2, farmers=50)
        top = router.top_priority()
        print(f"Serve first: {top.action} (score: {top.compute_score():.2f})")
    """
    
    def __init__(self):
        self.priorities: List[DharmaPriority] = []
    
    def add(self, action: str, crop_importance: float, season_urgency: float,
            severity_weight: float, farmers_affected: int, ahimsa_score: float = 1.0):
        """Register an action with its Dharma priority parameters."""
        self.priorities.append(DharmaPriority(
            action=action,
            crop_importance=min(1.0, max(0.0, crop_importance)),
            season_urgency=min(1.0, max(0.0, season_urgency)),
            severity_weight=min(1.0, max(0.0, severity_weight)),
            farmers_affected=farmers_affected,
            ahimsa_score=min(1.0, max(0.0, ahimsa_score))
        ))
    
    def top_priority(self) -> DharmaPriority:
        """Return highest Dharma-priority action."""
        if not self.priorities:
            return None
        return max(self.priorities, key=lambda p: p.compute_score())
    
    def sorted_by_priority(self) -> List[DharmaPriority]:
        """All actions sorted by Dharma priority (highest first)."""
        return sorted(self.priorities, key=lambda p: p.compute_score(), reverse=True)
    
    def filter_by_ahimsa(self, min_ahimsa: float = 0.7) -> List[DharmaPriority]:
        """Filter out actions that violate Ahimsa threshold."""
        return [p for p in self.priorities if p.ahimsa_score >= min_ahimsa]
    
    def explain(self) -> str:
        """Human-readable explanation of routing decisions."""
        lines = ["॥ Rta-Dharma Priority Routing ॥", ""]
        for i, p in enumerate(self.sorted_by_priority()):
            lines.append(
                f"  {i+1}. {p.action} (score: {p.compute_score():.3f})"
            )
            lines.append(
                f"     Crop={p.crop_importance:.2f} Season={p.season_urgency:.2f} "
                f"Severity={p.severity_weight:.2f} Farmers={p.farmers_affected} "
                f"Ahimsa={p.ahimsa_score:.2f}"
            )
        return "\n".join(lines)


if __name__ == "__main__":
    router = RtaDharmaRouter()
    
    router.add("Emergency irrigation for paddy", 
               crop_importance=0.95, season_urgency=0.90, 
               severity_weight=0.85, farmers_affected=800)
    router.add("Pest alert for mustard crop",
               crop_importance=0.60, season_urgency=0.70,
               severity_weight=0.55, farmers_affected=200)
    router.add("Chemical pesticide recommendation",
               crop_importance=0.50, season_urgency=0.40,
               severity_weight=0.60, farmers_affected=100,
               ahimsa_score=0.3)  # Low ahimsa — chemical harm
    router.add("Organic composting tip",
               crop_importance=0.40, season_urgency=0.30,
               severity_weight=0.20, farmers_affected=50,
               ahimsa_score=1.0)  # High ahimsa
    
    print(router.explain())
    
    print(f"\nAfter Ahimsa filter (≥0.7):")
    for p in router.filter_by_ahimsa(0.7):
        print(f"  ✓ {p.action} (ahimsa: {p.ahimsa_score:.2f})")
