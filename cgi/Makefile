BASEDIR = ../
include $(BASEDIR)Make.env
include $(BASEDIR)MakeL.env

CFLAGS += -rdynamic

INCS	= $(INC_PUB) $(INC_ROBOT) $(INC_MEVENT) $(INC_MNL) $(INC_MEMC) $(INC_CS) $(INC_JSON) $(INC_FCGI) $(INC_SYSTEM)
#LIBS	= $(LIB_ROBOT) $(LIB_PUB) $(LIB_ROBOT) $(LIB_PUB) $(LIB_MEVENT) $(LIB_MNL) $(LIB_MEMC) $(LIB_CS) $(LIB_JSON) $(LIB_FCGI) $(LIB_SYSTEM)
LIBS	= $(LIB_ROBOT) $(LIB_ROBOT) $(LIB_MEVENT) $(LIB_MNL) $(LIB_MEMC) $(LIB_CS) $(LIB_JSON) $(LIB_FCGI) $(LIB_SYSTEM)

SOURCES = $(wildcard *.c)
BINARY = $(patsubst %.c, %, $(SOURCES))
DEPEND = .depend

all: preload $(BINARY)

$(DEPEND): $(SOURCES)
	@$(CC) $(CFLAGS) -MM $^ $(INCS) > $@

preload:
	@$(MAKE) -C $(MNLBASE)
	#@$(MAKE) -C $(PUBBASE)
	@$(MAKE) -C $(ROBOTBASE)

include $(DEPEND)
%:%.c
	@if [ "$<" = `ls *.c|awk '{print $1}'|sed -n '1p'` ]; then \
		echo "=== "$(CFLAGS); \
		echo "=== "$(INCS); \
		echo "=== "$(LIBS); fi
	@echo "$(CC) -o $@"
	@$(CC) $(CFLAGS) $< -o $@ $(INCS) ${LIBS}

cflag:
	@echo "=== "$(CFLAGS)
	@echo "=== "$(INCS)
install:all
	@mkdir -p -m 775 $(PATH_CGI)
	@strip $(BINARY)
	@cp $(BINARY) $(PATH_CGI)/
	@chmod a+x $(PATH_CGI)/$(BINARY)
clean:
	@rm -rf ${BINARY}
distclean:
	@rm -rf $(PATH_CGI)/*
	@rm -rf ${BINARY}
