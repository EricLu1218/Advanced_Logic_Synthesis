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

ifeq (test,$(firstword $(MAKECMDGOALS)))
  TESTCASE_NAME := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(TESTCASE_NAME):;@:)
endif

test: $(EXEC)
	@echo test on $(TESTCASE_NAME)
	./$(EXEC) -k 4 ../testcase/$(TESTCASE_NAME).blif ../output/$(TESTCASE_NAME).4.blif

.PHONY: all clean test
-include $(DEPS)
