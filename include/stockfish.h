#include <windows.h>
#include <iostream>
#include <string>
#include <stdexcept>
#include <chrono>
#include <thread>

class StockfishProcess {
public:
    StockfishProcess() {
        const std::string stockfishPath = "../stockfish/stockfish-windows-x86-64-avx2";
        std::vector<char> mutableStockfishPath(stockfishPath.begin(), stockfishPath.end());
        mutableStockfishPath.push_back('\0');
        SECURITY_ATTRIBUTES saAttr;
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;

        // Create a pipe for the child process's STDOUT.
        if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0)) {
            throw std::runtime_error("StdoutRd CreatePipe");
        }

        // Ensure the read handle to the pipe for STDOUT is not inherited.
        if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
            throw std::runtime_error("Stdout SetHandleInformation");
        }

        // Create a pipe for the child process's STDIN.
        if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0)) {
            throw std::runtime_error("Stdin CreatePipe");
        }

        // Ensure the write handle to the pipe for STDIN is not inherited.
        if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0)) {
            throw std::runtime_error("Stdin SetHandleInformation");
        }

        // Create the child process.
        PROCESS_INFORMATION piProcInfo;
        STARTUPINFO siStartInfo;
        BOOL bSuccess = FALSE;

        ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
        ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
        siStartInfo.cb = sizeof(STARTUPINFO);
        siStartInfo.hStdError = g_hChildStd_OUT_Wr;
        siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
        siStartInfo.hStdInput = g_hChildStd_IN_Rd;
        siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

        // Create the child process.
        bSuccess = CreateProcessA(
            NULL,
            mutableStockfishPath.data(), // shows like error but compiles idk
            NULL,          // process security attributes
            NULL,          // primary thread security attributes
            TRUE,          // handles are inherited
            0,             // creation flags
            NULL,          // use parent's environment
            NULL,          // use parent's current directory
            &siStartInfo,  // STARTUPINFO pointer
            &piProcInfo);  // receives PROCESS_INFORMATION

        // If an error occurs, exit the application.
        if (!bSuccess) {
            throw std::runtime_error("CreateProcess failed");
        }

        // Store the process information and close unneeded handles.
        hProcess = piProcInfo.hProcess;
        CloseHandle(piProcInfo.hThread);
        CloseHandle(g_hChildStd_OUT_Wr);
        CloseHandle(g_hChildStd_IN_Rd);
    }

    ~StockfishProcess() {
        CloseHandle(g_hChildStd_IN_Wr);
        CloseHandle(g_hChildStd_OUT_Rd);
        TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
    }

    void sendCommand(const std::string& command) {
        std::cout << "RECEIVED COMMAND: " << command << std::endl;
        DWORD dwWritten;
        if (!WriteFile(g_hChildStd_IN_Wr, command.c_str(), command.length(), &dwWritten, NULL)) {
            throw std::runtime_error("StdInWr WriteFile");
        }

        // Write a newline to ensure the command is sent
        if (!WriteFile(g_hChildStd_IN_Wr, "\n", 1, &dwWritten, NULL)) {
            throw std::runtime_error("StdInWr WriteFile");
        }
    }

    std::string readOutput() {
        // Read from the pipe that is the standard output for the child process.
        DWORD dwRead;
        CHAR chBuf[8192];
        std::string result;

        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        for (;;) {
            BOOL bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, 8192, &dwRead, NULL);
            if (!bSuccess || dwRead == 0) break;
            result.append(chBuf, dwRead);
            if (result.find("\n") != std::string::npos) {
                break;
            }
        }
        return result;
    }

private:
    HANDLE g_hChildStd_IN_Rd = NULL;
    HANDLE g_hChildStd_IN_Wr = NULL;
    HANDLE g_hChildStd_OUT_Rd = NULL;
    HANDLE g_hChildStd_OUT_Wr = NULL;
    HANDLE hProcess = NULL;
};