// 7/13/2017

#include "percolation.hpp"

int main(int argc, char** argv) {
  // Parameters
  int width = 1000, height = 1000;
  int size = -1;
  int partition = 10, rep = 5;
  double minP = 0.55;
  double maxP = 0.65;

  // Seed rand
  srand48( std::time(0) );

  ArgParse parser(argc, argv);
  parser.get("width", width);
  parser.get("height", height);
  parser.get("size", size);
  parser.get("maxP", maxP);
  parser.get("minP", minP);
  parser.get("partition", partition);
  parser.get("rep", rep);
  try {
    parser.check();
  }
  catch (ArgParse::UncheckedToken illegal) {
    cout << "Illegal option: [" << illegal.token << "]. Exiting.\n";
    exit(1);
  }

  // Reset width and height
  if (size>0) width = height = size;

  // Create a percolation
  Percolation percolation;
  percolation.setDoBMP(false);
  percolation.setDims(width, height);

  // Timing
  auto start = high_resolution_clock::now();
  // Store data
  vector<pair<double, double> > maxPercent, aveCluster, aveFiniteCluster;
  // Do trials
  for (int i=0; i<partition; ++i) {
    double max = 0, ave = 0, aveF = 0, prob = i*(maxP-minP)/partition + minP;
    percolation.setProbability(prob);
    for (int j=0; j<rep; ++j) {
      percolation.percolate();
      max += percolation.getMaxPercent();
      ave += percolation.getAveClusterSize();
      aveF += percolation.getAveFiniteClusterSize();
    }
    max /= rep;
    ave /= rep;
    aveF /= rep;
    maxPercent.push_back(pair<double, double>(prob, max));
    aveCluster.push_back(pair<double, double>(prob, ave));
    aveFiniteCluster.push_back(pair<double, double>(prob, aveF));
  }
  // Timing
  auto end = high_resolution_clock::now();
  duration<double> span = duration_cast<duration<double>>(end-start);
  double time = span.count();

  // Print data
  cout << "Partitions:  " << partition << "\n";
  cout << "Repetitions: " << rep << "\n";
  cout << "Dimensions:  " << width << "x" << height << "\n";
  cout << "Time:        " << time << "\n";
  cout << endl;
  cout << "maxCluster=" << maxPercent << ";\n";
  cout << "aveCluster=" << aveCluster << ";\n";
  cout << "aveFiniteCluster=" << aveFiniteCluster << ";\n";
  cout << "ListLinePlot[maxCluster]\n";
  cout << "ListLinePlot[aveCluster]\n";
  cout << "ListLinePlot[aveFiniteCluster]\n";

  return 0;
}
