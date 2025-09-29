// editorweb.cpp
// 需要链接: -lurlmon -lole32 -lwinpthread -luuid
// 使用 C++14
#include <windows.h>
#include <urlmon.h>
#include <objidl.h>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <memory>
#include <algorithm>
#include <cassert>
#include <vector>
#include <locale>
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "ole32.lib")
#pragma once

#include "editorchar.cpp"

// downinfo 下载状态类
class downinfo {
public:
    uint64_t total_bytes = 0;       // ulProgressMax，未知为0
    uint64_t downloaded_bytes = 0;  // ulProgress
    double speed = -1.0;            // bytes/sec，根据上一次计算，未知时为 -1
    DWORD status_code = 0;          // ulStatusCode (BINDSTATUS_*)
    std::string status_text;        // UTF-8 状态文本（如果有）
    bool completed = false;         // 是否完成（成功或失败后的完成）
    HRESULT hr = S_OK;              // 最后一次 HRESULT（成功或错误）
};

// urldown 网络下载类
class urldown {
public:
    urldown() = default;
    ~urldown() {
        shutdown();
    }

    // 初始化 URL、文件（UTF-8）
    void init(const std::string& url_utf8, const std::string& file_utf8) {
        std::lock_guard<std::mutex> lg(state_mtx_);
        url_ = url_utf8;
        file_ = file_utf8;
    }

    // 在后台启动下载（非阻塞）
    bool downht() {
        std::lock_guard<std::mutex> lg(state_mtx_);
        if (running_.load()) return false; // 已在运行

        // reset shared state
        shared_ = std::make_shared<SharedState>();
        shared_->info = downinfo();
        shared_->last_tick_ms = GetTickCount();
        shared_->last_bytes = 0;
        shared_->cancelRequested.store(false);
        shared_->pBinding.store(nullptr);
        running_.store(true);

        // copy url/file for thread use
        const std::string url_copy = url_;
        const std::string file_copy = file_;

        worker_ = std::thread([this, url_copy, file_copy]() {
            // Initialize COM in this thread
            HRESULT hr_co = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
            (void)hr_co; // 忽略返回值（若需要可以记录）

            // 创建回调对象（初始 ref=1）
            BindStatusCallback* cb = new BindStatusCallback(shared_);

            // URL 和文件名转宽字符
            std::wstring url_w = Utf8ToUtf16(url_copy);
            std::wstring file_w = Utf8ToUtf16(file_copy);

            // URLDownloadToFileW 是阻塞调用，传入我们的回调
            HRESULT hr = URLDownloadToFileW(nullptr, url_w.c_str(), file_w.c_str(), 0, cb);

            // 更新最终状态
            {
                std::lock_guard<std::mutex> lg(shared_->mtx);
                shared_->info.hr = hr;
                shared_->info.completed = true;
            }

            // 释放回调对象（URLMon 可能已经对其 AddRef/Release）
            cb->Release();

            // 释放存储的 IBinding*（若有）
            IBinding* b = shared_->pBinding.exchange(nullptr);
            if (b) {
                b->Release();
            }

            running_.store(false);
            CoUninitialize();
        });

        return true;
    }

    // 将最近下载状态保存到 DI（线程安全）
    void xzinfo(downinfo& DI) {
        if (!shared_) return;
        std::lock_guard<std::mutex> lg(shared_->mtx);
        DI = shared_->info;
    }

    // 停止下载，删除正在下载的文件（阻塞直到线程结束）
    void shutdown() {
        // request cancel
        if (shared_) {
            shared_->cancelRequested.store(true);
            // 交换出绑定指针，并由调用方负责 Release
            IBinding* pb = shared_->pBinding.exchange(nullptr);
            if (pb) {
                // 正确调用 Abort()（无参数）
                pb->Abort();
                pb->Release();
            }
        }

        // join worker
        if (worker_.joinable()) {
            worker_.join();
        }

        running_.store(false);

        // 删除临时文件（如果存在）
        if (!file_.empty()) {
            std::wstring file_w = Utf8ToUtf16(file_);
            DeleteFileW(file_w.c_str());
        }
    }

    // n_url 返回 url UTF-8
    std::string n_url() {
        std::lock_guard<std::mutex> lg(state_mtx_);
        return url_;
    }

    // n_file 返回文件名 UTF-8
    std::string n_file() {
        std::lock_guard<std::mutex> lg(state_mtx_);
        return file_;
    }

private:
    // 共享状态结构体（callback 与 urldown 之间共享）
    struct SharedState {
        std::mutex mtx;               // 保护 info / last_tick_ms / last_bytes
        downinfo info;
        unsigned int last_tick_ms = 0;    // 上一次计时（毫秒），使用 GetTickCount64
        uint64_t last_bytes = 0;
        std::atomic<IBinding*> pBinding; // 保存 OnStartBinding 提供的 IBinding*（已 AddRef）
        std::atomic<bool> cancelRequested{ false };
    };

