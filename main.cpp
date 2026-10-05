#include <opencv2/opencv.hpp>

#include <array>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

struct Camera {
    int index{};
    cv::VideoCapture capture;
    cv::Mat frame;
    std::mutex mutex;
    std::atomic<bool> running{false};
    std::thread worker;
    double fps{};
    std::string backend;

    bool open(int deviceIndex) {
        index = deviceIndex;
        // DirectShow is the most compatible OpenCV backend for many Windows USB webcams.
        if (!capture.open(index, cv::CAP_DSHOW)) {
            // Fallback in case this driver only exposes the Media Foundation backend.
            if (!capture.open(index, cv::CAP_MSMF)) return false;
        }
        capture.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
        capture.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
        capture.set(cv::CAP_PROP_FPS, 30);
        backend = capture.getBackendName();
        running = true;
        worker = std::thread([this] {
            using clock = std::chrono::steady_clock;
            auto windowStart = clock::now();
            int frames = 0;
            while (running) {
                cv::Mat next;
                if (!capture.read(next) || next.empty()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    continue;
                }
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    frame = std::move(next);
                }
                ++frames;
                const auto now = clock::now();
                const double elapsed = std::chrono::duration<double>(now - windowStart).count();
                if (elapsed >= 1.0) {
                    fps = frames / elapsed;
                    frames = 0;
                    windowStart = now;
                }
            }
        });
        return true;
    }

    cv::Mat snapshot() {
        std::lock_guard<std::mutex> lock(mutex);
        return frame.clone();
    }

    void close() {
        running = false;
        if (worker.joinable()) worker.join();
        if (capture.isOpened()) capture.release();
    }
};

static void saveSnapshots(const std::vector<Camera*>& cameras) {
    const auto stamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::ostringstream folder;
    folder << "capture_" << stamp;
    std::string dir = folder.str();
    try {
        std::filesystem::create_directory(dir);
    } catch (...) {}
    for (std::size_t i = 0; i < cameras.size(); ++i) {
        cv::Mat image = cameras[i]->snapshot();
        if (!image.empty()) {
            const std::string path = dir + "/camera_" + std::to_string(i + 1) + ".jpg";
            cv::imwrite(path, image);
            std::cout << "Gespeichert: " << path << '\n';
        }
    }
}

int main() {
    std::cout << "Dart Kamera-Test – suche USB-Kameras (0 bis 9)...\n";
    std::vector<std::unique_ptr<Camera>> found;
    for (int i = 0; i < 10 && found.size() < 3; ++i) {
        auto camera = std::make_unique<Camera>();
        if (camera->open(i)) {
            // Allow the first frame to arrive before deciding the device is usable.
            std::this_thread::sleep_for(std::chrono::milliseconds(400));
            if (camera->snapshot().empty()) {
                camera->close();
                continue;
            }
            std::cout << "Kameraindex " << i << " geöffnet (" << camera->backend << ")\n";
            found.push_back(std::move(camera));
        }
    }

    if (found.empty()) {
        std::cerr << "Keine Kamera geöffnet. Prüfe USB-Verbindung und Kamerazugriff in Windows.\n";
        std::cerr << "Tasten: ESC beendet, S speichert Bilder.\n";
        return 1;
    }

    std::cout << found.size() << " Kamera(s) aktiv. ESC beendet, S speichert drei Beispielbilder.\n";
    std::vector<Camera*> pointers;
    for (auto& camera : found) pointers.push_back(camera.get());

    bool keepRunning = true;
    while (keepRunning) {
        for (std::size_t i = 0; i < found.size(); ++i) {
            cv::Mat image = found[i]->snapshot();
            if (image.empty()) continue;
            std::ostringstream label;
            label << "Kamera " << (i + 1) << " | Index " << found[i]->index
                  << " | " << image.cols << "x" << image.rows
                  << " | " << std::fixed << std::setprecision(1) << found[i]->fps << " FPS";
            cv::putText(image, label.str(), {14, 28}, cv::FONT_HERSHEY_SIMPLEX, 0.7,
                        {0, 255, 0}, 2, cv::LINE_AA);
            cv::imshow("Dart Kamera " + std::to_string(i + 1), image);
        }
        const int key = cv::waitKey(1) & 0xff;
        if (key == 27 || key == 'q' || key == 'Q') keepRunning = false;
        if (key == 's' || key == 'S') saveSnapshots(pointers);
    }
    for (auto& camera : found) camera->close();
    cv::destroyAllWindows();
    return 0;
}
