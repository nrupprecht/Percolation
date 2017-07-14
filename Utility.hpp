// 7/14/2017 
#ifndef __UTILITY_PERCOLATION_HPP
#define __UTILITY_PERCOLATION_HPP

#include "ArgParse.h"
#include <ctime>
#include <ostream>
using std::ostream;
#include <fstream>
using std::ofstream;
#include <string>
using std::string;
#include <ctime>
#include <fstream>
using std::ofstream;
#include <string>
using std::string;

template<typename T> void writeToFile(string filename, const vector<T>& lst) {
  // Open filestream
  ofstream fout(filename);
  if (fout.fail()) {
    cout << "File " << filename << "] failed to open. Exiting.\n";
    exit(0);
  }
  for (auto &data : lst) fout << data << endl;
  // Close stream
  fout.close();
}

template<typename S, typename T> void writeToFile(string filename, const map<S, T>& lst){
  // Open filestream
  ofstream fout(filename);
  if (fout.fail()) {
    cout << "File " << filename << "] failed to open. Exiting.\n";
    exit(0);
  }
  for (auto &data : lst) fout << data.first << "," << data.second << endl;
  // Close stream
  fout.close();
}

template<typename S, typename T> inline std::ostream& operator<<(std::ostream& out, const std::pair<S,T>& pr) {
  out << "{" << pr.first << "," << pr.second << "}";
  return out;
}

template<typename T> inline std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec) {
  out << "{";
  for (int i=0; i<vec.size(); ++i) {
    out << vec.at(i);
    if (i!=vec.size()-1) out << ",";
  }
  out << "}";
  return out;
}

#endif // __UTILITY_PERCOLATION_HPP
