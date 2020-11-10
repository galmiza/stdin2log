/*
 * Forwards log stream from stdin into log files
 * Support rotation and compression
 * Log names are <basename>.<index>.<log|gz>
 * Options
 *  -o <string> basepath of the log files (e.g. /tmp/mylog)
 *  -s <int> max size of log files
 *  -n <int> max number of log files to keep
 *  -z <int> compression level 1 (fastest) to 9 (best)
 */

#include <string>
#include <fstream>
#include <iostream> /* cout, cerr */
#include <cstring>  /* strcat */
#include <unistd.h> /* getopt */
using namespace std;

// Entry point
int main(int argc, char *argv[]) {

  // Define default options
  int MAX_FILES = 4;
  int MAX_SIZE = 1024*1024*8;
  int Z_LEVEL = -1;
  char LOG_BASEPATH[128] = {0};
  char LOG_FILENAME[128] = {0};
  char LOG_EXT[4] = {0};

  // Process command line arguments and update options
  int opt;
  while((opt = getopt(argc, argv, ":o:n:s:z:")) != -1) {
    switch(opt) {
      case 'o': // output path
        strcat(LOG_BASEPATH,optarg);
        break;
      case 's': // max size per file (uncompressed)
        MAX_SIZE = atoi(optarg);
        break;
      case 'n': // max number of files
        MAX_FILES = atoi(optarg);
        break;
      case 'z': // compression level
        Z_LEVEL = atoi(optarg);
        break;
      case ':':
        cerr << "option needs a value" << endl;
        break;
      case '?':
        cerr << "unknown option: " << optopt << endl;
        break;
    }
  }

  // Debug
  cerr << "MAX_FILES = " << MAX_FILES << endl;
  cerr << "MAX_SIZE = " << MAX_SIZE << endl;
  cerr << "LOG_BASEPATH = " << LOG_BASEPATH << endl;
  cerr << "Z_LEVEL = " << Z_LEVEL << endl;

  // Precompute constants from options
  sprintf(LOG_FILENAME,"%s.log",LOG_BASEPATH);
  strcat(LOG_EXT,Z_LEVEL==-1?"log":"gz");

  // Find current log index based on existing files
  char str[128] = {0};
  bool done = false;
  int li = 1000;
  while (!done && li!=0) {
    sprintf(str,"%s.%i.%s",LOG_BASEPATH,--li,LOG_EXT);
    ifstream f(str);
    if (f.good()) done = true;
  }

  // Continuously read stdin and stream to rotating files
  fstream is;
  is.open(LOG_FILENAME,ios::app);
  for (string line; getline(cin,line);) {

    // If file would be too big already, rename it
    int size = is.tellg();
    if (size+line.size()+1>MAX_SIZE) {
      li++;
      sprintf(str,"cp %s %s.%i.log && truncate -s 0 %s", // never rename the active log file not to break its stream
        LOG_FILENAME,LOG_BASEPATH,li,LOG_FILENAME);
      system(str);
      if (Z_LEVEL!=-1) {
        sprintf(str,"(cat %s.%i.log | bzip2 -%i > %s.%i.gz && rm %s.%i.log) &", // compress latest archive (async)
          LOG_BASEPATH,li,Z_LEVEL,LOG_BASEPATH,li,LOG_BASEPATH,li);
        system(str);
      }
      sprintf(str,"rm %s.%i.%s 2>/dev/null",LOG_BASEPATH,li-MAX_FILES,LOG_EXT); // lazy remove oldest file
      system(str);
    }

    // Write line into current logfile
    is << line << endl;
  }
  is.close();
  return 0;
}