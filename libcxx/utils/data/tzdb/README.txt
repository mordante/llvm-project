Contains parts of the IANA time zone data base.


To update all files to the last published database issue the following commands
in the directory containing this file.

wget https://data.iana.org/time-zones/tzdata-latest.tar.gz 
tar xf tzdata-latest.tar.gz -C data

Note that not all files are shipped with libc++


xxx
