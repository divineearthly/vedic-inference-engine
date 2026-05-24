"""
Para-Vak — 4-Tier Multilingual Embedding System
Source: Vakyapadiya 1.1 — Bhartrihari's 4 levels of speech

Para      → Pure concept (language-independent meaning core)
Pashyanti → Vision layer (semantic representation)
Madhyama  → Mental layer (language-specific encoding)
Vaikhari  → Spoken layer (surface text output)

Enables single embedding space for Hindi, Bengali, Assamese, Tamil.
Shruti-level meaning preserved across languages.
"""

from enum import IntEnum
from dataclasses import dataclass, field
from typing import List, Dict, Optional, Tuple
import math


class VakLevel(IntEnum):
    PARA = 0        # Pure concept — language-independent
    PASHYANTI = 1   # Vision — semantic form
    MADHYAMA = 2    # Mental — language encoding
    VAIKHARI = 3    # Spoken — surface text


# Core agricultural concepts in 4 languages (Para-level semantic anchors)
PARA_CONCEPTS = {
    "soil": {
        "hi": "मिट्टी", "bn": "মাটি", "as": "মাটি", "ta": "மண்",
        "en": "soil", "embedding": [0.9, 0.1, 0.05, 0.0],
    },
    "water": {
        "hi": "पानी", "bn": "জল", "as": "পানী", "ta": "நீர்",
        "en": "water", "embedding": [0.1, 0.9, 0.05, 0.0],
    },
    "seed": {
        "hi": "बीज", "bn": "বীজ", "as": "গুটি", "ta": "விதை",
        "en": "seed", "embedding": [0.05, 0.1, 0.9, 0.0],
    },
    "crop": {
        "hi": "फसल", "bn": "ফসল", "as": "শস্য", "ta": "பயிர்",
        "en": "crop", "embedding": [0.3, 0.6, 0.1, 0.0],
    },
    "harvest": {
        "hi": "फसल कटाई", "bn": "ফসল কাটা", "as": "চপোৱা", "ta": "அறுவடை",
        "en": "harvest", "embedding": [0.2, 0.3, 0.5, 0.0],
    },
    "fertilizer": {
        "hi": "खाद", "bn": "সার", "as": "সাৰ", "ta": "உரம்",
        "en": "fertilizer", "embedding": [0.4, 0.4, 0.2, 0.0],
    },
    "pesticide": {
        "hi": "कीटनाशक", "bn": "কীটনাশক", "as": "কীটনাশক", "ta": "பூச்சிக்கொல்லி",
        "en": "pesticide", "embedding": [0.0, 0.0, 0.0, 1.0],  # Tamasic — avoid
    },
    "irrigation": {
        "hi": "सिंचाई", "bn": "সেচ", "as": "জলসিঞ্চন", "ta": "நீர்ப்பாசனம்",
        "en": "irrigation", "embedding": [0.1, 0.8, 0.1, 0.0],
    },
    "rain": {
        "hi": "बारिश", "bn": "বৃষ্টি", "as": "বৰষুণ", "ta": "மழை",
        "en": "rain", "embedding": [0.05, 0.85, 0.1, 0.0],
    },
    "sun": {
        "hi": "धूप", "bn": "রোদ", "as": "ৰ'দ", "ta": "சூரியன்",
        "en": "sun", "embedding": [0.0, 0.5, 0.5, 0.0],
    },
    "cow": {
        "hi": "गाय", "bn": "গরু", "as": "গৰু", "ta": "பசு",
        "en": "cow", "embedding": [0.8, 0.2, 0.0, 0.0],  # Sattvic — sacred
    },
    "panchgavya": {
        "hi": "पंचगव्य", "bn": "পঞ্চগব্য", "as": "পঞ্চগব্য", "ta": "பஞ்சகவ்யா",
        "en": "panchgavya", "embedding": [1.0, 0.0, 0.0, 0.0],  # Pure Sattva
    },
}


@dataclass
class VakToken:
    """A token traversing the 4 levels of speech."""
    concept_key: str           # Para-level key
    text: str                  # Surface form
    language: str              # hi/bn/as/ta/en
    vak_level: VakLevel
    embedding: List[float] = field(default_factory=lambda: [0.0, 0.0, 0.0, 0.0])
    guna: str = "rajas"


