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

#define printFullDate true

#if LOGLEVEL == 0 //Trace level
/**
 * @brief This has to be this ugly macro for that juicy 
 * `__FILE__`, `__LINE__` and `__func__` macros.
 */
#define LogTrace(logger, message) do { \
    logger.trace("[", __FILE__, "/", __func__, ":", __LINE__, "] ", message); \
} while(0)
#define LogDebug(logger, message)
#define LoggerLineTerminator std::endl
#elif LOGLEVEL == 1 //Debug level
#define LogTrace(logger, message)
#define LogDebug(logger, message) do { \
    logger.debug(message); \
} while(0)
#define LoggerLineTerminator endl
#else
#define LogTrace(logger, message)
#define LogDebug(logger, message)
#define LoggerLineTerminator '\n'
#endif /* LOGLEVEL */

class Logger {
    private:
        std::ofstream stream;
        time_t latest = 0;
        //Using a C string for performance
        char date[20] = {0};

        void setDateString(bool full);
        
        bool checkTimestamp() { return time(nullptr) > latest; }
    public:
        ~Logger();
        
        /**
         * @brief Prints the log prefix shared across all log levels...
         * which just prints the date.
         */
        void printShared();

        /**
         * @brief Get the internal date string. DO NOT FREE IT!!
         * 
         * @param full whether to return full date or hh:mm:ss only
         * 
         * @return `const char*` to the date string
         */
        const char* getDateString(bool full) { return full ? date : date + 11; }

        /**
         * @brief Get the internal date string, but if it's out-of-date, update it beforehand. DO NOT FREE IT!!
         * 
         * @param full whether to return full date or hh:mm:ss only
         * 
         * @return `const char*` to the date string
         */
        const char* getCurrentDateString(bool full);

        /**
         * @brief Initializes the logger.
         * 
         * @param pathToFile path to file
         * @return 0 on success, 1 on failure
         */
        NoDiscard Enums::Status init(const std::string& pathToFile);

        /**
         * @brief Initializes the logger.
         * 
         * @param pathToFile path to file
         * @return 0 on success, 1 on failure
         */
        NoDiscard Enums::Status init(const char* pathToFile);

        void flush() { this->stream << std::endl; }

        template<class...Args> void print(Args...args) {
            (this->stream << ... << args);
        }

        template<class...Args> void println(Args...args) {
            (this->stream << ... << args) << LoggerLineTerminator;
        }

        template<class...Args> void debug(Args... args) {
            this->printShared();
            (this->stream << " [Debug] " << ... << args) << std::endl;
        }

        template<class...Args> void info(Args... args) {
            this->printShared();
            (this->stream << " [Info] " << ... << args) << LoggerLineTerminator;
        }

        template<class...Args> void warn(Args... args) {
            this->printShared();
            (this->stream << " [Warning] " << ... << args) << LoggerLineTerminator;
        }

        template<class...Args> void error(Args... args) {
            this->printShared();
            (this->stream << " [Error] " << ... << args) << LoggerLineTerminator;
        }

        template<class...Args> void fatal(Args... args) {
            this->printShared();
            (this->stream << " [Fatal] " << ... << args) << std::endl;
        }

        template<class...Args> void trace(Args... args) {
            this->printShared();
            (this->stream << " [Trace] " << ... << args) << std::endl;
        }
};

#endif /* Header */