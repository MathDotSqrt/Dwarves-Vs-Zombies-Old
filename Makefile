CC = g++
CFLAGS = -std=c++17

OUT = server

BINARY = x64
MODE = Debug

SOURCE_DIR = ./DVZ_Make
OBJ_DIR = $(SOURCE_DIR)/x64/Debug
OUT_DIR = ./x64/Debug

DEPS = $(wildcard $(SOURCE_DIR)/*.h)
SRCS = $(wildcard $(SOURCE_DIR)/*.cpp)
OBJ = $(patsubst $(SOURCE_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

INCLUDES = \
./ENTT/single_include/entt/ \
./GLM/


RAKNET_LIB = RakNetLib
RAKNET_LIB_DIR = ./RakNet/lib
RAKNET_LIB_PATH = $(RAKNET_LIB_DIR)/$(RAKNET_LIB).a
RAKNET_SRC_DIR = ./RakNet/include/src

RAKNET_BIN = ./RakNet/bin
RAKNET_SRC = $(wildcard $(RAKNET_SRC_DIR)/*.cpp) $(wildcard $(RAKNET_SRC_DIR)/crypto/*.cpp)
LIB =




$(OUT): $(OUT_DIR)/$(OUT)

$(OUT_DIR)/$(OUT): $(OBJ)
	$(CC) -o $@ $^

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $< $(LIB) -I $(INCLUDES)

gay:
	echo $(RAKNET_SRC)

clean:
	rm $(OUT_DIR)/$(OUT)
	rm $(OBJ_DIR)/*.o
