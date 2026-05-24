import os
from ultralytics import YOLO

def extract_and_freeze_model():
    print("⏳ Initializing ECLIPSE-V Phase 1: Accessing YOLO26-Nano weights...")
    
    # 1. Load the pre-trained production model
    model = YOLO("yolo26n.pt")
    
    print("🚀 Exporting computational graph to serialized ONNX format...")
    
    # 2. Export to ONNX with specific constraints for C++ edge deployment
    # We enforce a static image size of 640x640 for memory-safe pre-allocation.
    # 'simplify=True' uses onnxslim to optimize the node graph for CPU execution.
    success_path = model.export(
        format="onnx",
        imgsz=640,
        dynamic=False,
        simplify=True,
        opset=12  # Highly compatible with native ONNX Runtimes
    )
    
    if os.path.exists("yolo26n.onnx"):
        print("\n✅ PHASE 1 COMPLETE!")
        print(f"Pristine model frozen at: {os.path.abspath('yolo26n.onnx')}")
        print("All PyTorch overhead has been successfully stripped away.")
    else:
        print("❌ Export verification failed.")

if __name__ == "__main__":
    extract_and_freeze_model()