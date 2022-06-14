CC       = g++
OPENCV_I = /home/kulopo/lib/opencv/include/opencv4
OPENCV_L = /home/kulopo/lib/opencv/lib
PQXX_I   = /home/kulopo/lib/libpqxx/include
PQXX_L   = /home/kulopo/lib/libpqxx/lib
TARGET   = test_create_index test_frame_comparison test_interval_comparison test_radon_transform test_video_comparison test_video_decode create_invert_index searcher video_search_engine

SRC_DIR  = ./src
BIN_DIR  = ./bin

LIBS     = -lstdc++ -lstdc++fs -lm -lpthread -lpqxx -lpq -lopencv_core -lopencv_videoio -lopencv_imgproc -lopencv_ximgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_shape -lopencv_plot

PARAM    = -std=c++17 -Iheader -I$(OPENCV_I) -I$(PQXX_I) -L$(OPENCV_L) -L$(PQXX_L)

all: $(TARGET)

test_create_index:
	@mkdir -p $(BIN_DIR)
	$(CC) -D DEBUG_CREATE_INDEX $(PARAM) ./src/algo.cpp ./src/debug.cpp ./src/similar.cpp ./src/utils.cpp ./src/io.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

test_frame_comparison:
	@mkdir -p $(BIN_DIR)
	$(CC) $(PARAM) ./src/algo.cpp ./src/debug.cpp ./src/similar.cpp ./src/utils.cpp ./src/io.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

test_interval_comparison:
	@mkdir -p $(BIN_DIR)
	$(CC) -D DEBUG_INTERVAL_COMPARISON $(PARAM) ./src/similar.cpp ./src/utils.cpp ./src/io.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

test_radon_transform:
	@mkdir -p $(BIN_DIR)
	$(CC) $(PARAM) ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

test_video_comparison:
	@mkdir -p $(BIN_DIR)
	$(CC) -D DEBUG_INTERVAL_COMPARISON $(PARAM) ./src/algo.cpp ./src/similar.cpp ./src/utils.cpp ./src/io.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

test_video_decode:
	@mkdir -p $(BIN_DIR)
	$(CC) $(PARAM) ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

create_invert_index:
	@mkdir -p $(BIN_DIR)
	$(CC) $(PARAM) ./src/io.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

create_query_videos:
	@mkdir -p $(BIN_DIR)
	$(CC) $(PARAM) ./src/io.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

demo_searcher:
	@mkdir -p $(BIN_DIR)
	$(CC) -D SHOW_PROGRESS $(PARAM) ./src/algo.cpp ./src/similar.cpp ./src/utils.cpp ./src/io.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

searcher:
	@mkdir -p $(BIN_DIR)
	$(CC) $(PARAM) ./src/algo.cpp ./src/similar.cpp ./src/utils.cpp ./src/io.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

video_search_engine:
	@mkdir -p $(BIN_DIR)
	$(CC) -D DEBUG_PERFORMANCE $(PARAM) ./src/algo.cpp ./src/similar.cpp ./src/utils.cpp ./src/io.cpp ./$@/$@.cpp $(LIBS) -o $(BIN_DIR)/$@

.PHONY: clean
clean:
	rm -r $(BIN_DIR)

