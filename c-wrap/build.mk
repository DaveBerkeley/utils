#
#

CC = $(EXPORT) $(CROSS)gcc
CXX = $(EXPORT) $(CROSS)g++
CFLAGS += -Wall -Wextra -fPIC
CPPFLAGS += 

TARGET ?= native
OBJBASE = obj
OBJDIR = $(OBJBASE)/$(TARGET)
OBJDIR_CC  = $(OBJDIR)/c
OBJDIR_CPP = $(OBJDIR)/cpp

OBJS  = $(SRC_CC:%.c=$(OBJDIR_CC)/%.o)
OBJS += $(SRC_CPP:%.cpp=$(OBJDIR_CPP)/%.o)

DEPS  = $(SRC_CC:%.c=$(OBJDIR_CC)/%.d)
DEPS += $(SRC_CPP:%.cpp=$(OBJDIR_CPP)/%.d)

LFLAGS += $(LIBS:%=-l%)
CFLAGS += $(DEFINES:%=-D%)

#

all: $(APP) $(LIB) $(DEPS)

clean:
	rm $(APP) -rf $(OBJBASE)

test: $(APP)
	./$(APP)

$(APP): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LFLAGS)

$(LIB): $(OBJS)
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LFLAGS)

#

MAKEDEPEND = $(CC) -MM $(CPPFLAGS) -MT $(basename $@).o -o $(basename $@).d $<

#	Build dependencies and compile C files.

$(OBJDIR_CC)/%.d : %.c
	@mkdir -p $(dir $@)
	$(MAKEDEPEND)

$(OBJDIR_CC)/%.o : %.c $(OBJDIR_CC)/%.d 
	$(CC) $(CFLAGS) -c -o $@ $<

#	Build dependencies and compile C++ files.

$(OBJDIR_CPP)/%.d : %.cpp
	@mkdir -p $(dir $@)
	$(MAKEDEPEND)

$(OBJDIR_CPP)/%.o : %.cpp $(OBJDIR_CPP)/%.d 
	$(CXX) $(CFLAGS) -c -o $@ $<

#	Include auto-built dependencies

-include $(DEPS)

# 	FIN
