"""
Kala-Chakra — Cyclic Positional Encoding for Seasonal Data
Ported from VedaRta/kernels/vr_kala_chakra.h
Source: Jyotisha, Vedic time cycles (Yuga, Manvantara, Ritu)

Replaces sinusoidal positional encoding with cyclic encoding
based on Vedic time units: Muhurta → Tithi → Paksha → Ritu → Ayana → Samvatsara
Perfect for agricultural data with seasonal patterns.
"""

from enum import IntEnum
import math
from typing import List
from dataclasses import dataclass

PHI = 1.618033988749895


class Ritu(IntEnum):
    """Six Vedic seasons."""
    VASANTA = 0    # Spring (Mar-May)
    GRISHMA = 1    # Summer (May-Jul)
    VARSHA = 2     # Monsoon (Jul-Sep)
    SHARAD = 3     # Autumn (Sep-Nov)
    HEMANTA = 4    # Pre-winter (Nov-Jan)
    SHISHIRA = 5   # Winter (Jan-Mar)


class Tithi(IntEnum):
    """30 lunar tithis in a month."""
    PRATIPADA = 1
    DVITIYA = 2
    TRITIYA = 3
    CHATURTHI = 4
    PANCHAMI = 5
    SHASHTI = 6
    SAPTAMI = 7
    ASHTAMI = 8
    NAVAMI = 9
    DASHAMI = 10
    EKADASHI = 11
    DVADASHI = 12
    TRAYODASHI = 13
    CHATURDASHI = 14
    PURNIMA = 15       # Full moon
    AMAVASYA = 30      # New moon


class Paksha(IntEnum):
    SHUKLA = 0   # Waxing
    KRISHNA = 1  # Waning


@dataclass
class VedicDate:
    """A point in Vedic cyclical time."""
    samvatsara: int = 0       # Year
    ayana: int = 0            # 0=Uttarayana, 1=Dakshinayana
    ritu: Ritu = Ritu.VASANTA
    paksha: Paksha = Paksha.SHUKLA
    tithi: int = 1            # 1-30
    day_of_year: int = 1      # 1-365


class KalaChakra:
    """
    Cyclic positional encoding based on Vedic time cycles.
    
    Unlike linear positions (sinusoidal), Kala-Chakra encodes time
    as nested cycles: day → tithi → paksha → ritu → ayana → year.
    This captures seasonal agricultural patterns naturally.
    
    Usage:
        kc = KalaChakra(embedding_dim=8)
        date = VedicDate(ritu=Ritu.VARSHA, tithi=10, day_of_year=180)
        encoding = kc.encode(date)  # 8-dim cyclic embedding
    """
    
    def __init__(self, embedding_dim: int = 8):
        self.dim = embedding_dim
    
    def encode(self, date: VedicDate) -> List[float]:
        """
        Generate Kala-Chakra embedding for a Vedic date.
        Embeds multiple nested cycles into a single vector.
        """
        emb = [0.0] * self.dim
        
        # Cycle 1: Daily cycle (Muhurta rhythm)
        # 1 year = 360°, day_of_year maps to angle
        day_angle = 2 * math.pi * date.day_of_year / 365.0
        if self.dim >= 2:
            emb[0] = math.sin(day_angle) * PHI
            emb[1] = math.cos(day_angle) * PHI
        
        # Cycle 2: Tithi cycle (lunar month, 30 days)
        tithi_angle = 2 * math.pi * date.tithi / 30.0
        if self.dim >= 4:
            emb[2] = math.sin(tithi_angle)
            emb[3] = math.cos(tithi_angle)
        
        # Cycle 3: Ritu cycle (6 seasons)
        ritu_angle = 2 * math.pi * int(date.ritu) / 6.0
        if self.dim >= 6:
            emb[4] = math.sin(ritu_angle) * (1.0 / PHI)
            emb[5] = math.cos(ritu_angle) * (1.0 / PHI)
        
        # Cycle 4: Paksha cycle (waxing/waning)
        paksha_val = 1.0 if date.paksha == Paksha.SHUKLA else -1.0
        if self.dim >= 7:
            emb[6] = paksha_val * (1.0 / (PHI * PHI))
        
        # Cycle 5: Ayana cycle (northward/southward sun)
        ayana_val = 1.0 if date.ayana == 0 else -1.0
        if self.dim >= 8:
            emb[7] = ayana_val * (1.0 / (PHI * PHI * PHI))
        
        return emb
    
    def seasonal_similarity(self, ritu1: Ritu, ritu2: Ritu) -> float:
        """Compute cyclic similarity between two seasons."""
        angle1 = 2 * math.pi * int(ritu1) / 6.0
        angle2 = 2 * math.pi * int(ritu2) / 6.0
        
        emb1 = [math.sin(angle1), math.cos(angle1)]
        emb2 = [math.sin(angle2), math.cos(angle2)]
        
        dot = emb1[0] * emb2[0] + emb1[1] * emb2[1]
        norm1 = math.sqrt(emb1[0]**2 + emb1[1]**2)
        norm2 = math.sqrt(emb2[0]**2 + emb2[1]**2)
        
        return dot / (norm1 * norm2) if norm1 * norm2 > 0 else 0.0
    
    def ritu_for_day(self, day_of_year: int) -> Ritu:
        """Map day of year to Ritu (approximate for Indian climate)."""
        # Simplified mapping for Assam climate
        if day_of_year <= 60:    return Ritu.SHISHIRA   # Jan-Feb
        elif day_of_year <= 121: return Ritu.VASANTA    # Mar-Apr
        elif day_of_year <= 182: return Ritu.GRISHMA    # May-Jun
        elif day_of_year <= 243: return Ritu.VARSHA     # Jul-Aug
        elif day_of_year <= 304: return Ritu.SHARAD     # Sep-Oct
        else:                    return Ritu.HEMANTA    # Nov-Dec
    
    def sowable(self, ritu: Ritu, crop_type: str = "rice") -> bool:
        """Check if a Ritu is suitable for sowing a crop."""
        # Sali rice: sown in Grishma-Varsha (May-Aug)
        # Boro rice: sown in Hemanta-Shishira (Nov-Feb)
        sowable_ritus = {
            "rice_sali": [Ritu.GRISHMA, Ritu.VARSHA],
            "rice_boro": [Ritu.HEMANTA, Ritu.SHISHIRA],
            "mustard": [Ritu.SHARAD, Ritu.HEMANTA],
            "vegetables": [Ritu.VASANTA, Ritu.SHARAD],
        }
        return ritu in sowable_ritus.get(crop_type, [])


