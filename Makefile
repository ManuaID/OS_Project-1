CXX = g++
CXXFLAGS = -std=c++20
LIBRARY = -pthread

SORUCES_MM = mmcopier.cpp
TARGET_MM = mmcopier.out	

SOURCES_MS = mscopier.cpp 
TARGET_MS = mscopier.out

SOURCE_TXT = source.txt

DEST_FILES = destination_dir/*.txt

mm: ${SORUCES}
	$(CXX) $(CXXFLAGS) $(LIBRARY) $(SORUCES_MM) -o $(TARGET_MM) 

ms: ${SOURCES}
	$(CXX) $(CXXFLAGS) $(LIBRARY) $(SOURCES_MS) -o $(TARGET_MS) 

clean:
	rm -rf $(TARGET_MS) $(TARGET_MM) $(DEST_FILES) $(SOURCE_TXT)