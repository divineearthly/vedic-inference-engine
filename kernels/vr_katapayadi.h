#pragma once
inline int katapayadi_value(const char* syllable) {
    static const char* map = "k1K2g3G4N5c1C2j3J4Y5t1T2d3D4n5p1P2b3B4m5y1r2l3v4s5S6x7h8";
    if (!syllable[0]) return 0;
    for(int i=0; map[i]; i+=2) if(syllable[0]==map[i]) return map[i+1]-'0';
    return 0;
}
inline float katapayadi_embed(const char* text, int dim_index, int total_dim) {
    float val = 0;
    for(int i=0; text[i]; i++) { int kp = katapayadi_value(&text[i]); if(kp > 0) val += sinf(kp * dim_index * 1.618f / total_dim) * 0.1f; }
    return val;
}
