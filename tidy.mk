TIDY_SRCS = $(shell find ./src -name "*.c" -o -name "*.h")
TIDY_CHECKS = \
    performance-*,\
    misc-*,\
    android-cloexec-*,\
    readability-duplicate-include,\
    readability-misleading-indentation,\
    bugprone-assert-side-effect,\
    bugprone-macro-repeated-side-effects,\
    bugprone-infinite-loop,\
    bugprone-macro-parentheses,\
    bugprone-posix-return,\
    -bugprone-reserved-identifier,\
    bugprone-signal-handler,\
    bugprone-signed-char-misuse,\
    bugprone-sizeof-expression,\
    bugprone-branch-clone,\
    -clang-analyzer-security.insecureAPI.*,\
    -misc-no-recursion,\
    -misc-include-cleaner,\
    -android-cloexec-fopen

NPROC ?= $(shell nproc)

tidy:
	$(Q)find ./src \( -name "*.c" -o -name "*.h" \) -print0 | \
	    xargs -0 -P$(NPROC) -I{} clang-tidy {} \
	        -checks="$(TIDY_CHECKS)" \
	        -- -std=c2x