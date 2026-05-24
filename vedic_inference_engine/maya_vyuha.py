"""
Maya-Vyuha — Vedic Structured Dropout
Source: Advaita Vedanta (Maya as cosmic illusion), Mahabharata (Vyuha formations)
Bhagavad Gita 7.14: "This divine Maya of mine is hard to overcome."

Maya obscures Brahman to create the appearance of multiplicity.
Vyuha is strategic arrangement (Krishna's battle formations).
Together: structured dropout following golden spiral patterns.
"""

import math
import random
from typing import List

PHI = 1.618033988749895

class MayaVyuha:
    """
    Vedic Dropout — Maya obscures, Vyuha arranges.
    
    Not random dropout. Strategic obscuration following:
    1. Golden spiral pattern (φ-based)
    2. Chakra-vyuha formation (concentric circles)
    3. Maya principle: obscure to reveal deeper unity
    
    Usage:
        mv = MayaVyuha(dropout_rate=0.3)
        mask = mv.vyuha_mask(shape=(256,), step=100)
        # Apply mask to layer output
    """
    
    def __init__(self, dropout_rate: float = 0.3, vyuha_type: str = "chakra"):
        """
        Args:
            dropout_rate: Fraction of neurons obscured (0.0-0.5)
            vyuha_type: "chakra" (concentric), "garuda" (eagle), "makara" (crocodile)
        """
        self.rate = min(0.5, max(0.0, dropout_rate))
        self.vyuha_type = vyuha_type
        self._step = 0
    
    def _golden_spiral_mask(self, size: int) -> List[float]:
        """Golden spiral pattern — neurons at φ-intervals are obscured."""
        mask = [1.0] * size
        obscured = int(size * self.rate)
        
        # Obscure neurons at golden ratio intervals
        positions = []
        pos = 0.0
        for _ in range(obscured):
            pos = (pos + PHI) % size
            idx = int(pos)
            if idx < size:
                positions.append(idx)
        
        for idx in positions:
            mask[idx] = 0.0  # Maya obscures
        
        return mask
    
    def _chakra_vyuha_mask(self, size: int) -> List[float]:
        """Chakra vyuha — concentric circles, outer ring obscured."""
        mask = [1.0] * size
        
        # Chakra has 7 layers (Sapta-Chakra from Yoga)
        layers = 7
        layer_size = size // layers
        
        obscured_layers = int(layers * self.rate)
        for l in range(obscured_layers):
            start = l * layer_size
            end = min(start + layer_size, size)
            for i in range(start, end):
                mask[i] = 0.0
        
        return mask
    
    def _garuda_vyuha_mask(self, size: int) -> List[float]:
        """Garuda vyuha — eagle formation, wings obscured."""
        mask = [1.0] * size
        center = size // 2
        
        # Wings spread from center at φ angles
        wing_span = int(size * self.rate)
        for i in range(wing_span):
            left = center - i - 1
            right = center + i + 1
            if left >= 0:
                mask[left] = 0.0
            if right < size:
                mask[right] = 0.0
        
        return mask
    
    def vyuha_mask(self, shape: tuple, step: int = None) -> List[float]:
        """
        Generate Maya-Vyuha dropout mask.
        Pattern shifts with step like rotating vyuha formation.
        """
        if step is None:
            self._step += 1
            step = self._step
        
        size = 1
        for dim in shape:
            size *= dim
        
        # Rotate the vyuha with each step
        rotation = step % size
        
        if self.vyuha_type == "chakra":
            mask = self._chakra_vyuha_mask(size)
        elif self.vyuha_type == "garuda":
            mask = self._garuda_vyuha_mask(size)
        else:
            mask = self._golden_spiral_mask(size)
        
        # Rotate mask
        mask = mask[rotation:] + mask[:rotation]
        
        return mask
    
    def obscuration_density(self) -> float:
        """Current density of Maya obscuration."""
        return self.rate * (1.0 - 1.0 / (PHI * PHI))  # ~0.618 * rate
    
    def brahman_factor(self) -> float:
        """How much of the underlying unity (Brahman) remains visible."""
        return 1.0 - self.obscuration_density()


if __name__ == "__main__":
    print("॥ Maya-Vyuha — Vedic Structured Dropout ॥\n")
    
    mv = MayaVyuha(dropout_rate=0.3, vyuha_type="chakra")
    
    # Demonstrate all 3 vyuha types
    for vyuha in ["chakra", "garuda", "golden_spiral"]:
        mv.vyuha_type = vyuha
        mask = mv.vyuha_mask((64,), step=0)
        obscured = sum(1 for m in mask if m == 0.0)
        print(f"  {vyuha:15s}: {obscured}/{len(mask)} obscured ({obscured/len(mask)*100:.0f}%)")
    
    print(f"\n  Maya density: {mv.obscuration_density():.3f}")
    print(f"  Brahman visible: {mv.brahman_factor():.3f}")
    
    # Show pattern evolution
    print("\n  Chakra-vyuha rotation:")
    mv.vyuha_type = "chakra"
    for step in [0, 16, 32, 48]:
        mask = mv.vyuha_mask((64,), step=step)
        # Visual representation
        viz = ''.join('░' if m == 0 else '█' for m in mask[:64])
        print(f"    Step {step:2d}: {viz}")
