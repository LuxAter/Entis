
SHELL=/bin/bash
ifndef .VERBOSE
    .SILENT:
endif
ROOT=$(shell pwd)
ROOT=/home/arden/Programming/c/entis
CC=clang
CCIGNORE=
CCFLAGS=-fPIC -Wall -Wpedantic --static
LINK=-lxcb -lm -lz
SOURCE=src
INCLUDE_DIR=include
INCLUDE=-I$(ROOT)/build/libpng.a/include -I$(ROOT)/build/libfreetype.a/include/freetype2
BUILD=build
EXTERNAL=external
COMMON_INCLUDE=-I$(ROOT)/$(INCLUDE_DIR) $(INCLUDE)

SCAN_COLOR=\033[1;35m
BUILD_COLOR=\033[32m
CLEAN_COLOR=\033[1;33m
LINK_COLOR=\033[1;32m
INSTALL_COLOR=\033[1;36m
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
str=$$(realpath --relative-to="$(ROOT)" "$(1)");    printf "%b%s%b\n" "$(BUILD_COLOR)" "Building C object $$str" "\033[0m"
endef
define print_build_cpp
str=$$(realpath --relative-to="$(ROOT)" "$(1)");    printf "%b%s%b\n" "$(BUILD_COLOR)" "Building C++ object $$str" "\033[0m"
endef
define print_link_lib
str=$$(realpath --relative-to="$(ROOT)" "$(1)");    printf "%b%s%b\n" "$(LINK_COLOR)" "Linking static library $$str" "\033[0m"
endef
define print_link_exe
str=$$(realpath --relative-to="$(ROOT)" "$(1)");    printf "%b%s%b\n" "$(LINK_COLOR)" "Linking executable $$str" "\033[0m"
endef
define print_run_cmd
printf "%b%s%b\n" "$(CMD_COLOR)" "Running '$(1)'" "\033[0m"
endef
define help
printf "%b%*s%b: %s\n" "$(HELP_COLOR)" 20 "$(1)" "\033[0m" "$(2)"
endef

all: build-entis

clean: clean-entis clean-libentis.a

# ENTIS {{{

ENTIS=/home/arden/Programming/c/entis/entis
ENTIS_FILES=src/main.c
ENTIS_OBJS=$(ENTIS_FILES:%=$(ROOT)/$(BUILD)/%.o)
-include $(ENTIS_OBJS:.o=.d)

build-entis: build-libentis_loop.a build-libentis.a pre-entis $(ENTIS)
	$(call complete_target,$(shell basename $(ENTIS)))

clean-entis:
	$(call clean_target,$(shell basename $(ENTIS)))
	if [ -e "$(ENTIS)" ]; then rm $(ENTIS); fi

pre-entis:
	$(call scan_target,$(shell basename $(ENTIS)))

$(ENTIS): $(ENTIS_OBJS) FORCE
	$(call print_link_exe,$(shell basename $(ENTIS)))
ifdef $(PRODUCTION)
	$(CC) $(ENTIS_OBJS) $(LIBENTIS_LOOP.A) $(LINK) $(COMMON_INCLUDE) -o $(ENTIS)
else
	$(CC) $(ENTIS_OBJS) $(LIBENTIS_LOOP.A) $(ROOT)/build/libfreetype.a/lib/libfreetype.a  $(ROOT)/build/libpng.a/lib/libpng.a $(LINK) $(COMMON_INCLUDE) -o $(ENTIS)
endif

install-entis: build-entis
	$(call install_target,$(shell basename $(ENTIS)))
	mkdir -p $(INSTALL_PATH)/bin/
	cp $(ENTIS) $(INSTALL_PATH)/bin

uninstall-entis:
	$(call uninstall_target,$(shell basename $(ENTIS)))
	if [ -e "$(INSTALL_PATH)/bin/$(shell basename $(ENTIS))" ]; then rm $(INSTALL_PATH)/bin/$(shell basename $(ENTIS)); fi

# }}}
# LIBENTIS_LOOP.A {{{

LIBENTIS_LOOP.A=build/libentis_loop.a
LIBENTIS_LOOP.A_FILES=$(shell find "src/" -name "*.c")
LIBENTIS_LOOP.A_OBJS=$(LIBENTIS_LOOP.A_FILES:%=$(ROOT)/$(BUILD)/%.o)
-include $(LIBENTIS_LOOP.A_OBJS:.o=.d)

build-libentis_loop.a: build-libpng.a build-libfreetype.a pre-libentis_loop.a $(LIBENTIS_LOOP.A)
	$(call complete_target,$(shell basename $(LIBENTIS_LOOP.A)))

clean-libentis_loop.a: clean-libpng.a clean-libfreetype.a
	$(call clean_target,$(shell basename $(LIBENTIS_LOOP.A)))
	if [ -e "$(LIBENTIS_LOOP.A)" ]; then rm $(LIBENTIS_LOOP.A); fi

pre-libentis_loop.a:
	$(call scan_target,$(shell basename $(LIBENTIS_LOOP.A)))

$(LIBENTIS_LOOP.A): $(LIBENTIS_LOOP.A_OBJS) FORCE
	$(call print_link_lib,$(shell basename $(LIBENTIS_LOOP.A)))
	ar rcs $@ $(LIBENTIS_LOOP.A_OBJS)
ifdef $(PRODUCTION)
	printf "C\n"
	mkdir -p $(ROOT)/tmp/libpng.a && cd $(ROOT)/tmp/libpng.a && ar x /home/arden/Programming/c/entis/build/libpng.a/lib/libpng.a && ar qc $(ROOT)/$@ $(ROOT)/tmp/libpng.a/*.o && rm -rf $(ROOT)/tmp/libpng.a
	mkdir -p $(ROOT)/tmp/libfreetype.a && cd $(ROOT)/tmp/libfreetype.a && ar x /home/arden/Programming/c/entis/build/libfreetype.a/lib/libfreetype.a && ar qc $(ROOT)/$@ $(ROOT)/tmp/libfreetype.a/*.o && rm -rf $(ROOT)/tmp/libfreetype.a
	rm $(ROOT)/tmp -rf
endif

install-libentis_loop.a: build-libentis_loop.a
	$(call install_target,$(shell basename $(LIBENTIS_LOOP.A)))
	mkdir -p $(INSTALL_PATH)/lib/
	mkdir -p $(INSTALL_PATH)/include/$(NAME)/
	cp $(LIBENTIS_LOOP.A) $(INSTALL_PATH)/lib
	if [ ! -z "$(INCLUDE_DIR)" ]; then cp -R $(INCLUDE_DIR)/ $(INSTALL_PATH)/include/$(NAME)/; fi
	if [ ! -z "$(shell find $(SOURCE_DIR) -name "*.h")" ]; then cd $(SOURCE_DIR) && cp --parents $(sehll cd $(SOURCE_DIR) && find . -name "*.h") $(INSTALL_PATH)/include/$(NAME); fi
	if [ ! -z "$(shell find $(SOURCE_DIR) -name "*.hpp")" ]; then cd $(SOURCE_DIR) && cp --parents $(sehll cd $(SOURCE_DIR) && find . -name "*.hpp") $(INSTALL_PATH)/include/$(NAME); fi

uninstall-libentis_loop.a:
	$(call uninstall_target,$(shell basename $(LIBENTIS_LOOP.A)))
	if [ ! -e "$(INSTALL_PATH)/lib/$(shell basename $(LIBENTIS_LOOP.A))" ]; then rm $(INSTALL_PATH)/lib/$(shell basename $(LIBENTIS_LOOP.A)); fi
	if [ ! -e "$(INSTALL_PATH)/include/$(NAME)" ]; then rm $(INSTALL_PATH)/include/$(NAME) -r; fi

# }}}
# LIBENTIS.A {{{

LIBENTIS.A=build/libentis.a
LIBENTIS.A_FILES=$(filter-out src/main.c $(wildcard src/loop/*.c), $(shell find "src/" -name "*.c"))
LIBENTIS.A_OBJS=$(LIBENTIS.A_FILES:%=$(ROOT)/$(BUILD)/%.o)
-include $(LIBENTIS.A_OBJS:.o=.d)

build-libentis.a: build-libpng.a build-libfreetype.a pre-libentis.a $(LIBENTIS.A)
	$(call complete_target,$(shell basename $(LIBENTIS.A)))

clean-libentis.a: clean-libpng.a clean-libfreetype.a
	$(call clean_target,$(shell basename $(LIBENTIS.A)))
	if [ -e "$(LIBENTIS.A)" ]; then rm $(LIBENTIS.A); fi

pre-libentis.a:
	$(call scan_target,$(shell basename $(LIBENTIS.A)))

$(LIBENTIS.A): $(LIBENTIS.A_OBJS) FORCE
	$(call print_link_lib,$(shell basename $(LIBENTIS.A)))
	ar rcs $@ $(LIBENTIS.A_OBJS)
ifdef $(PRODUCTION)
	printf "C\n"
	mkdir -p $(ROOT)/tmp/libpng.a && cd $(ROOT)/tmp/libpng.a && ar x /home/arden/Programming/c/entis/build/libpng.a/lib/libpng.a && ar qc $(ROOT)/$@ $(ROOT)/tmp/libpng.a/*.o && rm -rf $(ROOT)/tmp/libpng.a
	mkdir -p $(ROOT)/tmp/libfreetype.a && cd $(ROOT)/tmp/libfreetype.a && ar x /home/arden/Programming/c/entis/build/libfreetype.a/lib/libfreetype.a && ar qc $(ROOT)/$@ $(ROOT)/tmp/libfreetype.a/*.o && rm -rf $(ROOT)/tmp/libfreetype.a
	rm $(ROOT)/tmp -rf
endif

install-libentis.a: build-libentis.a
	$(call install_target,$(shell basename $(LIBENTIS.A)))
	mkdir -p $(INSTALL_PATH)/lib/
	mkdir -p $(INSTALL_PATH)/include/$(NAME)/
	cp $(LIBENTIS.A) $(INSTALL_PATH)/lib
	if [ ! -z "$(INCLUDE_DIR)" ]; then cp -R $(INCLUDE_DIR)/ $(INSTALL_PATH)/include/$(NAME)/; fi
	if [ ! -z "$(shell find $(SOURCE_DIR) -name "*.h")" ]; then cd $(SOURCE_DIR) && cp --parents $(sehll cd $(SOURCE_DIR) && find . -name "*.h") $(INSTALL_PATH)/include/$(NAME); fi
	if [ ! -z "$(shell find $(SOURCE_DIR) -name "*.hpp")" ]; then cd $(SOURCE_DIR) && cp --parents $(sehll cd $(SOURCE_DIR) && find . -name "*.hpp") $(INSTALL_PATH)/include/$(NAME); fi

uninstall-libentis.a:
	$(call uninstall_target,$(shell basename $(LIBENTIS.A)))
	if [ ! -e "$(INSTALL_PATH)/lib/$(shell basename $(LIBENTIS.A))" ]; then rm $(INSTALL_PATH)/lib/$(shell basename $(LIBENTIS.A)); fi
	if [ ! -e "$(INSTALL_PATH)/include/$(NAME)" ]; then rm $(INSTALL_PATH)/include/$(NAME) -r; fi

# }}}
# LIBPNG.A {{{

build-libpng.a: pre-libpng.a
	if [ ! -f "external/libpng/configure" ]; then $(call print_run_cmd,autogen.sh) && cd external/libpng && ./autogen.sh; fi
	if [ ! -f "external/libpng/Makefile" ]; then $(call print_run_cmd,configure) && cd external/libpng && ./configure --prefix=/home/arden/Programming/c/entis/build/libpng.a; fi
	if [ ! -d "/home/arden/Programming/c/entis/build/libpng.a" ]; then $(call print_run_cmd,make) && cd external/libpng && make install; fi
	$(call complete_target,libpng.a)

clean-libpng.a:
	$(call clean_target,libpng.a)
	if [ -e "external/libpng/Makefile" ]; then cd external/libpng && make clean && rm Makefile; fi
	if [ -d "/home/arden/Programming/c/entis/build/libpng.a" ]; then rm /home/arden/Programming/c/entis/build/libpng.a -r; fi

pre-libpng.a:
	$(call scan_target,libpng.a)

# }}}
# LIBFREETYPE.A {{{

build-libfreetype.a: pre-libfreetype.a
	# $(call print_run_cmd,autogen.sh) && cd external/freetype2 && ./autogen.sh
	# $(call print_run_cmd,configure) && cd external/freetype2 && ./configure --prefix=/home/arden/Programming/c/entis/build/libfreetype.a --without-harfbuzz --without-bzip2 --without-zlib --without-png
	if [ ! -d "/home/arden/Programming/c/entis/build/libfreetype.a" ]; then $(call print_run_cmd,make) && cd external/freetype2 && make install; fi
	$(call complete_target,libfreetype.a)

clean-libfreetype.a:
	$(call clean_target,libfreetype.a)
	if [ -d "/home/arden/Programming/c/entis/build/libfreetype.a" ]; then rm /home/arden/Programming/c/entis/build/libfreetype.a -r; fi

pre-libfreetype.a:
	$(call scan_target,libfreetype.a)

# }}}

$(ROOT)/$(BUILD)/%.cpp.o: %.cpp
	$(call print_build_cpp,$@)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -MMD -c $(COMMON_INCLUDE) $< -o $@

$(ROOT)/$(BUILD)/%.c.o: %.c
	$(call print_build_c,$@)
	mkdir -p $(@D)
	$(CC) $(CCFLAGS) -MMD -c $(COMMON_INCLUDE) $< -o $@

FORCE:
