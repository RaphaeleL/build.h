V =

ifeq ($(strip $(V)),)
	E = @echo
	Q = @
else
	E = @\#
	Q =
endif
export E Q

SRC := $(shell find . -name "*.c")

CC      = cc
WARNINGS= -Wall -Wextra
CFLAGS  = -g -O3 $(WARNINGS) -std=c99 #-I/opt/homebrew/opt/llvm/include
#LDFLAGS = -L/opt/homebrew/opt/llvm/lib
JUNK    = *.DS_Store *~ core a.out *.dSYM

# Create executables list: convert ./dir/file.c -> dir/file
EXECS := $(patsubst ./%.c,%,$(SRC))

# Default target
all: $(EXECS)

# Compile each C file into its own executable in the same directory
%: %.c
	$(E) "  CC      " $<
	$(Q) $(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

clean:
	$(Q) for f in $(EXECS) $(JUNK); do echo "  CLEAN   " $$f; done
	$(Q) rm -rf $(EXECS) $(JUNK)

mrproper:
	$(E) "  PRUNE      "
	$(Q) git clean -fdx
