
CC = gcc
CFLAGS = -Wall

SRC = main.c 
#SRC += c/c.c

APP = maker
OBJDIR = obj

OBJS = $(SRC:%.c=$(OBJDIR)/%.o)
DEPS = $(SRC:%.c=$(OBJDIR)/%.d)

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
	$(CC) $(CFLAGS) -c -o $@ $<

-include $(DEPS)


