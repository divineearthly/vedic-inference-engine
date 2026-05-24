"""Nyaya 5-Step Reasoning — ported from VedaRta/kernels/vr_pramana_nyaya.h
Source: Nyaya Sutras, Tarka-Sangraha
Pratijna → Hetu → Udaharana → Upanaya → Nigamana
Pure Python. No C dependency. ARM64-ready in Termux."""

from enum import IntEnum
from dataclasses import dataclass
from typing import List


class PramanaSource(IntEnum):
    PRATYAKSHA = 0   # Direct perception — sensor, direct input
    ANUMANA = 1       # Inference — logically derived
    UPAMANA = 2       # Analogy — pattern-matched from known
    SHABDA = 3        # Testimony — trusted prior knowledge (Shruti)
    UNGROUNDED = 4    # Hallucination risk — flag immediately


@dataclass
class PramanaToken:
    text: str
    source: PramanaSource
    confidence: float        # 0.0 to 1.0
    hetu: str = ""           # Reason (Hetu)
    udaharana: str = ""      # Example (Udaharana)
    upanaya: str = ""        # Application
    nigamana: str = ""       # Conclusion


class NyayaScaffold:
    """5-step Nyaya reasoning scaffold.
    
    Usage:
        nyaya = NyayaScaffold()
        nyaya.tag("Soil moisture is 23%", PramanaSource.PRATYAKSHA, 0.95)
        nyaya.tag("Drought risk high", PramanaSource.ANUMANA, 0.72, 
                   hetu="moisture below threshold", udaharana="2024 pre-monsoon pattern")
        
        if nyaya.detect_hallucinations():
            print("WARNING: Ungrounded claims detected")
        
        print(nyaya.explain())
        print(f"Overall confidence: {nyaya.overall_confidence():.2f}")
    """
    
    def __init__(self):
        self.tokens: List[PramanaToken] = []
    
    def tag(self, text: str, source: PramanaSource, confidence: float,
            hetu: str = "", udaharana: str = "", upanaya: str = "", nigamana: str = ""):
        """Tag a token with its Pramana source and Nyaya reasoning steps."""
        self.tokens.append(PramanaToken(
            text=text, source=source, confidence=confidence,
            hetu=hetu, udaharana=udaharana, upanaya=upanaya, nigamana=nigamana
        ))
    
    def detect_hallucinations(self) -> List[PramanaToken]:
        """Return UNGROUNDED or low-confidence tokens — hallucination risk."""
        return [t for t in self.tokens 
                if t.source == PramanaSource.UNGROUNDED or t.confidence < 0.5]
    
    def get_sattvic_tokens(self, min_confidence: float = 0.7) -> List[PramanaToken]:
        """Return high-confidence, verified tokens (Sattvic knowledge)."""
        return [t for t in self.tokens 
                if t.source != PramanaSource.UNGROUNDED and t.confidence >= min_confidence]
    
    def explain(self) -> str:
        """Generate full 5-step Nyaya reasoning trace."""
        steps = ["Pratijna (Claim)", "Hetu (Reason)", "Udaharana (Example)", 
                 "Upanaya (Application)", "Nigamana (Conclusion)"]
        lines = ["॥ Nyaya 5-Step Reasoning ॥", ""]
        
        for i, token in enumerate(self.tokens[:5]):
            step = steps[i] if i < len(steps) else f"Step {i+1}"
            line = f"  {i+1}. {step}: {token.text} [{token.source.name}]"
            if token.hetu and i == 1:
                line += f"\n     → Because: {token.hetu}"
            if token.udaharana and i == 2:
                line += f"\n     → Example: {token.udaharana}"
            if token.upanaya and i == 3:
                line += f"\n     → Applied: {token.upanaya}"
            if token.nigamana and i == 4:
                line += f"\n     → Therefore: {token.nigamana}"
            lines.append(line)
        
        ungrounded = self.detect_hallucinations()
        if ungrounded:
            lines.append(f"\n  ⚠ Hallucination risk: {len(ungrounded)} ungrounded claim(s)")
        else:
            lines.append(f"\n  ✓ No hallucinations detected")
        
        lines.append(f"  Confidence: {self.overall_confidence():.2f}")
        return "\n".join(lines)
    
    def overall_confidence(self) -> float:
        """Weighted confidence: Pratyaksha highest, Ungrounded penalized."""
        if not self.tokens:
            return 0.0
        weights = {
            PramanaSource.PRATYAKSHA: 1.0,
            PramanaSource.ANUMANA: 0.8,
            PramanaSource.UPAMANA: 0.6,
            PramanaSource.SHABDA: 0.7,
            PramanaSource.UNGROUNDED: -0.5
        }
        total = sum(t.confidence * weights[t.source] for t in self.tokens)
        return max(0.0, total / len(self.tokens))
    
    def clear(self):
        """Reset scaffold for new reasoning chain."""
        self.tokens.clear()


if __name__ == "__main__":
    nyaya = NyayaScaffold()
    
    nyaya.tag("NASA POWER shows soil moisture at 23% today", 
              PramanaSource.PRATYAKSHA, 0.95)
    nyaya.tag("Moisture below 30% threshold indicates pre-monsoon stress",
              PramanaSource.ANUMANA, 0.82, 
              hetu="Historical threshold from ICAR data",
              udaharana="April 2024 Barak Valley pattern")
    nyaya.tag("Similar dryness pattern to 2023 pre-monsoon week",
              PramanaSource.UPAMANA, 0.68,
              udaharana="2023-04-15 Silchar reading")
    nyaya.tag("ICAR advisory: irrigate when moisture < 30% for 3+ days",
              PramanaSource.SHABDA, 0.90)
    nyaya.tag("Farmers should irrigate paddy fields within 48 hours",
              PramanaSource.ANUMANA, 0.85,
              hetu="Moisture declining at 2%/day, threshold breach imminent",
              nigamana="Nigamana: Timely irrigation prevents 15-20% yield loss")
    
    print(nyaya.explain())
    
    nyaya.tag("Aliens are stealing crop water", PramanaSource.UNGROUNDED, 0.05)
    hall = nyaya.detect_hallucinations()
    print(f"\n⚠ Hallucinations found: {len(hall)}")
    for h in hall:
        print(f"   - '{h.text}' (confidence: {h.confidence})")
