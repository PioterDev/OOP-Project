#include <cstring>

#include "logging.hpp"
#include "util.hpp"

Status Logger::init(const char* pathToFile) {
    this->stream.open(pathToFile, ofstream::out | ofstream::app);
    if(!this->stream.is_open()) return Status::FAILURE;
    this->setDateString(printFullDate);
    return Status::SUCCESS;
}

Status Logger::init(const string& pathToFile) {
    return Logger::init(pathToFile.c_str());
}

Logger::~Logger() {
    this->stream.close();
}

void Logger::setDateString(bool full) {
    time(&this->latest);
    struct tm* now = localtime(&this->latest);
    if(full) {
        snprintf(
            this->date, sizeof(this->date), "%04d-%02d-%02d %02d:%02d:%02d",
            now->tm_year + 1900, now->tm_mon + 1, now->tm_mday,
            now->tm_hour, now->tm_min, now->tm_sec
        );
    }
    else {
        snprintf(
            this->date + 11, sizeof(this->date) - 11ull, "%02d:%02d:%02d",
            now->tm_hour, now->tm_min, now->tm_sec
        );
    }
}

void Logger::printShared() {
    if(this->checkTimestamp()) this->setDateString(printFullDate);
#if printFullDate
    this->stream << '[' << this->date << ']';
#else
    this->stream << '[' << this->date + 11 << ']';
#endif
}

const char* Logger::getCurrentDateString(bool full) {
    if(this->checkTimestamp()) this->setDateString(printFullDate);
    return full ? this->date : this->date + 11;
}