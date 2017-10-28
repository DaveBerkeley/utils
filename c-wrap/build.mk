#
#

CC = $(CROSS)gcc
CXX = $(CROSS)g++
CFLAGS += -Wall -Wextra
CPPFLAGS += 

OBJBASE = obj
OBJDIR = $(OBJBASE)/c
OBJDIR_CC = $(OBJBASE)/cpp

OBJS  = $(SRC:%.c=$(OBJDIR)/%.o)
OBJS += $(SRC_CC:%.cpp=$(OBJDIR_CC)/%.o)

DEPS  = $(SRC:%.c=$(OBJDIR)/%.d)
DEPS += $(SRC_CC:%.cpp=$(OBJDIR_CC)/%.d)

LFLAGS += $(LIBS:%=-l%)
CFLAGS += $(DEFINES:%=-D%)

MAKEDEPEND = $(CC) -MM $(CPPFLAGS) -MT $(dir $@)$*.o -o $(dir $@)$*.d $<

#

all: $(APP) $(DEPS)

clean:
	rm $(APP) -rf $(OBJBASE)

test: $(APP)
	./$(APP)

$(APP): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LFLAGS)

#	Build dependencies and compile C files.

$(OBJDIR)/%.d : %.c
	@mkdir -p $(dir $@)
	$(MAKEDEPEND)

$(OBJDIR)/%.o : %.c $(OBJDIR)/%.d 
	$(CC) $(CFLAGS) -c -o $@ $<

#	Build dependencies and compile C++ files.

$(OBJDIR_CC)/%.d : %.cpp
	@mkdir -p $(dir $@)
	$(MAKEDEPEND)

$(OBJDIR_CC)/%.o : %.cpp $(OBJDIR_CC)/%.d 
	$(CXX) $(CFLAGS) -c -o $@ $<

#	Include auto-built dependencies

-include $(DEPS)

# 	FIN
