# -------------------------------------
# Makefile for building influenza
#
# Authors: J.Barthelemy
# Date   : 20 May 2014  
# -------------------------------------

export CXX            = mpicxx 
export CXXFLAGS       = -Wall -O3 -DNDEBUG -march='native' -mtune='native' -flto -std=c++11
export EXEC_NAME      = influenza

CXXFLAGSDEBUG  = -Wall -O0 -ggdb -pg -std=c++11

SRC_DIR   = ./src/
BIN_DIR   = ./bin/

all :
	@(cd $(SRC_DIR) && $(MAKE))

debug : CXXFLAGS = $(CXXFLAGSDEBUG)
debug :
	@(cd $(SRC_DIR) && $(MAKE) debug)

clean :
	@rm $(SRC_DIR)*.o $(BIN_DIR)$(EXEC_NAME)

