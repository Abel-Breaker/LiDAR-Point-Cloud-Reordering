TIDY_SRCS = $(shell find ./src -name "*.c")

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
    bugprone-reserved-identifier,\
    bugprone-signal-handler,\
    bugprone-signed-char-misuse,\
    bugprone-sizeof-expression,\
    bugprone-branch-clone,\
    -clang-analyzer-security.insecureAPI.*,\
    -misc-no-recursion,\
	-misc-include-cleaner

tidy:
	$(Q)clang-tidy $(TIDY_SRCS) \
		--quiet \
		-checks="$(TIDY_CHECKS)" \
		-- -std=c2x