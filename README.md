# An Efficient Near Duplicate Video Retrieval System Based on Time Intervals

## Abstract
The fast grow of video data on the Internet makes it a new challenge to detect duplicate or plagiarized video in an efficient and accurate manner. In this paper, we proposed an efficient video retrieval system based on the time intervals between keyframes and uses numerical comparisons only, ensuring fast response time during query operations. The proposed method demonstrates excellent performance in accuracy and efficiency, with a 100\% accuracy rate on the MUSCLE\_VCD\_2007 ST1 query and strong robustness in various test scenarios. This system offers a promising solution to the growing challenge of video data management in the era of big data.

## Build

### Windows

1. Install PostgreSQL
2. Install vcpkg
3. ```vcpkg install opencv[contrib,nonfree,ffmpeg,ipp]:x64-windows --recurse```
4. ```vcpkg install libpqxx:x64-windows```
5. ```git clone https://github.com/kuloPo/video_search_engine.git```
6. ```cd video_search_engine; mkdir build; cd build; cmake ..; make```

### Linux
1. Install PostgreSQL
2. ```sudo apt install pkg-config libpqxx-dev```
3. Compile OpenCV and OpenCV_contrib from source code
4. ```git clone https://github.com/kuloPo/video_search_engine.git```
5. ```cd video_search_engine; mkdir build; cd build; cmake ..; make```
