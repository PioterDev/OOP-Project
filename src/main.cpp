#include "Bindings.h"
#include "Testing.h"

#include <SDL_messagebox.h>

#include "Game/Main/Game.hpp"

int test();

using namespace Enums;

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
#ifdef DO_TEST
    return test();
#else
    Game game;
    Status status = game.init();
    if(status != Status::SUCCESS) Unlikely {
        char message[128];
        snprintf(
            message, sizeof(message), 
            "The program could not start and exited with code %d."
            " For further information, please contact the developer.", 
            static_cast<int>(status)
        );
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR, "Could not start", message, nullptr
        );
        return static_cast<int>(status);
    }
    
    try {
        game.run();
    }
    catch(int code) {
        //TODO: finish this
    }
    catch(const std::bad_alloc& e) {
        char message[128];
        snprintf(
            message, sizeof(message),
            "The program encountered a fatal exception: OutOfMemory and"
            "cannot continue normally."
        );
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR, "No memory",
            message, game.getWindow()
        );
        return static_cast<int>(Status::ALLOC_FAILURE);
    }


    return 0;
#endif
}


size_t numberOfTests = 0;
size_t numberOfPassedTests = 0;
Vector<void(*)(), 1024> testHooks;
FILE* testLog = nullptr;
const char* testLogPath = "testLog.log";

#if defined (WINDOWS)
#include <windows.h>
#elif defined(LINUX)
//unistd.h is already present
#endif

void __registerTestFunction__(fptr function) {
    if(testHooks.size() == testHooks.capacity()) {
        //Let's begin the ugly part of being able to
        //write an information that there is no space left
        //for registering tests and exiting
        //portably before libc is even initialized.
        //And the only way to do this is with raw OS-dependent functions
        const char info[] = "There is not enough space for registering tests. "
        "Increase the buffer size.";
#if defined(WINDOWS)
        HANDLE fd = CreateFile(testLogPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(fd == INVALID_HANDLE_VALUE) { //I mean, what the f*ck is the process supposed to do in this situation...
            ExitProcess(2);
        }
        DWORD b;
        WriteFile(fd, info, sizeof(info) - 1, &b, NULL);
        CloseHandle(fd);
        ExitProcess(1);
#elif defined(LINUX)
        int fd = open(testLogPath, O_CREAT | O_WRITE | O_TRUNC);
        if(fd == -1) {
            //literally the only way to exit on Linux
            //as stdlib.h's _exit conflicts with unistd.h's one
            //fortunately, Linux syscall numbers are stable
            //so this can stay
            __asm__(
                "movq $60, %rax\n\t"
                "movq $2, %rdi\n\t"
                "syscall"
            );
        }
        write(fd, info, sizeof(info));
        close(fd);
        __asm__(
            "movq $60, %rax\n\t"
            "movq $2, %rdi\n\t"
            "syscall"
        );
#endif
    }
    testHooks.append(function);
}

void __checkTestEq__(int v, const char isFatal, const char* failFmt, ...) {
    numberOfTests++;
    if(v == 0) {
        va_list args;
        va_start(args, failFmt);

        if(isFatal) {
            fprintf(testLog, "[⛔] ");
            vfprintf(testLog, failFmt, args);
            va_end(args);
            fprintf(testLog, "\n");
            fflush(testLog);
            exit(2);
        }
        else {
            fprintf(testLog, "[❌] ");
            vfprintf(testLog, failFmt, args);
            va_end(args);
            fprintf(testLog, "\n");
            fflush(testLog);
        }
    }
    else numberOfPassedTests++;
}

int test() {
    setlocale(LC_ALL, "");

    testLog = fopen(testLogPath, "w+");
    if(testLog == nullptr) exit(1);
    for(auto it = testHooks.begin(); it != testHooks.end(); ++it) {
        (*it)();
    }
    fprintf(testLog, "%llu/%llu tests passed\n", numberOfPassedTests, numberOfTests);
    fclose(testLog);

    char message[128];
    snprintf(message, sizeof(message), "%llu/%llu tests passed.", numberOfPassedTests, numberOfTests);
    SDL_ShowSimpleMessageBox(
        numberOfPassedTests < numberOfTests ?
        SDL_MESSAGEBOX_ERROR : SDL_MESSAGEBOX_INFORMATION,
        "Tests", message, nullptr
    );
    return numberOfTests - numberOfPassedTests;
}