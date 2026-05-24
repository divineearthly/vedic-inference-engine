"""
Indra-Net — Inter-Repository Communication Bus
Source: Avatamsaka Sutra — Indra's Jeweled Net

Every node reflects every other node.
Shared Pramana bus for Krishi ↔ ASI ↔ KAVACH ↔ Rasa communication.
All repos communicate through a single unified protocol.
"""

from dataclasses import dataclass, field
from typing import List, Dict, Optional, Any, Callable
import time
import json

PHI = 1.618033988749895


class MessageType:
    """Indra-Net message types."""
    SENSOR_DATA = "sensor_data"
    PRAMANA_QUERY = "pramana_query"
    PRAMANA_RESPONSE = "pramana_response"
    AHIMSA_CHECK = "ahimsa_check"
    DHARMA_PRIORITY = "dharma_priority"
    KOSHA_STORE = "kosha_store"
    KOSHA_RECALL = "kosha_recall"
    MANAS_ATTEND = "manas_attend"
    SPHOTA_REQUEST = "sphota_request"
    ALERT = "alert"
    HEARTBEAT = "heartbeat"


@dataclass
class IndraMessage:
    """A message traveling through Indra's Net."""
    msg_id: str
    msg_type: str
    source_repo: str       # e.g., "krishi-veda", "divine-asi", "kavach"
    target_repo: str       # e.g., "*" for broadcast
    payload: Any
    pramana_source: str = "pratyaksha"
    confidence: float = 0.5
    timestamp: float = field(default_factory=time.time)
    ttl: int = 7           # Time-to-live (Indra's 7 jewels)


class IndraNet:
    """
    Indra's Jeweled Net — every jewel reflects all others.
    
    Unified communication bus for all Divine Earthly repos.
    Messages are Pramana-tagged and Dharma-prioritized.
    
    Usage:
        net = IndraNet()
        
        # Register repos
        net.register("krishi-veda")
        net.register("divine-asi")
        net.register("kavach")
        
        # Send message
        msg = net.send("krishi-veda", "*", MessageType.SENSOR_DATA,
                        {"ph": 5.8, "n": 25}, pramana="pratyaksha")
        
        # Receive messages
        msgs = net.receive("kavach")
    """
    
    def __init__(self):
        self.repos: Dict[str, Dict] = {}          # Registered repos
        self._message_queue: List[IndraMessage] = []  # Jeweled net
        self._msg_counter: int = 0
        self._handlers: Dict[str, List[Callable]] = {}  # Message handlers
    
    def register(self, repo_id: str, metadata: Dict = None):
        """Register a repository in Indra's Net."""
        self.repos[repo_id] = {
            "id": repo_id,
            "registered_at": time.time(),
            "metadata": metadata or {},
            "messages_sent": 0,
            "messages_received": 0,
            "online": True,
        }
        self._handlers[repo_id] = []
        return True
    
    def on_message(self, repo_id: str, handler: Callable):
        """Register a message handler for a repo."""
        if repo_id in self._handlers:
            self._handlers[repo_id].append(handler)
    
    def send(self, source: str, target: str, msg_type: str,
             payload: Any, pramana: str = "pratyaksha",
             confidence: float = 0.5, ttl: int = 7) -> IndraMessage:
        """Send a message through Indra's Net."""
        self._msg_counter += 1
        
        msg = IndraMessage(
            msg_id=f"indra-{self._msg_counter:06d}",
            msg_type=msg_type,
            source_repo=source,
            target_repo=target,
            payload=payload,
            pramana_source=pramana,
            confidence=confidence,
            ttl=ttl,
        )
        
        self._message_queue.append(msg)
        
        if source in self.repos:
            self.repos[source]["messages_sent"] += 1
        
        # Deliver to target immediately if handlers exist
        if target in self._handlers:
            for handler in self._handlers[target]:
                try:
                    handler(msg)
                except:
                    pass
        elif target == "*":
            # Broadcast
            for repo_id, handlers in self._handlers.items():
                if repo_id != source:
                    for handler in handlers:
                        try:
                            handler(msg)
                        except:
                            pass
        
        return msg
    
    def receive(self, repo_id: str, msg_type: str = None) -> List[IndraMessage]:
        """Receive messages for a repo from Indra's Net."""
        messages = []
        remaining = []
        
        for msg in self._message_queue:
            is_target = (msg.target_repo == repo_id or 
                        msg.target_repo == "*")
            
            if is_target and (msg_type is None or msg.msg_type == msg_type):
                messages.append(msg)
                if repo_id in self.repos:
                    self.repos[repo_id]["messages_received"] += 1
            else:
                remaining.append(msg)
        
        self._message_queue = remaining
        return messages
    
    def broadcast_alert(self, source: str, alert_type: str, 
                        details: Any, urgency: str = "normal"):
        """Broadcast an alert to all repos."""
        return self.send(
            source=source,
            target="*",
            msg_type=MessageType.ALERT,
            payload={
                "alert_type": alert_type,
                "details": details,
                "urgency": urgency,
            },
            pramana="pratyaksha",
            confidence=0.9 if urgency == "crisis" else 0.7,
        )
    
    def status(self) -> Dict:
        """Indra-Net status report."""
        return {
            "jewels": len(self.repos),
            "messages_in_net": len(self._message_queue),
            "total_sent": self._msg_counter,
            "repos": {
                rid: {
                    "online": r["online"],
                    "sent": r["messages_sent"],
                    "received": r["messages_received"],
                }
                for rid, r in self.repos.items()
            },
        }
    
    def cleanup(self, max_age_sec: float = 3600):
        """Remove expired messages (TTL exceeded or too old)."""
        now = time.time()
        self._message_queue = [
            m for m in self._message_queue
            if m.ttl > 0 and (now - m.timestamp) < max_age_sec
        ]
        # Decrement TTL
        for m in self._message_queue:
            m.ttl -= 1


if __name__ == "__main__":
    net = IndraNet()
    
    # Register the repositories
    net.register("krishi-veda", {"role": "agricultural_agent"})
    net.register("divine-asi", {"role": "reasoning_core"})
    net.register("kavach", {"role": "security_shield"})
    net.register("rasa-engine", {"role": "voice_interface"})
    
    print("॥ इन्द्रजाल — Indra's Jeweled Net ॥\n")
    
    # Krishi sends sensor data
    net.send("krishi-veda", "divine-asi", MessageType.SENSOR_DATA,
             {"ph": 5.8, "n": 25, "p": 30, "k": 35},
             pramana="pratyaksha", confidence=0.95)
    
    # Divine-ASI responds with reasoning
    net.send("divine-asi", "krishi-veda", MessageType.PRAMANA_RESPONSE,
             {"diagnosis": "acidic_soil", "confidence": 0.82},
             pramana="anumana", confidence=0.82)
    
    # KAVACH performs Ahimsa check
    net.send("kavach", "*", MessageType.AHIMSA_CHECK,
             {"substance": "urea", "verdict": "blocked"},
             pramana="shabda", confidence=0.90)
    
    # Broadcast alert
    net.broadcast_alert("kavach", "pest_crisis",
                         "Aphid infestation detected in Barak Valley",
                         urgency="crisis")
    
    # Receive messages for each repo
    for repo_id in ["krishi-veda", "divine-asi", "kavach"]:
        msgs = net.receive(repo_id)
        print(f"  {repo_id}: {len(msgs)} messages")
        for m in msgs:
            print(f"    ← {m.source_repo}: [{m.msg_type}] {str(m.payload)[:60]}...")
    
    print(f"\n  Net status: {net.status()}")
