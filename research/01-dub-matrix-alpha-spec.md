# Partícula Dub Matrix — Alpha 0.1 Functional Specification

## Research question

Can a compact stereo delay with frequency-dependent regeneration create controllable, evolving echo structures suitable for Dub Techno production?

## Signal model

For each channel, the initial feedback system is:

\[
y[n] = (1-m)x[n] + m d[n]
\]

\[
u[n] = x[n] + g\,S(F(d[n]))
\]

where:

- \(x[n]\): input
- \(d[n]\): delayed sample
- \(m\): wet proportion
- \(g\): feedback coefficient
- \(F\): cascaded one-pole low-pass and high-pass filtering
- \(S\): soft saturation function

The delay buffer stores \(u[n]\). The audible output is a dry/wet mix of the input and delayed signal.

## Initial parameter ranges

| Parameter | Range | Default |
|---|---:|---:|
| Delay time | 20–2000 ms | 420 ms |
| Feedback | 0–0.92 | 0.58 |
| Wet | 0–1 | 0.35 |
| Low-pass | 300–18000 Hz | 6500 Hz |
| High-pass | 20–1500 Hz | 90 Hz |
| Drive | 0–12 dB | 1.5 dB |

## Validation plan

1. Impulse response: measure echo interval and decay.
2. Sine sweep: estimate frequency-dependent feedback coloration.
3. Feedback stress test: evaluate bounded input and high feedback.
4. Silence test: inspect residual output and denormal behavior.
5. Host test: validate Ableton loading, automation and state recall.
6. CPU test: measure average and peak processing cost at multiple sample rates and buffer sizes.

## Known limitations

- No parameter smoothing in this alpha.
- No tempo synchronization.
- No dedicated mono/stereo routing modes beyond stereo buses.
- No tape model, diffusion or reverb.
- Stability must be measured; the feedback ceiling alone is not a universal stability guarantee.
