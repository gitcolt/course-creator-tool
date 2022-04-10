TARGET_BIN = main
INCLUDES = -I./glad/include -I./imgui/include
LDFLAGS = -lglfw -lGL -lm
SRC_DIRS = src glad/src imgui/src
SRCS = $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c)
BUILD_DIR = build
OBJS = $(SRCS:%=$(BUILD_DIR)/%.o)
CPPFLAGS = $(INCLUDES) -MMD -MP -g -Wall
DEPS = $(OBJS:.o=.d)

$(BUILD_DIR)/$(TARGET_BIN): $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: all clean
clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
