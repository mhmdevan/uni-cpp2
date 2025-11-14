CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -O2

INCLUDE_DIR = include
SRC_DIR = src
BUILD_DIR = build

SOURCES = \
    $(SRC_DIR)/main.cpp \
    $(SRC_DIR)/application.cpp \
    $(SRC_DIR)/contact.cpp \
    $(SRC_DIR)/phone_number.cpp \
    $(SRC_DIR)/validation.cpp \
    $(SRC_DIR)/file_contact_repository.cpp

OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

TARGET = $(BUILD_DIR)/phonebook

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: all clean
