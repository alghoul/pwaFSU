#ifndef _PWA_FIT_VIEW
#define _PWA_FIT_VIEW


/*********************************************
 *
 * pwaFitResults.h :
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




class PwaFitResult{
private:
  int iFileNumber;
  double iMassBin;
  int iNumberBins;
  double iMinusLogLikelihood;
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
  vector<PwaWaveSet> waveSet;
public:
  WaveList *iWave;
  PwaFitResult(double aMassBin, double aMinusLogLikelihood, unsigned long aNumberDataEvents,unsigned long aNumberAccMcEvents, unsigned long aNumberRawMcEvents, int aNumberRanks, int aNumberWaves, WaveList *aWaves, complex<double> **aV, double **aErrorMatrix,  complex<double> **aAccNI, complex<double> **aRawNI );
  void setMassBin( const double aMassBin ) { iMassBin = aMassBin; }
  void setMinusLogLikelihood( const double aMinusLogLikelihood ) { iMinusLogLikelihood = aMinusLogLikelihood; }
  void setNumberRanks( const int aNumber ) { iNumberRanks = aNumber; }
  void setNumberWaves( const int aNumber ) { iNumberWaves = aNumber; }
  void setNumberWaveSets( const int aNumber ) { iNumberWaveSets = aNumber; }
  void setNumberDataEvents( const int aNumber ) { iNumberDataEvents = aNumber; }
  void setNumberAccMcEvents( const int aNumber ) { iNumberAccMcEvents = aNumber; }
  void setNumberRawMcEvents( const int aNumber ) { iNumberRawMcEvents = aNumber; }
  double minusLogLikelihood() const { return iMinusLogLikelihood ;}
  double massBin() const { return iMassBin ; }

  int numberBins(int filenumber) const {return iNumberBins;}

  unsigned long numberDataEvents() const { return iNumberDataEvents; }
  unsigned long numberAccMcEvents() const { return iNumberAccMcEvents; }
  unsigned long numberRawMcEvents() const { return iNumberRawMcEvents; }
  int numberRanks() const { return iNumberRanks; }
  int numberWaves() const { return iNumberWaves; }
  int numberWaveSets() const { return iNumberWaveSets; }
  double errorMatrix( int iRow, int iColumn )const { return iErrorMatrix[iRow][iColumn]; }
  int incrementNumberWaveSets() { iNumberWaveSets++; }
  void setWaveList();
  int getWaveIndex( const char *aWaveName );
  void dumpWaveSets();
  bool isInterfering( int a, int b);
  void printProductionAmps();
  double phaseDif( int a, int b );
  double phaseDifError( int a, int b );
  double phaseDiffError( int a, int b );
  double phaseDifError(double a, double b, double c, double d, double **eMat );
  double phaseDiffError(double A, double B, double a, double b, double c, double d, double **eMat );
  double yield( int aNumberWaves, int *aWaves );
  double yieldError( int aNumberWaves, int *aWaves );
  double yieldErrorTerm( int aWave, int bWave, double a, double b, double c, double d, double **eMat  );
  double yieldErrorTotal(int aNumberWaves,double **J , double **eMat  );
};


class PwaFitResults{
 public:
  int numberBins[10];
  int numberfile;
  PwaFitResult *fitResult[10][1500];
  double maxBin(int filenumber);
  double minBin(int filenumber);
};


	

#endif




