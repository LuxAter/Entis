SHELL = /bin/bash
ifndef .VERBOSE
  .SILENT:
endif

NAME=entis

CC=gcc
CXX=g++
CFLAGS=-Wall
CXXFLAGS=-Wall
LINK=-lxcb -lm
INCLUDE=

SOURCE_DIR=src
INCLUDE_DIR=
BUILD_DIR=build
DOC_DIR=docs
TEST_DIR=test
EXAMPLE_DIR=example
INSTALL_PATH?=/usr/local
ROOT=$(shell pwd)

SCAN_COLOR=\033[1;35m
BUILD_COLOR=\033[32m
CLEAN_COLOR=\033[1;33m
LINK_COLOR=\033[1;32m
INSTALL_COLOR=\033[01;36m
CMD_COLOR=\033[1;34m
HELP_COLOR=\033[1;34m

define scan_target
printf "%b%s%b\n" "$(SCAN_COLOR)" "Scaning dependencies for target $(1)" "\033[0m"
endef
define complete_target
printf "%s\n" "Built target $(1)"
endef
define clean_target
printf "%b%s%b\n" "$(CLEAN_COLOR)" "Cleaning target $(1)" "\033[0m"
endef
define install_target
printf "%b%s%b\n" "$(INSTALL_COLOR)" "Installing target $(1)" "\033[0m"
endef
define uninstall_target
printf "%b%s%b\n" "$(INSTALL_COLOR)" "Unnstalling target $(1)" "\033[0m"
endef
define print_build_c
str=$$(realpath --relative-to="$(ROOT)" "$(1)");\
    printf "%b%s%b\n" "$(BUILD_COLOR)" "Building C object $$str" "\033[0m"
endef
define print_build_cpp
str=$$(realpath --relative-to="$(ROOT)" "$(1)");\
    printf "%b%s%b\n" "$(BUILD_COLOR)" "Building Cpp object $$str" "\033[0m"
endef
define print_link_lib
str=$$(realpath --relative-to="$(ROOT)" "$(1)");\
    printf "%b%s%b\n" "$(LINK_COLOR)" "Linking static library $$str" "\033[0m"
endef
define print_link_exe
str=$$(realpath --relative-to="$(ROOT)" "$(1)");\
    printf "%b%s%b\n" "$(LINK_COLOR)" "Linking executable $$str" "\033[0m"
endef
define print_run_cmd
printf "%b%s%b\n" "$(CMD_COLOR)" "Running '$(1)'" "\033[0m"
endef
define help
printf "%b%*s%b: %s\n" "$(HELP_COLOR)" 20 "$(1)" "\033[0m" "$(2)"
endef

LIB=$(ROOT)/$(BUILD_DIR)/lib$(NAME).a
EXE=$(ROOT)/$(NAME)

LIB_FILES = $(filter-out $(SOURCE_DIR)/main.cpp, $(filter-out $(SOURCE_DIR)/main.c, $(shell find "$(SOURCE_DIR)" -name "*.c")))
LIB_OBJS = $(LIB_FILES:%=$(ROOT)/$(BUILD_DIR)/%.o)
	EXE_FILES = $(shell find "$(SOURCE_DIR)" -name "*.c") $(shell find "$(SOURCE_DIR)" -name "*.cpp")
	EXE_OBJS = $(EXE_FILES:%=$(ROOT)/$(BUILD_DIR)/%.o)

all: source

clean: clean-source clean-docs

install: install-source

uninstall: uninstall-source

source: build-exe

doc:
	$(call print_run_cmd,doxygen)
	doxygen

clean-docs:
	$(call clean_target,docs)
	if [ -d "$(DOC_DIR)/html" ]; then rm "$(DOC_DIR)/html" -r ;fi
	if [ -d "$(DOC_DIR)/latex" ]; then rm "$(DOC_DIR)/latex" -r ;fi
	if [ -d "$(DOC_DIR)/xml" ]; then rm "$(DOC_DIR)/xml" -r ;fi

clean-source: clean-lib clean-exe
	if [ -e "$(ROOT)/$(BUILD_DIR)/$(SOURCE_DIR)" ]; then rm $(ROOT)/$(BUILD_DIR)/$(SOURCE_DIR) -r; fi

install-source: install-lib

uninstall-source: uninstall-lib

build-lib: pre-lib $(LIB)
	$(call complete_target,$(shell basename $(LIB)))

clean-lib:
	$(call clean_target,$(shell basename $(LIB)))
	if [ -e "$(LIB)" ]; then rm $(LIB); fi

install-lib: build-lib
	$(call install_target,$(shell basename $(LIB)))
	mkdir -p $(INSTALL_PATH)/lib/
	mkdir -p $(INSTALL_PATH)/include/$(NAME)/
	cp $(LIB) $(INSTALL_PATH)/lib
	if [ ! -z "$(INCLUDE_DIR)" ]; then cp -R $(INCLUDE_DIR)/ $(INSTALL_PATH)/include/$(NAME)/; fi
	if [ ! -z "$(shell find $(SOURCE_DIR) -name "*.h")" ]; then cd $(SOURCE_DIR) && cp --parents $(shell cd $(SOURCE_DIR) && find . -name "*.h") $(INSTALL_PATH)/include/$(NAME); fi
	if [ ! -z "$(shell find $(SOURCE_DIR) -name "*.hpp")" ]; then cd $(SOURCE_DIR) && cp --parents $(shell cd $(SOURCE_DIR) && find . -name "*.hpp") $(INSTALL_PATH)/include/$(NAME); fi

uninstall-lib:
	$(call uninstall_target,$(shell basename $(LIB)))
	if [ ! -e "$(INSTALL_PATH)/lib/$(shell basename $(LIB))" ]; then rm "$(INSTALL_PATH)/lib/$(shell basename $(LIB))"; fi
	if [ ! -e "$(INSTALL_PATH)/include/$(NAME)" ]; then rm "$(INSTALL_PATH)/include/$(NAME)" -r; fi

$(LIB): $(LIB_OBJS)
	$(call print_link_lib,$(shell basename $(LIB)))
	ar rcs $@ $(LIB_OBJS)

pre-lib:
	$(call scan_target,$(shell basename $(LIB)))

build-exe: build-lib pre-exe $(EXE)
	$(call complete_target,$(shell basename $(EXE)))

clean-exe:
	$(call clean_target,$(shell basename $(EXE)))
	if [ -e "$(EXE)" ]; then rm $(EXE); fi

install-exe:
	$(call install_target,$(shell basename $(EXE)))
	mkdir -p $(INSTALL_PATH)/bin/
	cp $(EXE) $(INSTALL_PATH)/bin

uninstall-exe:
	$(call uninstall_target,$(shell basename $(EXE)))
	if [ -e "$(INSTALL_PATH)/bin/$(shell basename $(EXE))" ]; then rm $(INSTALL_PATH)/bin/$(shell basename $(EXE)); fi

$(EXE): $(EXE_OBJS)
	$(call print_link_exe,$(shell basename $(EXE)))
	$(CC) $(CFLAGS) $(INCLUDE) $(EXE_OBJS) $(LIB) $(LINK) -o $@

pre-exe:
	$(call scan_target,$(shell basename $(EXE)))

$(ROOT)/$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(@D)
	$(call print_build_c,$@)
	$(CC) $(CFLAGS) -MMD -c $(INCLUDE) $^ -o $@

$(ROOT)/$(BUILD_DIR)/./src/%.cpp.o: %.cpp
	mkdir -p $(@D)
	$(call print_build_cpp,$@)
	$(CXX) $(CXXFLAGS) -MMD -c $(INCLUDE) $^ -o $@
