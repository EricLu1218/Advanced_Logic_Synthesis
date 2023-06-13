CXX       := g++
CXXFLAGS  := -std=c++17 -O3 -Wall -Wextra -MMD
LEDA_ROOT := ../include/LEDA
LIBS      := -L$(LEDA_ROOT) -lG -lL -lm
INCLUDE   := -isystem $(LEDA_ROOT)/incl
EXEC      := ../bin/map
SRC_DIRS  := .\
			 Decomposer\
			 FlowMap\
			 Parser\
             ResultWriter
SRCS      := $(wildcard $(SRC_DIRS:=/*.cpp))
OBJS      := $(SRCS:.cpp=.o)
DEPS      := $(OBJS:.o=.d)

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE)

clean:
	rm -rf $(EXEC) $(OBJS) $(DEPS)

ifeq (test, $(firstword $(MAKECMDGOALS)))
  TESTCASE := $(word 2, $(MAKECMDGOALS))
  K        := $(word 3, $(MAKECMDGOALS))
  $(eval $(TESTCASE):;@:)
  $(eval $(K):;@:)
endif

test: $(EXEC)
	@echo test on $(TESTCASE) with k=$(K)
	./$(EXEC) -k $(K) ../testcase/$(TESTCASE).blif ../output/$(TESTCASE).$(K).blif

.PHONY: all clean test
-include $(DEPS)
