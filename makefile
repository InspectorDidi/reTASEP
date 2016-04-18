#####################
#file: makefile
#author: Marvin Böttcher (boettcher@evolbio.mpg.de)
#2016
#####################

#Make targets:
# make all
# make run
# make clean
# make dep

SHELL= /bin/bash

#compilation target name
TARGET := reTASEP

#directories for source code and build-files
BUILDDIR :=  build
SRCDIR :=  src
SRCEXT := cpp
INCLUDEDIR := include


#COMPILATION AND LIBARY FLAGS##########################################
CC:=g++ 

# LDLIBS = #-lm 
# -lefence -lboost_random -lboost_thread-mt  -lboost_random  -stdlib=libc++

WARNINGS := -Wall -pedantic -Wextra -Wshadow  -Wcast-qual  -Wfloat-equal -Wunreachable-code   -Wdisabled-optimization #-Wconversion
#--Wstrict-overflow=5 Wpointer-arith -Wunused

OPTIFLAGS := -O3 # release mode: -O2 or -O3 -ffast-math debug mode: -g -ggdb  profile mode: -pg 

#resulting CFLAGS... -std=c++11 required
CFLAGS := -std=c++11 $(OPTIFLAGS) $(WARNINGS) -I./$(INCLUDEDIR)/


#formatting of variables##################################################
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
DEPS = $(OBJECTS:%.o=%.d)


#building all objects (main program) #################################
all: $(TARGET)
	
#build the main target
$(TARGET): $(OBJECTS)
	$(CC)  $(CFLAGS)  -o $(TARGET) $(OBJECTS) $(LDLIBS)


#all .o rules  ################################################### 
./$(BUILDDIR)/%.o : ./$(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS)  -c -o $@ $<



#create dependencies ############################################
dep: $(DEPS)

-include $(DEPS) #include depencies (saved in .d files)

#rule for creating the dependency files##########################
$(BUILDDIR)/%.d: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MM  -MT '$(subst .d,.o,$@)'  $< -MF $@ 



#clean ##################################################################
.PHONY : clean

clean : 
	rm -f $(TARGET) $(OBJECTS) $(DEPS) $(OBJSUT) $(OBJSPT)


#run the compiled program#############################
.PHONY: run

run: $(TARGET)
	./$(TARGET)

install: $(TARGET)
	cp $(TARGET) ~/.local/bin/

