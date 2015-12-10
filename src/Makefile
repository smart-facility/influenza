SOURCES   = $(wildcard *.cpp)
OBJECTS   = $(SOURCES:.cpp=.o)
BIN_DIR   = ../bin/
LIBS      = -lboost_system -lboost_mpi -lboost_serialization -lboost_filesystem -lrepast_hpc-2.1 -lnetcdf_c++ -lxml++-2.6 -lxml2 -lglibmm-2.4 -lgobject-2.0 -lglib-2.0 -lsigc-2.0
INC_XML   = -I/usr/include/libxml++-2.6 -I/usr/lib64/libxml++-2.6/include -I/usr/include/libxml2 -I/usr/include/glibmm-2.4 -I/usr/lib64/glibmm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/sigc++-2.0 -I/usr/lib64/sigc++-2.0/include

debug : all
profile_use : all
profile_gen : all
all : $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LIBS) -o $(BIN_DIR)$(EXEC_NAME)

%.o : %.cpp ../include/%.hpp
	$(CXX) $(CXXFLAGS) $(INC_XML) -o $@ -c $<

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INC_XML) -o $@ -c $<


