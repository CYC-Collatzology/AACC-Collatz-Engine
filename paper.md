---
title: 'ACT-Engine: A C++ Arbitrary-Precision Tracker for Generalized Asymmetric Collatz Mappings'
tags:
  - C++
  - arbitrary-precision
  - number theory
  - dynamical systems
  - Collatz conjecture
  - GMP
authors:
  - name: Ying-Chao Chen
    orcid: 0009-0001-5119-7048
    affiliation: 1
affiliations:
 - name: Independent Researcher, Taiwan
   index: 1
date: 3 September 2026
bibliography: paper.bib
---

# Summary

The `ACT-Engine` (Arithmetic Chiral Topodynamics Engine) is a high-performance, arbitrary-precision numerical simulation suite written in C++. It is specifically engineered to track, record, and analyze the extreme mathematical trajectories of generalized, asymmetric modulo-4 extensions of the Collatz mapping. By leveraging the GNU Multiple Precision Arithmetic Library (GMP), the engine calculates iterative sequence behaviors for numbers spanning thousands of digits, avoiding the standard 64-bit or 128-bit hardware limits. It introduces automated save/resume checkpointing and multi-threaded structure tracking, enabling continuous mathematical sweeps reaching up to a billion calculation steps without data loss due to system interruptions.

# Statement of need

The classic $3x+1$ problem (Collatz conjecture) has been studied extensively, but computational tools are predominantly hardcoded for its specific symmetric operational rules [@Lagarias:1985]. Recently, mathematical research has expanded into discrete dynamical systems with chiral non-commutativity, where odd residue classes modulo 4 are assigned distinct multipliers ($N_1$ and $N_2$). Standard computational environments fail to investigate these asymmetric Extended Collatz Functions (ECF) when trajectories experience explosive growth prior to collapsing into absolute convergence or diverging to infinity. 

Computational mathematicians and independent researchers require a robust framework to test deterministic topological boundaries, such as the Net Drift Discriminant ($\Delta'$). The `ACT-Engine` fills this gap by providing an extensible, arbitrary-precision C++ tracker [@ACT_Engine:2026]. It allows researchers to seamlessly configure disparate multipliers, configure structural tracking modules, and parse out discrete lattice gravity indices ($\rho \in \{1, 2, 3\}$). With its built-in automated checkpointing, `ACT-Engine` offers the specialized infrastructure necessary to rigorously test counter-examples and simulate massively volatile mathematical hybridizations (e.g., pairing divergent components like $7x+1$ and $9x+1$) that fundamentally break commutative behaviors.

# Acknowledgements

This software was developed entirely as an independent research initiative, utilizing the open-source GMP library for critical arbitrary-precision arithmetic.

