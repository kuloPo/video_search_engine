CC       = /home/kulopo/lib/gcc-8.5.0/bin/g++
OPENCV_I = /usr/local/include/opencv4
PQXX_I   = /usr/local/include
OPENCV_L = /usr/local/lib
PQXX_L   = /usr/local/lib
TARGET   = test_create_index test_interval_comparison test_radon_transform test_video_comparison test_video_decode create_invert_index searcher video_search_engine

SRC_DIR  = ./src
BIN_DIR  = ./bin

LIBS     = -lstdc++ -lstdc++fs -lm -lpthread -lpq -lpqxx -lopencv_core -lopencv_videoio -lopencv_imgproc -lopencv_ximgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_shape

all: $(TARGET)

test_create_index:
	@mkdir -p $(BIN_DIR)
	$(CC) -D DEBUG_CREATE_INDEX -std=c++17 -Iheader -I$(OPENCV_I) -I$(PQXX_I) -L$(OPENCV_L) -L$(PQXX_L) ./src/algo.cpp ./src/debug.cpp ./src/similar.cpp ./src/utils.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

test_frame_comparison:
	@mkdir -p $(BIN_DIR)
	$(CC) -std=c++17 -Iheader -I$(OPENCV_I) -I$(PQXX_I) -L$(OPENCV_L) -L$(PQXX_L) ./src/algo.cpp ./src/debug.cpp ./src/similar.cpp ./src/utils.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

test_interval_comparison:
	@mkdir -p $(BIN_DIR)
	$(CC) -D DEBUG_INTERVAL_COMPARISON -std=c++17 -Iheader -I$(OPENCV_I) -I$(PQXX_I) -L$(OPENCV_L) -L$(PQXX_L) ./src/similar.cpp ./src/utils.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

test_radon_transform:
	@mkdir -p $(BIN_DIR)
	$(CC) -std=c++17 -Iheader -I$(OPENCV_I) -I$(PQXX_I) -L$(OPENCV_L) -L$(PQXX_L) ./src/similar.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

test_video_comparison:
	@mkdir -p $(BIN_DIR)
	$(CC) -D DEBUG_INTERVAL_COMPARISON -std=c++17 -Iheader -I$(OPENCV_I) -I$(PQXX_I) -L$(OPENCV_L) -L$(PQXX_L) ./src/algo.cpp ./src/similar.cpp ./src/utils.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

test_video_decode:
	@mkdir -p $(BIN_DIR)
	$(CC) -std=c++17 -Iheader -I$(OPENCV_I) -I$(PQXX_I) -L$(OPENCV_L) -L$(PQXX_L) ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

create_invert_index:
	@mkdir -p $(BIN_DIR)
	$(CC) -std=c++17 -Iheader -I$(OPENCV_I) -I$(PQXX_I) -L$(OPENCV_L) -L$(PQXX_L) ./src/io.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

searcher:
	@mkdir -p $(BIN_DIR)
	$(CC) -std=c++17 -Iheader -I$(OPENCV_I) -I$(PQXX_I) -L$(OPENCV_L) -L$(PQXX_L) ./src/algo.cpp ./src/similar.cpp ./src/utils.cpp ./src/io.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

video_search_engine:
	@mkdir -p $(BIN_DIR)
	$(CC) -D DEBUG_PERFORMANCE -std=c++17 -Iheader -I$(OPENCV_I) -I$(PQXX_I) -L$(OPENCV_L) -L$(PQXX_L) ./src/algo.cpp ./src/similar.cpp ./src/utils.cpp ./src/io.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

.PHONY: clean
clean:
	rm -r $(BIN_DIR)

