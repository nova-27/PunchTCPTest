SRCS := main.cpp stunutil.cpp
OBJDIR := ./build/
OBJS := $(SRCS:%.cpp=$(OBJDIR)%.o)

all: build/main

build/main: $(OBJS)
	g++ -g $(OBJS) -o $@

$(OBJDIR)%.o: %.cpp
	g++ -g -c $< -o $@

clean:
	rm -f $(OBJDIR)*