CXX  := g++ 
EXEC    := Contacts.C HHContact.C
TARGET  := ${EXEC:.C=} 
CFGDIR  := Config
SRCS    := ContactErr.C ContactMatrix.C CSVParser.C BitArray.C Utilities.C $(wildcard $(CFGDIR)/*.C)
HEADERS := $(wildcard *.h) $(wildcard $(CFGDIR)/*.h) 
OBJS    := ${SRCS:.C=.o}  
CPPFLAGS  := -g -O3 -DNDEBUG -std=c++11

# Temporary dependency directory
DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
# $ DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
DEPFLAGS = 

COMPILE = $(CXX) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) 

GIT := $(shell command -v git 2> /dev/null)

all : ${TARGET}

HHContact : ${OBJS} ${HEADERS} HHContact.o Makefile
	${COMPILE} $(OBJS) $@.C -o $@ 

Contacts : ${OBJS} ${HEADERS} Contacts.o Makefile
	${COMPILE} $(OBJS) $@.C -o $@ ${LDFLAGS} 

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

-include $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS)))

.PHONY: all clean dist print debug
test:: 
	g++ -std=c++11 drive.C

Version.C : FORCE 
	echo "#include \"Version.h\"" > Version.C
ifdef GIT
	$(GIT) rev-parse HEAD | awk '{print "string Version::build_git_sha = \"" $$0"\";"}' >> Version.C
	$(GIT) describe --abbrev=4 --dirty --always --tags | awk '{print "string Version::build_git_describe = \"" $$0"\";"}' >> Version.C
else
	$(info git is not available - version information will not be compiled into executable)
	echo "string Version::build_git_sha = \"unavailable at compile-time\";" >> Version.C
	echo "string Version::build_git_describe = \" unavailable at compile-time\";" >> Version.C
endif
	date | awk '{print "string Version::build_git_time = \""$$0"\";"}' >> Version.C

FORCE : 

debug::
	$(MAKE) CFLAGS="-g -std=c++11 -DDEBUG -DBUFSIZE="1024" -DCHECK_GRAPH -Wall -Wno-sign-compare "

print:: 
	@echo ${LDFLAGS}
	@echo ${SRCS}

clean:: 
	rm -rf *~ ${OBJS} ${TARGET} ${DEPDIR}

dist::
	tar cvfz ${TARGET}.tar.gz ${EXEC} ${SRCS} ${HEADERS} Makefile CodeDoc.pdf 
