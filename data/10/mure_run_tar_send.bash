#!/bin/bash

line=$(cat $1)
# Setup
source load.bash
source ${home}/std_setup.bash
source ${home}/install_mcnp_mure_data_PSPEC.bash
cd geom

# compilation
g++  -o MOX MOX.cc -I$MURE_include -I$MURE_ExternalPkg -L$MURE_lib -lMUREpkg -lvalerr -lmctal -fopenmp

# run
./MOX $line

# taring
tar -zcf ${1}.tar.gz U5*.dat U5*.log U5*.info
rm U5*.dat U5*.log U5*.info
tar -zcf ${1}_raw.tar.gz U5*

# sending to box
cp $home/.netrc ~/
curl -1 -v --disable-epsv --ftp-skip-pasv-ip --ftp-ssl --upload-file ${1}_raw.tar.gz  -netrc ftp://ftp.box.com/MODEL_DATA/PWR_TRU_MOX_PSPEC/10/${1}_raw.tar.gz
rm -rf ~/.netrc

# backup the rest
mv ${1}.tar.gz ${home}/
cd ${home}
rm -rf *.bash tmp
