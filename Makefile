CC = g++
CC_FLAGS = -std=c++11 -Wall -O2
LD_LIBS = -pthread -lboost_system -lboost_regex -lssl -lcrypto
# -ltcmalloc_minimal 

EXEC = http_client
SOURCES = $(wildcard *.cpp) lib/json11.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC) $(LD_LIBS)

%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@

clean:
	rm -rf $(EXEC) $(OBJECTS)