    // 自定义 IBindStatusCallback
    class BindStatusCallback : public IBindStatusCallback {
    public:
        explicit BindStatusCallback(std::shared_ptr<SharedState> shared)
            : ref_(1), shared_(shared) {}

        // IUnknown
        STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) override {
            if (!ppvObject) return E_POINTER;
            if (riid == IID_IUnknown || riid == IID_IBindStatusCallback) {
                *ppvObject = static_cast<IBindStatusCallback*>(this);
                AddRef();
                return S_OK;
            }
            *ppvObject = nullptr;
            return E_NOINTERFACE;
        }
        STDMETHODIMP_(ULONG) AddRef() override {
            return InterlockedIncrement(&ref_);
        }
        STDMETHODIMP_(ULONG) Release() override {
            ULONG c = InterlockedDecrement(&ref_);
            if (c == 0) delete this;
            return c;
        }

        // IBindStatusCallback
        STDMETHODIMP OnStartBinding(DWORD /*dwReserved*/, IBinding* pib) override {
            if (pib) {
                pib->AddRef();
                IBinding* prev = shared_->pBinding.exchange(pib);
                if (prev) prev->Release();
            }
            return S_OK;
        }
        STDMETHODIMP GetPriority(LONG* /*pnPriority*/) override { return E_NOTIMPL; }
        STDMETHODIMP OnLowResource(DWORD /*reserved*/) override { return S_OK; }

        STDMETHODIMP OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText) override {
            // 使用 GetTickCount64 计算速度
            unsigned int now_ms = GetTickCount();

            std::lock_guard<std::mutex> lg(shared_->mtx);
            shared_->info.downloaded_bytes = ulProgress;
            shared_->info.total_bytes = ulProgressMax;
            shared_->info.status_code = ulStatusCode;
            if (szStatusText) shared_->info.status_text = Utf16ToUtf8(szStatusText);
            else shared_->info.status_text.clear();

            // 计算速度（bytes/sec），基于上一次记录
            uint64_t last_ms = shared_->last_tick_ms;
            if (last_ms == 0) {
                // 初次记录
                shared_->last_tick_ms = now_ms;
                shared_->last_bytes = ulProgress;
                shared_->info.speed = -1.0;
            } else {
                uint64_t delta_ms = 0;
                if (now_ms >= last_ms) delta_ms = now_ms - last_ms;
                else {
                    // 如果发生回绕（GetTickCount64 实际上很难回绕），则重置
                    delta_ms = 0;
                }

                if (delta_ms > 50) { // 50ms 最小间隔，避免噪声
                    uint64_t deltaBytes = 0;
                    if (ulProgress >= shared_->last_bytes) deltaBytes = ulProgress - shared_->last_bytes;
                    else deltaBytes = 0;

                    if (delta_ms > 0) {
                        shared_->info.speed = static_cast<double>(deltaBytes) * 1000.0 / static_cast<double>(delta_ms);
                    } else {
                        shared_->info.speed = -1.0;
                    }

                    shared_->last_tick_ms = now_ms;
                    shared_->last_bytes = ulProgress;
                }
                // 若间隔太小，则保持之前的 speed
            }
            return S_OK;
        }

        STDMETHODIMP OnStopBinding(HRESULT hrStatus, LPCWSTR /*szError*/) override {
            std::lock_guard<std::mutex> lg(shared_->mtx);
            shared_->info.hr = hrStatus;
            shared_->info.completed = true;
            return S_OK;
        }

        STDMETHODIMP GetBindInfo(DWORD* /*grfBINDF*/, BINDINFO* /*pbindinfo*/) override { return E_NOTIMPL; }
        STDMETHODIMP OnDataAvailable(DWORD /*grfBSCF*/, DWORD /*dwSize*/, FORMATETC* /*pformatetc*/, STGMEDIUM* /*pstgmed*/) override {
            // 不需要处理 URLDownloadToFile 的数据回调
            return S_OK;
        }
        STDMETHODIMP OnObjectAvailable(REFIID /*riid*/, IUnknown* /*punk*/) override { return S_OK; }

    private:
        ~BindStatusCallback() = default;
        LONG ref_;
        std::shared_ptr<SharedState> shared_;
    };

    // 成员
    std::mutex state_mtx_;
    std::string url_;
    std::string file_;

    std::shared_ptr<SharedState> shared_;
    std::thread worker_;
    std::atomic<bool> running_{ false };
};
