CC       = gcc
CFLAGS   = -I.
DEPS     = thread-pool.h time-utils.h http-api.h http-request.h http-response.h http-channel.h
OBJ      = thread-pool.o time-utils.o http-api.o http-request.o http-response.o http-channel.o
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


$(DIST)/http-api-test: $(OBJECTS) $(BUILD_TEST_DIR)/http-api-test.o
	$(CC) $^ -o $@ $(LIBS_SDR)

$(DIST)/time-utils-test: $(BUILDDIR)/time-utils.o $(BUILD_TEST_DIR)/time-utils-test.o
	$(CC) $^ -o $@ $(LIBS_SDR)

$(DIST)/thread-pool-test: $(BUILDDIR)/thread-pool.o $(BUILD_TEST_DIR)/thread-pool-test.o
	$(CC) $^ -o $@ $(LIBS_SDR)

$(DIST)/http-request-test: $(BUILDDIR)/http-request.o $(BUILD_TEST_DIR)/http-request-test.o
	$(CC) $^ -o $@ $(LIBS_SDR)


all: dir $(TESTS_OBJECTS) $(DIST)/time-utils-test $(DIST)/thread-pool-test $(DIST)/http-request-test $(DIST)/http-api-test



# %-test.o: %-test.c $(DEPS)
# 	mkdir -p ${OBJ_DIR}
# 	$(CC) -g -ggdb -c -o0 -o ${OBJ_DIR}/$@ $< $(CFLAGS)

# %.o: %.c %.h $(DEPS)
# 	mkdir -p ${OBJ_DIR}
# 	$(CC) -g -ggdb -c -o0 -o ${OBJ_DIR}/$@ $< $(CFLAGS)

# time-utils-test: time-utils.o time-utils-test.o
# 	$(CC) -g -ggdb -o0 -o $@ ${OBJ_DIR}/time-utils.o ${OBJ_DIR}/time-utils-test.o $(CFLAGS)

# thread-pool-test: thread-pool.o thread-pool-test.o
# 	$(CC) -g -o $@ ${OBJ_DIR}/thread-pool.o ${OBJ_DIR}/thread-pool-test.o $(CFLAGS) -pthread

# http-request-test: http-request.o http-request-test.o
# 	$(CC) -o $@ ${OBJ_DIR}/http-request.o ${OBJ_DIR}/http-request-test.o $(CFLAGS)

# http-api-test: $(OBJ) http-api-test.o
# 	$(CC) -g -ggdb -o0 -o $@ -I${OBJ_DIR}/*.o $(CFLAGS) $(LIBS_SDR)

# #	$(CC) -g -ggdb -o0 -o $@ $^ $(CFLAGS) $(LIBS_SDR)


# all: time-utils-test thread-pool-test http-request-test http-api-test

# clean:
# 	rm -f *-test
# 	rm -f $(OBJ_DIR)/*.o $(OBJ_DIR)/*.out
