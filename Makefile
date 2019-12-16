CC = g++
CFLAGS =



#BINARY_64 = ./x64
#BINARY_32 = ./x86

#BINARY = $(BINARY_64)

#DEBUG_MODE = Debug
#RELEASE_MODE = Release

#DVZ_SOURCE_DIR = ./DVZ
#DVZ_SERVER_SOURCE_DIR = ./DVZ\ Server

#DVZ_BIN_DIR = $(DVZ_SOURCE_DIR)/$(BINARY)/$(DEBUG_MODE)
#DVZ_SERVER_BIN_DIR = $(DVZ_SERVER_SOURCE_DIR)/$(BINARY)/$(DEBUG_MODE)

#DVZ_OUT = DVZ
#DVZ_SERVER_OUT = DVZ_Server
OUT = server

SOURCE_DIR = ./DVZ_Make
OBJ_DIR = ./DVZ_Make/x64/Debug
OUT_DIR = ./x64/Debug

DEPS = $(wildcard $(SOURCE_DIR)/*.h)
SRCS = $(wildcard $(SOURCE_DIR)/*.cpp)

#_OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))
OBJ = $(patsubst $(SOURCE_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

server: $(OUT_DIR)/server
	
$(OUT_DIR)/server: $(OBJ)
	$(CC) -o $@ $^

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

run:
	$(OUT_DIR)/$(OUT)
