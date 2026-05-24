"""
Tanmatra Fusion — 5-Element Multi-Modal Feed-Forward Network
Ported from VedaRta/kernels/tanmatra_fusion.h
Source: Samkhya — Pancha Tanmatra (5 subtle elements)

Shabda  → Sound/Audio data (voice commands, acoustic sensors)
Sparsha → Touch/Pressure data (soil compaction, tactile)
Rupa    → Form/Visual data (NDVI, satellite imagery)
Rasa    → Taste/Chemical data (pH, NPK, soil chemistry)
Gandha  → Smell/Volatile data (moisture, temperature, gas)

Each element has its own FFN branch. Output fused via Mahabhuta integration.
"""

from enum import IntEnum
from dataclasses import dataclass, field
from typing import List, Dict, Optional, Any
import math

PHI = 1.618033988749895


class TanmatraElement(IntEnum):
    SHABDA = 0   # Sound — voice, audio
    SPARSHA = 1  # Touch — pressure, tactile
    RUPA = 2     # Form — visual, imagery
    RASA = 3     # Taste — chemical, pH
    GANDHA = 4   # Smell — volatile, gas


@dataclass
class TanmatraInput:
    element: TanmatraElement
    values: List[float]
    confidence: float = 0.5
    source: str = ""


class TanmatraBranch:
    """Single Tanmatra FFN branch with element-specific processing."""
    
    def __init__(self, element: TanmatraElement, dim: int = 4):
        self.element = element
        self.dim = dim
        # Element-specific weights (simulated — real impl uses learned params)
        self.weights = [PHI ** (i % 3) for i in range(dim * dim)]
        self.bias = [0.0] * dim
    
    def forward(self, x: List[float]) -> List[float]:
        """Simple linear projection with PHI scaling."""
        if len(x) != self.dim:
            # Pad or truncate
            x = (x + [0.0] * self.dim)[:self.dim]
        
        output = [0.0] * self.dim
        for i in range(self.dim):
            for j in range(self.dim):
                output[i] += x[j] * self.weights[i * self.dim + j]
            output[i] += self.bias[i]
        
        # Element-specific activation
        if self.element == TanmatraElement.SHABDA:
            # Sound: oscillatory (sin)
            output = [math.sin(v * math.pi) for v in output]
        elif self.element == TanmatraElement.SPARSHA:
            # Touch: threshold gate
            output = [v if abs(v) > 0.1 else 0.0 for v in output]
        elif self.element == TanmatraElement.RUPA:
            # Form: contrast enhancement
            output = [math.tanh(v) for v in output]
        elif self.element == TanmatraElement.RASA:
            # Taste: logarithmic scaling (pH-like)
            output = [math.log1p(abs(v)) * (1 if v >= 0 else -1) for v in output]
        elif self.element == TanmatraElement.GANDHA:
            # Smell: exponential decay
            output = [v * math.exp(-abs(v) / PHI) for v in output]
        
        return output


