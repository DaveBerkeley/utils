
CC = gcc
CXX = g++
CFLAGS = -Wall

SRC = main.c 

#SRC += c/c.c

SRC_CC = lock.cpp

APP = maker
OBJBASE = obj
OBJDIR = $(OBJBASE)/c
OBJDIR_CC = $(OBJBASE)/cpp

OBJS  = $(SRC:%.c=$(OBJDIR)/%.o)
OBJS += $(SRC_CC:%.cpp=$(OBJDIR_CC)/%.o)

DEPS  = $(SRC:%.c=$(OBJDIR)/%.d)
DEPS += $(SRC_CC:%.cpp=$(OBJDIR_CC)/%.d)

MAKEDEPEND = $(CC) -MM $(CPPFLAGS) -MT $(OBJDIR)/$*.o -o $(OBJDIR)/$*.d $<
MAKEDEPEND_CC = $(CC) -MM $(CPPFLAGS) -MT $(OBJDIR_CC)/$*.o -o $(OBJDIR_CC)/$*.d $<

all: $(APP) $(DEPS)

clean:
	rm $(APP) -rf $(OBJBASE)

test: $(APP)
	./$(APP)

$(APP): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.d : %.c
	@mkdir -p $(dir $(OBJDIR)/$<)
	$(MAKEDEPEND)

$(OBJDIR_CC)/%.d : %.cpp
	@mkdir -p $(dir $(OBJDIR_CC)/$<)
	$(MAKEDEPEND_CC)

$(OBJDIR)/%.o : %.c $(OBJDIR)/%.d
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR_CC)/%.o : %.cpp $(OBJDIR_CC)/%.d
	$(CC) $(CFLAGS) -c -o $@ $<

-include $(DEPS)


