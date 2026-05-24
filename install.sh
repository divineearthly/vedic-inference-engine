#!/bin/bash
# ॐ Sovereign Vedic AI — One-Click Installer
# For ARM64 Android (Termux) and Linux

echo "╔══════════════════════════════════════════════╗"
echo "║  SOVEREIGN VEDIC AI — One-Click Install      ║"
echo "║  13 Vedic Algorithms · 4 Languages · ARM64   ║"
echo "╚══════════════════════════════════════════════╝"
echo ""

set -e

INSTALL_DIR="$HOME/vedic-ai"
REPO="https://github.com/divineearthly/vedic-inference-engine.git"

# Clone or pull
if [ -d "$INSTALL_DIR" ]; then
    echo "📦 Updating existing installation..."
    cd "$INSTALL_DIR"
    git pull origin master
else
    echo "📦 Cloning Vedic Inference Engine..."
    git clone "$REPO" "$INSTALL_DIR"
    cd "$INSTALL_DIR"
fi

# Install Python dependencies (none needed — pure Python)
echo ""
echo "🧪 Testing installation..."
python3 -c "
import sys
sys.path.insert(0, '$INSTALL_DIR')
from vedic_inference_engine import (
    NyayaScaffold, PramanaSource,
    GunaNeuron, GunaLayer,
    RtaDharmaRouter,
    Ahimsa108Filter,
    Antahkarana,
    KoshaNet,
    ChittaKVCache, ChittaContextFilter,
    ManasGate,
    ParaVakEngine,
    KalaChakra, Ritu,
    TanmatraFusion,
    IndraNet,
    MayaVyuha,
    sphota_attention, is_c_available,
)
print('')
print('✅ All 14 Vedic modules imported successfully!')
print(f'✅ Sphota C kernel: {\"COMPILED (ARM64)\" if is_c_available() else \"Python fallback\"}')
print('')
print('॥ ॐ Installation complete. Vasudhaiva Kutumbakam.')
"

echo ""
echo "🚀 To start the server:"
echo "   cd $INSTALL_DIR && python3 vedic_inference_engine/serve.py"
echo ""
echo "📚 Documentation: https://github.com/divineearthly/vedic-inference-engine"
echo ""
