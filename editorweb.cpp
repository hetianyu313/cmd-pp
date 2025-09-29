// editorweb.cpp
// ��Ҫ����: -lurlmon -lole32 -lwinpthread -luuid
// ʹ�� C++14
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

// downinfo ����״̬��
class downinfo {
public:
    uint64_t total_bytes = 0;       // ulProgressMax��δ֪Ϊ0
    uint64_t downloaded_bytes = 0;  // ulProgress
    double speed = -1.0;            // bytes/sec��������һ�μ��㣬δ֪ʱΪ -1
    DWORD status_code = 0;          // ulStatusCode (BINDSTATUS_*)
    std::string status_text;        // UTF-8 ״̬�ı�������У�
    bool completed = false;         // �Ƿ���ɣ��ɹ���ʧ�ܺ����ɣ�
    HRESULT hr = S_OK;              // ���һ�� HRESULT���ɹ������
};

// urldown ����������
class urldown {
public:
    urldown() = default;
    ~urldown() {
        shutdown();
    }

    // ��ʼ�� URL���ļ���UTF-8��
    void init(const std::string& url_utf8, const std::string& file_utf8) {
        std::lock_guard<std::mutex> lg(state_mtx_);
        url_ = url_utf8;
        file_ = file_utf8;
    }

    // �ں�̨�������أ���������
    bool downht() {
        std::lock_guard<std::mutex> lg(state_mtx_);
        if (running_.load()) return false; // ��������

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
            (void)hr_co; // ���Է���ֵ������Ҫ���Լ�¼��

            // �����ص����󣨳�ʼ ref=1��
            BindStatusCallback* cb = new BindStatusCallback(shared_);

            // URL ���ļ���ת���ַ�
            std::wstring url_w = Utf8ToUtf16(url_copy);
            std::wstring file_w = Utf8ToUtf16(file_copy);

            // URLDownloadToFileW ���������ã��������ǵĻص�
            HRESULT hr = URLDownloadToFileW(nullptr, url_w.c_str(), file_w.c_str(), 0, cb);

            // ��������״̬
            {
                std::lock_guard<std::mutex> lg(shared_->mtx);
                shared_->info.hr = hr;
                shared_->info.completed = true;
            }

            // �ͷŻص�����URLMon �����Ѿ����� AddRef/Release��
            cb->Release();

            // �ͷŴ洢�� IBinding*�����У�
            IBinding* b = shared_->pBinding.exchange(nullptr);
            if (b) {
                b->Release();
            }

            running_.store(false);
            CoUninitialize();
        });

        return true;
    }

    // ���������״̬���浽 DI���̰߳�ȫ��
    void xzinfo(downinfo& DI) {
        if (!shared_) return;
        std::lock_guard<std::mutex> lg(shared_->mtx);
        DI = shared_->info;
    }

    // ֹͣ���أ�ɾ���������ص��ļ�������ֱ���߳̽�����
    void shutdown() {
        // request cancel
        if (shared_) {
            shared_->cancelRequested.store(true);
            // ��������ָ�룬���ɵ��÷����� Release
            IBinding* pb = shared_->pBinding.exchange(nullptr);
            if (pb) {
                // ��ȷ���� Abort()���޲�����
                pb->Abort();
                pb->Release();
            }
        }

        // join worker
        if (worker_.joinable()) {
            worker_.join();
        }

        running_.store(false);

        // ɾ����ʱ�ļ���������ڣ�
        if (!file_.empty()) {
            std::wstring file_w = Utf8ToUtf16(file_);
            DeleteFileW(file_w.c_str());
        }
    }

    // n_url ���� url UTF-8
    std::string n_url() {
        std::lock_guard<std::mutex> lg(state_mtx_);
        return url_;
    }

    // n_file �����ļ��� UTF-8
    std::string n_file() {
        std::lock_guard<std::mutex> lg(state_mtx_);
        return file_;
    }

private:
    // ����״̬�ṹ�壨callback �� urldown ֮�乲��
    struct SharedState {
        std::mutex mtx;               // ���� info / last_tick_ms / last_bytes
        downinfo info;
        unsigned int last_tick_ms = 0;    // ��һ�μ�ʱ�����룩��ʹ�� GetTickCount64
        uint64_t last_bytes = 0;
        std::atomic<IBinding*> pBinding; // ���� OnStartBinding �ṩ�� IBinding*���� AddRef��
        std::atomic<bool> cancelRequested{ false };
    };

    // �Զ��� IBindStatusCallback
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
            // ʹ�� GetTickCount64 �����ٶ�
            unsigned int now_ms = GetTickCount();

            std::lock_guard<std::mutex> lg(shared_->mtx);
            shared_->info.downloaded_bytes = ulProgress;
            shared_->info.total_bytes = ulProgressMax;
            shared_->info.status_code = ulStatusCode;
            if (szStatusText) shared_->info.status_text = Utf16ToUtf8(szStatusText);
            else shared_->info.status_text.clear();

            // �����ٶȣ�bytes/sec����������һ�μ�¼
            uint64_t last_ms = shared_->last_tick_ms;
            if (last_ms == 0) {
                // ���μ�¼
                shared_->last_tick_ms = now_ms;
                shared_->last_bytes = ulProgress;
                shared_->info.speed = -1.0;
            } else {
                uint64_t delta_ms = 0;
                if (now_ms >= last_ms) delta_ms = now_ms - last_ms;
                else {
                    // ����������ƣ�GetTickCount64 ʵ���Ϻ��ѻ��ƣ���������
                    delta_ms = 0;
                }

                if (delta_ms > 50) { // 50ms ��С�������������
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
                // �����̫С���򱣳�֮ǰ�� speed
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
            // ����Ҫ���� URLDownloadToFile �����ݻص�
            return S_OK;
        }
        STDMETHODIMP OnObjectAvailable(REFIID /*riid*/, IUnknown* /*punk*/) override { return S_OK; }

    private:
        ~BindStatusCallback() = default;
        LONG ref_;
        std::shared_ptr<SharedState> shared_;
    };

    // ��Ա
    std::mutex state_mtx_;
    std::string url_;
    std::string file_;

    std::shared_ptr<SharedState> shared_;
    std::thread worker_;
    std::atomic<bool> running_{ false };
};
