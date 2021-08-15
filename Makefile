SRC_F := $(shell find src -name *.c) $(shell find libs -name *.c)
SRC_CPP := $(shell find libs/soloud/src -name *.cpp)
OBJ_FOR_CLEAN_F := $(shell find ./src -name *.o)
SRC_O := $(patsubst %.c,%.o,$(SRC_F))
SRC_CPP_O := $(patsubst %.cpp,%.o,$(SRC_CPP))

LIBS := -lpthread -lm -lglfw -lGL -lstdc++ -ldl -lasound

FLAGS__DEBUG := -g3 -O0 
FLAGS_DEBUG := -O3 -fdata-sections -ffunction-sections
FLAGS := -Wall -Wextra -Ilibs/include -Ilibs/soloud/include -Ilibs/GLAD/include

TARGET := bin/main.bin
.PONY: clean

all: $(SRC_O) $(SRC_CPP_O) copy_assets
	clang $(FLAGS_DEBUG) $(FLAGS) $(SRC_O) $(SRC_CPP_O) -o $(TARGET) $(LIBS)

run_main: all
	$(TARGET)

%.o: %.c
	clang -c $(FLAGS_DEBUG) $(FLAGS) $^ -o $@

%.o: %.cpp
	gcc -c $(FLAGS_RELEASE) $(FLAGS) -lstdc++ -ldl -lasound -DWITH_ALSA=1 $^ -o $@

clean:
	rm -rf $(OBJ_FOR_CLEAN_F)
	rm -rf $(TARGET)
	rm -rf bin/assets

deep_clean: clean
	rm -rf $(SRC_CPP_O)
	

copy_assets:
	cp -r assets bin
