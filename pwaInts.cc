#include <iostream>
#include <fstream>
#include <complex>
#include <string>
#include <vector>

using namespace std;


void printUsage(char *name){
  cerr << "Usage: " << name << " [OPTIONS]  -m<massBin>  <ampFile> <ampFile> ... <ampFile>" << endl;
  cerr << "\tREQUIRED:\n" 
       << "\t -m<massBin>\t // mass bin value" << endl
       << "\tOPTIONS: " << endl
       << "\t -w<wavelist>\t // use wavelist file rather than using ampFiles" << endl
       << "\t -F\t // read complex<float> rather than complex<double>" << endl
       << "\t -d\t // turn on debug statements" << endl;

 exit(0);
}



int main(int argc, char **argv){
  bool isDebug= false;
  bool isUsingWaveList = false;
  bool isUsingFloats = false;
  int numberWaves = 0; 
  double massBin = 0;
  ifstream tmpF;  

  ifstream waveListFile;

  //  string *waveName = new string[ argc - 2];  
  vector<string> waveName;

ofstream fout("NI_werrors.txt");


  if( argc < 3)
    printUsage( argv[0] ); 

  for( int i = 1; i < argc ; i++ ){
    if( *argv[i] == '-' ){
      switch( *(argv[i]+1) ){
      case 'm':
	massBin = atof ( argv[i] + 2 );
	cerr << "MassBin: " << massBin <<  endl;
	break;
    case 'd':
	cerr << "Turning on debug statements" << endl;
	isDebug = true;
	break;
      case 'F':
	cerr << "Reading complex<float> from amp file" << endl;
	isUsingFloats = true;
	break;
      case 'w':
	cerr << "Using wavelist file: " << (argv[i] + 2) << endl;
	isUsingWaveList = true;
	waveListFile.open( (argv[i]+2), ios::in );
	break;
      default:
	cerr << "Unknown option\n\n";
	printUsage( argv[0] );
      }
    }
    else if( !isUsingWaveList ) { //it is an amp file
      //cerr << "amps: " << argv[i]  << endl;
      string s(argv[i]);
      int length = s.size();
      s.resize(length - 5); // remove ".amps"
      waveName.push_back(s);
      cerr << "wave: " << waveName[numberWaves] << endl;
      numberWaves++;

      //waveName[numberWaves] = s;
      //waveName[numberWaves].resize(length - 5); // remove ".amps"      
      //ampFile[numberWaves].open( argv[i], ios::in | ios::binary );
      //if( numberWaves == 0 ) tmpF.open( argv[i], ios::in | ios::binary );
    }
  }
  
  if( isUsingWaveList ){  //get wavelist
    int rank, eps, number;
    int numberWaveSets, numberRanks;
    int checkNumberWaves=0;
    string wave;
    waveListFile >> numberWaves;
    waveListFile >> numberRanks;
    waveListFile >> numberWaveSets;
    for( int ws =0; ws < numberWaveSets; ws++){
      waveListFile >> number;
      checkNumberWaves += number;
    }
    if( numberWaves != checkNumberWaves ){
      cerr << "Failed wave number check!" << endl;
      exit(-1);
    }
      

    for( int i = 0; i < numberWaves; i++ ){
      waveListFile >> wave;
      waveListFile >> eps;
      waveName.push_back( wave );
    }
  }
  


  ifstream *ampFile =  new ifstream[numberWaves];


  //read amplitude files
  for( int i = 0; i < numberWaves; i++){
    string suffix(".amps");
    string ampFileName = waveName[i] + suffix;
    cerr << "Opening amp file: " << ampFileName << endl;
    ampFile[i].open( ampFileName.c_str(), ios::in | ios::binary );
    if( i == 0 ) tmpF.open( ampFileName.c_str(), ios::in | ios::binary );
  }


  if ( massBin == 0 )
    printUsage( argv[0] );
  
  
  // count amps
  unsigned long numberEvents = 0;
  complex<double> amp; 
  complex<float> ampF; 

  char *ampPtr;
  size_t ampSize;
  if( isUsingFloats ){
    ampSize = sizeof( complex<float> );
    ampPtr = (char*)&ampF;
  } else{
    ampSize = sizeof( complex<double> );
    ampPtr = (char*)&amp;
  }

  while( tmpF.read( ampPtr, ampSize ) ) 
    numberEvents++;
  
  cerr << "Total event count: " << numberEvents << endl;

  /// init amps memory
  /******
  vector<vector< complex<double> > > decayAmp;  
  decayAmp.resize(numberWaves);
  for(int i = 0; i < numberWaves; i++)
    decayAmp[i].resize(numberEvents);
  *********/
  complex<double> **decayAmp;
  decayAmp = new complex<double>* [numberWaves];
  for(int i = 0; i < numberWaves; i++)
    decayAmp[i] = new complex<double> [numberEvents];

  // read all amps
  for( int i = 0; i < numberWaves; i++ ){
    int j = 0;
    //cerr << " i is " << i << endl;
    for( unsigned long n = 0; n < numberEvents; n++ ){
      ampFile[i].read( ampPtr, ampSize );
      if( isUsingFloats )
	amp = ampF;
      decayAmp[i][n] = amp;
    }
    //   for(int j = 0; j< numberEvents; j++)
    //  cerr << "amp  is " << decayAmp[i][j]  << endl;

  }


  // allocate NI memory
  vector<vector< complex<double> > > NI;
  NI.resize(numberWaves);
  for(int i = 0; i < numberWaves; i++)
    NI[i].resize(numberWaves);
  complex<double> zero(0,0);

   double **sq_element;
   sq_element = new double *[numberWaves];
  for(int i = 0; i < numberWaves; i++)
   sq_element[i] = new double [numberWaves];	

   double  sumx_xbar;
   double  standardev;
   double  mean_std;
	mean_std=0;
	sumx_xbar=0;
	standardev=0;
  // sum amps 
  for( int i = 0; i < numberWaves; i++)
    for( int j = 0; j < numberWaves; j++){
      NI[i][j] = zero;
	sq_element[i][j]=0;
      //cerr << i << j;
      for( int k = 0; k < numberEvents; k++){
	NI[i][j] += (decayAmp[i][k] * conj( decayAmp[j][k])) ;

	sq_element[i][j] += real((decayAmp[i][k] * conj( decayAmp[j][k]))*(decayAmp[i][k] * conj( decayAmp[j][k]))) ;
//if(i==j)
	//cout<<"element squared " <<sq_element[i][j]<<endl;

	//cerr << decayAmp[i][k] << " " << conj( decayAmp[j][k] ) << " + ";
      }
      NI[i][j] /= (double)(numberEvents);
      //      cerr << endl;
if(i==j){
sumx_xbar = sq_element[i][j]-(double(numberEvents)*real(NI[i][j]*conj(NI[i][j])));

standardev=sqrt((1/(double(numberEvents)-1))*sumx_xbar);
mean_std=standardev/sqrt(double(numberEvents));
fout<<real(NI[i][j])<<" "<<mean_std<<endl;
}

    }
 
  // dump NI
  cout << massBin << endl;
  cout << numberEvents << endl;
  cout << numberWaves << " " << numberWaves << endl;
  
  for(int i = 0; i < numberWaves; i++){
    for(int j = 0; j < numberWaves; j++)
      cout << NI[i][j] << " ";
    cout << endl;
  }

  // dump wave list
  cout << numberWaves << endl;
  for(int i = 0; i < numberWaves; i++){
    cout << waveName[i] << endl;
  }
     
}
