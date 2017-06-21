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
 * Edited by Hussein Al Ghoul, FSU July 2015
 **********************************************/

#include <iostream>
#include <fstream>
#include <complex>
#include <vector>

using namespace std;
#include "TApplication.h"

#include "pwaFitView_Gui.h"
#include "PwaFitResults.h"


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

int main(int argc,char **argv){
  int argcc = argc;
 char *argptr; 
 cerr << "argc is " << argc << endl;
  for (int i = 0; i < argc; i++ ){
    argptr = argv[i];
    cerr << "argv[" << i << "] is " << argptr << endl;
  }

 
  char fitFile[128];
  char outputFile[128] = "fit.dat";
  char fitlistfile[128];
  ofstream outputFp;
  ifstream fitFp;
  ifstream fitlistf;
  bool useDebug = false;  
  bool useFixedInputParms = true;
	int z=0;

  int long maxBins = 1500;
    int long Bins=0;
       // PwaFitResults *PwaResults = new PwaFitResults;
 	// PwaResults->numberBins = numberBins;

  PwaFitResult*** PwaResultall = new PwaFitResult**[10];
	char addresses[100][20];
        std::string fitfilename[50];
	fitlistf.open( argptr, ios::in);
	int filesnumber;
fitlistf>>filesnumber;
  int long numberBins[filesnumber];
	int long counter;
  PwaFitResult *PwaResult[filesnumber][filesnumber*1000];
  PwaFitResult *PwaFitresultall[filesnumber][filesnumber*1000];
  cerr.precision(10);
  cerr << "argc is " << argc << endl;
  for (int i = 0; i < argcc; i++ ){
    argptr = argv[i];
    cerr << "argv[" << i << "] is " << argptr << endl;
  }

  if (argc == 1){
    printUsage(argv[0]);
    exit (0);
  }

  for (int i = 1; i < argc; i++ ){
    argptr = argv[i];
    if( useDebug ) cerr << "argv[" << i << "] is " << argptr << endl;
    if ( *argptr == '-' ) {
      argptr++;
      switch (*argptr) {
      case 'N':
	useFixedInputParms = false;
	break;
      case 'd':
	useDebug = true;
	cerr << "Printing Degub Info" << endl;;
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
      case 'l':
	sprintf(fitlistfile,"%s",++argptr);
	cerr << "fits list file: " << fitlistfile << endl;
	fitlistf.open( fitlistfile, ios::out );
	break;

      default:
	fprintf(stderr,"Unrecognized argument -%s\n\n",argptr);
	printUsage(argv[0]);    
	exit(-1);
	break;
      }
   
    }
  
    
      if( useDebug ) cerr << "opening input file: " << argptr << endl;

	const char* fitfname[10]={};

	for( int n=0 ; n<filesnumber ; n++)
{
        PwaResultall[n] = new PwaFitResult*[maxBins];
}
	for( int m=0 ; m<filesnumber ; m++){
          //int m = 0;
	fitfilename[m].clear();
	fitlistf>>fitfilename[m];







	//fitfname[m] = fitfilename[m].c_str();

// cout << fitfilename[m]<<endl;
}  
	int count=0;	//counter=0;	
    string s;
    string massString("Mass:");
  	for( int z=0 ; z<filesnumber ; z++){
  fitFp.open( fitfilename[z].c_str() );
		numberBins[z]=0;
		counter =0;


 // cout<<"opening "<< fitfname[z] <<endl;
  	
/*	std::string file;
	fitlistf>>filesnumber;
	//fitlistf>>file;
	//cout<<"number of files : " << filesnumber << "file name : " << file << endl;
	//std::string filename("file:");


	while(!fitlistf.eof()){
	
	std:string line;
       fitlistf>>line;
       fitFp.open( line.c_str());
*/
 // cout<<"opening "<< line <<endl;
 
    //
    // Read mass bin and minusLogLikelihood
    //

    fitFp >> s; //"Mass:"
    
    while( s == massString ){
          
      if( useDebug )
	cerr << "s is \"" << s << "\"" << endl;
      
      double massBin, minusLogLikelihood;
      unsigned long numberDataEvents;
      fitFp >> massBin;
      double GeVperMeV = 0.001;
      massBin = massBin * GeVperMeV; 
      fitFp >> minusLogLikelihood;      
      fitFp >> numberDataEvents;

      if( useDebug ) cerr << " Mass: " << massBin << " minusLogLikelihood: " << minusLogLikelihood << " #events: " << numberDataEvents << endl;
     // count++;
	//cout<<"bin number "<<count<<endl;
      
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
	cerr << "Done reading bin[" << numberBins[z] << "]" << endl;
      
      
      PwaResult[z][numberBins[z]] =  new PwaFitResult( massBin, minusLogLikelihood, numberDataEvents, numberAccMcEvents, numberRawMcEvents, numberRanks, numberWaves, wave, Vamp, errorMatrix, accNI, rawNI);
      
      if( useDebug ) PwaResult[z][numberBins[z]]->printProductionAmps();
      if( useDebug ) PwaResult[z][numberBins[z]]->dumpWaveSets();
      


      //cout << massBin << " " << real(accNI[i][i])  <<endl  ;
//cout << " " << PwaResult[numberBins]->phaseDifError(0,1) << " z: "<< z;
 
     // cout << endl;
				 //PwaFitresultall[z][numberBins-counter] = PwaResult[numberBins];
		
	//cout<<"number of bin " << numberBins[z]<<" massbin "<<PwaResult[z][numberBins[z]]->massBin()<<endl;
	//cout<<"number of raw events "<<numberRawMcEvents<<endl;
      numberBins[z]++;
	counter = numberBins[z];
	//counter++;
      fitFp >> s; //"Mass:"

    }
	

	cout<<"counter is "<<counter<<endl;


//cout<<"check if z loop works: " << PwaResultall[z][1]->massBin()<<" and compare to"<<endl;
fitFp.close();	  
  }

}

fitlistf.close();


			for(int i=0 ; i<filesnumber ; i++){
cout<< "number of bins is " <<numberBins[i]<<endl;
				for(int j=0 ; j<numberBins[i] ; j++){
		 PwaFitresultall[i][j] = PwaResult[i][j];

					}
						}


 PwaFitResults *PwaResults = new PwaFitResults;


		for(int i=0; i<filesnumber; i++){
  PwaResults->numberBins[i] = numberBins[i];
 	cout<<"number of bins "<<PwaResults->numberBins[i]<<endl;
			for(int j=0 ; j<numberBins[i]; j++){

  PwaResults->fitResult[i][j] = PwaFitresultall[i][j];

  //cout<<i<<" : "<<j<<" mass "<<PwaFitresultall[i][j]->massBin()<<"compare to: "<<PwaResults->fitResult[i][j]->massBin() <<endl;
		}

				}

	//cout<<"maxbins "<<PwaResults->fitResult[1][21]->massBin();
  if( useDebug ) cerr << "Done reading fit file" << endl; 
  TApplication theApp("App",&argc,argv);
  MyMainFrame *PwaViewerMainFrame =  new MyMainFrame(gClient->GetRoot(), 800, 800, filesnumber , PwaResults, argptr);
  theApp.Run();

}// end of main()







