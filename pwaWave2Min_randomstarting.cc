/*********************************************
 *
 * wv2min : This program takes a wave
 *   file as input and creates a minuit
 *   input file.
 * 
 *
 * Paul Eugenio
 * Florida State University
 * March 2011
 **********************************************/


#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <sstream>
using namespace std;




int PrintUsage(char *processName)
{
  
  fprintf(stderr,"%s usage: [switches]   \n",processName);
  fprintf(stderr,"\t-w <name>    The wave file.\n");
  fprintf(stderr,"\t-NP    Do not fix any fit parameters\n");
  fprintf(stderr,"\t-h Print this help message\n\n");
}

/*
 ***********************
 *                     *
 *  Main()             *
 *                     *
 ***********************
 */

main(int argc,char **argv)
{
   srand (time(NULL));
  char *argptr;
  int npar;
  bool 	 useFixedInputParms = true;
  //  ofstream minFp("minuit.input");
  string waveFile;
  ifstream waveListFile;
 
  int numberWaves = 0;  // multiply by 2 for number of parameters

  if (argc == 1){
    PrintUsage(argv[0]);
    exit (0);
  }
  else {
    for (int i = 1; i<argc; i++) {
      argptr = argv[i];
      if ((*argptr == '-') && (strlen(argptr) > 1)) {
        argptr++;
        switch (*argptr) {
        case 'h':
          PrintUsage(argv[0]);
          exit(0);
          break;
	case 'N':
	  useFixedInputParms = false;
	  cerr << "Not fixing any input parameters" << endl;
	  break;
        case 'w':
	  {
	    waveListFile.open( argv[i+1], ios::in );
	    string s(argv[i+1]);
	    cerr << "wave file is " << s << endl;
	    waveFile = s;
	  }
          break;
        default:
	  //          fprintf(stderr,"Unrecognized argument -%s\n\n",argptr);
          PrintUsage(argv[0]);    
          exit(-1);
          break;
	}
      }
    }

    
    /*
     * Read wave file and count parameters.
     */

    int rank, eps;
    int numberWaveSets, numberRanks;
    int checkNumberWaves=0;
    string wave;
    waveListFile >> numberWaves;
    cerr << "numberWaves is " << numberWaves << endl;
    waveListFile >> numberRanks;
    waveListFile >> numberWaveSets;
    int *numberWavesInSet = new int [numberWaveSets];
    for( int ws = 0; ws < numberWaveSets; ws++){
      waveListFile >> numberWavesInSet[ws];
      cerr << "number of waves in set is " << numberWavesInSet[ws] << endl;
      checkNumberWaves += numberWavesInSet[ws];
    }
    
    if( numberWaves != checkNumberWaves ){
      cerr << "Failed wave number check!" << endl;
      exit(-1);
    }
    
    /*
     * Now write the minuit input file.
     */


    cout << "SET TITLE\n";
    cout << "Fit for wave file: " << waveFile << "\n";
    cout << "PARAMETERS\n";

    string wset_neg(" 'wset_-1 ' ");
    string wset_pos(" 'wset_+1 ' ");
    string wset_zer(" 'wset_0  ' ");
        std::stringstream parname;
srand(time(NULL));
    for( int i = 0; i < numberWaves; i++ ){
      waveListFile >> wave;
      waveListFile >> eps;

	//float rpr= static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float rpr= static_cast <float> (rand()) %5;
  	//float rpi= static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float rpi= static_cast <float> (rand()) %5;
	parname.str("");
      switch( eps ){

      case -1: 

	parname<< wset_neg<<rpr<<" "<<rpi;
	break;
      case +1:

	parname<< wset_pos<<rpr<<" "<<rpi; 
	break;
      case 0:

	parname<<wset_zer<<rpr<<" "<<rpi; 
	break;
      default:
	cout << "ERROR IN WAVELIST FILE: UNKNOWN WAVE SET OR REFLECTIVITY" << endl;
      }

      cout << 2*i+1 << parname.str() << endl;
      cout << 2*i+2 << parname.str() << endl;

    }

    cout << endl;

    //
    // Now fix parameters
    //
    // "SET PARAM 4 xxx"
    // FIX 4
    int offset = 0;
    if( useFixedInputParms )
      for( int i =0; i < numberWaveSets; i++){
	cout << "SET PARAM " << (2 + 2*offset)  << " 0.0" << endl;
	cout << "FIX " << 2 + 2*offset << endl;
	offset += numberWavesInSet[i];
      }
    

  }
}




