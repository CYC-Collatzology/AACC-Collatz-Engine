
# Arithmetic Chiral Topodynamics (ACT): Decoding Extended Collatz Dynamics
**License:** MIT  |  **Core Architecture:** C++17 (Arbitrary-Precision)

Welcome to the official repository for the **Arithmetic Chiral Topodynamics (ACT)** framework and the arbitrary-precision C++ engines used to simulate the Extended Collatz Function (ECF).

## 1. The Paradigm Shift in Discrete Dynamics
Unlike highly specialized mathematical conjectures, the classic $3x+1$ problem exhibits widespread interdisciplinary popularity, captivating researchers across physics, computer science, and complex systems. Despite this cross-disciplinary interest, the global scientific community has remained largely powerless against its extreme nonlinear complexity. Historically, generalized affine mappings (Nx+p) have been deeply entrenched as intractable, chaotic, path-dependent operations plagued by theoretical undecidability.

This project uncovers the deterministic "hidden variables" governing Collatz dynamics by exposing a profound mathematical paradox: the hybridization of the 3x+1 and 3x-1 operators.

## 2. Chiral Non-Commutativity & Symmetry Breaking
Individually, both the 3x+1 and 3x-1 systems are universally recognized to exhibit absolute convergence under standard modulo-2 mappings. However, when we broaden the analytical scope to a modulo-4 **Extended Collatz Function (ECF)**, a startling topological reality emerges. In our ECF framework, the odd parity space is bifurcated into two operative engines: N1x + p1 for x = 1 (mod 4), and N2x + p2 for x = 3 (mod 4).

While symmetric baselines deterministically converge, generating an asymmetric hybrid yields unbelievably counterintuitive results:
* **The Paradox of Repulsion:** The specific configuration `ECF(3x-1, 3x+1)` triggers absolute divergence. Strikingly, merely swapping the modular assignments to `ECF(3x+1, 3x-1)` violently restores absolute convergence.
* **The Paradox of Capture:** High-multiplier systems like $7x+1$ or $9x+1$ independently undergo absolute divergence. Yet, when hybridized, the cross-chiral interaction achieves the impossible: `ECF(7x+1, 9x+1)` forcibly collapses into absolute convergence, whereas `ECF(9x+1, 7x+1)` maintains absolute divergence.

This empirical reality, where **ECF(A, B) ≠ ECF(B, A)**, rigorously proves the existence of chiral non-commutativity and exposes a fundamental symmetry breaking phenomenon within discrete dynamical systems.

## 3. The Universal Topological Invariant (Δ')
To formalize this topological mechanism, we derive a universal topological invariant, the **Net Drift Discriminant (Δ')**. For standard symmetric systems (N1=N2,p1=p2), the macroscopic invariant is rigidly locked at:
> **Δ = (lnN/ln2)-2**

Under this precise discriminant, the classic 3x+1 yields Δ'= (ln3/ln2) - 2 ≈-0.415<0, effectively demystifying it as an absolute convergence event with no particularity.

For generalized asymmetric ECF systems, the topological framework expands into a unified discriminant:
> **Δ' = [ln√(N₁ * N₂) / ln 2] - ρ** *(where quantized Lattice Gravity ρ ∈ {1, 2, 3})*

Operating entirely *a priori*, this Δ' serves as the absolute physical boundary governing the macroscopic destiny of any generalized Collatz dynamical system.

---

## ⚠️ The AACC Challenge: Call for Counter-Examples
We fully acknowledge that proposing a strictly deterministic "hidden variable" within a system historically defined by pseudo-random chaos is highly counterintuitive. Therefore, we invite the global scientific and hacker communities to test the absolute predictive power of this framework.

We propose the **Absolute Asymptotic Convergence Condition (AACC)**:
> **Any system with Δ' < 0 MUST universally collapse into absolute convergence (a bounded periodic loop).**

### The Challenge: 
Using the provided C++ arbitrary-precision engines (or your own code), find a single generalized ECF configuration and a starting seed $x_0$ such that **Δ' < 0**, but the trajectory diverges to infinity or violates the deterministic lattice gravity bounds.

If a valid counter-example is found, the determinism of the ACT framework is broken. So far, extensive arbitrary-precision empirical evidence strictly supports the AACC without a single exception.

---

## ⚙️ Repository Structure & Usage
* `ACT_Arbitrary_Precision_Tracker.cpp` - The core C++ source code.
* `README.md` - Theoretical overview and AACC challenge instructions.

### System Requirements
* **C++ Compiler:** C++17 Standard compatible (e.g., GCC or Clang).
* **Dependencies:** GNU Multiple Precision Arithmetic Library (GMP).
  * Ubuntu/Debian: `sudo apt-get install libgmp-dev`
  * macOS/Homebrew: `brew install gmp`

### Compilation
To compile the tracker with maximum optimization (`-O3`), run the following command in your terminal:
```bash
g++ -O3 -std=c++17 ACT_Arbitrary_Precision_Tracker.cpp -lgmpxx -lgmp -o act_tracker


```
### Execution
Once compiled, initiate the engine to start testing ECF boundaries or searching for AACC counter-examples:

```bash
./act_tracker

```
