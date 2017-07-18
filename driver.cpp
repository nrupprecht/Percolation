// 7/13/2017

#include "percolation.hpp"

int main(int argc, char** argv) {
  float probability = 0.4;
  int width = 100;
  int height = 100;
  int size = -1;
  bool bmp = true;
  bool transition = false;
  double pmin = 0;
  double pmax = 1;

  // Seed rand
  srand48( std::time(0) );

  // Parse command line arguments
  ArgParse parser(argc, argv);
  parser.get("prob", probability);
  parser.get("width", width);
  parser.get("height", height);
  parser.get("size", size);
  parser.get("bmp", bmp);
  parser.get("transition", transition);
  parser.get("pmin", pmin);
  parser.get("pmax", pmax);
  // Check for illegal tokens
  try {
    parser.check();
  }
  catch (ArgParse::UncheckedToken illegal) {
    cout << "Illegal option: [" << illegal.token << "]. Exiting.\n";
    exit(1);
  }

  // Create percolation
  Percolation percolation;
  percolation.setTransition(transition);
  percolation.setPMin(pmin);
  percolation.setPMax(pmax);
  percolation.setProbability(probability);
  if (size>0) width = height = size;
  percolation.setDims(width, height);
  percolation.setDoBMP(bmp);
  // Do the percolation
  percolation.percolate();
  
  //percolation.siftLargest();
  percolation.getBorder();

  // Print how long it took
  cout << "Time: " << percolation.getTime() << endl;

  // Print what percent of the total the maximum cluster takes up
  cout << "Max: " << percolation.getMaxPercent() << endl;

  // Print the image
  if (bmp) {
    percolation.createImage();
    BMP &image = percolation.getImage();
    image.WriteToFile("image.bmp");
  }

  // Get and print the size record
  vector<int> &sizeRecord = percolation.getSizeRecord();
  writeToFile("size.csv", sizeRecord);

  // Get and print size distribution record
  auto &sizeDistribution = percolation.getSizeDistribution();
  writeToFile("distribution.csv", sizeDistribution);

  // Print dimensions
  vector<int> dims(2);
  dims.at(0) = width; dims.at(1) = height;
  writeToFile("dims.csv", dims);

  return 0;
}
