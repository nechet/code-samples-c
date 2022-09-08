#include <time.h>
#include <stdio.h>
#include <string.h>

#include "time-utils.h"

char week[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Dat"};
char year[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};


void timestamp(char* timestamp) {
    time_t ltime = time(NULL); /* get current cal time */
    struct tm tm = *localtime(&ltime);

    char* weekDay = week[tm.tm_wday];
    char* month = year[tm.tm_mon];

    sprintf (timestamp, "%s, %02d %s %d %02d:%02d:%02d %s",
        weekDay, tm.tm_mday, month, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, tm.tm_zone);
}

