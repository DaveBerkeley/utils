#
#

CC = $(EXPORT) $(CROSS)gcc
CXX = $(EXPORT) $(CROSS)g++
CFLAGS += -Wall -Wextra
CPPFLAGS += 

TARGET ?= native
OBJBASE = obj/$(TARGET)
OBJDIR = $(OBJBASE)/c
OBJDIR_CC = $(OBJBASE)/cpp

OBJS  = $(SRC_CC:%.c=$(OBJDIR)/%.o)
OBJS += $(SRC_CPP:%.cpp=$(OBJDIR_CC)/%.o)

DEPS  = $(SRC_CC:%.c=$(OBJDIR)/%.d)
DEPS += $(SRC_CPP:%.cpp=$(OBJDIR_CC)/%.d)

LFLAGS += $(LIBS:%=-l%)
CFLAGS += $(DEFINES:%=-D%)

#

all: $(APP) $(DEPS)

clean:
	rm $(APP) -rf $(OBJBASE)

test: $(APP)
	./$(APP)

$(APP): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LFLAGS)

#

MAKEDEPEND = $(CC) -MM $(CPPFLAGS) -MT $(basename $@).o -o $(basename $@).d $<

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
