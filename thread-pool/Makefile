CC       = gcc
CFLAGS   = -I.
DEPS     = thread-pool.h
OBJ      = thread-pool.o
LIBS_SDR = -pthread
OBJ_DIR  = obj
ODIR     = .

FLAGS = -g -c
SOURCEDIR=src
TESTDIR=test
#src/code
BUILDDIR=obj
SOURCES=$(wildcard $(SOURCEDIR)/*.c)
OBJECTS=$(patsubst $(SOURCEDIR)/%.c,$(BUILDDIR)/%.o,$(SOURCES))

DIST=dist

TESTS=$(wildcard $(TESTDIR)/*.c)
TESTS_OBJECTS=$(patsubst $(TESTDIR)/%.c,$(BUILD_TEST_DIR)/%.o,$(TESTS))

BUILD_TEST_DIR=obj-test


dir:
	mkdir -p $(BUILDDIR)
	mkdir -p $(BUILD_TEST_DIR)
	mkdir -p $(DIST)

$(TESTS_OBJECTS): $(BUILD_TEST_DIR)/%.o : $(TESTDIR)/%.c
	$(CC) $(FLAGS) $< -o $@

$(OBJECTS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.c $(SOURCEDIR)/%.h
	$(CC) $(FLAGS) $< -o $@

clean:
	rm -rf $(BUILDDIR) $(DIST) $(BUILD_TEST_DIR)

$(DIST)/thread-pool-test: $(BUILDDIR)/thread-pool.o $(BUILD_TEST_DIR)/thread-pool-test.o
	$(CC) $^ -o $@ $(LIBS_SDR)

all: dir $(TESTS_OBJECTS) $(DIST)/thread-pool-test
