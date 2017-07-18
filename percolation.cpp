#include "percolation.hpp"

Percolation::Percolation() : lattice(nullptr), type(SITE), transition(false), pmin(0), pmax(1), probability(0.5), width(100), height(100), doBMP(true), time(0), maxCluster(0), numClusters(0) {};

Percolation::Percolation(int w, int h) : lattice(nullptr), type(SITE), transition(false), pmin(0), pmax(1), probability(0.5), width(w), height(h), doBMP(true), time(0), maxCluster(0), numClusters(0) {};

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

void Percolation::createImage() {
  // Create array of random colors
  vector<int> colors(numClusters+1);
  for (auto &c : colors) {
    int r = 200*drand48()+55, g=200*drand48()+55, b=200*drand48()+55;
    c = 256*256*r+256*g+b;
  }
  // Set image size
  image.SetSize(width, height);
  // Create a lambda for fetching a pixel color
  auto getColor = [&] (int i) {
    int c = colors.at(i);
    int R = c % 256;
    int G = (c/256) % 256;
    int B = c/(256*256);
    return RGBApixel(R, G, B);
  };
  // Set the image colors
  for (int y=0; y<height; ++y)
    for (int x=0; x<width; ++x) {
      int cluster = lattice[y*width+x];
      if (cluster<0) image.SetPixel(x, y, RGBApixel(0,0,0));
      else image.SetPixel(x, y, getColor(cluster)); 
    }
}

void Percolation::siftLargest() {
  // Remove all clusters except the largest
  for (int y=0; y<height; ++y)
    for (int x=0; x<width; ++x) {
      int cluster = lattice[y*width+x];
      if (cluster<0) continue;
      if (sizeRecord.at(cluster)!=maxCluster) 
	lattice[y*width+x]=-1;
      else lattice[y*width+x]=0; // Set the max cluster to be the zero cluster
    }
}

void Percolation::getBorder() {
  if (lattice==nullptr) return;
  // Remove all clusters except the largest
  siftLargest();
  // Initialization
  if (lattice[0]==-1) lattice[0]=1;
  // Remove largest cluster
  for (int y=height-1; y>0; --y)
    for (int x=0; x<width; ++x) {
      int cluster = lattice[y*width+x];
      if (cluster<0) {
	// Do some early clustering so unite takes less time
	if (x>0 && lattice[y*width+x-1]>0) 
	  lattice[y*width+x] = lattice[y*width+x-1];
	else if (y<height && lattice[(y+1)*width+x]>0) 
	  lattice[y*width+x] = lattice[(y+1)*width+x];
	else
	  lattice[y*width+x] = y*width+x+1;
      }
      else lattice[y*width+x] = -1;
    }
  // Unite regions
  unite();
  // Exchange head numbers for cluster numbers
  cluster();
  // Extract the largest cluster
  siftLargest();
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

void Percolation::setPMin(double p) {
  pmin = max(0., min(p, 1.));
}

void Percolation::setPMax(double p) {
  pmax = min(1., max(p, 0.));
}

void Percolation::sitePercolation() {
  if (lattice) delete [] lattice;
  // Allocate a new array
  lattice = new int[width*height];
  // Set sites
  double prob = probability;
  for (int y=0; y<height; ++y) {
    if (transition) prob = ((double)y/height*(pmax-pmin)+pmin);
    for (int x=0; x<width; ++x) {
      if (drand48()<=prob) lattice[y*width+x] = y*width+x;
      else lattice[y*width+x] = -1;
    }
  }
  // Group sites together
  unite();
  // Assign cluster numbers
  cluster();
  /*
  int clusters = 0;
  map<int, int> dictionary; // First int is the head #, second is the cluster #
  sizeRecord.clear();
  // Replace head numbers with cluster numbers
  for (int y=0; y<height; ++y)
    for (int x=0; x<width; ++x) {
      int head = lattice[y*width+x];
      if (head<0) { // Skip over index == -1
	if (doBMP) image.SetPixel(x, y, RGBApixel(0,0,0));
	continue; 
      }
      auto iter = dictionary.find(head);
      // First time encountering this cluster #
      if (iter==dictionary.end()) { 
	// Assign a cluster number
	dictionary.insert(pair<int,int>(head, clusters));
	lattice[y*width+x] = clusters; 
	// Create a size entry
	sizeRecord.push_back(1);
	// Increment the cluster number
	++clusters;
      }
      // Not the first time, the cluster # for the head # is iter->second
      else {
	lattice[y*width+x] = iter->second;
	++sizeRecord.at(iter->second);
      }
    }
  // Count the number of clusters
  numClusters = clusters;

  // Get the size distribution
  sizeDistribution.clear(); // { Cluster size, number of such clusters }
  auto end = sizeDistribution.end();
  for (const auto size : sizeRecord) {
    auto iter = sizeDistribution.find(size);
    if (iter!=end) ++iter->second;
    else sizeDistribution.insert(pair<int,int>(size,1));
  }
  
  // Find largest cluster sizes
  if (!sizeDistribution.empty()) maxCluster = sizeDistribution.rbegin()->first;
  else maxCluster = 0;
  */
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

inline void Percolation::cluster() {
  int clusters = 0;
  map<int, int> dictionary; // First int is the head #, second is the cluster #
  sizeRecord.clear();
  // Replace head numbers with cluster numbers
  for (int y=0; y<height; ++y)
    for (int x=0; x<width; ++x) {
      int head = lattice[y*width+x];
// Skip over index == -1
      if (head<0) continue;
      auto iter = dictionary.find(head);
      // First time encountering this cluster #
      if (iter==dictionary.end()) {
        // Assign a cluster number
        dictionary.insert(pair<int,int>(head, clusters));
        lattice[y*width+x] = clusters;
        // Create a size entry
        sizeRecord.push_back(1);
        // Increment the cluster number
        ++clusters;
      }
      // Not the first time, the cluster # for the head # is iter->second
      else {
        lattice[y*width+x] = iter->second;
        ++sizeRecord.at(iter->second);
      }
    }
  // Count the number of clusters
  numClusters = clusters;
  // Get the size distribution
  sizeDistribution.clear(); // { Cluster size, number of such clusters }
  auto end = sizeDistribution.end();
  for (const auto size : sizeRecord) {
    auto iter = sizeDistribution.find(size);
    if (iter!=end) ++iter->second;
    else sizeDistribution.insert(pair<int,int>(size,1));
  }
  // Find largest cluster sizes
  if (!sizeDistribution.empty()) maxCluster = sizeDistribution.rbegin()->first;
  else maxCluster = 0;
}

inline int Percolation::getHead(int index) {
  if (index<0) return -1;
  while (lattice[index]!=index && -1<lattice[index]) index = lattice[index];
  return index;
}
