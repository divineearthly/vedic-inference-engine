"""Vedic Inference Engine — unified Vedic algorithm imports for all Divine Earthly repos.
सर्वं खल्विदं ब्रह्म — All this is Brahman

13 modules — Phase 5 Samadhi complete.
The full Sovereign Vedic AI stack.
"""

from .pramana import NyayaScaffold, PramanaSource, PramanaToken
from .guna import Guna, GunaNeuron, GunaLayer
from .rta_dharma import RtaDharmaRouter, DharmaPriority
from .ahimsa108 import Ahimsa108Filter, AhimsaLevel, AhimsaVerdict
from .antahkarana import Antahkarana, AntahkaranaPart, SensoryStream, BuddhiDecision
from .kosha import KoshaNet, KoshaLayer, KoshaEntry
from .chitta_kv import ChittaKVCache, ChittaContextFilter, ChittaGuna
from .sphota import sphota_attention, sphota_speedup_estimate, is_c_available
from .manas_gate import ManasGate, SensoryPriority
from .para_vak import ParaVakEngine, VakLevel, VakToken
from .tanmatra import TanmatraFusion, TanmatraElement, TanmatraInput
from .kala_chakra import KalaChakra, Ritu, Tithi, Paksha, VedicDate
from .indra_net import IndraNet, IndraMessage, MessageType

__all__ = [
    # Phase 1-4
    "NyayaScaffold", "PramanaSource", "PramanaToken",
    "Guna", "GunaNeuron", "GunaLayer",
    "RtaDharmaRouter", "DharmaPriority",
    "Ahimsa108Filter", "AhimsaLevel", "AhimsaVerdict",
    "Antahkarana", "AntahkaranaPart", "SensoryStream", "BuddhiDecision",
    "KoshaNet", "KoshaLayer", "KoshaEntry",
    "ChittaKVCache", "ChittaContextFilter", "ChittaGuna",
    "sphota_attention", "sphota_speedup_estimate", "is_c_available",
    "ManasGate", "SensoryPriority",
    "ParaVakEngine", "VakLevel", "VakToken",
    # Phase 5
    "TanmatraFusion", "TanmatraElement", "TanmatraInput",
    "KalaChakra", "Ritu", "Tithi", "Paksha", "VedicDate",
    "IndraNet", "IndraMessage", "MessageType",
]

# Phase 5+ additions
from .maya_vyuha import MayaVyuha
