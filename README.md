# libsahm
A bare-metal C library implementing a Hyper-Modular Arithmetic System via RNS and Geometric Algebra.
# SAHM: Hyper-Modular Arithmetic System v4.1 
> *Redesigning computer arithmetic from the ALU up.*

**SAHM** (Sistema Aritmético Hiper-Modular) is an experimental bare-metal C library that reimplements basic arithmetic operations, bypassing traditional floating-point limitations and standard integer overflows.

This project merges concepts from modern cryptography (**Residue Number Systems / Chinese Remainder Theorem**) with two-dimensional topology (**Geometric Algebra and Euler Rotations**) to build a computational hyperspace where negative magnitudes do not exist.

##  Core Concepts

1. **Remainder Isolation (RNS):** Numbers do not dissolve into infinite decimals. They are packed into a "coprime prism" (base `{2, 3, 5}`). Addition, subtraction, and multiplication occur in parallel without propagating intermediate carries.
2. **Strict Sign Rotor:** Negativity is treated as a geometric rotation ($\pi$ radians or `180°`), not as a magnitude less than zero. This is implemented using Binary Angular Measure (BAM), where the most significant bit (`0x80000000`) dictates the vector's direction.
3. **Logic Gate Sign Law:** Multiplying signs is reduced to a single, zero-cost processor instruction: a bitwise `XOR` operation on the angular vectors.
4. **Hybrid ALU:** Integrates internal CRT (Chinese Remainder Theorem) decoders to seamlessly manage master carries and exact divisions without breaking structural integrity.

## ⚙️ Specimen Structure (Memory Footprint)

Every classic integer is encoded into a `UnifiedModZ` struct, designed for optimal register alignment:

```c
typedef struct {
    uint32_t q;                         // Master Cycles (Real Domain)
    uint32_t c_rem[NUM_COPRIMES];       // Isolated Residue Spectrum
    uint32_t theta;                     // BAM Direction: 0x00000000 (+) | 0x80000000 (-)
    uint8_t lazy_sqrt;                  // Lazy evaluation state flag
} UnifiedModZ;
