BUILD_DIR = .
EXECUTABLE_FILE = $(NAME).out

CC = g++
CFLAGS = -std=c++11
LINK_FLAGS = -I $(INCLUDE_DIR) -I $(INLINE_DIR) -pthread

.PHONY: all make_build_dir build_objects clean

all: make_build_dir build_objects

OBJECTS = \
	$(BUILD_DIR)/$(NAME).o \

make_build_dir:
	mkdir -p $(BUILD_DIR)

build_objects: $(OBJECTS)
	$(CC) $(CFLAGS) $(LINK_FLAGS) $(OBJECTS) -o $(EXECUTABLE_FILE)

clean:
	rm -rf $(BUILD_DIR) *.o *.out

$(BUILD_DIR)/$(NAME).o: $(NAME).c
	$(CC) $(CFLAGS) $(LINK_FLAGS) -c $(NAME).c -o $(BUILD_DIR)/$(NAME).o