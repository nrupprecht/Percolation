#include "percolation.hpp"

Percolation::Percolation() : lattice(0), type(SITE), probability(0.5), width(100), height(100), doBMP(true), time(0), maxCluster(0), numClusters(0) {};

Percolation::Percolation(int w, int h) : lattice(0), type(SITE), probability(0.5), width(w), height(h), doBMP(true), time(0), maxCluster(0), numClusters(0) {};

Percolation::~Percolation() {
  if (lattice) delete [] lattice;
}

void Percolation::percolate() {
  auto start = high_resolution_clock::now();
  switch (type) {
  default:
  case SITE:
    sitePercolation();
    break;
  case BOND:
    bondPercolation();
    break;
  }
  auto end = high_resolution_clock::now();
  duration<double> span = duration_cast<duration<double>>(end-start);
  time = span.count();
}

double Percolation::getAveClusterSize() {
  double ave;
  for (auto s : sizeDistribution)
    ave += s.first*s.second;
  return ave / numClusters;
}

double Percolation::getAveFiniteClusterSize() {
  double ave;
  for (auto s : sizeDistribution)
    if (s.first!=maxCluster) 
      ave += s.first*s.second;
  return ave / numClusters;
}

void Percolation::setDims(int w, int h) {
  width = w; height = h;
}

void Percolation::setProbability(float p) {
  probability = p;
}

void Percolation::sitePercolation() {
  if (lattice) delete [] lattice;
  // Allocate a new array
  lattice = new int[width*height];
  // Set sites
  for (int y=0; y<height; ++y)
    for (int x=0; x<width; ++x) {
      if (drand48()<=probability) lattice[y*width+x] = y*width+x;
      else lattice[y*width+x] = -1;
    }
  // Group sites
  unite();
  // Assign cluster numbers
  if (doBMP) image.SetSize(width, height);
  vector<int> colors; // Colors for the clusters
  auto getColor = [&] (int i) {
    int R = colors.at(i) % 256;
    int G = (colors.at(i)/256) % 256;
    int B = colors.at(i)/(256*256);
    return RGBApixel(R, G, B);
  };
  set<int> indices;
  int clusters = 0;
  map<int, int> dictionary; // First int is the head #, second is the cluster #
  sizeRecord.clear();       // Clear out the size record
  for (int y=0; y<height; ++y)
    for (int x=0; x<width; ++x) {
      int head = lattice[y*width+x];
      if (head<0) { // Skip over index == -1
	if (doBMP) image.SetPixel(x, y, RGBApixel(0,0,0));
	continue; 
      }
      auto iter = dictionary.find(head);
      if (iter==dictionary.end()) { // First time encountering this cluster #
	dictionary.insert(pair<int,int>(head, clusters));
	++clusters;
	sizeRecord.push_back(1);
	if (doBMP) {
	  colors.push_back((int)(drand48()*256)*65536 + (int)(drand48()*256)*256 + (int)(drand48()*256));
	  image.SetPixel(x, y, getColor(clusters-1));
	}
      }
      else { // The cluster # for the head # is iter->second
	lattice[y*width+x] = iter->second;
	++sizeRecord.at(iter->second);
	if (doBMP) image.SetPixel(x, y, getColor(iter->second));
      }
    }
  // Record number of clusters
  numClusters = sizeRecord.size();
  // Get the size distribution
  sizeDistribution.clear(); // { Cluster size, number of such clusters }
  auto end = sizeDistribution.end();
  for (const auto size : sizeRecord) {
    auto iter = sizeDistribution.find(size);
    if (iter!=end) ++iter->second;
    else sizeDistribution.insert(pair<int,int>(size,1));
  }
  // Find largest cluster sizes
  maxCluster = sizeDistribution.rbegin()->first;
}

void Percolation::bondPercolation() {

}

inline void Percolation::unite() { 
  // Find out which volumes are connected
  for (int y=0; y<height; ++y)
    for (int x=0; x<width; ++x) {
      // Check the sectors around you, point yourself to the largest head
      int h0=-1, h1=-1, h2=-1, h3=-1;
      if (lattice[width*y+x]!=-1) {
	int head = getHead(width*y+x);
	if (0<x && -1<lattice[width*y+x-1]) { // Left
	  h0 = getHead(width*y+(x-1));
	  if (h0<head) head = h0;
	}
	if (x+1<width && -1<lattice[width*y+x+1]) { // Right
	  h1 = getHead(width*y+(x+1));
	  if (h1<head) head = h1;
	}
	if (0<y && -1<lattice[width*(y-1)+x]) { // Bottom
	  h2 = getHead(width*(y-1)+x);
	  if (h2<head) head = h2;
	}
	if (y+1<height && -1<lattice[width*(y+1)+x]) { // Top
	  h3 = getHead(width*(y+1)+x);
	  if (h3<head) head = h3;
	}
	// Set your head and the heads of all your neighbors as well
	lattice[width*y+x] = head;
	if (-1<h0) lattice[h0] = head;
	if (-1<h1) lattice[h1] = head;
	if (-1<h2) lattice[h2] = head;
	if (-1<h3) lattice[h3] = head;
      }
    }
  // Set each lattice to point to its head
  for (int y=0; y<height; ++y)
    for (int x=0; x<width; ++x)
      lattice[y*width+x] = getHead(lattice[y*width+x]);
}

inline int Percolation::getHead(int index) {
  if (index<0) return -1;
  while (lattice[index]!=index && -1<lattice[index]) index = lattice[index];
  return index;
}
