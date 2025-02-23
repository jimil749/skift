#pragma once

#include <libsystem/Common.h>

#define EPOCH_YEAR 1970

#define SECONDS_PER_MINUTE (60)
#define SECONDS_PER_HOURS (SECONDS_PER_MINUTE * 60)
#define SECONDS_PER_DAY (SECONDS_PER_HOURS * 24)
#define MONTH_PER_YEAR 12

static const int DAYS_PER_MONTH[2][MONTH_PER_YEAR] = {
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
};

static const int DAYS_PER_YEAR[2] = {365, 366};

#define IS_LEAP_YEAR(__year) \
    ((!((__year) % 4) && (__year) % 100) || !((__year) % 400))

typedef uint32_t ElapsedTime;

typedef uint32_t TimeStamp;

typedef uint32_t Timeout;

struct Time
{
    int second;
    int minute;
    int hour;
};

struct Date
{
    int day;
    int month;
    int year;
};

union DateTime
{
    struct
    {
        Time time;
        Date date;
    };

    struct
    {
        int second;
        int minute;
        int hour;
        int day;
        int month;
        int year;
    };
};

TimeStamp timestamp_now();

Time timestamp_to_time(TimeStamp timestamp);

Date timestamp_to_date(TimeStamp timestamp);

DateTime timestamp_to_datetime(TimeStamp timestamp);

TimeStamp datetime_to_timestamp(DateTime datetime);

DateTime datetime_now();