class ParaVakEngine:
    """
    4-tier multilingual embedding engine.
    
    Para → Pashyanti → Madhyama → Vaikhari
    
    All 4 languages share the same Para (concept) space.
    Shruti-level meaning preserved across language boundaries.
    
    Usage:
        pv = ParaVakEngine()
        
        # Translate concept across languages
        concept = pv.to_para("मिट्टी", "hi")  # Hindi → Para concept
        bengali = pv.to_vaikhari("soil", "bn")  # Para → Bengali text
        assamese = pv.to_vaikhari("soil", "as")  # Para → Assamese text
        
        # Get language-independent embedding
        emb = pv.embed("মাটি", "bn")  # Bengali → Para embedding
    """
    
    def __init__(self):
        self.concepts = PARA_CONCEPTS
        # Build reverse index: language text → concept key
        self._text_index: Dict[str, Dict[str, str]] = {}
        for key, data in self.concepts.items():
            for lang in ["hi", "bn", "as", "ta", "en"]:
                if lang in data:
                    if lang not in self._text_index:
                        self._text_index[lang] = {}
                    self._text_index[lang][data[lang].lower()] = key
    
    def to_para(self, text: str, language: str) -> Optional[str]:
        """Convert surface text to Para concept key (language-independent)."""
        lang_dict = self._text_index.get(language, {})
        return lang_dict.get(text.lower())
    
    def to_vaikhari(self, concept_key: str, language: str) -> Optional[str]:
        """Convert Para concept to surface text in target language."""
        concept = self.concepts.get(concept_key, {})
        return concept.get(language)
    
    def to_pashyanti(self, concept_key: str) -> Optional[List[float]]:
        """Get Pashyanti (vision/semantic) embedding for a concept."""
        concept = self.concepts.get(concept_key, {})
        return concept.get("embedding")
    
    def embed(self, text: str, language: str) -> Optional[List[float]]:
        """Get Para-level embedding for any surface text."""
        concept_key = self.to_para(text, language)
        if concept_key:
            return self.to_pashyanti(concept_key)
        
        # Unknown word — return neutral embedding
        return [0.25, 0.25, 0.25, 0.25]
    
    def translate(self, text: str, from_lang: str, to_lang: str) -> Optional[str]:
        """Translate agricultural term between any supported languages."""
        concept_key = self.to_para(text, from_lang)
        if concept_key:
            return self.to_vaikhari(concept_key, to_lang)
        return None
    
    def similarity(self, text1: str, lang1: str, text2: str, lang2: str) -> float:
        """Compute cross-lingual semantic similarity via Para space."""
        emb1 = self.embed(text1, lang1)
        emb2 = self.embed(text2, lang2)
        
        if not emb1 or not emb2:
            return 0.0
        
        # Cosine similarity
        dot = sum(a * b for a, b in zip(emb1, emb2))
        norm1 = math.sqrt(sum(a * a for a in emb1))
        norm2 = math.sqrt(sum(b * b for b in emb2))
        
        if norm1 == 0 or norm2 == 0:
            return 0.0
        
        return dot / (norm1 * norm2)
    
    def guna_classify(self, concept_key: str) -> str:
        """Classify concept by Guna based on embedding."""
        emb = self.to_pashyanti(concept_key)
        if not emb:
            return "rajas"
        
        # Sattva dimension (index 0) high → sattvic
        # Tamas dimension (index 3) high → tamasic
        if emb[3] > 0.5:
            return "tamas"
        if emb[0] > 0.5:
            return "sattva"
        return "rajas"
    
    def list_concepts(self) -> List[str]:
        """List all Para-level concepts."""
        return list(self.concepts.keys())
    
    def list_languages(self) -> List[str]:
        """List supported languages."""
        return list(self._text_index.keys())
    
    def add_concept(self, key: str, translations: Dict[str, str],
                    embedding: List[float] = None):
        """Add a new concept to the Para-Vak space."""
        if embedding is None:
            embedding = [0.25, 0.25, 0.25, 0.25]
        
        self.concepts[key] = {"embedding": embedding}
        self.concepts[key].update(translations)
        
        for lang, text in translations.items():
            if lang not in self._text_index:
                self._text_index[lang] = {}
            self._text_index[lang][text.lower()] = key


if __name__ == "__main__":
    pv = ParaVakEngine()
    
    print("॥ Para-Vak — 4-Tier Multilingual Embedding ॥\n")
    
    # Demo 1: Cross-lingual translation
    print("=== Cross-Lingual Translation ===")
    hindi_word = "मिट्टी"
    concept = pv.to_para(hindi_word, "hi")
    print(f"  Hindi '{hindi_word}' → Para: '{concept}'")
    
    for lang in ["bn", "as", "ta", "en"]:
        translated = pv.to_vaikhari(concept, lang)
        print(f"    → {lang}: '{translated}'")
    
    # Demo 2: Cross-lingual similarity
    print("\n=== Cross-Lingual Similarity ===")
    pairs = [
        ("मिट्टी", "hi", "মাটি", "bn"),   # soil-soil: should be 1.0
        ("पानी", "hi", "জল", "bn"),        # water-water: should be 1.0
        ("बीज", "hi", "நீர்", "ta"),       # seed-water: should be low
        ("गाय", "hi", "পঞ্চগব্য", "bn"),   # cow-panchgavya: high (both Sattvic)
    ]
    
    for t1, l1, t2, l2 in pairs:
        sim = pv.similarity(t1, l1, t2, l2)
        print(f"  sim('{t1}'({l1}), '{t2}'({l2})) = {sim:.3f}")
    
    # Demo 3: Guna classification
    print("\n=== Guna Classification ===")
    for concept in ["soil", "panchgavya", "pesticide", "cow"]:
        guna = pv.guna_classify(concept)
        vaikhari = pv.to_vaikhari(concept, "en")
        print(f"  '{vaikhari}' → {guna.upper()}")
    
    # Demo 4: Para embedding space
    print("\n=== Para Embedding Space (4D) ===")
    for concept in ["panchgavya", "water", "seed", "pesticide"]:
        emb = pv.to_pashyanti(concept)
        vaikhari = pv.to_vaikhari(concept, "en")
        print(f"  {vaikhari:15s}: [{emb[0]:.1f}, {emb[1]:.1f}, {emb[2]:.1f}, {emb[3]:.1f}]")
    
    print(f"\n  Supported: {pv.list_languages()}")
    print(f"  Concepts: {len(pv.list_concepts())}")
