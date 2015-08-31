#!/bin/bash

cd ./bin/
mpirun -np $1 ./influenza config.props model.props
cd ..
