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

#include "PwaFitResults.h"


PwaWaveSet::PwaWaveSet(int aEps, unsigned int aNumberWaves, unsigned int aIndex){
  setEps( aEps );
  setNumberWaves( aNumberWaves );
  setIndex( aIndex );
}

double PwaFitResults::maxBin(int j ){
  double maxBin = 0;
  for (int n = 0; n < this->numberBins[j]; n++)
    if( this->fitResult[j][n]->massBin() > maxBin )
      maxBin =  this->fitResult[j][n]->massBin();
  return maxBin;
}

double PwaFitResults::minBin(int j ){
  double minBin = this->fitResult[j][0]->massBin();
  for (int n = 0; n < this->numberBins[j]; n++)
    if( this->fitResult[j][n]->massBin() < minBin )
      minBin =  this->fitResult[j][n]->massBin();
  return minBin;
}





PwaFitResult::PwaFitResult( double aMassBin, double aMinusLogLikelihood, unsigned long aNumberDataEvents,unsigned long aNumberAccMcEvents, unsigned long aNumberRawMcEvents, int aNumberRanks, int aNumberWaves, WaveList *aWave, complex<double> **aV, double **aErrorMatrix,  complex<double> **aAccNI, complex<double> **aRawNI ){
  setMinusLogLikelihood( aMinusLogLikelihood );
  setMassBin( aMassBin );
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



	




int PwaFitResult::getWaveIndex( const char * aWaveName){
  int windex = -1; 
  string w( aWaveName );
  for( int i = 0; i < numberWaves(); i++ ){
    if( w == iWave[i].name )
      windex = i;
  }
  if( windex == -1 ){
    cerr << "ERROR! PwaFitResult::getWaveIndex() failed to find wavename" << endl;
    //exit(-1);
  }
  return windex;
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
  
  for( int a = 0; a < aNumberWaves; a++ ){
    for( int b = 0; b < aNumberWaves; b++ ){
      if( isInterfering( aWave[a], aWave[b] ) )
	 for( int k = 0; k < numberRanks(); k++)
	   y += numberDataEvents() * ( numberRawMcEvents() / numberAccMcEvents() ) * real( iV[k][ aWave[a] ] * conj( iV[k][ aWave[b] ] ) * iRawNI[ aWave[a] ][ aWave[b] ] );
    }
}
  return y;
}



double PwaFitResult::yieldError( int aNumberWaves, int *aWave ){
  double errsq = 0;
  double coef = double(numberDataEvents()) * ( double(numberRawMcEvents()) / double(numberAccMcEvents()) );
 double **J = new double*[1];
  for( int n = 0; n <1; n++ ){
    J[n] = new double [2*aNumberWaves];
	}

int k;
  double **eMat = new double * [2*aNumberWaves];
  for( int n = 0; n < 2*aNumberWaves; n++ ){
    eMat[n] = new double [2*aNumberWaves];
}

  for( int a = 0; a < aNumberWaves; a++ ){
    for( int b = 0; b < aNumberWaves; b++ ){
	for( int k = 0; k < numberRanks(); k++){
	//k=0;
	  /////// set eMat[][]

	  double element;
	  for( int i = 0; i < 2; i++){
	    for( int j = 0; j < 2; j++ ){
	      eMat[i+(a*2)][j+(b*2)] = errorMatrix( 2*aWave[a]+i + k*numberWaves(), 2*aWave[b] + j + k*numberWaves() );
		//cout<<aWave[a]<<" "<<aWave[b]<<" This is the value of eMat["<< i+(a*2) <<"]["<<j+(b*2)<<"]"<<eMat[i+(a*2)][j+(b*2)]<<endl;
	}
	}
/*
			J[0][2*a] += 2* norm * real( iV[k][ aWave[a] ]) * real(iRawNI[ aWave[a] ][ aWave[b] ] );
			J[0][2*a+1] += 2* norm * imag( iV[k][ aWave[a] ]) * real(iRawNI[ aWave[a] ][ aWave[b] ] );
				cout<<"values of even J "<< J[0][2*a]<<" and this norm "<<norm<<endl;
				cout<<"values of odd J "<< J[0][2*a+1]<<endl;	
*/
	}
    }
 } 
/*
			for(int a; a<aNumberWaves ; a++){
				for(int b; b<2*aNumberWaves ; b++){
		J[a][b]=0;
}
}
*/
//J[0][2*aNumberWaves]=0;
			for(int a=0; a<aNumberWaves ; a++){

			J[0][2*a] =0; 
			J[0][(2*a)+1]=0;

				for(int b=0; b<aNumberWaves ; b++){
     					 if( isInterfering( aWave[a], aWave[b] ) )
						for( int k = 0; k < numberRanks(); k++){

		J[0][2*a] =J[0][2*a] + 2*coef*(real( iV[k][ aWave[b] ]) * (iRawNI[ aWave[a] ][ aWave[b] ] )).real();
                                         
		J[0][(2*a)+1] = J[0][(2*a)+1] + 2*coef*(imag( iV[k][ aWave[b] ]) *(iRawNI[ aWave[a] ][ aWave[b] ] )).real();

			//if((2*a) ==0){cout<<"im(v) "<<imag( iV[k][ aWave[b] ])<<" real(NI) "<<real(iRawNI[ aWave[a] ][ aWave[b] ] )<<" product "<<2*coef * real( iV[k][ aWave[b] ]) * real(iRawNI[ aWave[a] ][ aWave[b] ] )<<"J[0][0] "<<J[0][(2*a)]<<endl;}			
									}
								}
				//cout<<"values of even J "<< J[0][2*a]<<"odd J "<<J[0][2*a+1]<<endl;
							}
/*
		for(int i=0; i<2*aNumberWaves; i++){
	cout<<"this is J[0]["<<i<<"] :"<<J[0][i]<<endl;
		}
*/			  errsq = yieldErrorTotal( aNumberWaves ,J, eMat );

		
  for( int n = 0; n < 2*aNumberWaves; n++ ){
    delete [] eMat[n];

			}
  delete [] eMat;
  for( int n = 0; n < 1; n++ ){
    delete [] J[n];}
    delete [] J;
  return sqrt( errsq );
}


double PwaFitResult::yieldErrorTotal(int aNumberWaves, double **J, double **eMat  ){
  double err = 0;

 double **JT = new double*[2*aNumberWaves];
  for( int n = 0; n <2*aNumberWaves; n++ ){
    JT[n] = new double [1];
	}

   for( int n = 0; n <2*aNumberWaves; n++ ){
   JT[n][0] = J[0][n];
	cout<<"transpose values "<<JT[n][0]<<endl;
	}

  for( int i = 0; i < 2*aNumberWaves; i++ )
    for( int j = 0; j < 2*aNumberWaves; j++ ){
      err += J[0][i] *eMat[i][j] * JT[j][0]   ;

    }


	//cout<<"error the initial "<< err<<endl;
  return  fabs( err );


  for( int n = 0; n < 2*aNumberWaves; n++ ){
    delete [] JT[n];}
  delete [] JT;

}





double PwaFitResult::yieldErrorTerm( int aWave, int bWave, double a, double b, double c, double d, double **eMat  ){
  
  int dim = 4;
  double err = 0;
  double deriv[4];
  complex<double> im(0,1);
  complex<double> cn1(c,d);
  complex<double> cn2(a,b);
 double icn1_norm = 1/sqrt(norm(cn1));
  double icn2_norm = 1/sqrt(norm(cn2));  
  double norm = double(numberDataEvents()) * ( double(numberRawMcEvents()) / double(numberAccMcEvents()) );
  complex<double> Knorm(norm,0);
  //complex<double> Konsta =  iRawNI[aWave][aWave] * Knorm; 
  complex<double> Konst =  iRawNI[aWave][bWave] * Knorm; 
if( aWave == bWave){
  deriv[0] =  real( (real(cn2) * Konst + real(cn1) * Konst) );
  deriv[1] =  real((imag(cn2) * Konst + imag(cn1) * Konst));
  deriv[2] =  real( (real(cn1) * Konst + real(cn2) * Konst) );
  deriv[3] =  real( (imag(cn2) * Konst + imag(cn1) * Konst) );
	}
if( aWave != bWave ){

  deriv[0] =  real( cn1 * Konst );
  deriv[1] =  real(im * cn1 * Konst);
  deriv[2] =  real( cn2 * Konst );
  deriv[3] =  real( im * cn2 * Konst );
	}
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
  double A=0, B=0;
  for( int k = 0; k < numberRanks(); k++ ){
 A +=(iV[k][aWave]*conj(iV[k][bWave])).real();
 B +=(iV[k][aWave]*conj(iV[k][bWave])).imag();
}


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
     delete [] eMat[n];
   delete [] eMat;
  return val;
}




double PwaFitResult::phaseDiffError( int aWave, int bWave ){
  int dim = 4;
  double a=0, b=0, c=0, d=0;
  double **eMat = new double *[dim];
  for( int n = 0; n < dim; n++)
    eMat[n] = new double [dim];
  double A=0, B=0;
  for( int k = 0; k < numberRanks(); k++ ){
 A +=(iV[k][aWave]*conj(iV[k][bWave])).real();
 B +=(iV[k][aWave]*conj(iV[k][bWave])).imag();
}

  for( int k = 0; k < numberRanks(); k++ ){
    a += iV[k][aWave].real();
    b += iV[k][aWave].imag();
    c += iV[k][bWave].real();
    d += iV[k][bWave].imag();
  }

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
  double val = phaseDiffError( A, B,a,b,c,d, eMat ); 
   for( int n = 0; n < dim; n++)
     delete [] eMat[n];
   delete [] eMat;
  return val;
}


//////////////////////////working here///////////////////////////

double PwaFitResult::phaseDiffError(double A, double B,double a, double b, double c, double d, double **eMat ){


  int dim = 4;
  double errsq = 0;
  double derivT[2];
  double sigma_sq[2][2];
 double **JT = new double*[4];
  for( int n = 0; n <4; n++ ){
    JT[n] = new double [2];
	}


 double **J = new double*[2];
  for( int n = 0; n <2; n++ ){
    J[n] = new double [4];
	}
  derivT[0] = A/(A*A + B*B);
  derivT[1] = -B/(A*A + B*B);

J[0][0]=c;
J[0][1]=d;
J[0][2]=a;
J[0][3]=b;
J[1][0]=-d;
J[1][1]=c;
J[1][2]=b;
J[1][3]=-a;


   for( int m = 0; m <2; m++ ){
   for( int n = 0; n <4; n++ ){
   JT[n][m] = J[m][n];
	} 
        }

  for( int i = 0; i < 2; i++ ){
    for( int j = 0; j < 2; j++ ){
      for( int n = 0; n <4; n++ ){ 
           for( int m = 0; m <4; m++ ){

sigma_sq[i][j] += J[i][n] *eMat[n][m] * JT[m][j] ;
                           }
                    }
              }
         }


  for( int i = 0; i < 2; i++ ){
    for( int j = 0; j < 2; j++ ){

errsq += (derivT[i]*derivT[j])*sigma_sq[i][j]; 
        }
   }

  return sqrt ( errsq );
  for( int n = 0; n < 4; n++ ){
    delete [] JT[n];
    delete [] J[n]; }
  delete [] JT;
  delete [] J;    
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



