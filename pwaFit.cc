/*********************************************
 *
 * pwaFit.c : The FSU pwa fitting program
 *   which calls minuit.
 *
 * 
 *
 * Paul Eugenio
 * Florida State University
 * March 2011
 **********************************************/

#include <iostream>
#include <fstream>
#include <complex>
#include <vector>

using namespace std;

class WaveList{
public:
  string name;
  int eps;
  int rank;
};

class PwaWaveSet{
public:
  unsigned int numberWaves;
  unsigned int index;
};


/***********************
 * PROTOTYPE FUNCTIONS
 ***********************/


void fcn(int *,double *, double *, double *, int *);

extern "C" {
  extern void fort_open_(char *name,int);
  extern void minuit_(...);

  /* 
 typedef struct {
    int nvarl[350];
    int niofex[350];
    int nexofi[350];
  } minuit_common_n;
  extern minuit_common_n mn7inx_;
  */

  void mnemat_(double*,int*);
  //  void mnexcm_(...);
  

}

void fort_open(char *name)
{ /*put exactly 20 char in the name-- The power of FORTRAN */
  int filled;
  static char *filler="                    ";
  filled=strlen(name);
  if(20-filled >0)strncat(name,filler,8-filled);
  fort_open_(name,20);
}



int printUsage(char *processName)
{
  
  fprintf(stderr,"%s usage: [switches]   \n",processName);
  //  fprintf(stderr,"\t-o<name> The output  file.\n");
  fprintf(stderr,"\t-w<name> \t The wave file.\n");
  fprintf(stderr,"\t-n<ACCNIfile> \t The Accepted normalization integral file.\n");
  fprintf(stderr,"\t-r<RAWNIfile> \t The Raw normalization integral file.\n");
  //fprintf(stderr,"\t-t<tag> The tag used to create the data amp files.\n");
  // fprintf(stderr,"\t-N The numbers of data events\n");
  fprintf(stderr,"\t-s<name> \t Save parameters for Minuit input(20 char max)\n");
  cerr << "\t-P \t\t Use this switch if minuit.input is not fixing parameters" << endl;
  cerr << "\t-F \t\t Reading complex<float> amps rather than complex<double> amps" << endl;
  fprintf(stderr,"\t-h \t\t Print this help message\n\n");
}


namespace pwa{
  bool Debug = false;
  bool isNotFixed = false;
  bool isUsingFloats = false;
  ifstream  waveFp;
  ofstream  outputFp;
  ifstream  accIntFp;
  ifstream  rawIntFp;
}
using namespace pwa;


/*
 ***********************
 *                     *
 *  Main()             *
 *                     *
 ***********************
 */

main(int argc,char **argv)
{
  char *argptr;
  char ACCNIfile[128];
  char RAWNIfile[128];
  char wavefile[128],rm_old_file[128];
  char outputFile[128] ="output.fit";
  char min2savefile[128] ="lastfit.minput"; 
  int i,unit,nff,nfs;

  cerr.precision(10);


  if (argc == 1){
    printUsage(argv[0]);
    exit (0);
  }
  else {
    for (i=1; i<argc; i++) {
      argptr = argv[i];
      if ((*argptr == '-') && (strlen(argptr) > 1)) {
        argptr++;
        switch (*argptr) {
        case 'D':
          break;
        case 'd':
          Debug =1;
          break;
        case 'h':
          printUsage(argv[0]);
          exit(0);
          break;
	case 'P':
	  cerr << "Assuming minuit.input is not fixing parameters" << endl;
	  isNotFixed = true;
	  break;
	case 'F':
	  cerr << "Reading complex<float> from amp file" << endl;
	  isUsingFloats = true;
	  break;
	case 'o':
	  sprintf(outputFile,"%s",++argptr);
	  cerr << "savingf fit result to file: " << outputFile << endl;
	  outputFp.open( outputFile, ios::out );
	  break;
        case 'w':
	  sprintf(wavefile,"%s",++argptr);
	  cerr << "opening wavelist: " << wavefile << endl;
	  waveFp.open( wavefile, ios::in );
	  break;
	case 'r':
          sprintf(RAWNIfile,"%s",++argptr);
	  cerr << "opening RNI file: " << RAWNIfile << endl;
	  rawIntFp.open(RAWNIfile);
	  if(!( rawIntFp.is_open() ) ){
	    cerr << "Fail to open RAWNI file!\n";
	    exit(-1);
	  }
          break;
	case 'n':
          sprintf(ACCNIfile,"%s",++argptr);
	  cerr << "opening ANI file: " << ACCNIfile << endl;
	  accIntFp.open(ACCNIfile);
	  if(!( accIntFp.is_open() ) ){
	    cerr << "Fail to open ACCNI file!\n";
	    exit(-1);
	  }
          break;

	 case 's':
	   sprintf(min2savefile,"%s\0",++argptr);
	   sprintf(rm_old_file,"rm %s",min2savefile);
	   system(rm_old_file);
	   break; 
        default:
          fprintf(stderr,"Unrecognized argument -%s\n\n",argptr);
          printUsage(argv[0]);    
          exit(-1);
          break;
	}
      }
    }
    setlinebuf(stdout);
    setlinebuf(stderr);
    
    
    /*
     *   CALL MINUIT
     */
    
    sprintf(rm_old_file,"rm %s",min2savefile);
    system(rm_old_file);

    fort_open(min2savefile);
    minuit_(fcn,0); 
    
  }
}



