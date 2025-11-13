import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# --- Load CSV ---
spectrum_filename = "normalized_spectrum.csv"

df = pd.read_csv(spectrum_filename)

# Reverse arrays so energy increases
energy = df["Energy (MeV)"].values[::-1]
counts = df["Normalized Counts"].values[::-1]

# --- Compute the integral (area) under the raw PDF ---
area_before = np.trapz(counts, energy)

# --- Normalize the counts using the *true integral*, not the sum ---
pdf = counts / area_before

# --- Verify normalization ---
area_after = np.trapz(pdf, energy)

# --- Compute CDF using true integration (works for nonuniform bins) ---
cdf = np.cumsum(pdf * np.gradient(energy))
cdf /= cdf[-1]  # ensure last value = 1

# --- Report areas ---
print(f"Area under PDF before normalization: {area_before:.6f}")
print(f"Area under PDF after normalization:  {area_after:.6f}")

# --- Save to .hh file ---
with open("../include/spectrum_cdf.hh", "w") as f:
    f.write(f"// Auto-generated CDF from {spectrum_filename}\n")
    f.write("#pragma once\n\n")
    f.write("#include <vector>\n\n")
    f.write("static const std::vector<double> energyBins = {")
    f.write(", ".join([f"{e:.6f}" for e in energy]))
    f.write("};\n\n")
    f.write("static const std::vector<double> cdf = {")
    f.write(", ".join([f"{c:.6f}" for c in cdf]))
    f.write("};\n")

print("Wrote spectrum_cdf.hh")

# --- Plot PDF and CDF ---
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(10, 4))

ax1.plot(energy, pdf, marker='o')
ax1.set_xlabel("Energy (MeV)")
ax1.set_ylabel("Normalized Counts (PDF)")
ax1.set_title("Probability Density Function")
ax1.grid(True)

ax2.plot(energy, cdf, marker='o', color='orange')
ax2.set_xlabel("Energy (MeV)")
ax2.set_ylabel("Cumulative Probability")
ax2.set_title("Cumulative Distribution Function")
ax2.grid(True)

plt.tight_layout()
plt.show()