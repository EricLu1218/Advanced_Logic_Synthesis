#pragma once
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>

class GlobalTimer
{
    struct TimeInfo
    {
        std::chrono::high_resolution_clock::time_point startTime, endTime;

        TimeInfo() : startTime(std::chrono::high_resolution_clock::now()),
                     endTime(std::chrono::high_resolution_clock::now()) {}
    };

    std::unordered_map<std::string, TimeInfo *> timeInfos;
    std::chrono::seconds timeLimit;

    template <class ToDuration = std::chrono::milliseconds>
    ToDuration getDuration(const std::string &tag)
    {
        auto timeInfo = timeInfos.at(tag);
        return std::chrono::duration_cast<ToDuration>(timeInfo->endTime - timeInfo->startTime);
    }

public:
    GlobalTimer(int timeLimitInSecond)
        : timeLimit(std::chrono::seconds(timeLimitInSecond))
    {
        timeInfos.emplace("_timeLimit", new TimeInfo());
    }

    bool overTime()
    {
        auto const tag = "_timeLimit";
        timeInfos.at(tag)->endTime = std::chrono::high_resolution_clock::now();
        return getDuration<>(tag) >= timeLimit;
    }

    void startTimer(const std::string &tag)
    {
        if (timeInfos.find(tag) == timeInfos.end())
            timeInfos.emplace(tag, new TimeInfo());
        else
            timeInfos.at(tag)->startTime = std::chrono::high_resolution_clock::now();
    }

    void stopTimer(const std::string &tag)
    {
        timeInfos.at(tag)->endTime = std::chrono::high_resolution_clock::now();
    }

    void printTime(const std::string &tag)
    {
        auto duration = getDuration<>(tag).count();
        auto minute = duration / 1000 / 60;
        auto second = duration / 1000.0 - minute * 60;
        std::cout << std::setw(15) << std::left << tag + ":" << minute << " m " << second << " s" << std::endl;
    }
};