void fcn(int *npar,double *grad, double *fval, double *xval, int *iflag){

 

  static int numberWaves;
  static int numberRanks;
  static int numberWaveSets;
  static double massBin;
  static unsigned long numberEvents = 0;
  static unsigned long numberAccMcEvents = 0;
  static unsigned long numberRawMcEvents = 0;


  //static vector<WaveList> wave;
  //  static vector<vector< complex<double> > > decayAmp;
  // static vector<vector< complex<double> > > NI;
  //static vector< vector< complex<double> > > Vamp;
  
  static PwaWaveSet *waveSet;
  static WaveList *wave;
  static complex<double> **decayAmp;
  static complex<double> **ACCNI;
  static complex<double> **RAWNI;
  static complex<double> **Vamp;
  static complex<double> **nVamp;
  
  if( *iflag==2 ){
    cerr << "fcn called with iflag= " << *iflag << endl;
    cerr << "Doing nothing, letting Minuit calculate first derivatives of fcn" << endl;
  }
  
  
  if( *iflag == 1 ){ // initialize fitter

    //
    // Get wave list
    //
    //    ifstream  waveFp("wavelist");

    waveFp >> numberWaves;
    waveFp >> numberRanks;
    waveFp >> numberWaveSets;
    int offset = 0;
    waveSet = new PwaWaveSet [numberWaveSets];
    for( int ws = 0; ws < numberWaveSets; ws++){
      waveFp >> waveSet[ws].numberWaves;
      waveSet[ws].index = offset;
      offset += waveSet[ws].numberWaves;
    }
    if( numberWaves != offset ){
      cerr << "Wavelist file error: numberWaves != number of waves in all sets!" << endl;
      exit(-1);
    }
    //    vector<WaveList> wave;
    //    wave.resize( numberWaves );
    wave = new WaveList [numberWaves];
    
    for( int i = 0; i < numberWaves; i++){
      waveFp >> wave[i].name;
      waveFp >> wave[i].eps;
    }
    
    cerr << "found " << numberWaves <<" waves of rank" << numberRanks  << endl;
    for( int i = 0; i < numberWaves; i++)
      cerr << wave[i].name << " "
	   << wave[i].eps << endl;
    
    //get decay amps
    ifstream *ampFile =  new ifstream[numberWaves];
    ifstream tmpF;
    //string ampDir("test_data/");
    
    for( int i = 0; i < numberWaves; i++ ){
      //string s = ampDir + wave[i].name + ".amps";
      string s = wave[i].name + ".amps";
      cerr << "opening: " << s << endl;
      const char* p = s.c_str();
      ampFile[i].open( p, ios::in | ios::binary );
      if( i == 0 ) tmpF.open( p , ios::in | ios::binary );
    }

    //
    // count Events
    complex<double> amp;
    complex<float> ampF;
    size_t ampSize;
    char *ampPtr;
    numberEvents = 0;

    if( isUsingFloats ){
      ampPtr = (char*)&ampF;
      ampSize = sizeof( complex<float> );
    } else {
      ampPtr = (char*)&amp;
      ampSize = sizeof( complex<double> );
    }

    while( tmpF.read( ampPtr, ampSize ) )
      numberEvents++;
    cerr << "Total event count: " << numberEvents << endl;

    // init amps memory
    // vector<vector< complex<double> > > decayAmp;
    /*** replaced for faster code *********
    decayAmp.resize(numberWaves);
    for(int i = 0; i < numberWaves; i++)
      decayAmp[i].resize(numberEvents);
    *******************************/    
    decayAmp = new complex<double>* [numberWaves];
    for( int i = 0; i < numberWaves; i++)
      decayAmp[i] = new complex<double> [numberEvents];

    // read all amps
    for( int i = 0; i < numberWaves; i++ ){
      int j = 0;
      //cerr << " i is " << i << endl;
      for( unsigned long n = 0; n < numberEvents; n++){
	ampFile[i].read( ampPtr, ampSize );
	if( isUsingFloats )
	  amp = ampF;
	decayAmp[i][n] = amp;
      }
      if( Debug ){
	cerr << "Reading back " << numberEvents << " decay amps" << endl;
	for(int j = 0; j< numberEvents; j++)
	  cerr << "amp  is " << decayAmp[i][j]  << endl;
      }
      
    }
  

    // allocate NI memory
    /******** replaced for faster code
    //vector<vector< complex<double> > > NI;
    NI.resize(numberWaves);
    for(int i = 0; i < numberWaves; i++)
      NI[i].resize(numberWaves);
    *******************************/

    RAWNI = new complex<double>* [numberWaves];
    for(int i = 0; i < numberWaves; i++)
      RAWNI[i] = new complex<double> [numberWaves];
    
    // readNI
    
    //ifstream  accIntFp("accNI");
    //unsigned long numberAccMcEvents=0;
    unsigned int numberRawWaves;
    
    rawIntFp >> massBin;
    cerr << "massBin from RawInts: " << massBin << endl;
    rawIntFp >> numberRawMcEvents;
    cerr << "numberRawMcEvents from RawInts: " << numberRawMcEvents << endl;
    rawIntFp >> numberRawWaves;
    rawIntFp >> numberRawWaves;
    cerr << "numberRawWaves from RawInts: " << numberRawWaves << endl;
    
    for(int i = 0; i < numberWaves; i++){
      for(int j = 0; j < numberWaves; j++)
	rawIntFp >> RAWNI[i][j];
    }
    
    
    
    // dump ACCNI
    for(int i = 0; i < numberWaves; i++){
      for(int j = 0; j < numberWaves; j++)
	cout << RAWNI[i][j] << " ";
      cout << endl;
    }


////////////////////////////////////////////////////

    ACCNI = new complex<double>* [numberWaves];
    for(int i = 0; i < numberWaves; i++)
      ACCNI[i] = new complex<double> [numberWaves];
    
    // readNI
    
    //ifstream  accIntFp("accNI");
    //unsigned long numberAccMcEvents=0;
    unsigned int numberAccWaves;
    
    accIntFp >> massBin;
    cerr << "massBin from AccInts: " << massBin << endl;
    accIntFp >> numberAccMcEvents;
    cerr << "numberAccMcEvents from AccInts: " << numberAccMcEvents << endl;
    accIntFp >> numberAccWaves;
    accIntFp >> numberAccWaves;
    cerr << "numberAccWaves from AccInts: " << numberAccWaves << endl;
    
    for(int i = 0; i < numberWaves; i++){
      for(int j = 0; j < numberWaves; j++)
	accIntFp >> ACCNI[i][j];
    }
    
    
    
    // dump ACCNI
    for(int i = 0; i < numberWaves; i++){
      for(int j = 0; j < numberWaves; j++)
	cout << ACCNI[i][j] << " ";
      cout << endl;
    }








    // Check Wave list at Later TIME!!!
  
    // set up Production Amps
    /********** replaced for faster code **********
    //    vector< vector< complex<double> > > Vamp;
    Vamp.resize( numberRanks );
    for( int k = 0; k < numberRanks; k++)
      Vamp[k].resize(numberWaves);
    ****************************************/
    Vamp = new complex<double>* [numberRanks]; 
    for( int k = 0; k < numberRanks; k++)
      Vamp[k] = new complex<double> [numberWaves];

  
    nVamp = new complex<double>* [numberRanks]; 
    for( int k = 0; k < numberRanks; k++){
      nVamp[k] = new complex<double> [numberWaves];}


    cerr << "Finished fit initializations..." << endl;
  }// end of *iflag == 1

  if( Debug ){
    cerr << "Trying to sync parameters..." << endl;
    for( int i =0; i < *npar; i++ )
      cerr << "xval[" << i << "] = " << xval[i] << endl;
  }

  // sync fit parameters
  for( int k = 0; k < numberRanks; k++)
    for( int a =0; a < numberWaves; a++){
      if( Debug ) cerr << " k: " << k << " a: " << a << endl;
      //      complex<double> c( xval[2*a], xval[2*a+1] );
      Vamp[k][a] = complex<double>(xval[2*a], xval[2*a+1] );
      if( Debug ) cerr << "Vamp["<<k<<"]["<<a<<"] = " << Vamp[k][a] << endl;
    }
  // calculate -logLikelihood 
  
  /*
   * Calulate -loglikelihood
   * -ln(L) = Sum_{rank,alpha,alpha'}[ V{^k_alpha} V*{^k_alpha'} NI{^k_alpha,alpha'}] -
   *          Sum_{dataAmps}[ ln( Sum_{rank,alpha,alpha'}[ V V* A A*])]
   *
   *  Note that NI[rank][a][b] is proportional to Sum{ A[rank][a] A*[rank][b]}
   */
  if( Debug )  cerr << "Starting to calculate loglikely..." << endl;
  double sum_VVNI =0;
  
  if( Debug )  cerr << "Trying to calculate sum_VVNI..." << endl;
  for( int k = 0; k < numberRanks; k++){
    /***********************************
    for( int a = 0; a < numberWaves; a++)
      for( int b =0; b < numberWaves; b++)
	if( wave[a].eps == wave[b].eps )
    *********************************/
    for( int ws = 0; ws < numberWaveSets; ws++)
      for( int a = waveSet[ws].index; a < ( waveSet[ws].index + waveSet[ws].numberWaves ); a++)
	for( int b = waveSet[ws].index; b < ( waveSet[ws].index + waveSet[ws].numberWaves ); b++)
	  sum_VVNI += real( Vamp[k][a] * conj(Vamp[k][b]) * ACCNI[a][b] );
  }
  
  

  double log_Sum_VVAA ;
  double sum_VVAA;
  
  if( Debug )  cerr << "Trying to calculate log_sum_VVAA..." << endl;
  log_Sum_VVAA = 0.0;
  for( int i = 0; i < numberEvents; i++){
    sum_VVAA = 0.0;
    for( int k = 0; k < numberRanks; k++)
      /**********************
      for( int a = 0; a < numberWaves; a++)
	for( int b = 0; b < numberWaves; b++)
	  if( wave[a].eps == wave[b].eps )
      **********************************/
      for( int ws = 0; ws < numberWaveSets; ws++)
	for( int a = waveSet[ws].index; a < ( waveSet[ws].index + waveSet[ws].numberWaves ); a++)
	  for( int b = waveSet[ws].index; b < ( waveSet[ws].index + waveSet[ws].numberWaves ); b++)
	    sum_VVAA += real( Vamp[k][a] * conj( Vamp[k][b] ) * decayAmp[a][i] * conj( decayAmp[b][i] ) );
    log_Sum_VVAA += log(sum_VVAA);
  }
  
  //  log_Prod_Sum_VVAA = log( prod_Sum_VVAA ); // faster than sum of ln()
  //cout<<"***************ETA_X ======" << float(numberAccMcEvents/numberRawMcEvents)<<endl;
  //double minusLogLikelihood = float(numberAccMcEvents/numberRawMcEvents) * sum_VVNI - log_Sum_VVAA ;

  double minusLogLikelihood =numberEvents * sum_VVNI - log_Sum_VVAA ;

  if( Debug )  cerr << "Finished calculating -loglikely..." << endl;

  *fval = minusLogLikelihood;
  
  static int callCounter = 0;
  
  if( (++callCounter%100) == 0 )
    cerr << "fcn: call number= " << callCounter 
	 << "\t-ln(Likelihood)= " << minusLogLikelihood 
	 << "\tsum_VVNI= " << sum_VVNI <<  endl;
  

  if( *iflag == 3 ){ // fit is done
    cerr << "Fit is DONE!"  << endl;
    
    // Write out fit results   
    cout <<"ZRESULT: " << massBin << " " << minusLogLikelihood << " " ;
    double sum =0;
    for( int k = 0; k < numberRanks; k++)
      for( int a = 0; a < numberWaves; a++ )
	for( int b = 0; b < numberWaves; b++ )
	  if( wave[a].eps == wave[b].eps )
	    sum += numberEvents * real( Vamp[k][a] * conj( Vamp[k][b] ) * ACCNI[a][b] );
    cout << sum;
    
    for( int k = 0; k < numberRanks; k++)
      for( int a = 0; a < numberWaves; a++ )
	cout << " " << numberEvents * real( Vamp[k][a] * conj( Vamp[k][a] ) * ACCNI[a][a] );
    cout << endl;

 

    //
    // Save mass, likly, and number of data events 
    //
    outputFp << "Mass: " << massBin << " " 
	     << minusLogLikelihood << " "
	     << numberEvents << endl;

    //
    // Save wavelist
    //
    outputFp << numberWaves << " " << numberRanks << " " << numberWaveSets;
    for( int ws = 0; ws < numberWaveSets; ws++ )
      outputFp << " " << waveSet[ws].numberWaves;
    outputFp << endl;
    for( int a = 0; a < numberWaves; a++ )
      outputFp << wave[a].name << " " << wave[a].eps << endl;
  






	float n_etax=
	 float(numberEvents*numberRawMcEvents/numberAccMcEvents);
    //
    // Save production amplitudes
    //
    outputFp << "V: " << numberRanks << " " << numberWaves << endl;
    for( int k = 0; k < numberRanks; k++){
      for( int a = 0; a < numberWaves; a++ ){
	//nVamp[k][a]=sqrt(numberEvents*numberRawMcEvents/numberAccMcEvents)*Vamp[k][a];
	outputFp << Vamp[k][a] << endl;
		}
	}
/*********************************************************************************on april 17 I commented out the Fixedparam statement to check its effect on the errors*********/
    //
    // Save error matrix
    //
    //    int nPars = numberWaves *2 - numberWaveSets;
    int nPars = numberWaves *2;

    double **errorMatrix = new double* [nPars];
    for( int i = 0; i < nPars; i++)
      errorMatrix[i] = new double [nPars];
    
    double* errmat = (double*) malloc( nPars*nPars*sizeof(double) );
    
    mnemat_( errmat, &nPars );
    for( int a = 0; a < nPars; a++ )
      for( int b = 0; b < nPars; b++ ){
	bool isFixedParam = false;
	/*for(int ws = 0; ws < numberWaveSets; ws++ )
	  if( 2 * waveSet[ws].index == (a -1) ||  2 * waveSet[ws].index == (b -1) )
	    isFixedParam = true;
	if( isFixedParam )
	  errorMatrix[a][b] = 0;
	else*/
	  //errorMatrix[a][b] = *( errmat + a * (nPars - numberWaveSets) + b );
		errorMatrix[a][b] = *( errmat + a * (nPars) + b );
      }

    outputFp << "ERMAT: " << nPars << " " << nPars << endl;
    
    for( int a = 0; a < nPars; a++ ){
      for( int b = 0; b < nPars; b++ )
	outputFp << errorMatrix[a][b] << " ";
      outputFp << endl;
    }

 
    //
    // Save Accepted Normalization Integrals
    //
    outputFp << "AccNI: " << numberAccMcEvents << endl;
    outputFp << numberWaves << endl;
    for( int a = 0; a < numberWaves; a++ ){
      for( int b = 0; b < numberWaves; b++ )
	outputFp << ACCNI[a][b] << " ";
      outputFp << endl;
    }

    //
    // Save Raw Normalization Integrals
    //
    outputFp << "RawNI: " << numberRawMcEvents << endl ;
    outputFp << numberWaves << endl;
    for( int a = 0; a < numberWaves; a++ ){
      for( int b = 0; b < numberWaves; b++ )
	outputFp << RAWNI[a][b] << " ";
      outputFp << endl;
    }

  }// end *iflag == 3
  
  
  
  
}// end of fcn()






