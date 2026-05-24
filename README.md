# ECLIPSE-V: High-Efficiency Native Edge Inference Engine

ECLIPSE-V is a framework-free, dependency-lean native C++ object detection engine designed for high-frequency robotic telemetry. By bypassing heavy computer vision libraries and high-overhead runtimes, the engine utilizes custom memory-mapped loops and close-to-the-silicon hardware scaling to achieve ultra-low latency execution on edge devices.

### 👁️ Inference Output Visualization
![ECLIPSE-V Real-Time Detection Output](output.jpg)

---

## 💻 Hardware Specification Baseline
All benchmark data below was captured deterministically under the following hardware environment:
* **CPU:** Intel® Core™ i7-14650HX (14th-Gen Architecture)
* **Topology:** 16 Cores / 24 Threads (8 Performance-cores / 8 Efficient-cores)
* **Max Turbo Frequency:** 5.20 GHz
* **Memory:** DDR5 Dual-Channel RAM
* **Execution Configuration:** Asynchronous Multi-Core Parallelization Unlocked (`SetIntraOpNumThreads(0)`)

---

## 📊 Performance Benchmarks
The following metrics were captured utilizing a **1024x1024 RGB input canvas** (`test.jpg`) processed through a continuous 100-frame stress test loop:

| Pipeline Stage | Operational Latency | Functional Responsibility |
| :--- | :--- | :--- |
| **Tensor Preprocessing** | ~4.86 ms | Interleaved-to-Planar layout transformation (HWC → CHW) & normalization. |
| **Neural Inference** | ~14.03 ms | Multithreaded execution graph optimization via YOLO26-Nano. |
| **In-Memory Rendering** | < 0.01 ms | Fast bitwise pixel manipulation for bounding box overlays. |
| **Total Pipeline Latency** | **18.90 ms** | **End-to-End frame processing cycle time.** |

### ⚡ True Throughput: **52.90 FPS** (Continuous Stream)
