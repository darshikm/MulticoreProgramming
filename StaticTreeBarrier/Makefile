include current.mk

TESTNAME = test1

all: $(TESTNAME) 

$(TESTNAME): $(TESTNAME).cc staticTreeBarrier.h
	$(CXX) -o $@ $< $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f $(TESTNAME) *.o
