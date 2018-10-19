
CC = gcc
CFLAGS = -Wall

LIBS = pthread
# create from LIBS ?
LFLAGS = -pthread

SRC = main.c 
#SRC += a/a.c

APP = queue
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
	$(CC) $(LFLAGS) -o $@ $^

$(OBJDIR)/%.d : %.c
	@mkdir -p $(dir $(OBJDIR)/$<)
	$(MAKEDEPEND)

$(OBJDIR)/%.o : %.c $(OBJDIR)/%.d
	$(CC) $(CFLAGS) -c -o $@ $<

-include $(DEPS)

