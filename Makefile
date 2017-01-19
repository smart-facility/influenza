# -------------------------------------
# Makefile for building influenza
#
# Authors: J.Barthelemy
# Date   : 20 May 2014  
# -------------------------------------

export CXX                = mpicxx 
export CXXFLAGS           = -Wall -Wno-deprecated -O3 -DNDEBUG -march='native' -mtune='native' -flto -Wdeprecated-declarations
export CXXFLAGS_PROF_GEN  = -Wall -O3 -march='native' -mtune='native' -flto -fprofile-generate
export CXXFLAGS_PROF_USE  = -Wall -O3 -march='native' -mtune='native' -flto -fprofile-use
export EXEC_NAME          = influenza

CXXFLAGSDEBUG  = -Wall -O0 -ggdb -pg

SRC_DIR   = ./src/
BIN_DIR   = ./bin/

all :
	@(cd $(SRC_DIR) && $(MAKE))

debug : CXXFLAGS = $(CXXFLAGSDEBUG)
debug :
	@(cd $(SRC_DIR) && $(MAKE) debug)

profile_gen : CXXFLAGS = $(CXXFLAGS_PROF_GEN)
profile_gen :
	@(cd $(SRC_DIR) && $(MAKE) profile_gen)
	
profile_use : CXXFLAGS = $(CXXFLAGS_PROF_USE)
profile_use :
	@(cd $(SRC_DIR) && $(MAKE) profile_use)
	
clean :
	@rm $(SRC_DIR)*.o $(BIN_DIR)$(EXEC_NAME)

