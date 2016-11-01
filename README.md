# Cache Simulator
####Part 1
Design a basic cache simulator. <br><br>
Cache specifications are passed from command line. <br>
The specifications include cache size, associativity and block size. <br>
Both cache size and block size are specified in bytes. <br><br>
Along with cache specifications, an input files is passed that has read and write commands with associated address and data values in hex format. <br><br>
Implement LRU policy for cache line replacement for read and write
miss. <br>
For a write miss you write data to LRU cache line and update its tag. <br>
Upon read miss, update only tag of the LRU cache line and leave the data untouched. <br><br>

####Part 2
Implement following new features:
<br>    1) Memory hierarchy (Two cache levels: L1 and L2, and a main memory)
<br>    2) Cache timing statistics 
<br>    3) Eviction policy for read misses and writes (write-through)
<br>    4) No-Write Allocate policy for write misses

####Check README file for more details.
