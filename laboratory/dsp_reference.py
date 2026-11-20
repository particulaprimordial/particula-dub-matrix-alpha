"""Offline reference model for Particula Dub Matrix.

This is a research model, not the real-time VST implementation.
"""
from __future__ import annotations
import numpy as np

def one_pole_lowpass(x: np.ndarray, cutoff_hz: float, sample_rate: float) -> np.ndarray:
    a = 1.0 - np.exp(-2.0 * np.pi * cutoff_hz / sample_rate)
    y = np.zeros_like(x, dtype=np.float64)
    state = 0.0
    for i, sample in enumerate(x):
        state += a * (float(sample) - state)
        y[i] = state
    return y

def one_pole_highpass(x: np.ndarray, cutoff_hz: float, sample_rate: float) -> np.ndarray:
    a = np.exp(-2.0 * np.pi * cutoff_hz / sample_rate)
    y = np.zeros_like(x, dtype=np.float64)
    previous_x = previous_y = 0.0
    for i, sample in enumerate(x):
        current = a * (previous_y + float(sample) - previous_x)
        y[i] = current
        previous_x, previous_y = float(sample), current
    return y

def process_delay(x: np.ndarray, sample_rate: int = 48000, delay_ms: float = 420.0,
                  feedback: float = 0.58, wet: float = 0.35,
                  lowpass_hz: float = 6500.0, highpass_hz: float = 90.0,
                  drive_db: float = 1.5) -> np.ndarray:
    x = np.asarray(x, dtype=np.float64)
    if x.ndim == 1:
        x = x[:, None]
    if x.ndim != 2 or x.shape[1] not in (1, 2):
        raise ValueError("Expected mono or stereo audio shaped (samples, channels).")
    if not 0 <= feedback <= 0.92:
        raise ValueError("feedback must be in [0, 0.92]")
    delay_samples = max(1, int(round(delay_ms * sample_rate / 1000)))
    gain = 10 ** (drive_db / 20)
    out = np.zeros_like(x)
    for ch in range(x.shape[1]):
        ring = np.zeros(delay_samples + 1, dtype=np.float64)
        write = 0
        lp_state = hp_x = hp_y = 0.0
        a_lp = 1 - np.exp(-2 * np.pi * lowpass_hz / sample_rate)
        a_hp = np.exp(-2 * np.pi * highpass_hz / sample_rate)
        for n, dry_sample in enumerate(x[:, ch]):
            delayed = ring[write]
            lp_state += a_lp * (delayed - lp_state)
            hp = a_hp * (hp_y + lp_state - hp_x)
            hp_x, hp_y = lp_state, hp
            regenerated = np.tanh(hp * gain) * feedback
            ring[write] = dry_sample + regenerated
            write = (write + 1) % len(ring)
            out[n, ch] = dry_sample * (1 - wet) + delayed * wet
    return out

def metrics(audio: np.ndarray, sample_rate: int) -> dict:
    x = np.asarray(audio, dtype=np.float64)
    peak = float(np.max(np.abs(x))) if x.size else 0.0
    rms = float(np.sqrt(np.mean(x * x))) if x.size else 0.0
    mono = x.mean(axis=1) if x.ndim == 2 else x
    spectrum = np.abs(np.fft.rfft(mono * np.hanning(len(mono)))) if len(mono) else np.array([0.0])
    freqs = np.fft.rfftfreq(len(mono), 1 / sample_rate) if len(mono) else np.array([0.0])
    centroid = float((freqs * spectrum).sum() / spectrum.sum()) if spectrum.sum() else 0.0
    return {"peak": peak, "rms": rms, "spectral_centroid_hz": centroid}
