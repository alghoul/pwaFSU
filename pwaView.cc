/*********************************************
 *
 * pwaView.c : The FSU pwa fit view program
 *   
 *
 * 
 *
 * Paul Eugenio
 * Florida State University
 * April 2011
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
  unsigned int iNumberWaves;
  unsigned int iIndex;
  int iEps;
  PwaWaveSet(){;}
  PwaWaveSet(int aEps, unsigned int aNumberWaves, unsigned int aIndex);
  void setEps( const int aEps){ iEps = aEps;}
  void setIndex( const int aIndex){ iIndex = aIndex;}
  void setNumberWaves( const int aNumberWaves ){ iNumberWaves = aNumberWaves;}
  int eps() const { return iEps; }
  unsigned int index() const { return iIndex; }
  unsigned int numberWaves() const { return iNumberWaves; }
};

PwaWaveSet::PwaWaveSet(int aEps, unsigned int aNumberWaves, unsigned int aIndex){
  setEps( aEps );
  setNumberWaves( aNumberWaves );
  setIndex( aIndex );
}


class PwaFitResult{
private:
  int iNumberRanks;
  int iNumberWaves;
  int iNumberWaveSets;
  unsigned long  iNumberDataEvents;
  unsigned long  iNumberAccMcEvents;
  unsigned long  iNumberRawMcEvents;
  complex<double>  **iV;
  double **iErrorMatrix;
  complex<double> **iAccNI;
  complex<double> **iRawNI;
  WaveList *iWave;
  vector<PwaWaveSet> waveSet;
public:
  PwaFitResult( unsigned long aNumberDataEvents,unsigned long aNumberAccMcEvents, unsigned long aNumberRawMcEvents, int aNumberRanks, int aNumberWaves, WaveList *aWaves, complex<double> **aV, double **aErrorMatrix,  complex<double> **aAccNI, complex<double> **aRawNI );
  void setNumberRanks( const int aNumber ) { iNumberRanks = aNumber; }
  void setNumberWaves( const int aNumber ) { iNumberWaves = aNumber; }
  void setNumberWaveSets( const int aNumber ) { iNumberWaveSets = aNumber; }
  void setNumberDataEvents( const int aNumber ) { iNumberDataEvents = aNumber; }
  void setNumberAccMcEvents( const int aNumber ) { iNumberAccMcEvents = aNumber; }
  void setNumberRawMcEvents( const int aNumber ) { iNumberRawMcEvents = aNumber; }
  unsigned long numberDataEvents() const { return iNumberDataEvents; }
  unsigned long numberAccMcEvents() const { return iNumberAccMcEvents; }
  unsigned long numberRawMcEvents() const { return iNumberRawMcEvents; }
  int numberRanks() const { return iNumberRanks; }
  int numberWaves() const { return iNumberWaves; }
  int numberWaveSets() const { return iNumberWaveSets; }
  double errorMatrix( int iRow, int iColumn )const { return iErrorMatrix[iRow][iColumn]; }
  int incrementNumberWaveSets() { iNumberWaveSets++; }
  void setWaveList();
  void dumpWaveSets();
  bool isInterfering( int a, int b);
  void printProductionAmps();
  double phaseDif( int a, int b );
  double phaseDifError( int a, int b );
  double phaseDifError(double a, double b, double c, double d, double **eMat );
  double yield( int aNumberWaves, int *aWaves );
  double yieldError( int aNumberWaves, int *aWaves );
  double yieldErrorTerm( int aWave, int bWave, double a, double b, double c, double d, double **eMat  );
};

PwaFitResult::PwaFitResult( unsigned long aNumberDataEvents,unsigned long aNumberAccMcEvents, unsigned long aNumberRawMcEvents, int aNumberRanks, int aNumberWaves, WaveList *aWave, complex<double> **aV, double **aErrorMatrix,  complex<double> **aAccNI, complex<double> **aRawNI ){

  setNumberDataEvents( aNumberDataEvents );
  setNumberAccMcEvents( aNumberAccMcEvents );
  setNumberRawMcEvents( aNumberRawMcEvents );
  setNumberRanks( aNumberRanks );
  setNumberWaves( aNumberWaves );
  setNumberWaveSets( 0 );
  iWave = aWave;
  setWaveList();
  iV = aV;
  iErrorMatrix = aErrorMatrix;
  iAccNI = aAccNI;
  iRawNI = aRawNI;
}

void PwaFitResult::dumpWaveSets(){
  for( int ws =0; ws < numberWaveSets(); ws++ )
    cerr << "WaveSet[" << ws << "] ( eps, number, index) is ( " << waveSet[ws].eps() << ", " <<  waveSet[ws].numberWaves() << ", " << waveSet[ws].index() << " )" << endl;  
}

void PwaFitResult::setWaveList(){
  bool isSetUp = false;
  //cerr << " setting wavelist for " << numberWaves() << " waves" << " with " << numberWaveSets() << " wavesets" <<endl;
  for( int a = 0; a < numberWaves(); a++ ){
    //cerr << "wave " << a << " wavset " << numberWaveSets() << endl;
 
    if( numberWaveSets() == 0 ){
      //cerr << " adding waveset" << endl;
     incrementNumberWaveSets();
     PwaWaveSet ws( iWave[a].eps, 1, a );
     waveSet.push_back(ws);
    }

   if( isSetUp ){ 
      if( waveSet[ numberWaveSets() -1 ].eps() == iWave[a].eps ){
	waveSet[ numberWaveSets() - 1 ].setNumberWaves( waveSet[ numberWaveSets() - 1 ].numberWaves() + 1  ); // increment #waves in waveset
	//cerr << " waveSet[ numberWaveSets() -1 ].eps() == iWave[a].eps " << endl;
      } else {
	//cerr << " adding waveset" << endl;
      incrementNumberWaveSets();
      PwaWaveSet ws( iWave[a].eps, 1, a );
      waveSet.push_back(ws);
      }
   }
   isSetUp = true;
  }
}

bool PwaFitResult::isInterfering( int a, int b){

  if( iWave[a].eps == iWave[b].eps )
    return true;
  else
    return false;

}

double PwaFitResult::yield( int aNumberWaves, int *aWave ){
  double y=0;
  
  for( int a = 0; a < aNumberWaves; a++ )
    for( int b = 0; b < aNumberWaves; b++ ){
      if( isInterfering( aWave[a], aWave[b] ) )
	 for( int k = 0; k < numberRanks(); k++)
	   y += numberDataEvents() * ( numberRawMcEvents() / numberAccMcEvents() ) * real( iV[k][ aWave[a] ] * conj( iV[k][ aWave[b] ] ) * iRawNI[ aWave[a] ][ aWave[b] ] );
    }
  return y;
}

double PwaFitResult::yieldError( int aNumberWaves, int *aWave ){
  int dim = 4;
  double errsq = 0;
  double **eMat = new double * [dim];
  for( int n = 0; n < dim; n++ )
    eMat[n] = new double [dim];

  for( int a = 0; a < aNumberWaves; a++ )
    for( int b = 0; b < aNumberWaves; b++ ){
      if( isInterfering( aWave[a], aWave[b] ) )
	for( int k = 0; k < numberRanks(); k++){
	  /////// set eMat[][]
	  double element;
	  for( int i = 0; i < 2; i++){
	    for( int j = 0; j < 2; j++ )
	      eMat[i][j] = errorMatrix( 2*aWave[a]+i + k*numberWaves(), 2*aWave[a] + j + k*numberWaves() );
	    for( int j = 0; j < 2; j++ )
	      eMat[i][j+2] = errorMatrix( 2*aWave[a]+i + k*numberWaves(), 2*aWave[b] + j + k*numberWaves() );
	  }
	  for( int i = 0; i < 2; i++){
	    for( int j = 0; j < 2; j++ )
	      eMat[i+2][j] = errorMatrix( 2*aWave[b]+i + k*numberWaves(), 2*aWave[a] + j + k*numberWaves() );
	    for( int j = 0; j < 2; j++ )
	      eMat[i+2][j+2] = errorMatrix( 2*aWave[b]+i + k*numberWaves(), 2*aWave[b] + j + k*numberWaves() );
	  }

	  errsq += yieldErrorTerm( a, b, iV[k][ aWave[a] ].real(), iV[k][ aWave[a] ].imag(), conj(iV[k][ aWave[b] ]).real(), conj( iV[k][ aWave[b] ] ).imag(), eMat );
	}
    }
  
  for( int n = 0; n < dim; n++ )
    delete [] eMat[n];
  delete [] eMat;
  return sqrt( errsq );
}


double PwaFitResult::yieldErrorTerm( int aWave, int bWave, double a, double b, double c, double d, double **eMat  ){
  
  int dim = 4;
  double err = 0;
  double deriv[4];
  complex<double> im(0,1);
  complex<double> cn1(c,-d);
  complex<double> cn2(a,b);
  double norm = double(numberDataEvents()) * ( double(numberRawMcEvents()) / double(numberAccMcEvents()) );
  complex<double> Knorm(norm,0);
  complex<double> Konst =  iRawNI[aWave][bWave] * Knorm; 
  deriv[0] =  real( cn1 * Konst );
  deriv[1] =  real(im * cn1 * Konst);
  deriv[2] =  real( cn2 * Konst );
  deriv[3] =  real( -im * cn2 * Konst );

  for( int i = 0; i < dim; i++ )
    for( int j = 0; j < dim; j++ ){
      err +=  deriv[i] * eMat[i][j] * deriv[j]   ;
    }
  return  fabs( err );

}


double PwaFitResult::phaseDif( int a, int b ){
  complex<double> ca(0,0), cb(0,0), cc(1,0);
  if( isInterfering( a, b ) ){
    for( int k = 0; k < numberRanks(); k++ ){
      ca += iV[k][a];
      cb += iV[k][b];
    }
    cc = ca / cb;
  }
  return arg( cc );
 }


double PwaFitResult::phaseDifError( int aWave, int bWave ){
  int dim = 4;
  double a=0, b=0, c=0, d=0;
  double **eMat = new double *[dim];
  for( int n = 0; n < dim; n++)
    eMat[n] = new double [dim];
  //
  // get real & imag parts and eMat
  for( int k = 0; k < numberRanks(); k++ ){
    a += iV[k][aWave].real();
    b += iV[k][aWave].imag();
    c += iV[k][bWave].real();
    d += iV[k][bWave].imag();
  }
  double element;
  /*************************
  for( int i = 0; i < dim; i++ )
    for( int j = 0; j < dim; j++ ){
      element = 0;
      for( int k = 0; k < numberRanks(); k++ ){
	int row = (k+1)*(2*aWave + i);
	int column = (k+1)*(2*bWave + j);
	//cerr << " i, j, k :" << i << " " << j << " " << k << endl;
	//cerr << " aWave, bWave: " << aWave << " " << bWave << endl;
	//cerr << "row , column: " << row << " " << column << endl;
	element += errorMatrix( row, column ) ;//THIS NEEDS TO BE CHECKED
      }
      //cerr << "eMat[i][j] is " << element << endl;
      eMat[i][j] = element; 
      //cerr << "eMat[i][j] is " << eMat[i][j] << endl;
    }
  ****************************/
  
  /////// set eMat[][]
  int rank = 0;////WORKS ONLY FOR RANK 1111111!!!!
  for( int i = 0; i < 2; i++){
    for( int j = 0; j < 2; j++ )
      eMat[i][j] = errorMatrix( 2*aWave +i + rank*numberWaves(), 2*aWave + j + rank*numberWaves() );
    for( int j = 0; j < 2; j++ )
      eMat[i][j+2] = errorMatrix( 2*aWave +i + rank*numberWaves(), 2*bWave + j + rank*numberWaves() );
  }
  for( int i = 0; i < 2; i++){
    for( int j = 0; j < 2; j++ )
      eMat[i+2][j] = errorMatrix( 2*bWave +i + rank*numberWaves(), 2*aWave + j + rank*numberWaves() );
    for( int j = 0; j < 2; j++ )
      eMat[i+2][j+2] = errorMatrix( 2*bWave +i + rank*numberWaves(), 2*bWave + j + rank*numberWaves() );
  }


  ////
  double val = phaseDifError( a, b, c, d, eMat ); 
   for( int n = 0; n < dim; n++)
     delete [] eMat[dim];
   delete [] eMat;
  return val;
}

