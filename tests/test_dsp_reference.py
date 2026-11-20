import numpy as np
from laboratory.dsp_reference import process_delay, metrics

def test_silence_remains_silent():
    x = np.zeros((48000, 2))
    y = process_delay(x)
    assert np.max(np.abs(y)) == 0.0

def test_output_shape_and_finite():
    x = np.zeros((48000, 2))
    x[0, :] = 1.0
    y = process_delay(x, feedback=0.85)
    assert y.shape == x.shape
    assert np.isfinite(y).all()

def test_feedback_bound_is_enforced():
    x = np.zeros(1000)
    try:
        process_delay(x, feedback=0.99)
    except ValueError:
        return
    raise AssertionError("Expected invalid feedback to raise ValueError")

def test_metrics_are_finite():
    x = np.zeros(4096)
    x[0] = 1.0
    result = metrics(x, 48000)
    assert all(np.isfinite(v) for v in result.values())
