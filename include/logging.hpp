#ifndef LOGGING_HPP
/**
 * @file logging.hpp
 * @author PioterDev
 * @brief A simple class for logging functionality
 * and a macro for trace logging to pinpoint 
 * the currently executing function.
 * @version 0.1
 * @date 2024-10-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#define LOGGING_HPP

#include "Bindings.h"

#include <iostream>
#include <fstream>

#include "deus.hpp"

using std::ios, std::ofstream, std::string, std::endl;

using namespace Enums;

#define printFullDate true

#if LOGLEVEL == 0 //Trace level
/**
 * @brief This has to be this ugly macro for that juicy 
 * `__FILE__`, `__LINE__` and `__func__` macros.
 */
#define LogTrace(logger, message) do { \
    logger.print_shared(); \
    logger.print(" [Trace] [", __FILE__, "/", __func__, ":", __LINE__, "] ", message, "\n");  \
} while(0)
#define LogDebug(logger, message)
#endif /* LOGLEVEL == Trace */

#if LOGLEVEL == 1 //Debug level
#define LogTrace(logger, message)
#define LogDebug(logger, message) do { \
    logger.debug(message); \
} while(0)
#endif /* LOGLEVEL == Debug */

class Logger {
    private:
        ofstream stream;
        time_t latest = 0;
        //Using a C string for performance
        char date[20] = {0};

        void setDateString(bool full);
        
        inline bool checkTimestamp() {
            return time(nullptr) > latest;
        }
        
    public:
        /**
         * @brief Prints the log prefix shared across all log levels...
         * which just prints the date.
         * 
         */
        inline void printShared() {
            if(checkTimestamp()) setDateString(printFullDate);
#if printFullDate
            stream << '[' << date << ']';
#else
            stream << '[' << date + 11 << ']';
#endif
        }

        /**
         * @brief Get the internal date string. DO NOT FREE IT!!
         * 
         * @param full whether to return full date or hh:mm:ss only
         * 
         * @return `const char*` to the date string
         */
        inline const char* getDateString(bool full) {
            return full ? date : date + 11;
        }

        /**
         * @brief Get the internal date string, but if it's out-of-date, update it beforehand. DO NOT FREE IT!!
         * 
         * @param full whether to return full date or hh:mm:ss only
         * 
         * @return `const char*` to the date string
         */
        inline const char* getCurrentDateString(bool full) {
            if(checkTimestamp()) setDateString(printFullDate);
            return full ? date : date + 11;
        }

        /**
         * @brief Initializes the logger.
         * 
         * @param pathToFile path to file
         * @return 0 on success, 1 on failure
         */
        NoDiscard Status init(const string& pathToFile);

        /**
         * @brief Initializes the logger.
         * 
         * @param pathToFile path to file
         * @return 0 on success, 1 on failure
         */
        NoDiscard Status init(const char* pathToFile);

        ~Logger();

        void flush() { this->stream << endl; }

        template<class...Args> inline void print(Args...args) {
            (this->stream << ... << args);
        }

        template<class...Args> inline void debug(Args... args) {
            printShared();
            (this->stream << " [Debug] " << ... << args) << endl;
        }

        template<class...Args> inline void info(Args... args) {
            printShared();
            (this->stream << " [Info] " << ... << args) << endl;
        }

        template<class...Args> inline void warn(Args... args) {
            printShared();
            (this->stream << " [Warning] " << ... << args) << endl;
        }

        template<class...Args> inline void error(Args... args) {
            printShared();
            (this->stream << " [Error] " << ... << args) << endl;
        }

        template<class...Args> inline void fatal(Args... args) {
            printShared();
            (this->stream << " [Fatal] " << ... << args) << endl;
        }
};

#endif /* Header */