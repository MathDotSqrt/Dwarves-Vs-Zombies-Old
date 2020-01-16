CC = g++
CFLAGS = -std=c++17 -O3

OUT = server

BINARY = x64
MODE = Debug

SOURCE_DIR = ./DVZ_server
OBJ_DIR = $(SOURCE_DIR)/$(BINARY)/$(MODE)
OUT_DIR = ./$(BINARY)/$(MODE)

DEPS = $(wildcard $(SOURCE_DIR)/*.h)
SRCS = $(wildcard $(SOURCE_DIR)/*.cpp)
OBJ = $(patsubst $(SOURCE_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))



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

INCLUDES = -I $(RAKNET_INC_DIR)/ -I ./ENTT/single_include/entt/ -I ./GLM/  
LIBS = -L $(RAKNET_LIB_DIR) -lRakNet -lpthread 

all: $(OUT)
	echo "Done."

#SERVER RUN
run: $(OUT)
	$(OUT_DIR)/$(OUT)

#SERVER BUILD
$(OUT): $(RAKNET_LIB) $(OUT_DIR)/$(OUT) 

$(OUT_DIR)/$(OUT): $(OBJ)
	$(CC) -o $@ $^ $(INCLUDES) $(LIBS)

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(DEPS)
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
