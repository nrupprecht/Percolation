#ifndef __PERCOLATION_HPP__
#define __PERCOLATION_HPP__

#include "EasyBMP/EasyBMP.h"

#include <chrono>
using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::duration_cast;

#include <iostream>
using std::cout;
using std::endl;


#include <vector>
using std::vector;
using std::pair;

#include <map>
using std::map;

#include <set>
using std::set;

enum PType { SITE, BOND };

class Percolation {
public:
  // Constructors
  Percolation();
  Percolation(int, int);
  ~Percolation();

  // Commands
  void percolate();

  // Accessors
  BMP& getImage() { return image; }
  vector<int>& getSizeRecord() { return sizeRecord; }
  map<int, int>& getSizeDistribution() { return sizeDistribution; }
  double getTime() { return time; }
  int getMaxCluster() { return maxCluster; }
  double getMaxPercent() { return (double)maxCluster/(probability*width*height); }
  double getAveClusterSize();
  double getAveFiniteClusterSize();

  // Mutators
  void setDims(int, int);
  void setProbability(float);
  void setDoBMP(bool b) { doBMP = b; }

private:
  // Private helper functions
  void sitePercolation();
  void bondPercolation();
  inline void unite();
  inline int getHead(int);

  int *lattice;

  PType type;
  float probability;
  int width, height;

  BMP image;
  bool doBMP;

  vector<int> sizeRecord;
  map<int, int> sizeDistribution;

  double time;

  int maxCluster, numClusters;
};

#endif // __PERCOLATION_HPP__
