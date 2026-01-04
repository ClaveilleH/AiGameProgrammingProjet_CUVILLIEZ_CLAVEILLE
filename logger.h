#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

#define COLOR_RED "\033[31m"
#define COLOR_PURPLE "\033[35m"
#define COLOR_GREEN "\033[32m"
#define RESET_COLOR "\033[0m"

// #ifdef DEBUG
// #define DEBUG_PRINT(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
// #else
// #define DEBUG_PRINT(fmt, ...) // No-op
// #endif

// #define COMPETE_PRINT(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)

#define INFO_PRINT(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
/*
#define EMIT_WARNING(fmt, ...) do { \
    fprintf(stderr, COLOR_PURPLE "[Warning] " RESET_COLOR fmt "\n", ##__VA_ARGS__); \
    fprintf(stderr, "          at line %d, near '%s'\n", yylineno, yytext); \
} while(0) // while(0) pour que les différents printf soient dans le même bloc

#define EMIT_ERROR(fmt, ...) do { \
    fprintf(stderr, COLOR_RED "[Error] " RESET_COLOR fmt "\n", ##__VA_ARGS__); \
    fprintf(stderr, "        at line %d, near '%s'\n", yylineno, yytext); \
    exit(1); \
} while(0)
*/

#define WARN_PRINT(fmt, ...) fprintf(stderr, COLOR_PURPLE "[Warning] " RESET_COLOR fmt "\n", ##__VA_ARGS__)

#define _log(fmt, ...) do { \
    if (LOGFILE) { \
        fprintf(LOGFILE, fmt "\n", ##__VA_ARGS__); \
        fflush(LOGFILE); \
    } \
} while(0)

extern const char* LOG_DIR;
extern FILE* LOGFILE;

void init_logger();
void close_logger();
void log_print(const char* message);



#endif // LOGGER_H