CXXFLAGS=-Wall -Werror -fno-exceptions
LIBS=-lpthread

concurrent_queue: main.cpp
	$(CXX) $(CXXFLAGS) -o concurrent_queue main.cpp $(LIBS)
