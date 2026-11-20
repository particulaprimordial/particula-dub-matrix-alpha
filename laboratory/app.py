from __future__ import annotations
import streamlit as st
import numpy as np
import matplotlib.pyplot as plt
from dsp_reference import process_delay, metrics

st.set_page_config(page_title="Partícula Dub Matrix Laboratory", page_icon="◉", layout="wide")
st.markdown("""
<style>
.stApp {background:#101718;color:#e9e7df}
h1,h2,h3 {letter-spacing:.08em}
[data-testid="stMetric"] {background:#1b2525;padding:14px;border:1px solid #35413d;border-radius:8px}
</style>
""", unsafe_allow_html=True)

st.title("PARTÍCULA DUB MATRIX")
st.caption("COMPUTATIONAL AUDIO LABORATORY  /  DSP REFERENCE MODEL  /  ALPHA 0.1")
st.info("This dashboard models the current delay algorithm offline. It is a research companion, not the VST plugin.")

with st.sidebar:
    st.subheader("EXPERIMENT PARAMETERS")
    sr = st.selectbox("Sample rate", [44100, 48000, 88200, 96000], index=1)
    bpm = st.slider("Tempo (BPM)", 60, 180, 124)
    division = st.selectbox("Delay division", ["1/4", "1/8", "1/8D", "1/8T", "1/16"])
    beat_ms = 60000 / bpm
    delay_default = {"1/4":beat_ms, "1/8":beat_ms/2, "1/8D":beat_ms*.75, "1/8T":beat_ms/3, "1/16":beat_ms/4}[division]
    delay_ms = st.slider("Delay time (ms)", 20.0, 2000.0, float(delay_default), 1.0)
    feedback = st.slider("Feedback", 0.0, 0.92, 0.58, 0.01)
    wet = st.slider("Wet", 0.0, 1.0, 0.35, 0.01)
    lp = st.slider("Low-pass (Hz)", 300, 18000, 6500, 50)
    hp = st.slider("High-pass (Hz)", 20, 1500, 90, 5)
    drive = st.slider("Drive (dB)", 0.0, 12.0, 1.5, 0.1)
    seconds = st.slider("Test duration (s)", 2, 12, 6)

n = int(sr * seconds)
t = np.arange(n) / sr
# Short broadband impulse-like musical test: transient plus low-level tone.
source = np.zeros(n)
source[0] = 0.8
source += 0.035 * np.sin(2*np.pi*110*t) * np.exp(-t*1.8)
st.subheader("Signal experiment")
col1, col2, col3 = st.columns(3)
col1.metric("Delay time", f"{delay_ms:.1f} ms")
col2.metric("Feedback", f"{feedback:.2f}")
col3.metric("Delay samples", f"{round(delay_ms*sr/1000):,}")

rendered = process_delay(source, sr, delay_ms, feedback, wet, lp, hp, drive)
m = metrics(rendered, sr)
a, b, c = st.columns(3)
a.metric("Peak", f"{m['peak']:.4f}")
b.metric("RMS", f"{m['rms']:.4f}")
c.metric("Spectral centroid", f"{m['spectral_centroid_hz']:.1f} Hz")

fig, ax = plt.subplots(figsize=(12, 3))
ax.plot(t, source, label="Input", alpha=.65, linewidth=.8)
ax.plot(t, rendered, label="Output", linewidth=.8)
ax.set_xlabel("Time (s)"); ax.set_ylabel("Amplitude"); ax.grid(alpha=.2); ax.legend()
st.pyplot(fig, use_container_width=True); plt.close(fig)

fig2, ax2 = plt.subplots(figsize=(12, 3))
window = np.hanning(len(rendered))
spec = np.abs(np.fft.rfft(rendered * window))
freq = np.fft.rfftfreq(len(rendered), 1/sr)
ax2.plot(freq, 20*np.log10(np.maximum(spec, 1e-9)), linewidth=.8)
ax2.set_xlim(20, min(20000, sr/2)); ax2.set_xscale("log")
ax2.set_xlabel("Frequency (Hz)"); ax2.set_ylabel("Magnitude (dB)")
ax2.grid(alpha=.2, which="both")
st.pyplot(fig2, use_container_width=True); plt.close(fig2)

st.subheader("Research notes")
st.markdown("""
- The delay buffer stores the input plus the processed regenerated signal.
- Low-pass and high-pass filters operate inside the feedback path, so each repetition can change spectrally.
- Saturation is applied to the regenerated signal, not the dry input.
- Compare parameter sweeps under identical input, duration and sample-rate conditions.
""")
