/*---------------
*
* Read Amps
*
*--------------*/

#include <iostream>
#include <fstream>
#include <complex>

using namespace std;

void printUsage(char *name){
  cerr << "Usage: " << name << " [OPTIONS] <ampFile> " << endl;
  cerr << "\tOPTIONS:\n" 
       << "\t -c\t // use to only count # amps in file" << endl
       << "\t -F\t // Read complex<float> rather than complex<double>" << endl;
 exit(0);
}

int main(int argc, char **argv){
  bool isOnlyCounting = false;
bool isUsingFloats = false;
  ifstream ampStream;

  if( argc == 1)
    printUsage( argv[0] );
  else{
    for( int i =1; i < argc; i++){
      char *ptr = argv[i];
      if( *ptr == '-' ){
	ptr++;
	switch( *ptr ){
	case 'c':
	    cerr << "Only counting events" << endl;
	    isOnlyCounting = true;	    
	    break;
	case 'F':
	    cerr << "Reading complex<float>:  default complex,double>" << endl;
	    isUsingFloats = true;	    
	    break;
	default:
	  cerr << "Unknown option\n\n";
	  printUsage( argv[0] );
	}


      }
      else
	ampStream.open(argv[i], ios::in | ios::binary );
    }
  }
 
  unsigned long ampCounter = 0;
  complex<float> ampF;
  complex<double> amp;
  char* ampPtr;
  size_t ampSize;

  if( isUsingFloats ){
    ampSize = sizeof( complex<float> );
    ampPtr = (char*)&ampF;
  } else {
    ampSize = sizeof( complex<double> );
    ampPtr = (char*)&amp;
  }
  
  while (ampStream.read( ampPtr, ampSize )){
    if( isUsingFloats )
      amp = ampF;
    
 
    if( !isOnlyCounting ) 
      cerr << amp << endl;
    ampCounter++;
  }
  //cerr << "Total amp count: " << ampCounter << endl;
 

}


