# Collatzology & Arithmetic Chiral Topodynamics (ACT)
**Decoding the Extended Collatz Dynamics via Universal Topological Invariants**

**License:** MIT | **Core Architecture:** C++17 (Arbitrary-Precision & O(1) Memory)
**Preprint:** [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.21996041.svg)](https://doi.org/10.5281/zenodo.21996041)

> **Collatzology** */kəˈlætsɒlədʒi/* (n.): The unified mathematical and computational study of generalized Collatz-like discrete dynamical systems. It transitions the 89-year-old 3x+1 problem from an isolated, path-dependent anomaly into a predictable, macroscopic topological architecture (The Extended Collatz Family).

For decades, the mathematical community has treated generalized $Nx+p$ mappings as fragmented, unpredictable puzzles. **Collatzology** is the Grand Unified Theory for this domain. 

This repository hosts the theoretical groundwork and the arbitrary-precision C++ engines of the **Arithmetic Chiral Topodynamics (ACT)** framework. ACT serves as the foundational physical law of Collatzology, replacing stochastic guesswork with strict algebraic geometry, deterministic topological invariants ($\Delta'$), and machine-verified logic.

Included here is the **AACC Crucible Fuzzer**—a weaponized computational engine built to simulate, fuzz, and definitively dissect the Extended Collatz Function (ECF). 
*(Note: AACC stands for **Absolute Asymptotic Convergence to a Cycle**, the fundamental macroscopic destiny this engine is built to test, verify, and mathematically enforce.)*

---

## 1. A Deterministic Special Case within Conway’s Generalized Collatz Mappings

In 1972, John Horton Conway established a monumental proof: generalized Collatz mappings are Turing-complete, rendering their macroscopic trajectories algorithmically undecidable. To achieve this, Conway utilized fractional mappings to construct linguistic FRACTRAN machines disguised as dynamical systems. 

The ACT framework does not dispute this overarching undecidability; rather, it isolates a profound, highly structured special case. By strictly enforcing the physical iron law of the classic Collatz system—**pure integer arithmetic and absolute parity-driven reduction (always x/2 if even)**—within a restricted Modulo-4 architecture, the system is stripped of its Turing-complete chaos. 

Within this specific boundary, the computational irreducibility collapses. What remains is no longer an unpredictable stochastic process, but a fully deterministic algebraic geometry problem.


## 2. The Extended Collatz Function (ECF)
The Extended Collatz Function system is denoted as **ECF(N₁x+p₁, N₂x+p₂)** and is defined by:

* `f(x) = N₁x + p₁` (when `x ≡ 1 mod 4`)
* `f(x) = N₂x + p₂` (when `x ≡ 3 mod 4`)
* `f(x) = x / 2` (when `x` is even)

*(Where N₁, N₂, p₁, p₂ are any odd integers, and the starting seed x₀ is any non-zero integer).*

### The "USB" Backward Compatibility
To prevent any misconception that ECF is an unrelated arbitrary invention, it perfectly maintains mathematical backward compatibility:
* **USB 2.0 (The Generalized Nx+p):** By setting N₁ = N₂ = N and p₁ = p₂ = p, it perfectly replicates any symmetric generalized Collatz system.
* **USB 1.0 (The Classic 3x+1):** By simply setting N₁ = N₂ = 3 and p₁ = p₂ = 1, the modulo-4 branches merge seamlessly back into the original 3x+1 operation.

## 3. Chiral Non-Commutativity & Symmetry Breaking
While symmetric baselines (like 3x+1 or 3x-1) deterministically converge, generating an asymmetric hybrid yields unbelievably counterintuitive results:

* **The Paradox of Repulsion:** `ECF(3x-1, 3x+1)` triggers astonishing divergence. Strikingly, merely swapping the modular assignments to `ECF(3x+1, 3x-1)` violently restores absolute convergence.
* **The Paradox of Capture:** High-multiplier systems like 7x+1 or 9x+1 independently undergo expected divergence. Yet, when hybridized, `ECF(7x+1, 9x+1)` forcibly collapses into unbelievable convergence, whereas `ECF(9x+1, 7x+1)` maintains anticipated divergence.

This empirical reality, where `ECF(A, B) ≠ ECF(B, A)`, strongly suggests the existence of a chiral non-commutative structure within discrete dynamical systems.

## 4. The Universal Topological Invariant (Δ')
To formalize this mechanism, we derive a universal topological invariant, the **Net Drift Discriminant (Δ')**. 

For generalized asymmetric ECF systems, the topological framework expands into a unified discriminant:
**Δ' = [ln√(|N₁ · N₂|) / ln 2] - ρ**  *(where quantized Lattice Gravity ρ ∈ {1, 2, 3})*


> **Determining Lattice Gravity (ρ):**
> Evaluated via `(N₁ + p₁, N₂ + p₂) mod 4 ≡ (C₁, C₂)`.
> * **Regime I (ρ=1):** (2, 0)
> * **Regime II (ρ=2):** (2, 2) or (0, 0)
> * **Regime III (ρ=3):** (0, 2)

### Predicting System Fate A Priori
Δ' acts exactly like the discriminant (b² - 4ac) in quadratic equations, determining the system's fate *before* a single computational step is taken:

* **If Δ' < 0 (Unconditional Convergence):** The system achieves AACC. It universally condenses into a finite periodic loop, rendering the chaotic variance of any microscopic seed irrelevant.
* **If Δ' > 0 (Contingent Divergence):** The system will diverge to infinity *unless* it gets prematurely trapped in a periodic micro-loop for a specific seed.

*(Note: Δ' can never be exactly 0, ensuring absolute determinism).*

---

## 5. Formal Verification (Lean 4)
The macroscopic flow conservation laws and the quantized lattice gravity (ρ ∈ {1, 2, 3}) derived within the ACT framework have been formally verified using the **Lean 4 theorem prover**. The absolute mathematical certainty of this algebraic routing mechanics is available in `ACT_Conservation.lean`.

## 6. The Crucible Fuzzer: Engineering Highlights
The included C++ engine (`AACC_Crucible_v5.4.cpp`) is not a simple script; it is a hardened, arbitrary-precision Topodynamic Fuzzer designed to withstand extreme computational stress:
* **O(1) Memory Anatomy Trace:** Utilizes Floyd's Tortoise and Hare algorithm to detect massive cycles without RAM exhaustion.
* **Infinity Overload Shield:** Automatically prevents Out-Of-Memory (OOM) crashes during exponential divergence by evaluating bit-length velocity.
* **Dynamic Evaporation Limit (Anti-Smuggling):** Adversarially defends against "fake theory violations." The engine dynamically calculates the absolute minimum steps required for a system to converge based on the combined digit mass of the seed and the lattice constants (p₁, p₂), actively overriding insufficient user-defined step limits.

Note on Parameters: Theoretically, the magnitudes of N1, N2, p1, p2 are unrestricted, but they strictly must be odd integers. However, for practical operability, this engine enforces specific bounded ranges.
Users can manually modify these limits for deeper or wider experimental searches.

## 7. The AACC Challenge (The Grail Hunt)
We invite the global hacker and mathematical communities to test the predictive power of this framework. 

**The Challenge:** Find a single generalized ECF configuration (N₁, N₂, p₁, p₂) and a starting seed x₀ such that **Δ' < 0**, but the trajectory strictly diverges to infinity, escaping the lattice gravity bounds.

If a valid counter-example is found, the determinism of this framework must be revised. So far, extensive arbitrary-precision fuzzing across multi-million-step trajectories strictly supports the AACC without a single exception. 
*(If you hunt the Grail, please open an issue specifying N₁, N₂, p₁, p₂, and the seed x₀).*

## 8. Usage & Compilation
**Requirements:** C++17 Compiler (GCC/Clang), GMP Library (`libgmp-dev`).
```bash
# Compile with maximum optimization
clang++ -O3 -std=c++17 -pthread AACC_Crucible_v5.4.cpp -lgmpxx -lgmp -o aacc_v5

# Execute the Fuzzer
./aacc_v5
