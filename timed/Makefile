
CC = g++
CFLAGS = -Wall -Wextra
LDFLAGS = -pthread

SRC = timed.c 
SRC += list.c 
XSRC = main.cpp

APP = timed
ODIR = obj
OBJDIR = $(ODIR)/c
XOBJDIR = $(ODIR)/cpp

OBJS = $(SRC:%.c=$(OBJDIR)/%.o)
DEPS = $(SRC:%.c=$(OBJDIR)/%.d)

XOBJS = $(XSRC:%.cpp=$(XOBJDIR)/%.o)
XDEPS = $(XSRC:%.cpp=$(XOBJDIR)/%.d)

MAKEDEPEND = $(CC) -MM $(CPPFLAGS) -MT $(OBJDIR)/$*.o -o $(OBJDIR)/$*.d $<
XMAKEDEPEND = $(CC) -MM $(CPPFLAGS) -MT $(XOBJDIR)/$*.o -o $(XOBJDIR)/$*.d $<

all: $(APP) $(DEPS)

clean:
	rm -r $(ODIR) -f $(APP)

test: $(APP)
	./$(APP)

$(APP): $(OBJS) $(XOBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.d : %.c
	@mkdir -p $(dir $(OBJDIR)/$<)
	$(MAKEDEPEND)

$(XOBJDIR)/%.d : %.cpp
	@mkdir -p $(dir $(XOBJDIR)/$<)
	$(XMAKEDEPEND)

$(OBJDIR)/%.o : %.c $(OBJDIR)/%.d
	$(CC) $(CFLAGS) -c -o $@ $<

$(XOBJDIR)/%.o : %.cpp $(XOBJDIR)/%.d
	$(CC) $(CFLAGS) -c -o $@ $<

-include $(DEPS) $(DEPS)

# FIN
