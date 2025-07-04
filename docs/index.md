
            _                                      _
           | |                                    | |
     _ __  | |__    __ _   __ _  ___  _ __    ___ | |_  ___
    | '_ \ | '_ \  / _` | / _` |/ __|| '_ \  / _ \| __|/ __|
    | |_) || | | || (_| || (_| |\__ \| | | ||  __/| |_ \__ \
    | .__/ |_| |_| \__, | \__,_||___/|_| |_| \___| \__||___/
    | |             __/ |
    |_|            |___/


# phgasnets

A speedy C++ implementation of port-Hamiltonian model for Gas Network with Compressors.

A code supplement for "Modelling Gas Networks with Compressors: A Port-Hamiltonian Approach" [![doi](https://img.shields.io/badge/DOI-10.1002/pamm.202400164-blue)](https://doi.org/10.1002/pamm.202400164)

Details:
  - Isothermal Euler equation model for pipes.
  - Emphasis on including four different compressor models.
  - Space discretization with second-order central finite differences.
  - Time discretization with implicit midpoint method.
  - Jacobian computation through automatic differentiation.
  - Nonlinear solve using Levenberg–Marquardt algorithm.

Authors:
  - [Ashwin S. Nayak](https://orcid.org/0000-0002-9855-2377)
  - [Thomas Bendokat](https://orcid.org/0000-0002-0671-6291)

Affiliation:
  - [Max Planck Institute for Dynamics of Complex Technical Systems](https://www.mpi-magdeburg.mpg.de/), Magdeburg, Germany.

Repository: [https://github.com/mpimd-csc/phgasnets](https://github.com/mpimd-csc/phgasnets)

Code DOI: 10.5281/zenodo.11387852

License: GNU GPL v3, see `LICENSE.md`.

Citation:

```bib
@Article{BenBGetal24,
  author = {Bendokat, T. and Benner, P. and Grundel, S. and Nayak, A.~S.},
  title  = {Modelling Gas Networks with Compressors: A port-{H}amiltonian Approach},
  journal = {Proc. Appl. Math. Mech.},
  volume = {24},
  number = {4},
  pages = {e202400164},
  doi = {10.1002/pamm.202400164},
  year = {2024}
}
```

## Contents
```{toctree}
:maxdepth: 2

getting-started
reproduce-results
```
