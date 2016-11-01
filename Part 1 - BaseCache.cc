#include <iostream>
#include "BaseCache.h"
#include <math.h>
using namespace std;

//WRITE ME
//Default constructor to set everything to '0'
BaseCache::BaseCache() {
 
  //cache base parameters
  cacheSize=0;
  associativity=0;
  blockSize=0;
  cacheLines=NULL;

  //cache derived parameters
  numSets=0;
  numOffsets=0;
  TagBits=0;
  IndexBits=0;
  OffsetBits=0;

  //cache access statistics
  numReads=0;
  numWrites=0;
  numReadHits=0;
  numReadMisses=0;
  numWriteHits=0;
  numWriteMisses=0;
}

//WRITE ME
//Constructor to initialize cache parameters, create the cache and clears it
BaseCache::BaseCache(uint32_t _cacheSize, uint32_t _associativity, uint32_t _blockSize) {
  cacheSize=_cacheSize;
  associativity=_associativity;
  blockSize=_blockSize;
  //Initialize cache derived parameters
  initDerivedParams();
  //Reset cache access statistics
  resetStats();
  //Create cache and clear it
  createCache();
  //Reset cache
  clearCache();
}

//WRITE ME
//Set cache base parameters
void BaseCache::setCacheSize(uint32_t _cacheSize) {
  cacheSize=_cacheSize;
}
void BaseCache::setAssociativity(uint32_t _associativity) {
  associativity=_associativity;
}
void BaseCache::setBlockSize(uint32_t _blockSize) {
  blockSize=_blockSize;
}

//WRITE ME
//Get cache base parameters
uint32_t BaseCache::getCacheSize(){
  return cacheSize;
}
uint32_t BaseCache::getAssociativity(){
  return associativity;
}
uint32_t BaseCache::getBlockSize(){
  return blockSize;
}

//WRITE ME
//Get cache access statistics
uint32_t BaseCache::getReadHits(){
  return numReadHits;
}
uint32_t BaseCache::getReadMisses(){
  return numReadMisses;
}
uint32_t BaseCache::getWriteHits(){
  return numWriteHits;
}
uint32_t BaseCache::getWriteMisses(){
  return numWriteMisses;
}
double BaseCache::getReadHitRate(){
  return 100*numReadHits/numReads;  
}
double BaseCache::getReadMissRate(){
  return 100*numReadMisses/numReads;  
}
double BaseCache::getWriteHitRate(){
  return 100*numWriteHits/numWrites;
}
double BaseCache::getWriteMissRate(){
  return 100*numWriteMisses/numWrites;
}
double BaseCache::getOverallHitRate(){
  return 100*(numReadHits+numWriteHits)/(numReads+numWrites);
}
double BaseCache::getOverallMissRate(){
  return 100*(numReadMisses+numWriteMisses)/(numReads+numWrites);
}

//WRITE ME
//Initialize cache derived parameters
void BaseCache::initDerivedParams() {
 numSets=cacheSize/blockSize/associativity;
 numBlocks=cacheSize/blockSize;
 numOffsets=blockSize;
 IndexBits=log(cacheSize/blockSize/associativity)/log(2);
 OffsetBits=log(blockSize)/log(2);
 TagBits=ADDR_BITS-IndexBits-OffsetBits;
}


//WRITE ME
//Reset cache access statistics
void BaseCache::resetStats() {
  numReads=0;
  numWrites=0;
  numReadHits=0;
  numReadMisses=0;
  numWriteHits=0;
  numWriteMisses=0;
}

//WRITE ME
//Create cache and clear it
void BaseCache::createCache() {
  uint32_t i,j,k;
  cacheLines=new cacheLine*[numSets];
  for(i=0;i<numSets;i++)
    {
      cacheLines[i]=new cacheLine[associativity];
      for(j=0;j<associativity;j++)
	{
	  cacheLines[i][j].data=new uint32_t[numOffsets];
	  for(k=0;k<numOffsets;k++)
	    {
	      cacheLines[i][j].data[k]=0;
	      cacheLines[i][j].tag=0;
	      cacheLines[i][j].LRUStackBits=0;
	    }
	}
    }
}

//WRITE ME
//Reset cache
void BaseCache::clearCache() {
  uint32_t i,j,k;
  for(i=0;i<numSets;i++)
    {
      for(j=0;j<associativity;j++)
	{
	  for(k=0;k<numOffsets;k++)
	    {
	      cacheLines[i][j].data[k]=0;
	      cacheLines[i][j].tag=0;
	      cacheLines[i][j].LRUStackBits=0;
	    }
	}
    }
  numReads=0;
  numWrites=0;
  numReadHits=0;
  numReadMisses=0;
  numWriteHits=0;
  numWriteMisses=0;
}

