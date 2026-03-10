#ifndef PERLUPB_TEST_H
#define PERLUPB_TEST_H

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include <stdio.h>
#include <string.h>
#include "upb/mem/arena.h"

#include <inttypes.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

// #include "ppport.h"

#define plan(n) fprintf(stderr, "1..%d\n", (n))
#define ok(val, name) fprintf(stderr, "%s %d - %s\n", (val) ? "ok" : "not ok", ++test_num, (name))
#define fail(name) ok(0, name)
#define is(got, expected, name) fprintf(stderr, "%s %d - %s\n", ((got) == (expected)) ? "ok" : "not ok", ++test_num, (name)); if ((got) != (expected)) { fprintf(stderr, "  # Got: %ld\n  # Expected: %ld\n", (long)(got), (long)(expected)); }
#define is_u(got, expected, name) fprintf(stderr, "%s %d - %s\n", ((got) == (expected)) ? "ok" : "not ok", ++test_num, (name)); if ((got) != (expected)) { fprintf(stderr, "  # Got: %" PRIu64 "\n  # Expected: %" PRIu64 "\n", (uint64_t)(got), (uint64_t)(expected)); }
#define is_string(got, expected, name) fprintf(stderr, "%s %d - %s\n", (strcmp((got), (expected)) == 0) ? "ok" : "not ok", ++test_num, (name)); if (strcmp((got), (expected)) != 0) { fprintf(stderr, "  # Got: %s\n  # Expected: %s\n", (got), (expected)); }
#define is_string_view(got, expected, len, name) fprintf(stderr, "%s %d - %s\n", (strncmp((got).data, (expected), (len)) == 0 && (got).size == (len)) ? "ok" : "not ok", ++test_num, (name)); if (strncmp((got).data, (expected), (len)) != 0 || (got).size != (len)) { fprintf(stderr, "  # Got: %.*s (len %zu)\n  # Expected: %s (len %zu)\n", (int)(got).size, (got).data, (got).size, (expected), (len)); }

#define like(str, pattern, name) \
    STMT_START { \
        bool _like_pass = false; \
        if (!str) { \
            fprintf(stderr, "# String to match is NULL for [%s]\n", name); \
        } else if (!pattern) { \
            fprintf(stderr, "# Pattern is NULL for [%s]\n", name); \
        } else { \
            int errornumber; \
            PCRE2_SIZE erroroffset; \
            pcre2_code *re = pcre2_compile( \
                (PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, 0, \
                &errornumber, &erroroffset, NULL); \
            if (re == NULL) { \
                PCRE2_UCHAR buffer[256]; \
                pcre2_get_error_message(errornumber, buffer, sizeof(buffer)); \
                fprintf(stderr, "# Regex compilation failed for [%s] at offset %d: %s\n", name, (int)erroroffset, buffer); \
            } else { \
                pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL); \
                int rc = pcre2_match(re, (PCRE2_SPTR)str, strlen(str), 0, 0, match_data, NULL); \
                if (rc >= 0) { \
                    _like_pass = true; \
                } else { \
                    fprintf(stderr, "  # String [%s] does not match pattern [%s] for [%s]\n", str, pattern, name); \
                } \
                pcre2_match_data_free(match_data); \
                pcre2_code_free(re); \
            } \
        } \
        ok(_like_pass, name); \
    } STMT_END

#define like_n(str, str_len, pattern, name) \
    STMT_START { \
        bool _like_pass = false; \
        if (!str) { \
            fprintf(stderr, "# String to match is NULL for [%s]\n", name); \
        } else if (!pattern) { \
            fprintf(stderr, "# Pattern is NULL for [%s]\n", name); \
        } else { \
            int errornumber; \
            PCRE2_SIZE erroroffset; \
            pcre2_code *re = pcre2_compile( \
                (PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, 0, \
                &errornumber, &erroroffset, NULL); \
            if (re == NULL) { \
                PCRE2_UCHAR buffer[256]; \
                pcre2_get_error_message(errornumber, buffer, sizeof(buffer)); \
                fprintf(stderr, "# Regex compilation failed for [%s] at offset %d: %s\n", name, (int)erroroffset, buffer); \
            } else { \
                pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL); \
                int rc = pcre2_match(re, (PCRE2_SPTR)str, str_len, 0, 0, match_data, NULL); \
                if (rc >= 0) { \
                    _like_pass = true; \
                } else { \
                    fprintf(stderr, "  # String [%.*s] does not match pattern [%s] for [%s]\n", (int)str_len, str, pattern, name); \
                } \
                pcre2_match_data_free(match_data); \
                pcre2_code_free(re); \
            } \
        } \
        ok(_like_pass, name); \
    } STMT_END

#define cdiag(fmt, ...) { fprintf(stderr, "# " fmt, ##__VA_ARGS__); fprintf(stderr, "\n"); }

// Helper for formatted test names
static char* sdiagnostic(const char *fmt, ...) {
    static char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    return buffer;
}

extern int test_num;

// Helper to get a new arena for testing
upb_Arena* test_arena_new(void);

#endif // PERLUPB_TEST_H