double PwaFitResult::phaseDifError(double a, double b, double c, double d, double **eMat ){
  int dim = 4;
  double errsq = 0;
  double deriv[4];
  double dbottom = a*a - 2.0*a*c + b*b - 2.0*b*d + c*c + d*d;
  deriv[0] = (d - b) / dbottom;
  deriv[1] = (a - c) / dbottom;
  deriv[2] = - deriv[0];
  deriv[3] = - deriv[1];
  
  
  for( int i = 0; i < dim; i++ )
    for( int j = 0; j < dim; j++ ){
      // cerr << "i,j: " << i << ", " << j
      //   << " deriv[i]*eMat[i][j]*deriv[j] " << deriv[i] <<  "*" << endl; //eMat[i][j] << "*" << eriv[j] << endl;
      errsq += fabs( deriv[i] * eMat[i][j] * deriv[j] );
      
    }
  return sqrt ( errsq );
      
}


void PwaFitResult::printProductionAmps(){
  for( int k = 0; k < numberRanks(); k++ )
    for( int a = 0; a < numberWaves(); a++ )
      cerr << "Vamp[" << k << "][" << a << "] = " << iV[k][a] << endl;
}



/***********************
 * PROTOTYPE FUNCTIONS
 ***********************/

int printUsage(char *processName)
{
  
  fprintf(stderr,"%s usage: [switches]   \n",processName);
  cerr << "\t-N \t\t Fit results did not fix any fit parameters" << endl;
  fprintf(stderr,"\t-h \t\t Print this help message\n\n");
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
  char *argptr;
  char fitFile[128];
  char outputFile[128] = "fit.dat";
  ofstream outputFp;
  ifstream fitFp;
  bool useDebug = false;  
  bool useFixedInputParms = true;


  cerr.precision(10);


  if (argc == 1){
    printUsage(argv[0]);
    exit (0);
  }
  
  for (int i = 1; i < argc; i++ ){
    argptr = argv[i];
    if ( *argptr == '-' ) {
      argptr++;
      switch (*argptr) {
      case 'N':
	useFixedInputParms = false;
	break;
      case 'd':
	useDebug = true;
	break;
      case 'h':
	printUsage(argv[0]);
	exit(0);
	break;
      case 'o':
	sprintf(outputFile,"%s",++argptr);
	cerr << "savingf fit result to file: " << outputFile << endl;
	outputFp.open( outputFile, ios::out );
	break;
      default:
	fprintf(stderr,"Unrecognized argument -%s\n\n",argptr);
	printUsage(argv[0]);    
	exit(-1);
	break;
      }
    }
    
    fitFp.open( argptr, ios::in );
  
  
    
    //
    // Read mass bin and minusLogLikelihood
    //
    string s;
    string massString("Mass:");
    fitFp >> s; //"Mass:"
    
    while( s == massString ){
          
      if( useDebug )
	cerr << "s is \"" << s << "\"" << endl;
      
      double massBin, minusLogLikelihood;
      unsigned long numberDataEvents;
      fitFp >> massBin;
      fitFp >> minusLogLikelihood;      
      fitFp >> numberDataEvents;

      if( useDebug ) cerr << " Mass: " << massBin << " minusLogLikelihood: " << minusLogLikelihood << " #events: " << numberDataEvents << endl;
      
      
      //
      // Read wavelist
      //
      int numberWaves, numberRanks, numberWaveSets;
      fitFp >> numberWaves;
      fitFp >> numberRanks;
      fitFp >> numberWaveSets;
      int offset = 0;
      PwaWaveSet *waveSet = new PwaWaveSet [numberWaveSets];
      for( int ws = 0; ws < numberWaveSets; ws++){
	unsigned int nwaves;
	fitFp >> nwaves;
	waveSet[ws].setNumberWaves( nwaves );
	waveSet[ws].setIndex( offset );
	offset += waveSet[ws].numberWaves();
      }
      if( numberWaves != offset ){
	cerr << "Wavelist file error: numberWaves != number of waves in all sets!" << endl;
	exit(-1);
      }
      //vector<WaveList> wave;
      //wave.resize( numberWaves );
      WaveList *wave = new WaveList [numberWaves];
      

      for( int i = 0; i < numberWaves; i++){
	fitFp >> wave[i].name;
	fitFp >> wave[i].eps;
      }
      
      if( useDebug){
	cerr << "found " << numberWaves <<" waves of rank" << numberRanks  << endl;
	for( int i = 0; i < numberWaves; i++)
	  cerr << wave[i].name << " "
	       << wave[i].eps << endl;
      }
      
      //
      // Read production amplitudes
      //
      fitFp >> s; //"V:"
      int numRanks, numWaves;
      fitFp >> numRanks;
      fitFp >> numWaves;
      if( numRanks != numberRanks || numWaves != numberWaves ){
	cerr << "ERROR! number ranks/waves in wavelist does not equal number ranks/waves from production amp list" << endl;
	cerr << "numRanks: " << numRanks << "numberRanks: " << numberRanks << endl;
	cerr << "numWaves: " << numWaves << "numberWaves: " << numberWaves << endl;
	exit( -1 );
      }
      
      complex<double> **Vamp = new complex<double>* [numRanks];
      for( int k = 0; k < numRanks; k++ )
	Vamp[k] = new complex<double> [numWaves];
      
      for( int k = 0; k < numRanks; k++ )
	for( int a = 0; a < numWaves; a++ ){
	  fitFp >> Vamp[k][a];
	  if( useDebug )
	    cerr << "Vamp[" << k << "][" << a << "] = " << Vamp[k][a] << endl;
	}
      
      //
      // Read error matrix
      //
      fitFp >> s; //"ERMAT:"
      int sizeErrorMatrix;
      fitFp >> sizeErrorMatrix;
      fitFp >> sizeErrorMatrix;
      
      double **errorMatrix = new double* [sizeErrorMatrix];
      for( int n = 0; n < sizeErrorMatrix; n++ )
	errorMatrix[n] = new double [sizeErrorMatrix];
      for( int i = 0; i < sizeErrorMatrix; i++ )
	for( int j = 0; j < sizeErrorMatrix; j++ ){
	  fitFp >> errorMatrix[i][j];
	  if( useDebug )
	    cerr << "errorMatrix[" << i << "][" << j << "] = " << errorMatrix[i][j] << endl;;
	}
      
      
      //
      // Read Acc NI
      //
      fitFp >> s; //"AccNI:"
      if( useDebug ) cerr << "AccNI: reads as " << s << endl;
      unsigned long numberAccMcEvents;
      fitFp >> numberAccMcEvents;
      int sizeNI;
      fitFp >> sizeNI;
      if( useDebug ) cerr << "sizeNI is " << sizeNI << endl;
      if( sizeNI != numberWaves){
	cerr << "Error: sizeNI != numberWaves" << endl;
	exit( -1 );
      }
      
      complex<double> **accNI = new complex<double>* [sizeNI]; 
      for( int n = 0; n < sizeNI; n++ )
	accNI[n] = new complex<double> [sizeNI];
      for( int i = 0; i < sizeNI; i++ )
	for( int j = 0; j < sizeNI; j++ ){
	  fitFp >> accNI[i][j];
	  if( useDebug )
	    cerr << "accNI[" << i << "][" << j << "] = " << accNI[i][j] << endl;;
	}
      
      //
      // Read Raw NI
      //
      fitFp >> s; //"RawNI:"
      unsigned long numberRawMcEvents;
      fitFp >> numberRawMcEvents;
      int sizeRawNI;
      fitFp >> sizeRawNI;
      if( sizeRawNI != numberWaves){
	cerr << "Error: sizeRawNI != numberWaves" << endl;
	exit( -1 );
      }
      
      complex<double> **rawNI = new complex<double>* [sizeRawNI]; 
      for( int n = 0; n < sizeRawNI; n++ )
	rawNI[n] = new complex<double> [sizeRawNI];
      for( int i = 0; i < sizeRawNI; i++ )
	for( int j = 0; j < sizeRawNI; j++ ){
	  fitFp >> rawNI[i][j];
	  if( useDebug )
	    cerr << "rawNI[" << i << "][" << j << "] = " << rawNI[i][j] << endl;;
	}
  
    
      
      
      //
      // Now do something
      //
      if( useDebug )
	cerr << "Done reading fit file" << endl;
      
      
      PwaFitResult PwaResult( numberDataEvents, numberAccMcEvents, numberRawMcEvents, numberRanks, numberWaves, wave, Vamp, errorMatrix, accNI, rawNI);
      
      if( useDebug ) PwaResult.printProductionAmps();
      if( useDebug ) PwaResult.dumpWaveSets();
      
      cout << massBin << " " << minusLogLikelihood ;

      //
      // get yields
      //
      int nwaves = numberWaves;
      int *ww = new int [nwaves];
      for( int a = 0; a < numberWaves; a++ )
	ww[a] = a;
      cout << " " << PwaResult.yield( nwaves, ww ) << " " << PwaResult.yieldError( nwaves, ww );
      // invidual wave yields
      nwaves = 1;
      for( int a = 0; a < numberWaves; a++ ){
	ww[0] = a;
	//cout << " " << PwaResult.yield( nwaves, ww ) << " " << PwaResult.yieldError( nwaves, ww );
      }
      
      nwaves = 1;
      ww[0] = 3;
      cout << " " <<  PwaResult.yield( nwaves, ww ) << " " << PwaResult.yieldError( nwaves, ww ); 
      ww[0] = 13;
      cout << " " <<  PwaResult.yield( nwaves, ww ) << " " << PwaResult.yieldError( nwaves, ww );
      ww[0] = 8;
      cout << " " <<  PwaResult.yield( nwaves, ww ) << " " << PwaResult.yieldError( nwaves, ww );
      ww[0] = 17;
      cout << " " <<  PwaResult.yield( nwaves, ww ) << " " << PwaResult.yieldError( nwaves, ww );
      ww[0] = 6;
      cout << " " <<  PwaResult.yield( nwaves, ww ) << " " << PwaResult.yieldError( nwaves, ww );
      ww[0] = 14;
      cout << " " <<  PwaResult.yield( nwaves, ww ) << " " << PwaResult.yieldError( nwaves, ww );
      ww[0] = 0;
      cout << " " <<  PwaResult.yield( nwaves, ww ) << " " << PwaResult.yieldError( nwaves, ww );
      ww[0] = 9;
      cout << " " <<  PwaResult.yield( nwaves, ww ) << " " << PwaResult.yieldError( nwaves, ww );
      



      cout << " " << PwaResult.phaseDif( 3, 8 ) << " " << PwaResult.phaseDifError( 3, 8);
      cout << " " << PwaResult.phaseDif( 13, 17 ) << " " << PwaResult.phaseDifError( 13, 17);

      cout << " " << PwaResult.phaseDif( 3, 6 ) << " " << PwaResult.phaseDifError( 3, 6);
      cout << " " << PwaResult.phaseDif( 13, 14 ) << " " << PwaResult.phaseDifError( 13, 14);
      
      cout << " " << PwaResult.phaseDif( 8, 6 ) << " " << PwaResult.phaseDifError( 8, 6);
      cout << " " << PwaResult.phaseDif( 17, 14 ) << " " << PwaResult.phaseDifError( 17, 14);

      cout << " " << PwaResult.phaseDif( 0, 6 ) << " " << PwaResult.phaseDifError( 0, 6);
      cout << " " << PwaResult.phaseDif( 9, 14 ) << " " << PwaResult.phaseDifError( 9, 14);

      cout << " " << PwaResult.phaseDif( 0, 3 ) << " " << PwaResult.phaseDifError( 0, 3);
      cout << " " << PwaResult.phaseDif( 9, 13 ) << " " << PwaResult.phaseDifError( 9, 13);

      //
      // get phase difference
      //
      for( int a = 0; a < 10 ; a++ )
	for( int b = a + 1; b < 10 ; b++)
	  if( PwaResult.isInterfering( a, b) )
	    ;//cout << " " << PwaResult.phaseDif( a, b ) << " " << PwaResult.phaseDifError( a, b);

      

      ww[0] = 5;
      cout << " " <<  PwaResult.yield( nwaves, ww ) << " " << PwaResult.yieldError( nwaves, ww );
      ww[0] = 16;
      cout << " " <<  PwaResult.yield( nwaves, ww ) << " " << PwaResult.yieldError( nwaves, ww );
      
      /*****************  
      ww[0] = 2; ww[1] = 3; ww[2] = 10; ww[3] = 11; ww[4] = 12; ww[5] = 13;// 1++
      nwaves = 6;
      cout << " " <<  PwaResult.yield( nwaves, ww ) << " " << PwaResult.yieldError( nwaves, ww );

      ww[0] = 8; ww[1] = 17; // 2++
      nwaves = 2;
      cout << " " <<  PwaResult.yield( nwaves, ww ) << " " << PwaResult.yieldError( nwaves, ww );

      ww[0] = 4; ww[1] = 5; ww[2] = 6; ww[3] = 7; ww[4] = 14; ww[5] = 15; ww[6] = 16;// 2-+
      nwaves = 7;
      cout << " " <<  PwaResult.yield( nwaves, ww ) << " " << PwaResult.yieldError( nwaves, ww );

      ww[0] = 0; ww[1] = 9; // 1-+
      nwaves = 2;
      cout << " " <<  PwaResult.yield( nwaves, ww ) << " " << PwaResult.yieldError( nwaves, ww );
      **********/

      ww[0] = 18; //background
      nwaves = 1;
      cout << " " <<  PwaResult.yield( nwaves, ww ) << " " << PwaResult.yieldError( nwaves, ww );




      cout << endl;
      
      fitFp >> s; //"Mass:"
      
    }
    fitFp.close();
  }
}// end of main()









