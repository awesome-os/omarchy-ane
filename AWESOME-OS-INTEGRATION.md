# Awesome-OS Integration: `awesome-os/omarchy-ane`

> Upstream source: [`chriskearney/omarchy-ane`](https://github.com/chriskearney/omarchy-ane) (MIT License, preserved intact in [`LICENSE`](LICENSE)).

`awesome-os/omarchy-ane` provides the Linux DRM accelerator kernel drivers (`ane.ko` for M1 T8103/T6000/T6001 and `ane_t6021.ko` for M2 T6020/T6021), device-tree overlays, firmware fetch tooling, and the `libane` C/Python userspace library for the Apple Neural Engine (ANE).

## Role in Awesome-OS

`omarchy-ane` bridges Apple Silicon NPU hardware into the Awesome-OS Linux stack:

- **[`awesome-os/t2desktop-omarchy`](https://github.com/awesome-os/t2desktop-omarchy)**: Ships DKMS packaging (`dkms.conf`), device-tree overlays (`packaging/dt/`), and verification utilities (`omarchy-ane-check`, `omarchy-ane-dt`, `omarchy-ane-smoke`, `omarchy-ane-probe`) that expose `/dev/accel/accel0` out of the box on Omarchy Apple Silicon desktops.
- **[`awesome-os/t2linux`](https://github.com/awesome-os/t2linux)**: Supplies the DRM accel kernel modules (`ane/` and `ane/t6021/`) with `BUILD_EXCLUSIVE_CONFIG="!CONFIG_DRM_ACCEL_ANE"` so out-of-tree DKMS builds cleanly coexist with in-tree `t2linux` / `aurora-silicon` ANE kernel support.
- **Local LLM & NPU Inference**: Exposes `libane` (C library + Python bindings under `bindings/python/`) for deterministic, bit-exact FP16 execution of local LLM decode (e.g., Qwen) and speech/encoder workloads (e.g., Parakeet) directly on the Apple Neural Engine alongside MLX/ONNX/Stealify runtimes.

## Quick Verification on Apple Silicon Linux

```sh
# Check driver bind, device-tree node, /dev/accel/accel0 permissions, and firmware pin
omarchy-ane-check

# Run the 20-call bit-exact FP16 smoke test through libane
omarchy-ane-check --smoke
```

## Upstream Attribution

All original kernel driver sources, `libane`, Python bindings, device-tree overlays, fixtures, and MIT licensing are preserved from [`chriskearney/omarchy-ane`](https://github.com/chriskearney/omarchy-ane).
