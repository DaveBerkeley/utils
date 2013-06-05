
CC = g++
CFLAGS = -Wall

SRC = main.cpp messages.cpp
APP = dispatch

OBJS = $(SRC:%.cpp=%.o)

MAKEDEPEND = $(CC) -MM $(CPPFLAGS) -o $*.d $<

all: $(APP)

clean:
	rm -f $(APP) $(OBJS) *.d

test: $(APP)
	./$(APP)

$(APP): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.d : %.cpp
	$(MAKEDEPEND)

%.o : %.cpp 
	$(CC) $(CFLAGS) -c -o $@ $<

-include $(SRC:%.cpp=%.d)

# FIN