if __name__ == "__main__":
    kc = KalaChakra(embedding_dim=8)
    
    print("॥ काल-चक्र — Kala-Chakra Cyclic Encoding ॥\n")
    
    # Encode different seasons
    dates = [
        VedicDate(ritu=Ritu.VASANTA, tithi=15, day_of_year=75, paksha=Paksha.SHUKLA),
        VedicDate(ritu=Ritu.VARSHA, tithi=10, day_of_year=195, paksha=Paksha.SHUKLA),
        VedicDate(ritu=Ritu.SHARAD, tithi=30, day_of_year=275, paksha=Paksha.KRISHNA),
        VedicDate(ritu=Ritu.HEMANTA, tithi=1, day_of_year=335, paksha=Paksha.SHUKLA),
    ]
    
    ritu_names = {
        Ritu.VASANTA: "Vasanta (Spring)",
        Ritu.GRISHMA: "Grishma (Summer)",
        Ritu.VARSHA: "Varsha (Monsoon)",
        Ritu.SHARAD: "Sharad (Autumn)",
        Ritu.HEMANTA: "Hemanta (Pre-winter)",
        Ritu.SHISHIRA: "Shishira (Winter)",
    }
    
    for date in dates:
        emb = kc.encode(date)
        ritu_name = ritu_names[date.ritu]
        paksha = "Shukla" if date.paksha == Paksha.SHUKLA else "Krishna"
        print(f"  {ritu_name:25s} | Tithi {date.tithi:2d} {paksha:7s} | Day {date.day_of_year:3d}")
        print(f"    Embedding: [{emb[0]:7.4f}, {emb[1]:7.4f}, {emb[2]:7.4f}, {emb[3]:7.4f}, ...]")
        print()
    
    # Seasonal similarity
    print("  Seasonal Similarity Matrix:")
    for r1 in Ritu:
        row = []
        for r2 in Ritu:
            sim = kc.seasonal_similarity(r1, r2)
            row.append(f"{sim:5.2f}")
        print(f"    {ritu_names[r1]:20s}: {' '.join(row)}")
    
    # Sowing check
    print(f"\n  Sowing suitability:")
    today = VedicDate(ritu=kc.ritu_for_day(165), day_of_year=165)
    print(f"    Day 165 = {ritu_names[today.ritu]}")
    print(f"    Sali rice sowable: {kc.sowable(today.ritu, 'rice_sali')}")
    print(f"    Mustard sowable: {kc.sowable(today.ritu, 'mustard')}")
