CXX=g++-7
CXXFLAGS=-g -std=c++11
BIN=apriori

SRC=$(wildcard *.cc)
OBJ=$(SRC:%.cc=%.o)

all: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(BIN) $^

%.o: %.c
	$(CXX) $(CXXFLAGS) $@ -c $<

clean:
	rm -f *.o
	rm $(BIN)
