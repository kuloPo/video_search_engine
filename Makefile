CC       = g++
OPENCV_I = /home/kulopo/lib/opencv/include/opencv4
OPENCV_L = /home/kulopo/lib/opencv/lib
PQXX_I   = /home/kulopo/lib/libpqxx/include
PQXX_L   = /home/kulopo/lib/libpqxx/lib
PQ_L     = /usr/local/pgsql/lib

MAIN   = video_search_engine create_invert_index searcher
HELPER = create_noise_videos create_query_videos demo_searcher
TESTS  = test_create_index test_frame_comparison test_interval_comparison test_radon_transform test_video_comparison test_video_decode
TARGETS  = $(MAIN) $(HELPER) $(TESTS)

SRC_DIR  = ./src
BIN_DIR  = ./bin
CPPFLAGS = -g -std=c++17 
HPPS     = -Iheader -I$(OPENCV_I) -I$(PQXX_I)
LIBS     = -lstdc++ -lstdc++fs -lm -lpthread -lpqxx -lpq -lopencv_core -lopencv_videoio -lopencv_imgproc -lopencv_ximgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_shape -lopencv_plot
OBJS     = $(addprefix $(BIN_DIR)/, algo.o debug.o imgproc.o io.o similar.o utils.o)
LIB_DIR  = -L$(OPENCV_L) -L$(PQXX_L) -L$(PQ_L)

all: $(TARGETS)

main: $(MAIN)
helper: $(HELPER)
tests: $(TESTS)

%: $(BIN_DIR)/%.o $(OBJS)
	$(CC) $(CPPFLAGS) $(LIB_DIR) -o $(BIN_DIR)/$@ $^ $(LIBS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CPPFLAGS) $(HPPS) -c $< -o $(BIN_DIR)/$*.o

.PHONY: clean
clean:
	rm -f $(BIN_DIR)/*
