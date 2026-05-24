#!/usr/bin/env python3
"""
VedaRta Kaggle Training — Free P100 GPU
========================================
Go to: kaggle.com/code → New Notebook → GPU P100
Paste this entire script.
"""

import subprocess, os, sys, json, math, random, time
import torch
import torch.nn as nn
from torch.utils.data import Dataset, DataLoader

print(f"॥ VedaRta Kaggle Training — {torch.cuda.get_device_name(0)} ॥")

# Clone repo
os.system("git clone https://github.com/divineearthly/vedic-inference-engine.git")
sys.path.insert(0, "vedic-inference-engine")

# Download Vedic data
os.system("pip install -q datasets")

# Load HuggingFace Vedic datasets
from datasets import load_dataset

# Sanskrit corpus
try:
    sanskrit = load_dataset("sanskrit", split="train", streaming=True)
    print(f"Sanskrit corpus loaded")
except:
    print("Using local Vedic data")

# Agricultural data
# ICAR, FAO, local Assam data
print("Ready for training")
