# A simple makefile for C and C++ mixed project (Version II)

# Name of the main executable file.
target = myshell

# Choose the compiler(e.g. gcc/clang)
compilerc = gcc
compilercpp = g++

compilecflags = -std=c99 -g -D_DEBUG
compilecppflags = -std=c++11 -g -D_DEBUG
compilelinkflags = -g

dirproj = $(realpath .)

dirbuild = build
dirbinary = $(dirbuild)/bin
dirobject = $(dirbuild)/obj

dirsrc = src
srcc = 
srccpp = 
header = 

objc = $(patsubst %.c,%.o,$(addprefix $(dirobject)/, $(patsubst $(dirsrc)/%,%,$(srcc))))
objcpp = $(patsubst %.cpp,%.o,$(addprefix $(dirobject)/, $(patsubst $(dirsrc)/%,%,$(srccpp))))

all: listsrc mkbuilddir $(target)

listsrc:
ifeq ($(OS), Windows_NT)
srcc = $(patsubst $(dirproj)/%,%,$(subst \,/,$(shell for /r $(dirsrc) %%i in (*.c) do @echo %%i)))
srccpp = $(patsubst $(dirproj)/%,%,$(subst \,/,$(shell for /r $(dirsrc) %%i in (*.cpp) do @echo %%i)))
header = $(patsubst $(dirproj)/%,%,$(subst \,/,$(shell for /r $(dirsrc) %%i in (*.h *.hpp) do @echo %%i)))
else
srcc = $(shell find $(dirsrc) -name "*.c")
srccpp = $(shell find $(dirsrc) -name "*.cpp")
header = $(shell find $(dirsrc) -name "*.h" -o -name "*.hpp")
endif

mkbuilddir:
ifeq ($(OS), Windows_NT)
	@echo off
	@if not exist $(subst /,\,$(dirbinary)) ( mkdir $(subst /,\,$(dirbinary)) )
	@if not exist $(subst /,\,$(dirobject)) ( mkdir $(subst /,\,$(dirobject)) )
else
	if [ ! -d $(dirbinary) ] ; then mkdir -p $(dirbinary); fi
	if [ ! -d $(dirobject) ] ; then mkdir -p $(dirobject); fi
endif

# Main Executable Binary: Link all obj files.
$(target): $(objc) $(objcpp)
	$(compilercpp) $(compilelinkflags) -o $(dirbinary)/$@ $^


# C Objects
$(objc): $(dirobject)/%.o: $(dirsrc)/%.c $(header)
ifeq ($(OS), Windows_NT)
	@if not exist $(subst /,\,$(dir $@)) ( mkdir $(subst /,\,$(dir $@)) )
else
	if [ ! -d $(dir $@) ] ; then mkdir -p $(dir $@); fi
endif
	$(compilerc) $(compilecflags) -c -o $@ $<

# C++ Objects
$(objcpp): $(dirobject)/%.o: $(dirsrc)/%.cpp $(header)
ifeq ($(OS), Windows_NT)
	@if not exist $(subst /,\,$(dir $@)) ( mkdir $(subst /,\,$(dir $@)) )
else
	if [ ! -d $(dir $@) ] ; then mkdir -p $(dir $@); fi
endif
	$(compilercpp) $(compilecppflags) -c -o $@ $<

# Run the main executable target
run:
ifeq ($(OS), Windows_NT)
	@$(dirbinary)/$(target)
else
	./$(dirbinary)/$(target)
endif


.PHONY: clean

clean:
ifeq ($(OS), Windows_NT)
#	@echo off
	@if exist $(dirbuild) ( rmdir /s/q $(dirbuild) )
else
	if [ -d $(dirbuild) ] ; then rm -r $(dirbuild); fi
endif