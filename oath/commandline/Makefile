CFLAGS := -g -MMD
CXXFLAGS := -MMD -std=c++11 -stdlib=libc++ $(shell pkg-config --cflags libusb) -g
LDFLAGS := $(shell pkg-config --libs libusb) -lc++

OBJDIR := obj
ALL_OBJ := $(addprefix $(OBJDIR)/,opendevice.o main.o test.o parse.o)
ALL_DEP := ${ALL_OBJ:.o=.d}

all: oath-cli run_tests

oath-cli: $(addprefix $(OBJDIR)/,opendevice.o main.o parse.o)
	$(CXX) $(LDFLAGS) -o $@ $^

run_tests: test
	./$^

test: $(addprefix $(OBJDIR)/,test.o parse.o)
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	rm -f oath-cli test
	rm -r obj

.PHONY: all clean run_tests

$(OBJDIR)/%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(ALL_OBJ): | $(OBJDIR)/.made

$(OBJDIR)/.made:
	mkdir -p $(OBJDIR) && touch $@


-include $(ALL_DEP)
