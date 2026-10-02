# Collatzology & Arithmetic Chiral Topodynamics (ACT): Decoding Extended Collatz Dynamics

**License:** MIT | **Core Architecture:** C++17 (Arbitrary-Precision & O(1) Memory)
**Preprint:** [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.21996041.svg)](https://doi.org/10.5281/zenodo.21996041)

Welcome to the official repository for Collatzology and the Arithmetic Chiral Topodynamics (ACT) framework. This repository hosts the theoretical groundwork and the arbitrary-precision C++ engines—collectively known as the AACC-Collatz-Engine—used to simulate, fuzz, and definitively dissect the Extended Collatz Function (ECF).
Note: AACC stands for Absolute Asymptotic Convergence to a Cycle, which is the foundational theorem this engine is built to test and verify.

## 1. An overview of the Conway Undecidability & The Collatz "USB" Protocol

In 1972, John Conway proved that a natural generalization of the Collatz conjecture is algorithmically undecidable (Turing complete). However, Conway achieved this by mapping rational numbers (fractions like 3/2 or 5/2) to specific moduli, effectively building a linguistic FRACTRAN machine disguised as a dynamical system.
The ACT framework exposes a profound mathematical discovery. By strictly enforcing the physical iron law of the original problem—pure integer math and absolute parity-driven reduction (if even, x/2) in a restricted class of Modulo-4 Collatz map —the system loses its Turing completeness and collapses into a pure algebraic geometry problem.

## 2. The Extended Collatz Function (ECF)
The Extended Collatz Function (ECF) system is denoted as ECF(N₁x+p₁, N₂x+p₂) and is defined by:

* **f(x) = N₁x + p₁** (when x ≡ 1 mod 4)
* **f(x) = N₂x + p₂** (when x ≡ 3 mod 4)
* **f(x) = x / 2** (when x is even)

Here, N₁, N₂, p₁, p₂ are any odd integers (Z⁺ or Z⁻), and the starting seed x is any non-zero integer.

The "USB" Backward Compatibility
To prevent any misconception that ECF is an unrelated arbitrary invention, we must explicitly demonstrate its perfect mathematical backward compatibility. ECF is the "USB 3.0" of Collatz dynamics:
USB 1.0 (The Classic 3x+1): By simply setting N₁ = N₂ = 3 and p₁ = p₂ = 1, the modulo-4 branches merge seamlessly back into a single 3x+1 operation for all odd numbers.
USB 2.0 (The Generalized Nx+p): By setting N₁ = N₂ = N and p₁ = p₂ = p, it perfectly replicates any symmetric generalized Collatz system.

## 3. Chiral Non-Commutativity & Symmetry Breaking
Individually, both the 3x+1 and 3x-1 systems are universally recognized to exhibit absolute convergence under standard modulo-2 mappings. However, when we broaden the analytical scope to a modulo-4 **Extended Collatz Function (ECF)**, a startling topological reality emerges. 

While symmetric baselines deterministically converge, generating an asymmetric hybrid yields unbelievably counterintuitive results:
* **The Paradox of Repulsion:** The specific configuration ECF(3x-1, 3x+1) triggers astonishing divergence. Strikingly, merely swapping the modular assignments to ECF(3x+1, 3x-1) violently restores absolute convergence.
* **The Paradox of Capture:** High-multiplier systems like 7x+1 or 9x+1 independently undergo expected divergence. Yet, when hybridized, the cross-chiral interaction achieves the impossible: ECF(7x+1, 9x+1) forcibly collapses into absolute convergence, whereas ECF(9x+1, 7x+1) maintains the anticipated divergence.

This empirical reality, where ECF(A, B) ≠ ECF(B, A), strongly suggests the existence of a chiral non-commutative structure and exposes a fundamental symmetry breaking phenomenon within discrete dynamical systems.

## 4. The Universal Topological Invariant (Δ')
To formalize this topological mechanism, we derive a universal topological invariant, the **Net Drift Discriminant (Δ')**. For standard symmetric systems (N₁=N₂, p₁=p₂), the macroscopic invariant is rigidly locked at:

> **Δ' = (ln|N| / ln 2) - 2**

For generalized asymmetric ECF systems, the topological framework expands into a unified discriminant:

> **Δ' = [ln√(|N₁ · N₂|) / ln 2] - ρ** 
*(where quantized Lattice Gravity ρ ∈ {1, 2, 3})*

## 5. Predicting System Fate *A Priori*
You can think of Δ' acting exactly like the discriminant (b² - 4ac) in quadratic equations to determine the system's fate *a priori*:

* If **Δ' < 0**, the system achieves AACC (Absolute Asymptotic Convergence to a Cycle). It universally condenses into a finite periodic loop, rendering the chaotic variance of any microscopic seed irrelevant.
* If **Δ' > 0**, the Collatz system is **contingently divergent** (it will diverge to infinity unless it gets prematurely trapped in a periodic loop for some specific seed; it is case-sensitive).

*(Note: Δ' can never be exactly 0. Since N₁ and N₂ are odd, the natural log fraction is never an integer, while ρ is always an integer. Therefore, either Unconditional Convergence or Contingent Divergence is the inevitable outcome. No ambiguity exists in the system.)*

### Determining Lattice Gravity (ρ)
How do we determine ρ? The rule is explicitly deduced based on the combined congruence of the system parameters. We evaluate (N₁ + p₁, N₂ + p₂) mod 4 and map it to a base state (C₁, C₂) mod 4.

By extension, this mathematically guarantees that adding any multiple of 4 (4u, 4v) preserves the exact same lattice gravity:
**(N₁ + p₁ + 4u, N₂ + p₂ + 4v) ≡ (N₁ + p₁, N₂ + p₂) ≡ (C₁, C₂) mod 4**

Mapping this to (C₁, C₂) mod 4 yields three regimes:
* **Regime I (ρ=1):** (C₁, C₂) ≡ (2, 0) mod 4
* **Regime II (ρ=2):** (C₁, C₂) ≡ (2, 2) or (0, 0) mod 4
* **Regime III (ρ=3):** (C₁, C₂) ≡ (0, 2) mod 4

### Practical Examples
**1. The classic 3x+1:** ECF(3x+1, 3x+1)
(C₁, C₂) = (3+1, 3+1) ≡ (0, 0) mod 4 → ρ = 2
Δ' = (ln 3 / ln 2) - 2 ≈ -0.415 < 0
*(This clearly demonstrates it as an unconditional convergence event.)*

**2. A massive asymmetric hybrid:** ECF(7x+1, 9x+1)
(C₁, C₂) = (7+1, 9+1) ≡ (0, 2) mod 4 → ρ = 3
Δ' = [ln√(7 · 9) / ln 2] - 3 ≈ 2.9886 - 3 = -0.011 < 0
*(This demarcates it as an unconditional convergence event, completely overriding the huge multipliers.)*

Operating entirely *a priori*, this Δ' serves as the absolute physical boundary governing the macroscopic destiny of any generalized Collatz dynamical system. 

### 🛡️ Formal Verification (Lean 4)
The macroscopic flow conservation laws and the quantized lattice gravity (ρ ∈ {1, 2, 3}) derived within the ACT framework have been formally verified using the **Lean 4** theorem prover. 
The absolute mathematical certainty of this algebraic routing mechanics is available in the formal proof file: [`ACT_Conservation.lean`](./ACT_Conservation.lean). For full theoretical breakdown, please see the [Zenodo Preprint](https://doi.org/10.5281/zenodo.21996041).

---

## The AACC Challenge: Call for Counter-Examples

We fully acknowledge that proposing a strictly deterministic "hidden variable" within a system historically defined by pseudo-random chaos is highly counterintuitive. Therefore, we invite the global scientific and hacker communities to test the absolute predictive power of this framework.

Using the provided C++ arbitrary-precision engines (or your own code according to the above rules), find a single generalized ECF configuration and a starting seed x₀ such that **Δ' < 0**, but the trajectory diverges to infinity or violates the deterministic lattice gravity bounds.

If a valid counter-example is found, the determinism of this framework must be revised. So far, extensive arbitrary-precision empirical evidence strictly supports the AACC without a single exception.

Please specify the N1,N2,p1,p2,and the seed x₀ if you hunt the Grail.

Note on Parameters: Theoretically, the magnitudes of N1, N2, p1, p2 are unrestricted, but they strictly must be odd integers. However, for practical operability, this engine enforces specific bounded ranges.
​Users can manually modify these limits in the source code if they wish to deploy the engine for deeper or wider experimental searches.

---

## Repository Structure & Usage
AACC_Crucible_v5.3.cpp - The ultimate dual-engine C++ arbitrary-precision tracker.
ACT_Conservation.lean - The Lean 4 formal proof of lattice gravity.
README.md - Theoretical overview and AACC challenge instructions.
System Requirements
C++ Compiler: C++17 Standard compatible (e.g., Clang or GCC).
Dependencies: GNU Multiple Precision Arithmetic Library (GMP).
Ubuntu/Debian: sudo apt-get install libgmp-dev
macOS/Homebrew: brew install gmp
Android (CxxDroid): GMP is supported natively.
Compilation
To compile the Crucible with maximum optimization, run:

clang++ -O3 -std=c++17 -pthread AACC_Crucible_v5.3.cpp -lgmpxx -lgmp -o aacc_v5

⚙️ Repository Structure & Usage
* `AACC_Crucible_v5.3.cpp - The ultimate dual-engine C++ arbitrary-precision tracker.
* `ACT_Conservation.lean` - The Lean 4 formal proof of lattice gravity.
* `README.md` - Theoretical overview and AACC challenge instructions.

### System Requirements
* **C++ Compiler:** C++17 Standard compatible (e.g., GCC or Clang).
* **Dependencies:** GNU Multiple Precision Arithmetic Library (GMP).
  * Ubuntu/Debian: `sudo apt-get install libgmp-dev`
  * macOS/Homebrew: `brew install gmp`
  * Android (CxxDroid): GMP is supported natively.
### Compilation
To compile the Crucible with maximum optimization (`-O3`). run the following command in your terminal:

```bash
clang++ -O3 -std=c++17 -pthread AACC_Crucible_v5.3.cpp -lgmpxx -lgmp -o aacc_v5

```

### Execution
Once compiled, initiate the engine to start testing ECF boundaries or searching for AACC counter-examples:
```bash
./aacc_v5

```
