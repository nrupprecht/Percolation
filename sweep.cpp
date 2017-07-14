// 7/13/2017

#include "percolation.hpp"
#include "Utility.hpp"

int main(int argc, char** argv) {
  // Parameters
  int width = 1000, height = 1000;
  double minP = 0.55;
  double maxP = 0.65;
  int partition = 10, rep = 5;

  // Seed rand
  srand48( std::time(0) );

  ArgParse parser(argc, argv);
  parser.get("width", width);
  parser.get("height", height);
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

  // Create a percolation
  Percolation percolation;
  percolation.setDoBMP(false);
  percolation.setDims(width, height);

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

  // Print data
  cout << "maxCluster=" << maxPercent << ";\n";
  cout << "aveCluster=" << aveCluster << ";\n";
  cout << "aveFiniteCluster=" << aveFiniteCluster << ";\n";

  return 0;
}
