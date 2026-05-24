"""
Ahimsa-108 Protocol — unified ethics filter for all Divine Earthly repos.
Source: Ahimsa principle, Charaka-Sushruta Samhitas, Panchgavya tradition
Moved from Krishi-Veda-Module to shared vedic-inference-engine.
All output paths MUST pass through this filter.
"""

from enum import IntEnum
from dataclasses import dataclass
from typing import List, Optional


class AhimsaLevel(IntEnum):
    AHIMSA_PURE = 0       # Fully organic, no harm
    AHIMSA_CAUTION = 1    # Minor concern, flag but allow
    AHIMSA_VIOLATION = 2  # Chemical/harmful — block or redirect


@dataclass
class AhimsaVerdict:
    level: AhimsaLevel
    score: float              # 0.0 (harmful) to 100.0 (pure)
    recommendation: str
    blocked_terms: List[str]
    panchgavya_alternative: str = ""


class Ahimsa108Filter:
    """
    Ahimsa-108 Protocol Filter.
    
    Usage:
        f = Ahimsa108Filter()
        verdict = f.evaluate("Apply urea 50kg and malathion spray")
        if verdict.level == AhimsaLevel.AHIMSA_VIOLATION:
            print(f"BLOCKED: {verdict.recommendation}")
            print(f"Alternative: {verdict.panchgavya_alternative}")
    """
    
    # Chemical/industrial terms — violation
    CHEMICAL_BLOCKLIST = [
        "urea", "dap", "mop", "superphosphate", "ammonium", "nitrate",
        "malathion", "chlorpyrifos", "endosulfan", "ddt", "lindane",
        "glyphosate", "paraquat", "atrazine", "2,4-d", "dicamba",
        "carbofuran", "phorate", "methyl parathion", "monocrotophos",
        "synthetic fertilizer", "chemical pesticide", "chemical herbicide",
        "npk", "triple superphosphate", "muriatic", "potassium chloride",
    ]
    
    # Caution terms — organic but potent
    CAUTION_LIST = [
        "neem oil concentrate", "tobacco decoction", "copper sulfate",
        "bordeaux mixture", "sulfur dust", "lime sulfur",
    ]
    
    # Panchgavya alternatives
    PANCHGAVYA_ALTERNATIVES = {
        "fertilizer": "Panchgavya (cow dung 5kg + urine 3L + milk 2L + curd 2L + ghee 1kg, ferment 15 days) at 3% foliar spray",
        "pesticide": "Neem-Astra (neem leaves 5kg crushed in 10L water + cow urine 5L, boil, strain) spray at 10% concentration",
        "fungicide": "Buttermilk spray (sour buttermilk 1L + water 10L) or Panchgavya at 5%",
        "herbicide": "Mulching with straw/crop residue + manual weeding at 21-day intervals",
        "soil": "Jeevamrut (cow dung 10kg + urine 10L + jaggery 2kg + pulse flour 2kg + soil 1kg in 200L water, ferment 7 days)",
    }
    
    def __init__(self, threshold: float = 75.0):
        self.threshold = threshold
    
    def evaluate(self, text: str) -> AhimsaVerdict:
        """Evaluate text against Ahimsa-108 protocol."""
        text_lower = text.lower()
        blocked = []
        caution = []
        
        for term in self.CHEMICAL_BLOCKLIST:
            if term in text_lower:
                blocked.append(term)
        
        for term in self.CAUTION_LIST:
            if term in text_lower:
                caution.append(term)
        
        # Scoring
        if blocked:
            score = max(0, 100 - len(blocked) * 30)
            level = AhimsaLevel.AHIMSA_VIOLATION
        elif caution:
            score = 100 - len(caution) * 10
            level = AhimsaLevel.AHIMSA_CAUTION
        else:
            score = 100.0
            level = AhimsaLevel.AHIMSA_PURE
        
        # Determine alternative
        alternative = ""
        if blocked:
            if any(t in text_lower for t in ["fertilizer", "urea", "dap", "mop", "npk", "ammonium"]):
                alternative = self.PANCHGAVYA_ALTERNATIVES["fertilizer"]
            elif any(t in text_lower for t in ["pesticide", "insecticide", "malathion", "chlorpyrifos"]):
                alternative = self.PANCHGAVYA_ALTERNATIVES["pesticide"]
            elif any(t in text_lower for t in ["fungicide", "sulfur"]):
                alternative = self.PANCHGAVYA_ALTERNATIVES["fungicide"]
            elif any(t in text_lower for t in ["herbicide", "glyphosate", "paraquat"]):
                alternative = self.PANCHGAVYA_ALTERNATIVES["herbicide"]
            else:
                alternative = self.PANCHGAVYA_ALTERNATIVES["soil"]
        
        # Recommendation
        if level == AhimsaLevel.AHIMSA_VIOLATION:
            rec = f"BLOCKED: Chemical agents detected: {', '.join(blocked)}. Use Panchgavya-based organic alternative."
        elif level == AhimsaLevel.AHIMSA_CAUTION:
            rec = f"CAUTION: Potent organic agents: {', '.join(caution)}. Verify application rate and necessity."
        else:
            rec = "✓ Ahimsa-compliant. No harmful substances detected."
        
        return AhimsaVerdict(
            level=level,
            score=score,
            recommendation=rec,
            blocked_terms=blocked + caution,
            panchgavya_alternative=alternative,
        )
    
    def is_triggered(self, stress_code: float) -> bool:
        """Check if Ahimsa-108 threshold is exceeded."""
        return stress_code >= self.threshold
    
    def purify(self, text: str) -> str:
        """Replace chemical recommendations with Panchgavya alternatives."""
        verdict = self.evaluate(text)
        if verdict.level == AhimsaLevel.AHIMSA_VIOLATION and verdict.panchgavya_alternative:
            return f"[AHIMSA-108 CORRECTED]\n{text}\n\n⚠ Organic Alternative:\n{verdict.panchgavya_alternative}"
        return text


if __name__ == "__main__":
    f = Ahimsa108Filter()
    
    print("=== Pure organic advice ===")
    v1 = f.evaluate("Apply Panchgavya 3% foliar spray and neem cake 200kg/ha")
    print(f"  Level: {v1.level.name}, Score: {v1.score:.0f}")
    print(f"  {v1.recommendation}")
    
    print("\n=== Chemical violation ===")
    v2 = f.evaluate("Apply urea 50kg and malathion spray for pest control")
    print(f"  Level: {v2.level.name}, Score: {v2.score:.0f}")
    print(f"  {v2.recommendation}")
    print(f"  Alternative: {v2.panchgavya_alternative[:100]}...")
    
    print("\n=== Purify test ===")
    print(f.purify("Apply urea 50kg and malathion spray for pest control")[:300])
