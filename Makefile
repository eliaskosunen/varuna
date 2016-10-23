APPNAME = Lang

CXX := c++
CXXFLAGS := -O -g -std=c++11 -Wall -pedantic
LINKFLAGS := -O -g
INCLUDE_DIRS := -Isrc -isystem ext/include
LIB_DIRS := -Lext/lib

VERSION :=
CXX_VER := "-DVERSION=\"$(VERSION)\""

FILES_CPP := $(wildcard src/*.cpp) $(wildcard src/*/*.cpp) $(wildcard src/*/*/*.cpp) $(wildcard src/*/*/*/*.cpp)
FILES_HPP := $(wildcard src/*.h) $(wildcard src/*/*.h) $(wildcard src/*/*/*.h) $(wildcard src/*/*/*/*.h)
FILES_DEP := $(patsubst src/%,build/%.dep,$(FILES_CPP))

SRC := $(FILES_CPP)
BIN := bin/$(APPNAME)
LIBS := 

# Hack for OS differences.
# On Windows, echo '1' produces literally '1' instead of 1.
ifeq "$(shell echo '1')" "'1'"
  WIN32 := 1
  mkdir = $(shell mkdir $(subst /,\,$(1)/dummy.mkdir) && rmdir $(subst /,\,$(1)/dummy.mkdir))
  rm_rf = $(foreach F,$(subst /,\,$(1)),$(shell rmdir /Q /S $(F) 2>NUL >NUL || del /Q /S $(F) 2>NUL >NUL)) $(1)

  # Get the version number from git; we must first check manually that git.exe exists
  ifeq "$(findstring git,$(shell for %%i in (git.exe) do @echo.%%~$$PATH:i))" "git"
    VERSION := $(strip $(shell git describe 2>NUL))
  endif
else
  mkdir = $(shell mkdir -p $(1))
  rm_rf = $(shell rm -rf $(1)) $(1)

  # Get the version number from git
  VERSION := $(strip $(shell git describe 2>/dev/null))

  # Something needs pthread.
  LIBS += -lpthread

  # Mac OS X needs different libraries
  ifeq "$(shell uname -s)" "Darwin"
    LIBS := $(patsubst -l%,-framework %,$(LIBS))
  endif

  ifeq "$(findstring mingw,$(CXX))" "mingw"
    WIN32 := 1
  endif
endif

ifdef WIN32
  BIN := $(BIN).exe
  LIBS := $(LIBS) -lws2_32
  CXXFLAGS2 := -D_WIN32_WINNT=0x0501
endif

# Abstract build rules.
.PHONY: all $(APPNAME) dirs
$(APPNAME): $(BIN)
clean: 
	@echo [RM] $(call rm_rf,build bin/*)
clean_deps:
	@echo [RM] $(call rm_rf,$(FILES_DEP))

# Directories
dirs: | bin/ build/ $(patsubst src/%,build/%,$(sort $(dir $(SRC))))
%/:
	@echo [MKDIR] $@
	@$(call mkdir,$@)

# Build rules for binaries.
$(BIN): $(patsubst src/%,build/%.o,$(SRC))

$(BIN): | dirs
	@echo [LINK] $@
	@$(CXX) $(LINKFLAGS) $(LIB_DIRS) -o $@ $(filter %.o,$^) $(LIBS)

# Include dependencies; generation rules are below.
-include $(FILES_DEP)

# Dependency generation.
build/%.dep: src/% | dirs
	@echo [DEPEND] $<
	@$(CXX) $(CXXFLAGS) $(CXXFLAGS2) $(INCLUDE_DIRS) $(CXX_VER) -MM $< -MT $@ -MP > $@

# Compilation
build/%.o: src/% build/%.dep | dirs
	@echo [CXX] $<
	@$(CXX) $(CXXFLAGS) $(CXXFLAGS2) $(INCLUDE_DIRS) $(CXX_VER) $< -c -o $@