CC = gcc
CFLAGS = -Wall

HERE := $(shell /bin/pwd)

SRC = main.c 
SRC += callback.c
SRC += log.c

APP = ctor
OBJDIR = obj

OBJS = $(SRC:%.c=$(OBJDIR)/%.o)
DEPS = $(SRC:%.c=$(OBJDIR)/%.d)

DEFS += -D __CWD__=$(HERE)

MAKEDEPEND = $(CC) -MM $(CPPFLAGS) -MT $(OBJDIR)/$*.o -o $(OBJDIR)/$*.d $<

all: $(APP) $(DEPS)

clean:
		rm -f $(OBJDIR) -r $(APP)

test: $(APP)
		./$(APP)

$(APP): $(OBJS)
		$(CC) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.d : %.c
		@mkdir -p $(dir $(OBJDIR)/$<)
		$(MAKEDEPEND)

$(OBJDIR)/%.o : %.c $(OBJDIR)/%.d
		$(CC) $(CFLAGS) $(DEFS) -c -o $@ $<

-include $(DEPS)

