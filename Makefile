CC=clang
CFLAGS=-Wall -Werror -g -fPIC

BUILD_DIR=build
LIB=$(BUILD_DIR)/liballocators.so
LIB_OBJS=$(BUILD_DIR)/allocators.o
TEST_OBJS=$(BUILD_DIR)/testing.o
EXEC=$(BUILD_DIR)/testing

all: $(EXEC)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(LIB): $(LIB_OBJS) | $(BUILD_DIR)
	$(CC) -shared -o $(LIB) $(LIB_OBJS)

$(BUILD_DIR)/allocators.o: allocators.c allocators.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c allocators.c -o $(BUILD_DIR)/allocators.o

$(BUILD_DIR)/testing.o: testing.c allocators.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c testing.c -o $(BUILD_DIR)/testing.o

$(EXEC): $(LIB) $(TEST_OBJS)
	$(CC) $(CFLAGS) $(TEST_OBJS) -L$(BUILD_DIR) -lallocators -o $(EXEC)

run: $(EXEC)
	LD_LIBRARY_PATH=$(BUILD_DIR) ./$(EXEC)

clean:
	rm -rf $(BUILD_DIR)
