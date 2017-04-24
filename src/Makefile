SOURCES   = $(wildcard *.cpp)
OBJECTS   = $(SOURCES:.cpp=.o)
BIN_DIR   = ../bin/

LDFLAGS = -L/opt/gridware/depots/8168fdff/el7/pkg/apps/python/2.7.8/gcc-4.8.5/lib/ -L/opt/gridware/depots/8168fdff/el7/pkg/libs/netcdf/4.3.0/gcc-4.8.5+hdf5_serial-1.8.13/lib -L/opt/gridware/local/el7/pkg/libs/netcdf/4.3.0/gcc-4.8.5+hdf5_serial-1.8.13/lib -L/opt/apps/netcdf++/lib -L/opt/gridware/depots/8168fdff/el7/pkg/libs/boost/1.60.0/gcc-4.8.5+openmpi-1.8.5+python-2.7.8/lib -L/opt/apps/libxml++/lib -L/opt/apps/repast/lib
LIBS      = -lpython2.7 -lboost_system -lboost_mpi -lboost_serialization -lboost_filesystem -lrepast_hpc-2.2.0 -lnetcdf_c++ -lxml++-3.0 -lxml2 -lglibmm-2.4 -lgobject-2.0 -lglib-2.0 -lsigc-2.0
INC_XML   = -I/opt//gridware/depots/8168fdff/el7/pkg/mpi/openmpi/1.10.2/gcc-4.8.5/include/ -I/opt//gridware/depots/8168fdff/el7/pkg/libs/boost/1.60.0/gcc-4.8.5+openmpi-1.8.5+python-2.7.8/include/ -I/home/alces/repast_hpc-2.2.0/src -I/opt/apps/libxml++/include/libxml++-3.0/ -I/usr/include/libxml2 -I/usr/include/glibmm-2.4 -I/usr/lib64/glibmm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/sigc++-2.0 -I/usr/lib64/sigc++-2.0/include

debug : all
profile_use : all
profile_gen : all
all : $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LIBS) $(LDFLAGS) -o $(BIN_DIR)$(EXEC_NAME)

%.o : %.cpp ../include/%.hpp
	$(CXX) $(CXXFLAGS) $(INC_XML) -o $@ -c $<

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INC_XML) -o $@ -c $<
