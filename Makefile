CC = g++
CFLAGS = -I

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

SOURCE_DIR = ./DVZ_Make
OBJ_DIR = ./DVZ_Make/x64/Debug
OUT_DIR = ./x64/Debug

_OBJ = test.o
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))


all:
	make server
	

# $(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.c
# 	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJ)
	$(CC) -o $(OUT_DIR)/$@ $^
