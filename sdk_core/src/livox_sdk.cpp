//
// The MIT License (MIT)
//
// Copyright (c) 2019 Livox. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include <memory>

#include "livox_sdk.h"
#include "command_handler/command_handler.h"
#include "data_handler/data_handler.h"
#include "base/logging.h"
#include "device_manager.h"
#ifdef WIN32
#include <winsock2.h>
#endif // WIN32


using namespace livox;

static std::unique_ptr<IOThread> g_thread = nullptr;
static bool is_initialized = false;

void GetLivoxSdkVersion(LivoxSdkVersion *version)
{
    if (version != NULL)
    {
        version->major = LIVOX_SDK_MAJOR_VERSION;
        version->minor = LIVOX_SDK_MINOR_VERSION;
        version->patch = LIVOX_SDK_PATCH_VERSION;
    }
}

bool Init()
{
    if (is_initialized)
    {
        LOG_ERROR("Failed to initialize Livox SDK - Already initialized.");
        return false;
    }

#ifdef WIN32
    WORD sockVersion = MAKEWORD(2, 0);
    WSADATA wsdata;
    if (WSAStartup(sockVersion, &wsdata) != 0)
    {
        return false;
    }
#endif // WIN32

    is_initialized = false;

    // Initialise the logging.
    InitLogger();

    if( nullptr != g_thread )
    {
        LOG_ERROR("Failed to initialize Livox SDK - Thread not null");
        return false;
    }

    // Create the global IO thread.
    g_thread = std::make_unique<IOThread>();
    const bool thread_initialized = g_thread->Init();
    if (!thread_initialized)
    {
        LOG_ERROR("Failed to initialize Livox SDK - Failed to initialize thread.");
        return false;
    }


    // Initialize the device discovery.
    const bool device_discovery_initialized = device_discovery().Init();
    if (!device_discovery_initialized)
    {
        LOG_ERROR("Failed to initialize Livox SDK - Failed to initialize device discovery.");
        return false;
    }

    // Initialize the device manager.
    const bool device_manager_initialized = device_manager().Init();
    if (!device_manager_initialized)
    {
        LOG_ERROR("Failed to initialize Livox SDK - Failed to initialize device manager.");
        return false;
    }

    // Initialize the command handler.
    const bool command_handler_initialized = command_handler().Init(g_thread->loop());
    if (!command_handler_initialized)
    {
        LOG_ERROR("Failed to initialize Livox SDK - Failed to initialize command handler.");
        return false;
    }

    // Initialize the data handler.
    const bool data_handler_initialized = data_handler().Init();
    if (!data_handler_initialized)
    {
        LOG_ERROR("Failed to initialize Livox SDK - Failed to initialize data handler.");
        return false;
    }

    is_initialized = true;
    return true;
}

void Uninit()
{
    if (!is_initialized)
    {
        LOG_ERROR("Failed to uninitialize Livox SDK - Not initialized.");
        return;
    }

#ifdef WIN32
    WSACleanup();
#endif // WIN32

    device_discovery().Uninit();

    command_handler().Uninit();

    data_handler().Uninit();

    device_manager().Uninit();

    if (nullptr != g_thread)
    {
        // Quit any running processes.
        g_thread->Quit();

        // Join any outstanding threads.
        g_thread->Join();

        // Uninitialize the thread.
        g_thread->Uninit();
    }

    UninitLogger();

    is_initialized = false;
}

bool Start()
{
    const bool thread_available = (nullptr != g_thread);
    if (!thread_available)
    {
        LOG_ERROR("Failed to start Livox SDK - thread not available.");
        return false;
    }

    const bool device_discovery_started = device_discovery().Start(g_thread->loop());
    if (!device_discovery_started)
    {
        LOG_ERROR("Failed to start Livox SDK - device discovery failed to start.");
        return false;
    }

    const bool thread_started = g_thread->Start();
    if (!thread_started)
    {
        LOG_ERROR("Failed to start Livox SDK - thread failed to start.");
        return false;
    }

    // Livox SDK started successfully.
    return true;
}

void SaveLoggerFile()
{
    is_save_log_file = true;
}

void DisableConsoleLogger()
{
    is_console_log_enable = false;
}
