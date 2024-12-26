CC := clang
MKDIR_P ?= mkdir -p

SRV_TARGET  ?= server.o
TEST_TARGET ?= test.o
BUILD_DIR   ?= ./build
SRC_DIRS    ?= ./src/test ./src/main
CSRC        := $(shell find $(SRC_DIRS) -name *.c)
CPPSRC      := $(shell find $(SRC_DIRS) -name *.cpp)
ASMSRC      := $(shell find $(SRC_DIRS) -name *.s)
SRCS        := $(CSRC) $(CPPSRC) $(ASMSRC)
OBJS        := $(CSRC:./src/%.c=$(BUILD_DIR)/%.o)   \
							 $(CPPSRC:./src/%.cpp=$(BUILD_DIR)/%.o) \
							 $(ASMSRC:./src/%.s=$(BUILD_DIR)/%.o)
INC_DIRS    := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS   := $(addprefix -I,$(INC_DIRS))
CPPFLAGS    := $(CPPFLAGS) $(INC_FLAGS) -MMD -MP

DEPS := $(OBJS:.o=.d)

$(BUILD_DIR)/$(TEST_TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/$(SRV_TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# assembly
$(BUILD_DIR)/%.o: ./src/%.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.o: ./src/%.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.o: ./src/%.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)
