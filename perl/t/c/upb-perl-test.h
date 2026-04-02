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

#define plan(n) fprintf(stderr, "%*s1..%d\n", indent_level * 4, "", (n))

extern const char* todo_reason;
extern int indent_level;

#define todo_start(reason) todo_reason = (reason)
#define todo_end() todo_reason = NULL

#define TODO(reason) for(int _todo_i = (todo_start(reason), 0); _todo_i < 1; _todo_i++, todo_end())

#define ok(val, name) fprintf(stderr, "%*s%s %d - %s%s%s\n", indent_level * 4, "", (val) ? "ok" : "not ok", ++test_num, (name), (todo_reason ? " # TODO " : ""), (todo_reason ? todo_reason : ""))
#define fail(name) ok(0, name)
#define is(got, expected, name) fprintf(stderr, "%*s%s %d - %s\n", indent_level * 4, "", ((got) == (expected)) ? "ok" : "not ok", ++test_num, (name)); if ((got) != (expected)) { fprintf(stderr, "%*s  # Got: %ld\n%*s  # Expected: %ld\n", indent_level * 4, "", (long)(got), indent_level * 4, "", (long)(expected)); }
#define is_u(got, expected, name) fprintf(stderr, "%*s%s %d - %s\n", indent_level * 4, "", ((got) == (expected)) ? "ok" : "not ok", ++test_num, (name)); if ((got) != (expected)) { fprintf(stderr, "%*s  # Got: %" PRIu64 "\n%*s  # Expected: %" PRIu64 "\n", indent_level * 4, "", (uint64_t)(got), indent_level * 4, "", (uint64_t)(expected)); }
#define is_string(got, expected, name) fprintf(stderr, "%*s%s %d - %s\n", indent_level * 4, "", (strcmp((got), (expected)) == 0) ? "ok" : "not ok", ++test_num, (name)); if (strcmp((got), (expected)) != 0) { fprintf(stderr, "%*s  # Got: %s\n%*s  # Expected: %s\n", indent_level * 4, "", (got), indent_level * 4, "", (expected)); }
#define is_blob(got, expected, len, name) fprintf(stderr, "%*s%s %d - %s\n", indent_level * 4, "", (memcmp((got), (expected), (len)) == 0) ? "ok" : "not ok", ++test_num, (name)); if (memcmp((got), (expected), (len)) != 0) { fprintf(stderr, "%*s  # Blobs differ\n", indent_level * 4, ""); }
#define is_string_view(got, expected, len, name) fprintf(stderr, "%*s%s %d - %s\n", indent_level * 4, "", (strncmp((got).data, (expected), (len)) == 0 && (got).size == (len)) ? "ok" : "not ok", ++test_num, (name)); if (strncmp((got).data, (expected), (len)) != 0 || (got).size != (len)) { fprintf(stderr, "%*s  # Got: %.*s (len %zu)\n%*s  # Expected: %s (len %zu)\n", indent_level * 4, "", (int)(got).size, (got).data, (got).size, indent_level * 4, "", (expected), (len)); }

#define subtest(name, block) \
    STMT_START { \
        fprintf(stderr, "%*s# Subtest: %s\n", indent_level * 4, "", name); \
        int _parent_test_num = test_num; \
        test_num = 0; \
        indent_level++; \
        block; \
        indent_level--; \
        test_num = _parent_test_num; \
        ok(1, name); \
    } STMT_END

#define LEAK_CHECK(block) \
    STMT_START { \
        /* TODO: Implement upb_Arena based leak checking */ \
        block; \
    } STMT_END

#define STRESS_THREADS(n, func, arg) \
    STMT_START { \
        /* TODO: Implement actual pthread-based thread stress runner */ \
        cdiag("STRESS_THREADS: Running %s with %d threads (STUB)", #func, (n)); \
        func(arg); \
    } STMT_END

#define like(str, pattern, name) \
    STMT_START { \
        bool _like_pass = false; \
        if (!str) { \
            fprintf(stderr, "%*s# String to match is NULL for [%s]\n", indent_level * 4, "", name); \
        } else if (!pattern) { \
            fprintf(stderr, "%*s# Pattern is NULL for [%s]\n", indent_level * 4, "", name); \
        } else { \
            int errornumber; \
            PCRE2_SIZE erroroffset; \
            pcre2_code *re = pcre2_compile( \
                (PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, 0, \
                &errornumber, &erroroffset, NULL); \
            if (re == NULL) { \
                PCRE2_UCHAR buffer[256]; \
                pcre2_get_error_message(errornumber, buffer, sizeof(buffer)); \
                fprintf(stderr, "%*s# Regex compilation failed for [%s] at offset %d: %s\n", indent_level * 4, "", name, (int)erroroffset, buffer); \
            } else { \
                pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL); \
                int rc = pcre2_match(re, (PCRE2_SPTR)str, strlen(str), 0, 0, match_data, NULL); \
                if (rc >= 0) { \
                    _like_pass = true; \
                } else { \
                    fprintf(stderr, "%*s  # String [%s] does not match pattern [%s] for [%s]\n", indent_level * 4, "", str, pattern, name); \
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
            fprintf(stderr, "%*s# String to match is NULL for [%s]\n", indent_level * 4, "", name); \
        } else if (!pattern) { \
            fprintf(stderr, "%*s# Pattern is NULL for [%s]\n", indent_level * 4, "", name); \
        } else { \
            int errornumber; \
            PCRE2_SIZE erroroffset; \
            pcre2_code *re = pcre2_compile( \
                (PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, 0, \
                &errornumber, &erroroffset, NULL); \
            if (re == NULL) { \
                PCRE2_UCHAR buffer[256]; \
                pcre2_get_error_message(errornumber, buffer, sizeof(buffer)); \
                fprintf(stderr, "%*s# Regex compilation failed for [%s] at offset %d: %s\n", indent_level * 4, "", name, (int)erroroffset, buffer); \
            } else { \
                pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL); \
                int rc = pcre2_match(re, (PCRE2_SPTR)str, str_len, 0, 0, match_data, NULL); \
                if (rc >= 0) { \
                    _like_pass = true; \
                } else { \
                    fprintf(stderr, "%*s  # String [%.*s] does not match pattern [%s] for [%s]\n", indent_level * 4, "", (int)str_len, str, pattern, name); \
                } \
                pcre2_match_data_free(match_data); \
                pcre2_code_free(re); \
            } \
        } \
        ok(_like_pass, name); \
    } STMT_END

#define cdiag(fmt, ...) { fprintf(stderr, "%*s# " fmt, indent_level * 4, "", ##__VA_ARGS__); fprintf(stderr, "\n"); }

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

// Helper to initialize and destroy a Perl interpreter for testing
PerlInterpreter* test_perl_init(int argc, char** argv);
void test_perl_destroy(PerlInterpreter *my_perl);

#endif // PERLUPB_TEST_H

// TODO: Implement SKIP functionality for C tests
#define SKIP(reason, count) \
    for(int _skip_i = 0; _skip_i < (count); _skip_i++) \
        fprintf(stderr, "ok %d - # skip %s\n", ++test_num, (reason))

// TODO: Implement world-class build and test capabilities
// 1. Configurable sanitizer support (ASan, UBSan, MSan)
// 2. Automate C-level benchmark compilation and execution (make bench)
// 3. Linker-level symbol visibility control for ABI stability
// 4. Indented subtest support in the C harness (TAP 13 compliant)
// 5. LEAK_CHECK { ... } block for local allocation neutrality
// 6. STRESS_THREADS(n, func) macro for concurrency verification
