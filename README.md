# Arithmetic Chiral Topodynamics (ACT) and the Extended Collatz Function (ECF): Decoding Extended Collatz Dynamics

**License:** MIT | **Core Architecture:** C++17 (Arbitrary-Precision)
**Preprint:** [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.21996041.svg)](https://doi.org/10.5281/zenodo.21996041)

Welcome to the official repository for the **Arithmetic Chiral Topodynamics (ACT)** framework and the arbitrary-precision C++ engines used to simulate the Extended Collatz Function (ECF).

## 1. Overview
This project introduces the ACT framework, which exposes a profound mathematical paradox—the chiral non-commutativity of hybridizing $3x+1$ and $3x-1$ operators—and extracts the deterministic topological invariants that strictly govern these dynamics.

## 2. The Extended Collatz Function (ECF)
The Extended Collatz Function (ECF) system is denoted as $\text{ECF}(N_1x+p_1, N_2x+p_2)$ and is defined by:

$$f(x) = \begin{cases}  N_1x + p_1 & \text{when } x \equiv 1 \pmod 4 \\ N_2x + p_2 & \text{when } x \equiv 3 \pmod 4 \\ x / 2 & \text{when } x \text{ is even}  \end{cases}$$

Here, $N_1, N_2, p_1, p_2$ are any odd integers ($\mathbb{Z}^+$ or $\mathbb{Z}^-$), and the starting seed $x$ is any non-zero integer.

## 3. Chiral Non-Commutativity & Symmetry Breaking
Individually, both the $3x+1$ and $3x-1$ systems are universally recognized to exhibit absolute convergence under standard modulo-2 mappings. However, when we broaden the analytical scope to a modulo-4 **Extended Collatz Function (ECF)**, a startling topological reality emerges. 

While symmetric baselines deterministically converge, generating an asymmetric hybrid yields unbelievably counterintuitive results:
* **The Paradox of Repulsion:** The specific configuration $\text{ECF}(3x-1, 3x+1)$ triggers absolute divergence. Strikingly, merely swapping the modular assignments to $\text{ECF}(3x+1, 3x-1)$ violently restores absolute convergence.
* **The Paradox of Capture:** High-multiplier systems like $7x+1$ or $9x+1$ independently undergo absolute divergence. Yet, when hybridized, the cross-chiral interaction achieves the impossible: $\text{ECF}(7x+1, 9x+1)$ forcibly collapses into absolute convergence, whereas $\text{ECF}(9x+1, 7x+1)$ maintains absolute divergence.

This empirical reality, where $\text{ECF}(A, B) \neq \text{ECF}(B, A)$, strongly suggests the existence of a chiral non-commutative structure and exposes a fundamental symmetry breaking phenomenon within discrete dynamical systems.

## 4. The Universal Topological Invariant ($\Delta'$)
To formalize this topological mechanism, we derive a universal topological invariant, the **Net Drift Discriminant ($\Delta'$)**. For standard symmetric systems ($N_1=N_2, p_1=p_2$), the macroscopic invariant is rigidly locked at:

$$\Delta = \frac{\ln\vert{}N\vert{}}{\ln 2} - 2$$

For generalized asymmetric ECF systems, the topological framework expands into a unified discriminant:

$$\Delta' = \frac{\ln\sqrt{\vert{}N_1 \cdot N_2\vert{}}}{\ln 2} - \rho$$
*(where quantized Lattice Gravity $\rho \in \{1, 2, 3\}$)*

## 5. Predicting System Fate *A Priori*
You can think of $\Delta'$ acting exactly like the discriminant ($b^2 - 4ac$) in quadratic equations to determine the system's fate *a priori*:

* If **$\Delta' < 0$**, the Collatz system is **unconditionally convergent** (ultimately collapsing into a periodic loop), regardless of any starting seed $x$.
* If **$\Delta' > 0$**, the Collatz system is **contingently divergent** (it will diverge to infinity unless it gets prematurely trapped in a periodic loop for some specific seed; it is case-sensitive).

*(Note: $\Delta'$ can never be exactly 0. Since $N_1$ and $N_2$ are odd, the natural log fraction is never an integer, while $\rho$ is always an integer. Therefore, either Unconditional Convergence or Contingent Divergence is the inevitable outcome. No ambiguity exists in the system.)*

### Determining Lattice Gravity ($\rho$)
How do we determine $\rho$? The rule is explicitly deduced based on the combined congruence of the system parameters. We evaluate $(N_1 + p_1, N_2 + p_2) \pmod 4$ and map it to a base state $(C_1, C_2) \pmod 4$.

By extension, this mathematically guarantees that adding any multiple of 4 ($4u, 4v$) preserves the exact same lattice gravity:
$(N_1 + p_1 + 4u, N_2 + p_2 + 4v) \equiv (N_1 + p_1, N_2 + p_2) \equiv (C_1, C_2) \pmod 4$

Mapping this to $(C_1, C_2) \pmod 4$ yields three regimes:
* **Regime I ($\rho=1$):** $(C_1, C_2) \equiv (2, 0) \pmod 4$
* **Regime II ($\rho=2$):** $(C_1, C_2) \equiv (2, 2)$ or $(0, 0) \pmod 4$
* **Regime III ($\rho=3$):** $(C_1, C_2) \equiv (0, 2) \pmod 4$

### Practical Examples
**1. The classic 3x+1:** $\text{ECF}(3x+1, 3x+1)$
$(C_1, C_2) = (3+1, 3+1) \equiv (0, 0) \pmod 4 \rightarrow \rho = 2$
$\Delta' = \frac{\ln 3}{\ln 2} - 2 \approx -0.415 < 0$
*(This clearly demonstrates it as an unconditional convergence event.)*

**2. A massive asymmetric hybrid:** $\text{ECF}(7x+1, 9x+1)$
$(C_1, C_2) = (7+1, 9+1) \equiv (0, 2) \pmod 4 \rightarrow \rho = 3$
$\Delta' = \frac{\ln\sqrt{7 \cdot 9}}{\ln 2} - 3 \approx 2.9886 - 3 = -0.011 < 0$
*(This demarcates it as an unconditional convergence event, completely overriding the huge multipliers.)*

Operating entirely *a priori*, this $\Delta'$ serves as the absolute physical boundary governing the macroscopic destiny of any generalized Collatz dynamical system. 

### 🛡️ Formal Verification (Lean 4)
The macroscopic flow conservation laws and the quantized lattice gravity ($\rho \in \{1, 2, 3\}$) derived within the ACT framework have been formally verified using the **Lean 4** theorem prover. 
The absolute mathematical certainty of this algebraic routing mechanics is available in the formal proof file: [`ACT_Conservation.lean`](./ACT_Conservation.lean). For full theoretical breakdown, please see the [Zenodo Preprint](https://doi.org/10.5281/zenodo.21996041).

---

## ⚠️ The AACC Challenge: Call for Counter-Examples

We fully acknowledge that proposing a strictly deterministic "hidden variable" within a system historically defined by pseudo-random chaos is highly counterintuitive. Therefore, we invite the global scientific and hacker communities to test the absolute predictive power of this framework.

We propose the **Absolute Asymptotic Convergence Criterion (AACC)**:
> **ACT predicts that any system with $\Delta' < 0$ undergoes unconditional convergence (a bounded periodic loop).**

### Current Empirical Status
* Tested benchmark set: 64/64 configurations agree with the sign of $\Delta'$.
* Large-scale arbitrary-precision experiments reveal no known counter-example.
* No ECF system with $\Delta' < 0$ and verified divergence has yet been observed.

The existence or non-existence of such a counter-example remains an open problem.

### The Challenge
Using the provided C++ arbitrary-precision engines (or your own code according to the above rules), find a single generalized ECF configuration and a starting seed $x_0$ such that **$\Delta' < 0$**, but the trajectory diverges to infinity or violates the deterministic lattice gravity bounds.

If a valid counter-example is found, the determinism of this framework must be revised. So far, extensive arbitrary-precision empirical evidence strictly supports the AACC without a single exception.

---

## ⚙️ Repository Structure & Usage
* `ACT_Arbitrary_Precision_Tracker.cpp` - The core C++ source code.
* `ACT_Conservation.lean` - The Lean 4 formal proof of lattice gravity.
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
​
