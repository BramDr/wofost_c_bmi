SRCDIRS = src src/bmi src/simulation src/wofost
INCLUDEDIRS = include include/bmi include/simulation include/wofost

SRCS = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c))
OBJS = $(SRCS:%.c=%.o)

EXECUTABLE = wofost
CC       = gcc

CPPFLAGS = $(foreach dir,$(INCLUDEDIRS),-I$(dir)) -I$(CONDA_PREFIX)/include
CFLAGS  = -g -ggdb -Wall -Wextra -Wformat=2 -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wshadow -O2 -std=c99
LDFLAGS = -L$(CONDA_PREFIX)/lib
LDLIBS = -lm -lnetcdf

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(LDFLAGS) -o $(EXECUTABLE) $(OBJS) $(LDLIBS)


%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

TAGS: $(SRCS)
	etags $(SRCS)

clean:
	rm -f TAGS $(EXECUTABLE) $(OBJS)
