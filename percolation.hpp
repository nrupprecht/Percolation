#ifndef __PERCOLATION_HPP__
#define __PERCOLATION_HPP__

#include "EasyBMP/EasyBMP.h"

#include "Utility.hpp"

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

  // Data processing
  void createImage();
  void siftLargest();
  void getBorder();

  // Accessors
  double getProbability() { return probability; }
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
  void setTransition(bool t) { transition = t; }
  void setPMin(double);
  void setPMax(double);

private:
  // Private helper functions
  void sitePercolation();
  void bondPercolation();
  inline void unite();
  inline void cluster();
  inline int getHead(int);

  int *lattice;

  PType type;
  bool transition;
  double pmin, pmax;
  float probability;
  int width, height;

  BMP image;
  bool doBMP;

  // Statistics and data
  vector<int> sizeRecord;         // Size, indexed by cluster number
  map<int, int> sizeDistribution; // Cluster size vs. # clusters
  int maxCluster, numClusters;

  double time;
};

#endif // __PERCOLATION_HPP__
