"""Guna-Neuron Weighting — ported from VedaRta/kernels/vr_guna.h
Source: Sankhya philosophy, Bhagavad Gita 14.5-18
Sattva (protection) · Rajas (plasticity) · Tamas (pruning)
Pure Python. No C dependency."""

from enum import IntEnum
from typing import List


class Guna(IntEnum):
    SATTVA = 0   # Protected knowledge — low learning rate
    RAJAS = 1    # Plastic/active — high learning rate
    TAMAS = 2    # Dormant/pruned — zero learning rate


class GunaNeuron:
    """Single neuron with Guna-weighted learning.
    
    Sattvic neurons: lr * 0.1 (protected knowledge)
    Rajasic neurons: lr * 5.0 (plastic, fast learning)
    Tamasic neurons: lr * 0.0 (pruned, dormant)
    """
    
    def __init__(self, guna: Guna = Guna.RAJAS, value: float = 0.0):
        self.guna = guna
        self.value = value
        self._lr_multipliers = {
            Guna.SATTVA: 0.1,
            Guna.RAJAS: 5.0,
            Guna.TAMAS: 0.0
        }
    
    def effective_lr(self, base_lr: float) -> float:
        """Return guna-weighted learning rate."""
        return base_lr * self._lr_multipliers[self.guna]
    
    def update(self, gradient: float, base_lr: float = 0.01):
        """Apply gradient with guna-weighted learning rate."""
        self.value -= gradient * self.effective_lr(base_lr)
    
    def promote(self):
        """Tamas → Rajas, Rajas → Sattva (knowledge consolidation)."""
        if self.guna == Guna.TAMAS:
            self.guna = Guna.RAJAS
        elif self.guna == Guna.RAJAS:
            self.guna = Guna.SATTVA
    
    def demote(self):
        """Sattva → Rajas, Rajas → Tamas (forgetting/unlearning)."""
        if self.guna == Guna.SATTVA:
            self.guna = Guna.RAJAS
        elif self.guna == Guna.RAJAS:
            self.guna = Guna.TAMAS


class GunaLayer:
    """Layer of GunaNeurons with collective Guna distribution tracking."""
    
    def __init__(self, size: int):
        self.neurons: List[GunaNeuron] = [
            GunaNeuron(Guna.RAJAS) for _ in range(size)
        ]
    
    def guna_distribution(self) -> dict:
        """Count neurons by Guna."""
        counts = {Guna.SATTVA: 0, Guna.RAJAS: 0, Guna.TAMAS: 0}
        for n in self.neurons:
            counts[n.guna] += 1
        return {g.name: c for g, c in counts.items()}
    
    def prunable(self) -> List[int]:
        """Return indices of Tamasic neurons (can be pruned)."""
        return [i for i, n in enumerate(self.neurons) if n.guna == Guna.TAMAS]
    
    def sattvic_ratio(self) -> float:
        """Fraction of protected Sattvic knowledge."""
        sattva = sum(1 for n in self.neurons if n.guna == Guna.SATTVA)
        return sattva / len(self.neurons) if self.neurons else 0.0


if __name__ == "__main__":
    layer = GunaLayer(10)
    
    # Consolidate some knowledge
    layer.neurons[0].promote()  # Rajas → Sattva
    layer.neurons[0].promote()  # (already Sattva, stays)
    layer.neurons[1].demote()   # Rajas → Tamas
    layer.neurons[2].demote()   # Rajas → Tamas
    
    print(f"Distribution: {layer.guna_distribution()}")
    print(f"Sattvic ratio: {layer.sattvic_ratio():.2f}")
    print(f"Prunable indices: {layer.prunable()}")
    
    # Test learning rate scaling
    sattvic = layer.neurons[0]
    rajasic = layer.neurons[3]
    tamasic = layer.neurons[1]
    
    print(f"\nBase LR=0.01 → Sattvic: {sattvic.effective_lr(0.01):.4f}, "
          f"Rajasic: {rajasic.effective_lr(0.01):.4f}, "
          f"Tamasic: {tamasic.effective_lr(0.01):.4f}")
