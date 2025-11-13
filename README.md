# laser_isotopes
This is code to simulate isotope production from laser driven ion accelerators

## Overview

This code is still under active development and not yet optimized for easy, general-purpose use. I will continue adding improvements to make it more complete, modular, and user-friendly.

If you would like to run a simulation and want guidance on how the code is structured or how the components interact, please feel free to reach out.

Below are some high-level notes to help you get started.

---

## How the Simulation Works

### Ion Spectrum CDF
The simulation samples ion energies using a **Cumulative Distribution Function (CDF)** derived from a measured, normalized ion spectrum.

- The script **`make_spectrum_cdf.py`** converts a measured spectrum into a CDF.
- It outputs a header file that the Geant4 simulation uses to generate ion energies according to the measured distribution.

---

## Key Files and Where Things Are Defined

### Number of Launched Ions
- Controlled in: **`src/MyGlobals.cc`**

This file defines global simulation parameters, including the number of primary ions to shoot.

---

### Target Geometry & Materials
- Defined in: **`src/DetectorConstruction.cc`**

This includes:
- Geometry setup  
- Target position  
- Material selection and composition  

---

### Ion Species & Beam Divergence
- Set in: **`src/PrimaryGeneratorAction.cc`**

This controls:
- Ion particle type  
- Energy sampling using the CDF  
- Beam divergence / angular spread  

---

### Scoring & Isotope Tracking
- Configured in: **`src/RunAction.cc`**

This is where you:
- Specify which isotope(s) to track  
- Define scoring output and file handling  
- Manage multithreaded histogram merging  

---

## GUI (Under Development)

A user-friendly GUI is currently in progress.  
It will allow you to adjust all major simulation settings — and many more — without needing to modify the code directly. This will make running parameter studies and exploratory simulations much simpler.
