#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "BaseCache.h"
using namespace std;

int main(int argc, char **argv) {
    string inp_line, parsed_line;
    string command;
    unsigned int address, data;
    uint32_t cacheSizeL1, associativityL1, blockSizeL1;
    uint32_t cacheSizeL2, associativityL2, blockSizeL2;
    cacheSizeL1 = atoi(argv[1]);
    associativityL1 = atoi(argv[2]);
    blockSizeL1 = atoi(argv[3]);
    cacheSizeL2 = atoi(argv[4]);
    associativityL2 = atoi(argv[5]);
    blockSizeL2 = atoi(argv[6]);
    ifstream fp_inp(argv[7]);
    ifstream fp_mem("memfoot.dat");

    //Create L1 Cache L2 Cache and Main Memory
    BaseCache BaseCacheL1(cacheSizeL1, associativityL1, blockSizeL1);
    BaseCache BaseCacheL2(cacheSizeL2, associativityL2, blockSizeL2);
    uint32_t *MainMemory = new uint32_t [1024*1024]; //Size: 1MB

    //Access Time
    uint32_t tmin = 1024*1024;
    uint32_t tmax = 0; 
    uint32_t taccess = 0; 
    uint32_t naccess = 0; 

    //Memory foot point
    if(fp_mem.is_open()) {
        while(getline(fp_mem, inp_line)) {
            stringstream inp_string(inp_line);
	    if(getline(inp_string,parsed_line, '\t')) 
		address = stoul(parsed_line, NULL, 16);
	    if(getline(inp_string,parsed_line, ' ')) {
	        data = stoul(parsed_line, NULL, 16);
		}
	    MainMemory[address] = data;
	    }
	fp_mem.close();
    }

    //File point
    if(fp_inp.is_open()) {
        while(getline(fp_inp, inp_line)) {
            stringstream inp_string(inp_line);
    	    if(getline(inp_string,parsed_line, ' ')) 
		command = parsed_line;
	    if(getline(inp_string,parsed_line, ' ')) 
		address = stoul(parsed_line, NULL, 16);
	    if (!(command.compare("w"))) {
	    	if(getline(inp_string,parsed_line, ' ')) {
		    data = stoul(parsed_line, NULL, 16);
		}
	    }
	   
	    //Issue read/write command
	    if (!(command.compare("w"))) { 
	      if(BaseCacheL1.write(address, data, 0)) {
	      } if(BaseCacheL2.write(address, data, 0)) {
	        }
	      MainMemory[address]=data;
	     }

	    if (!(command.compare("r"))) { 
	        if(BaseCacheL1.read(address, &data)) {

		  tmin=tmin>1?1:tmin;
		  tmax=tmax<1?1:tmax;
		  taccess = taccess + 1;
		  naccess = naccess + 1;
	        } else if(BaseCacheL2.read(address, &data)) {
		  BaseCacheL1.write(address, data, 1);
		  tmin=tmin>16?16:tmin;
		  tmax=tmax<16?16:tmax;
		  taccess = taccess + 16;
		  naccess = naccess + 1;
	        } else {
		  BaseCacheL1.write(address, MainMemory[address], 1);	  
		  BaseCacheL2.write(address, MainMemory[address], 1);
		  tmin=tmin>86?86:tmin;
		  tmax=tmax<86?86:tmax;
		  taccess = taccess + 86;
		  naccess = naccess + 1;
	        }
	    }
        }
        fp_inp.close();
    }

    cout <<endl;
    cout << "L1 Read Hits (HitRate): "<<BaseCacheL1.getReadHits()<<" ("<<BaseCacheL1.getReadHitRate()<<"%)"<<endl;
    cout << "L1 Read Misses (MissRate): "<<BaseCacheL1.getReadMisses() <<" ("<<BaseCacheL1.getReadMissRate()<<"%)"<<endl;
    cout << "L1 Write Hits (HitRate): "<<BaseCacheL1.getWriteHits()<<" ("<<BaseCacheL1.getWriteHitRate()<<"%)"<<endl;
    cout << "L1 Write Misses (MissRate): "<<BaseCacheL1.getWriteMisses() <<" ("<<BaseCacheL1.getWriteMissRate()<<"%)"<<endl;
    cout << "L1 Overall Hit Rate: "<<BaseCacheL1.getOverallHitRate() <<"%" << endl;
    cout << "L1 Overall Miss Rate: "<<BaseCacheL1.getOverallMissRate()<<"%"<<endl;

    cout <<endl;
    cout << "L2 Read Hits (HitRate): "<<BaseCacheL2.getReadHits()<<" ("<<BaseCacheL2.getReadHitRate()<<"%)"<<endl;
    cout << "L2 Read Misses (MissRate): "<<BaseCacheL2.getReadMisses() <<" ("<<BaseCacheL2.getReadMissRate()<<"%)"<<endl;
    cout << "L2 Write Hits (HitRate): "<<BaseCacheL2.getWriteHits()<<" ("<<BaseCacheL2.getWriteHitRate()<<"%)"<<endl;
    cout << "L2 Write Misses (MissRate): "<<BaseCacheL2.getWriteMisses() <<" ("<<BaseCacheL2.getWriteMissRate()<<"%)"<<endl;
    cout << "L2 Overall Hit Rate: "<<BaseCacheL2.getOverallHitRate() <<"%" << endl;
    cout << "L2 Overall Miss Rate: "<<BaseCacheL2.getOverallMissRate()<<"%"<<endl;

    cout <<endl;
    if(naccess == 0)
      {
	naccess=1;
      }
    uint32_t tavg = taccess/naccess;
    cout <<"Average memory access time (AMAT) (Reads): "<<tavg<<"ns"<<endl;
    cout <<"Minimum access time: "<<tmin<<"ns"<<endl;
    cout <<"Maximum access time: "<<tmax<<"ns"<<endl;
    delete[] MainMemory;
    return 1;
}

void BaseCache::update(uint32_t addr, uint32_t data, uint32_t full)
{
  uint32_t LRU=0;
  uint32_t w=0;
  uint32_t max=0;
  uint32_t tagAddr=0;
  uint32_t indexAddr=0;
  uint32_t offsetAddr=0;
  if (full == 1)
    {
      LRU=0;
      w=0;
      while(w<associativity)
	{
	  if(cacheLines[indexAddr][w].LRUStackBits<cacheLines[indexAddr][LRU].LRUStackBits)
	    { 
	      LRU=w;
	    }
	  w++;
	}
      cacheLines[indexAddr][LRU].data[offsetAddr]=data;
      cacheLines[indexAddr][LRU].tag=tagAddr;
      cacheLines[indexAddr][LRU].LRUStackBits=max+1;
      numWriteMisses--;
    }
}
