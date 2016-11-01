Cache Simulator Assignment

--------------------------


In this assignment you will design a basic cache simulator.


Cache specifications are passed from command line. 
The specifications include
cache size, associativity and block size. 
Both cache size and block size are
specified in bytes. 
Along with cache specifications, an input files is passed
that has read and write commands with associated address and data values in hex
format. 


I have provided a top level file 'cacheSim.cc' that reads command line
parameters, parses the inputs and calls appropriate methods of your base cache to
perform required operations. 
I have also provided BaseCache class skeleton,
that you will fill to complete. 
Feel free to add any additional class variables
or methods as per your design requirements. 
For your ease, I have labelled all
the methods, variables that you need to write with 'WRITE ME'.


Use Makefile provided to compile the assignment. 


Command to run the cache simulator (for 512B cache, associativity 2 and 
block size of 16bytes) is :

	./cacheSim 512 2 16 input1.txt

Sample input and output files are provided. 
Output files are generated for above
cache specifications.


You will implement LRU policy for cache line replacement for read and write
miss. 
For a write miss you write data to LRU cache line and update its tag. 
Upon
read miss, typically we fetch data from lower cache levels or main memory and
replace the LRU cache line with new data. 
However, for this part of the 
assignment as we don't ask you to implement cache hierarchy, update only tag of
the LRU cache line and leave the data untouched. 


Assumptions:

1. Writes and reads are always 32bits long.

2. Block Size is always multiple of 4 bytes.

3. Read and Write addresses are 32bit aligned. This removes the necessity to
check for any partial data availability.
