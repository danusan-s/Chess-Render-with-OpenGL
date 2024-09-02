#include <iostream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <thread>
#include <chrono>

class StockfishProcess {
public:
    StockfishProcess(const std::string& stockfishPath) {
        // Create pipes for standard input and output.
        if (pipe(stdinPipe) == -1) {
            throw std::runtime_error("Failed to create stdin pipe: " + std::string(strerror(errno)));
        }
        if (pipe(stdoutPipe) == -1) {
            throw std::runtime_error("Failed to create stdout pipe: " + std::string(strerror(errno)));
        }

        pid_t pid = fork();
        if (pid == -1) {
            throw std::runtime_error("Failed to fork process: " + std::string(strerror(errno)));
        }

        if (pid == 0) { // Child process
            // Close unused pipe ends.
            close(stdinPipe[1]);
            close(stdoutPipe[0]);

            // Redirect standard input and output to pipes.
            dup2(stdinPipe[0], STDIN_FILENO);
            dup2(stdoutPipe[1], STDOUT_FILENO);
            dup2(stdoutPipe[1], STDERR_FILENO);

            // Close file descriptors after duplication.
            close(stdinPipe[0]);
            close(stdoutPipe[1]);

            execl(stockfishPath.c_str(), stockfishPath.c_str(), (char*)nullptr);
            // If execl fails.
            _exit(1);
        } else { // Parent process
            // Close unused pipe ends.
            close(stdinPipe[0]);
            close(stdoutPipe[1]);

            pid_ = pid;
            stdinFd = stdinPipe[1];
            stdoutFd = stdoutPipe[0];
        }
    }

    ~StockfishProcess() {
        if (pid_ > 0) {
            // Close file descriptors.
            close(stdinFd);
            close(stdoutFd);

            // Terminate child process.
            kill(pid_, SIGKILL);
            waitpid(pid_, nullptr, 0);
        }
    }

    void sendCommand(const std::string& command) {
        if (write(stdinFd, command.c_str(), command.length()) == -1) {
            throw std::runtime_error("Failed to write to stdin: " + std::string(strerror(errno)));
        }
        // Write a newline to ensure the command is sent.
        if (write(stdinFd, "\n", 1) == -1) {
            throw std::runtime_error("Failed to write newline to stdin: " + std::string(strerror(errno)));
        }
    }

    std::string readOutput() {
        char buffer[8192];
        ssize_t bytesRead;
        std::string result;

        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        while ((bytesRead = read(stdoutFd, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytesRead] = '\0';
            result.append(buffer);
            if (result.find('\n') != std::string::npos) {
                break;
            }
        }

        if (bytesRead == -1) {
            throw std::runtime_error("Failed to read from stdout: " + std::string(strerror(errno)));
        }

        return result;
    }

private:
    pid_t pid_ = -1;
    int stdinPipe[2];
    int stdoutPipe[2];
    int stdinFd = -1;
    int stdoutFd = -1;
};