//WRITE ME
//Read data
//return true if it was a hit, false if it was a miss
//data is only valid if it was a hit, input data pointer
//is not updated upon miss. Make sure to update LRU stack
//bits. You can choose to separate the LRU bits update into
// a separate function that can be used from both read() and write().
bool BaseCache::read(uint32_t addr, uint32_t *data) {
 numReads++;
 uint32_t tagAddr=addr>>(ADDR_BITS-TagBits);
 uint32_t indexAddr=(addr<<TagBits)>>(TagBits+OffsetBits);
 if (IndexBits == 0)
   {
     indexAddr = 0;
   }
 uint32_t offsetAddr=(addr<<(ADDR_BITS-OffsetBits))>>(ADDR_BITS-OffsetBits);

 uint32_t r,max,LRU;
 max=LRUMAX(indexAddr);
 for(r=0;r<associativity;r++)
   {
     if(cacheLines[indexAddr][r].tag==tagAddr)
       {
	 numReadHits++;
	 *data=cacheLines[indexAddr][r].data[offsetAddr];
	 cacheLines[indexAddr][r].LRUStackBits=max+1;
	 return true;
       }
  }
 LRU=0;
 for(r=0;r<associativity;r++)
   {
     if(cacheLines[indexAddr][r].LRUStackBits<cacheLines[indexAddr][LRU].LRUStackBits)
      { 
	LRU=r; 
      }
   }
  numReadMisses++;
  cacheLines[indexAddr][LRU].tag=tagAddr;
  cacheLines[indexAddr][LRU].LRUStackBits=max+1;
  return false;
}

//WRITE ME
//Write data
//Function returns write hit or miss status. 
bool BaseCache::write(uint32_t addr, uint32_t data) {
  numWrites++;
  uint32_t tagAddr=addr>>(ADDR_BITS-TagBits);
  uint32_t indexAddr=(addr<<TagBits)>>(TagBits+OffsetBits);
  if (IndexBits == 0)
    {
      indexAddr = 0;
    }
  uint32_t offsetAddr=(addr<<(ADDR_BITS-OffsetBits))>>(ADDR_BITS-OffsetBits);

  uint32_t w, max, LRU;
  max=LRUMAX(indexAddr);
  for(w=0;w<associativity;w++)
    {
      if(cacheLines[indexAddr][w].tag==tagAddr)
	{
	  numWriteHits++;
	  cacheLines[indexAddr][w].data[offsetAddr]=data;
	  cacheLines[indexAddr][w].LRUStackBits=max+1;
	  return true;
	}
    }
  LRU=0;
  for(w=0; w<associativity; w++)
  {
    if(cacheLines[indexAddr][w].LRUStackBits<cacheLines[indexAddr][LRU].LRUStackBits)
      { 
	LRU=w;
      }
  }
  numWriteMisses++;
  cacheLines[indexAddr][LRU].data[offsetAddr]=data;
  cacheLines[indexAddr][LRU].tag=tagAddr;
  cacheLines[indexAddr][LRU].LRUStackBits=max+1;
  return false;
}

uint32_t BaseCache::LRUMAX(uint32_t set){
  uint32_t l;
  uint32_t max=0;
  for(l=0; l<associativity; l++)
    {
      if(cacheLines[set][l].LRUStackBits>max)
	{
	  max=cacheLines[set][l].LRUStackBits;
	}
    }
  return max;
}

 // uint32_t BaseCache::LRUMIN(uint32_t set) {
 //   uint32_t min = 0;
 //   uint32_t l;
 //   for (l = 0; l < associativity; l++)
 //     {
 //       if (cacheLines[set][l].LRUStackBits < cacheLines[set][min].LRUStackBits)
 // 	{
 // 	  min = l;
 // 	}
 //     }
 //   return min;
 // }

//WRITE ME
//Destructor to free all allocated memeroy.

BaseCache::~BaseCache() 
{
  for(uint32_t i=0;i<numSets;i++){
    for(uint32_t j=0;j<associativity;j++){
      delete[] cacheLines[i][j].data;
    }
  }
  for(uint32_t i=0;i<numSets;i++){
    delete[] cacheLines[i];
  }
  delete[] cacheLines;
}