class TanmatraFusion:
    """
    5-element multi-modal fusion network.
    
    Each Tanmatra processes its modality independently,
    then Mahabhuta integration fuses all into unified representation.
    
    Usage:
        fusion = TanmatraFusion(fusion_dim=8)
        fusion.add_input(TanmatraElement.RASA, [5.8, 25, 30, 35])  # Soil chemistry
        fusion.add_input(TanmatraElement.GANDHA, [45, 28, 80, 0.4])  # Moisture/temp
        unified = fusion.fuse()  # 8-dim unified embedding
    """
    
    def __init__(self, fusion_dim: int = 8):
        self.fusion_dim = fusion_dim
        self.branches: Dict[TanmatraElement, TanmatraBranch] = {}
        self.inputs: Dict[TanmatraElement, TanmatraInput] = {}
        
        # Initialize all 5 branches
        for element in TanmatraElement:
            self.branches[element] = TanmatraBranch(element, dim=4)
        
        # Mahabhuta fusion weights (Panchikarana)
        self._fusion_weights = [1.0 / 5.0] * 5  # Equal initial
    
    def add_input(self, element: TanmatraElement, values: List[float],
                  confidence: float = 0.5, source: str = ""):
        """Feed data into a Tanmatra element branch."""
        self.inputs[element] = TanmatraInput(
            element=element,
            values=values,
            confidence=confidence,
            source=source,
        )
    
    def fuse(self) -> List[float]:
        """
        Mahabhuta fusion: integrate all 5 Tanmatras.
        Missing elements contribute zero.
        """
        unified = [0.0] * self.fusion_dim
        
        for element in TanmatraElement:
            if element not in self.inputs:
                continue
            
            inp = self.inputs[element]
            branch = self.branches[element]
            
            # Process through element branch
            element_output = branch.forward(inp.values[:4])
            
            # Weighted contribution to fusion
            weight = inp.confidence * self._fusion_weights[int(element)]
            for i in range(min(len(element_output), self.fusion_dim)):
                unified[i] += element_output[i] * weight
        
        # Mahabhuta normalization (Panchikarana)
        norm = math.sqrt(sum(v * v for v in unified))
        if norm > 0:
            unified = [v / norm * PHI for v in unified]
        
        return unified
    
    def active_elements(self) -> List[TanmatraElement]:
        """List elements with active inputs."""
        return list(self.inputs.keys())
    
    def missing_elements(self) -> List[TanmatraElement]:
        """List elements without inputs (sensory gaps)."""
        return [e for e in TanmatraElement if e not in self.inputs]
    
    def mahabhuta_report(self) -> str:
        """Report on elemental balance."""
        lines = ["॥ पञ्च तन्मात्र — Pancha Tanmatra Fusion ॥", ""]
        
        element_names = {
            TanmatraElement.SHABDA: "🔊 Shabda (Sound)",
            TanmatraElement.SPARSHA: "🤚 Sparsha (Touch)",
            TanmatraElement.RUPA: "👁 Rupa (Form)",
            TanmatraElement.RASA: "👅 Rasa (Taste)",
            TanmatraElement.GANDHA: "👃 Gandha (Smell)",
        }
        
        for element in TanmatraElement:
            name = element_names[element]
            if element in self.inputs:
                inp = self.inputs[element]
                lines.append(f"  {name}: ACTIVE ({inp.source}, conf={inp.confidence:.2f})")
            else:
                lines.append(f"  {name}: — (missing)")
        
        if self.inputs:
            fused = self.fuse()
            lines.append(f"\n  Fused embedding ({len(fused)}D):")
            lines.append(f"  [{', '.join(f'{v:.3f}' for v in fused[:4])}...]")
        
        return "\n".join(lines)
    
    def clear(self):
        """Clear all inputs."""
        self.inputs.clear()


if __name__ == "__main__":
    fusion = TanmatraFusion(fusion_dim=8)
    
    # Agricultural multi-modal inputs
    fusion.add_input(
        TanmatraElement.RASA,  # Chemical
        [5.8, 25.0, 30.0, 35.0],  # pH, N, P, K
        confidence=0.9,
        source="soil_sensor"
    )
    
    fusion.add_input(
        TanmatraElement.GANDHA,  # Volatile
        [45.0, 28.0, 80.0, 0.4],  # moisture, temp, humidity, EC
        confidence=0.85,
        source="weather_station"
    )
    
    fusion.add_input(
        TanmatraElement.RUPA,  # Visual
        [0.65, 0.3, 0.8, 0.5],  # NDVI, leaf_area, greenness, canopy
        confidence=0.7,
        source="satellite_ndvi"
    )
    
    fusion.add_input(
        TanmatraElement.SHABDA,  # Sound
        [0.1, 0.05, 0.0, 0.0],  # pest_sound, wind, rain_acoustic, bird_call
        confidence=0.3,
        source="acoustic_sensor"
    )
    
    # No SPARSHA (touch/pressure) — missing
    
    print(fusion.mahabhuta_report())
    
    fused = fusion.fuse()
    print(f"\n  Missing elements: {[e.name for e in fusion.missing_elements()]}")
    print(f"  Active elements: {len(fusion.active_elements())}/5")
