
# compiler
CXX = g++

# compiler flags
CXXFLAGS = -Werror

# name for building the file name for the backup target
PROJECTNAME = CS4384_Project1_was170001


# src files to compile
SRCS = main.cc

# name of executable
EXEC = program.exe

#-----------------------------------------------------------------------
OBJS = $(SRCS:cc=o)

.PHONY: all clean

all: $(EXEC)
clean:
	rm -f $(OBJS) *.d *~ $(EXEC)


# Pattern for .d files.
%.d:%.cc
	@echo Updating .d Dependency File
	@set -e; rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$


#  This is a rule to link the files.
$(EXEC): $(OBJS)
	$(CXX) -o $(EXEC) $(LDFLAGS) $(OBJS)

Makefile: $(SRCS:.cc=.d)


# Backup Target
backup:	clean
	@mkdir -p ~/backups; chmod 700 ~/backups
	@$(eval CURDIRNAME := $(shell basename `pwd`))
	@$(eval MKBKUPNAME := ~/backups/$(PROJECTNAME)-$(shell date +'%Y.%m.%d-%H:%M:%S').tar.gz)
	@echo
	@echo Writing Backup file to: $(MKBKUPNAME)
	@echo
	@-tar zcfv $(MKBKUPNAME) ../$(CURDIRNAME)
	@chmod 600 $(MKBKUPNAME)
	@echo
	@echo Done!


-include $(SRCS:.cc=.d)
