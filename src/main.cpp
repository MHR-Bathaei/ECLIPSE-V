#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cstring> // For fast memory copying (memcpy)
#include <onnxruntime_cxx_api.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Note: stbi_write_jpg removed completely. We are fully headless now.

void draw_bounding_box(unsigned char* data, int img_w, int img_h, int x1, int y1, int x2, int y2, int thickness) {
    x1 = std::clamp(x1, 0, img_w - 1); x2 = std::clamp(x2, 0, img_w - 1);
    y1 = std::clamp(y1, 0, img_h - 1); y2 = std::clamp(y2, 0, img_h - 1);
    for (int t = 0; t < thickness; ++t) {
        for (int x = x1; x <= x2; ++x) {
            if (y1 + t < img_h) { int i = ((y1 + t) * img_w + x) * 3; data[i] = 255; data[i+1] = 0; data[i+2] = 0; }
            if (y2 - t >= 0) { int i = ((y2 - t) * img_w + x) * 3; data[i] = 255; data[i+1] = 0; data[i+2] = 0; }
        }
    }
    for (int t = 0; t < thickness; ++t) {
        for (int y = y1; y <= y2; ++y) {
            if (x1 + t < img_w) { int i = (y * img_w + (x1 + t)) * 3; data[i] = 255; data[i+1] = 0; data[i+2] = 0; }
            if (x2 - t >= 0) { int i = (y * img_w + (x2 - t)) * 3; data[i] = 255; data[i+1] = 0; data[i+2] = 0; }
        }
    }
}

int main() {
    std::cout << "🚀 ECLIPSE-V Omega Stress Test Initiated\n";
    std::cout << "==================================================\n";

    // 1. UNLOCK MULTI-CORE ARCHITECTURE
    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "EclipseEngine");
    Ort::SessionOptions session_options;
    // Setting to 0 unleashes all available CPU cores for execution
    session_options.SetIntraOpNumThreads(0); 
    session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

    const wchar_t* model_path = L"yolo26n.onnx";
    Ort::Session session(env, model_path, session_options);

    // 2. ESTABLISH THE "PRISTINE" SENSOR BUFFER
    const char* image_path = "test.jpg";
    int img_w, img_h, img_c;
    unsigned char* pristine_img = stbi_load(image_path, &img_w, &img_h, &img_c, 3);
    if (!pristine_img) return -1;
    
    size_t img_bytes = img_w * img_h * 3;
    unsigned char* working_img = new unsigned char[img_bytes];

    const int target_w = 640;
    const int target_h = 640;
    std::vector<float> input_tensor(3 * target_h * target_w);
    std::vector<int64_t> input_shape = {1, 3, target_h, target_w};
    
    Ort::MemoryInfo mem_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    const char* input_names[] = {"images"};
    const char* output_names[] = {"output0"};

    float x_ratio = (float)img_w / target_w;
    float y_ratio = (float)img_h / target_h;
    float scale_x = (float)img_w / target_w;
    float scale_y = (float)img_h / target_h;

    const int TEST_FRAMES = 100;
    std::cout << "[INFO] Spinning up high-frequency continuous loop (" << TEST_FRAMES << " frames)...\n";

    // ---------------------------------------------------------
    // THE REAL-TIME ENGINE LOOP
    // ---------------------------------------------------------
    auto loop_start = std::chrono::high_resolution_clock::now();

    for (int f = 0; f < TEST_FRAMES; ++f) {
        // A. Simulate fetching a fresh frame from the camera buffer
        std::memcpy(working_img, pristine_img, img_bytes);

        // B. High-Speed Preprocessing
        for (int h = 0; h < target_h; ++h) {
            int orig_y = (int)(h * y_ratio);
            for (int w = 0; w < target_w; ++w) {
                int src_idx = (orig_y * img_w + (int)(w * x_ratio)) * 3;
                input_tensor[0 * target_h * target_w + h * target_w + w] = working_img[src_idx + 0] / 255.0f;
                input_tensor[1 * target_h * target_w + h * target_w + w] = working_img[src_idx + 1] / 255.0f;
                input_tensor[2 * target_h * target_w + h * target_w + w] = working_img[src_idx + 2] / 255.0f;
            }
        }

        // C. Multi-Core Inference
        Ort::Value input_tensor_obj = Ort::Value::CreateTensor<float>(
            mem_info, input_tensor.data(), input_tensor.size(), input_shape.data(), input_shape.size()
        );
        auto output_tensors = session.Run(Ort::RunOptions{nullptr}, input_names, &input_tensor_obj, 1, output_names, 1);

        // D. In-Memory Rendering
        float* raw_output = output_tensors[0].GetTensorMutableData<float>();
        for (int i = 0; i < 300; ++i) {
            int offset = i * 6;
            if (raw_output[offset + 4] > 0.25f) {
                draw_bounding_box(working_img, img_w, img_h, 
                    (int)(raw_output[offset + 0] * scale_x), (int)(raw_output[offset + 1] * scale_y), 
                    (int)(raw_output[offset + 2] * scale_x), (int)(raw_output[offset + 3] * scale_y), 4);
            }
        }
    }

    auto loop_end = std::chrono::high_resolution_clock::now();
    
    // Clean up memory
    delete[] working_img;
    stbi_image_free(pristine_img);

    // ---------------------------------------------------------
    // CALCULATE FINAL TELEMETRY
    // ---------------------------------------------------------
    std::chrono::duration<float, std::milli> loop_ms = loop_end - loop_start;
    float avg_ms_per_frame = loop_ms.count() / TEST_FRAMES;
    float final_fps = 1000.0f / avg_ms_per_frame;

    std::cout << "\n📊 OMEGA ENGINE STRESS TEST RESULTS:\n";
    std::cout << "--------------------------------------------------\n";
    std::cout << " Frames Processed     : " << TEST_FRAMES << "\n";
    std::cout << " Total Test Time      : " << loop_ms.count() << " ms\n";
    std::cout << " Avg Time Per Frame   : " << avg_ms_per_frame << " ms\n";
    std::cout << "--------------------------------------------------\n";
    std::cout << " ⚡ TRUE PIPELINE SPEED : " << final_fps << " FPS\n";
    std::cout << "==================================================\n";
    std::cout << "Mission Accomplished. Systems standing down.\n";

    return 0;
}