#!/bin/bash -l
#$ -N influenza -wd $HOME/influenza -pe mpislots 16 -o $HOME/influenza/output/output.$JOB_NAME.$JOB_ID
module load mpi/openmpi/1.10.2/gcc-4.8.5
module load libs/boost/1.60.0/gcc-4.8.5+openmpi-1.8.5+python-2.7.8
module load libs/netcdf/4.3.0/gcc-4.8.5+hdf5_serial-1.8.13
module load apps/python/2.7.8/gcc-4.8.5
module load  /opt/apps/etc/modules/libxml++
module load /opt/apps/etc/modules/netcdf++
module load /opt/apps/etc/modules/repast
module list
cd $HOME/influenza/bin/
mpirun influenza config.props model.props
