# DHBW CC directories
OBJDIR = bin
SRCDIR = src

# Executables
CC = gcc
BISON = bison
FLEX = flex
RM = rm
DOXYGEN = doxygen
SPLINT = splint
VALGRIND = valgrind
MKDIR = mkdir

# Compiler Flags
STANDARDS = -std=gnu99
INCLUDES = -I$(SRCDIR) -I$(OBJDIR) -I$(SRCDIR)/include
CFLAGS = -O0 -Wall -Wextra -pedantic -g $(INCLUDES) $(STANDARDS) -Wno-unused-parameter -Wno-unused-function

all: compiler 

###############################################
# DHBW Compiler                               #
###############################################
DHBWCC_C_SOURCES = main.c diag.c resource_manager.c symboltabelle.c
DHBWCC_FLEX_SOURCES = scanner.l
DHBWCC_BISON_SOURCES = parser.y

DHBWCC_C_OBJECTS = $(addprefix $(OBJDIR)/, $(DHBWCC_C_SOURCES:.c=.o) $(DHBWCC_FLEX_SOURCES:.l=.l.o) $(DHBWCC_BISON_SOURCES:.y=.y.o))

## Convenience targets

.PHONY: compiler
compiler: $(OBJDIR)/dhbwcc

## Build rules

$(OBJDIR)/dhbwcc: $(DHBWCC_C_OBJECTS)
	$(MKDIR) -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $+


$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(MKDIR) -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


$(OBJDIR)/%.l.o: $(OBJDIR)/%.l.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.y.o: $(OBJDIR)/%.y.c
	$(CC) $(CFLAGS) -c $< -o $@


$(OBJDIR)/%.l.c: $(SRCDIR)/%.l $(addprefix $(OBJDIR)/, $(addsuffix .h, $(DHBWCC_BISON_SOURCES)))
	$(MKDIR) -p ${dir $@}
	$(FLEX) -o $@ $<

$(OBJDIR)/%.y.c $(OBJDIR)/%.y.h: $(SRCDIR)/%.y
	$(MKDIR) -p ${dir $@}
	$(BISON) -v -t --locations --defines=$(basename $@).h -o $(basename $@).c -rall --report-file=$(basename $@).out $<

.PRECIOUS: %.y.h %.y.c %.l.c


.PHONY: clean_compiler
clean_compiler:
	$(RM) -rf bin/*

###############################################
# DHBW Compiler RT-Check                      #
###############################################
SPLINT_OPTIONS = -unrecog
VALGRIND_OPTIONS = --leak-check=full

.PHONY: check_compiler
check_compiler:
	$(VALGRIND) $(VALGRIND_OPTIONS) bin/dhbwcc -p a.c

.PHONY: check
check: check_compiler

###############################################
# Doxygen Project Documentation               #
###############################################
DOXYGEN_CONFIG = doxygen/config
DOXYGEN_FLAGS = 

.PHONY: doxygen clean_doxygen
doxygen:
	$(DOXYGEN) $(DOXYGEN_FLAGS) $(DOXYGEN_CONFIG)

clean_doxygen:
	$(RM) -rf doxygen/html

.PHONY: clean
clean: clean_compiler clean_doxygen
	
