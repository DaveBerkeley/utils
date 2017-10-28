
SRC = main.c 

#SRC += c/c.c

SRC_CC = lock.cpp mutex.cpp

APP = maker

LIBS = pthread

#DEFINES = DEBUG

#
#

CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra

OBJBASE = obj
OBJDIR = $(OBJBASE)/c
OBJDIR_CC = $(OBJBASE)/cpp

OBJS  = $(SRC:%.c=$(OBJDIR)/%.o)
OBJS += $(SRC_CC:%.cpp=$(OBJDIR_CC)/%.o)

DEPS  = $(SRC:%.c=$(OBJDIR)/%.d)
DEPS += $(SRC_CC:%.cpp=$(OBJDIR_CC)/%.d)

LFLAGS += $(LIBS:%=-l%)
CFLAGS += $(DEFINES:%=-D%)

MAKEDEPEND = $(CC) -MM $(CPPFLAGS) -MT $(OBJDIR)/$*.o -o $(OBJDIR)/$*.d $<
MAKEDEPEND_CC = $(CC) -MM $(CPPFLAGS) -MT $(OBJDIR_CC)/$*.o -o $(OBJDIR_CC)/$*.d $<

all: $(APP) $(DEPS)

clean:
	rm $(APP) -rf $(OBJBASE)

test: $(APP)
	./$(APP)

$(APP): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LFLAGS)

$(OBJDIR)/%.d : %.c Makefile
	@mkdir -p $(dir $(OBJDIR)/$<)
	$(MAKEDEPEND)

$(OBJDIR_CC)/%.d : %.cpp Makefile
	@mkdir -p $(dir $(OBJDIR_CC)/$<)
	$(MAKEDEPEND_CC)

$(OBJDIR)/%.o : %.c $(OBJDIR)/%.d
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR_CC)/%.o : %.cpp $(OBJDIR_CC)/%.d
	$(CC) $(CFLAGS) -c -o $@ $<

-include $(DEPS)


