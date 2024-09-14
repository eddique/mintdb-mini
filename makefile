CC = gcc
CFLAGS = -Wall -Iinclude -Os -ffunction-sections -fdata-sections
LDFLAGS = # -Wl,--gc-sections -static
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
UNIT_TEST_DIR = tests/unit
INTEGRATION_TEST_DIR = tests/integration

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))
EXECUTABLE = $(BIN_DIR)/mint

UNIT_TEST_SOURCES = $(wildcard $(UNIT_TEST_DIR)/*.c)
UNIT_TEST_OBJECTS = $(patsubst $(UNIT_TEST_DIR)/%.c, $(OBJ_DIR)/%.o, $(UNIT_TEST_SOURCES))
UNIT_TEST_EXECUTABLES = $(patsubst $(UNIT_TEST_DIR)/%.c, $(BIN_DIR)/%.unit_test, $(UNIT_TEST_SOURCES))

INTEGRATION_TEST_SOURCES = $(wildcard $(INTEGRATION_TEST_DIR)/*.c)
INTEGRATION_TEST_OBJECTS = $(patsubst $(INTEGRATION_TEST_DIR)/%.c, $(OBJ_DIR)/%.o, $(INTEGRATION_TEST_SOURCES))
INTEGRATION_TEST_EXECUTABLES = $(patsubst $(INTEGRATION_TEST_DIR)/%.c, $(BIN_DIR)/%.integration_test, $(INTEGRATION_TEST_SOURCES))

all: dirs $(EXECUTABLE)
	strip $(EXECUTABLE)

dirs:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(UNIT_TEST_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(INTEGRATION_TEST_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/%.unit_test: $(OBJ_DIR)/%.o $(filter-out $(OBJ_DIR)/main.o, $(OBJECTS))
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(BIN_DIR)/%.integration_test: $(OBJ_DIR)/%.o $(filter-out $(OBJ_DIR)/main.o, $(OBJECTS))
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

run_unit_tests: $(UNIT_TEST_EXECUTABLES)
	@for test in $(UNIT_TEST_EXECUTABLES); do ./$$test; done

run_integration_tests: $(INTEGRATION_TEST_EXECUTABLES)
	@for test in $(INTEGRATION_TEST_EXECUTABLES); do ./$$test; done

clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(BIN_DIR)/*

.PHONY: all clean run_unit_tests run_integration_tests dirs
