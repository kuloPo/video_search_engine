CC       = /home/kulopo/lib/gcc-8.5.0/bin/g++
TARGET   = test_create_index test_interval_comparison test_radon_transform test_video_comparison test_video_decode create_invert_index searcher video_search_engine

SRC_DIR  = ./src
BIN_DIR  = ./bin

SOURCES := $(wildcard $(SRCDIR)/*.cpp)
LIBS     = -lstdc++ -lstdc++fs -lm -lpq -lpqxx -lopencv_core -lopencv_videoio -lopencv_imgproc -lopencv_ximgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_shape

all: $(TARGET)

#$(TARGET):
#	mkdir -p $(BIN_DIR)
#	g++-8 -std=c++17 -Iheader -I/usr/local/include/opencv4 -L/usr/local/lib ./src/algo.cpp ./src/similar.cpp ./src/utils.cpp ./src/io.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

test_create_index:
	@mkdir -p $(BIN_DIR)
	$(CC) -std=c++17 -Iheader -I/usr/local/include/opencv4 -L/usr/local/lib ./src/algo.cpp ./src/similar.cpp ./src/utils.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

test_interval_comparison:
	@mkdir -p $(BIN_DIR)
	$(CC) -std=c++17 -Iheader -I/usr/local/include/opencv4 -L/usr/local/lib ./src/similar.cpp ./src/utils.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

test_radon_transform:
	@mkdir -p $(BIN_DIR)
	$(CC) -std=c++17 -Iheader -I/usr/local/include/opencv4 -L/usr/local/lib ./src/similar.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

test_video_comparison:
	@mkdir -p $(BIN_DIR)
	$(CC) -std=c++17 -Iheader -I/usr/local/include/opencv4 -L/usr/local/lib ./src/algo.cpp ./src/similar.cpp ./src/utils.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

test_video_decode:
	@mkdir -p $(BIN_DIR)
	$(CC) -std=c++17 -Iheader -I/usr/local/include/opencv4 -L/usr/local/lib ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

create_invert_index:
	@mkdir -p $(BIN_DIR)
	$(CC) -std=c++17 -Iheader -I/usr/local/include/opencv4 -L/usr/local/lib ./src/io.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

searcher:
	@mkdir -p $(BIN_DIR)
	$(CC) -std=c++17 -Iheader -I/usr/local/include/opencv4 -L/usr/local/lib ./src/algo.cpp ./src/similar.cpp ./src/utils.cpp ./src/io.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

video_search_engine:
	@mkdir -p $(BIN_DIR)
	$(CC) -std=c++17 -Iheader -I/usr/local/include/opencv4 -L/usr/local/lib ./src/algo.cpp ./src/similar.cpp ./src/utils.cpp ./src/io.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

.PHONY: clean
clean:
	rm -r $(BIN_DIR)

