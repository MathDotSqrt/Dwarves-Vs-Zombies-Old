CC = g++
CFLAGS = -std=c++17 -O3

OUT = server

BINARY = x64
MODE = Debug

SOURCE_DIR = ./DVZ_server
SOURCE_COMMON_DIR = ./DVZ_common

OBJ_DIR = $(SOURCE_DIR)/$(BINARY)/$(MODE)
OBJ_COMMON_DIR = $(SOURCE_COMMON_DIR)/$(BINARY)/$(MODE)
OUT_DIR = ./$(BINARY)/$(MODE)

DEPS = $(wildcard $(SOURCE_DIR)/*.h)
SRCS = $(wildcard $(SOURCE_DIR)/*.cpp)

DEPS_COMMON = $(wildcard $(SOURCE_COMMON_DIR)/*.h)
SRCS_COMMON = $(wildcard $(SOURCE_COMMON_DIR)/*.cpp)

OBJ = $(patsubst $(SOURCE_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS)) 
OBJ_COMMON = $(patsubst $(SOURCE_COMMON_DIR)/%.cpp, $(OBJ_COMMON_DIR)/%.o, $(SRCS_COMMON))

RAKNET_LIB = RakNet
RAKNET_LIB_DIR = ./RakNet/lib
RAKNET_LIB_PATH = $(RAKNET_LIB_DIR)/lib$(RAKNET_LIB).a
RAKNET_SRC_DIR = ./RakNet/include/src
RAKNET_INC_DIR = ./RakNet/include
RAKNET_OBJ_DIR = ./RakNet/bin

RAKNET_BIN = ./RakNet/bin
RAKNET_SRC = $(wildcard $(RAKNET_SRC_DIR)/*.cpp) #$(wildcard $(RAKNET_SRC_DIR)/crypto/*.cpp)
RAKNET_INCLUDES = $(wildcard $(RAKNET_INC_DIR)/*.h)
RAKNET_OBJ = $(patsubst $(RAKNET_SRC_DIR)/%.cpp, $(RAKNET_OBJ_DIR)/%.o, $(RAKNET_SRC))

ZLIB_DIR = ./zlib
ZLIB_INCLUDE = $(ZLIB_DIR)/include
ZLIB_LIB_DIR = $(ZLIB_DIR)/lib/

INCLUDES = -I $(SOURCE_COMMON_DIR) -I $(RAKNET_INC_DIR)/ -I ./ENTT/single_include/entt/ -I ./GLM/ -I $(ZLIB_INCLUDE)
LIBS = -L $(RAKNET_LIB_DIR) -L $(ZLIB_LIB_DIR) -lRakNet -lz -lpthread 

all: $(OUT)
	echo "Done."

#SERVER RUN
run: $(OUT)
	$(OUT_DIR)/$(OUT)

#SERVER BUILD
$(OUT): $(RAKNET_LIB) $(OUT_DIR)/$(OUT) 

$(OUT_DIR)/$(OUT): $(OBJ) $(OBJ_COMMON)
	$(CC) -o $@ $^ $(INCLUDES) $(LIBS)

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(DEPS) $(DEPS_COMMON)
	$(CC) $(CFLAGS) -c -o $@ $< $(INCLUDES)

$(OBJ_COMMON_DIR)/%.o: $(SOURCE_COMMON_DIR)/%.cpp $(DEPS_COMMON)
	$(CC) $(CFLAGS) -c -o $@ $< $(INCLUDES)

#RAKNETLIB BUILD
$(RAKNET_LIB): $(RAKNET_LIB_PATH)

$(RAKNET_LIB_PATH): $(RAKNET_OBJ)
	ar rcs $@ $^

$(RAKNET_OBJ_DIR)/%.o: $(RAKNET_SRC_DIR)/%.cpp $(RAKNET_INCLUDES)
	$(CC) $(CFLAGS) -c -o $@ $< -lpthread -lstatic -I $(RAKNET_INC_DIR)


clean:
	rm $(OUT_DIR)/$(OUT)
	rm $(OBJ_DIR)/*.o
	rm $(RAKNET_OBJ_DIR)/*.o
	rm $(RAKNET_LIB_PATH)
