 /*********************************************
 *
 * sliceGamp.cc : slices gamp events into mass bins,
 *            translates between ascii and gamp 
 *            formats, and also cuts on simple
 *            topologies.  This code was lifted
 *            from ascii2gamp.c
 *
 * Paul Eugenio
 * Florida State University
 * 1 Dec 2001
 **********************************************/


#include <stdio.h>
#include <math.h>
//#include <string.h>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <sys/types.h>
#include <malloc.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <time.h>


using namespace std;
/*****************
 * GLOBALS
 *****************/
static int True=1;
static int False=0;
int Add_Recoil=0;
double Add_Beam=0;
double Wei=0;
int Debug=0;
#define RESTFRAME -1
#define N_MAX_PARTS 10
double Polarization_Angle=0;
/***************
 * STRUCTURES        
 **************/

typedef struct {
  int id,charge;
  double mass;
  double p[4]; /* four-vector x,y,z,t */
  /* double x,y,z,t; */
} part_t ;


typedef struct {
  int nparts;
  part_t part[3];
} parts_t ;

typedef struct {
  int nunknown;
  int ngamma;
  int npiplus;
  int npiminus;
  int npizero;
  int neta;
  int nKplus;
  int nKminus;
  int nproton;
  int nantiproton;
  int nneutron;
 // int nlambda;
}part_list_t;


/***********************
 * PROTOTYPE FUNCTIONS
 ***********************/
int PrintUsage(char *processName);
int getEvent(FILE *fp,int *nparts, double *alpha, double *wei, part_t *P,part_t *beam);
int writeEvent(FILE *fp,int nparts, double *alpha, double *wei, part_t *P, part_t *beam);
part_t get_beta(part_t *boost,int sign);
double   w_(double *p1,double *p2,double *p3,double *rho,double *egamma);
double   wlab_(double *p1,double *p2,double *p3,double *rho,double *egamma);
int getGampEvent(FILE *fp,int *nparts, part_t *P,part_t *beam,part_t *recoil);
int writeGampEvent(FILE *fp,int nparts,part_t *P,part_t *beam,part_t *recoil );
unsigned long  checkEvent(part_list_t *pl);
int getMode(part_list_t *pl);
void  getList(int nparts, part_t *parts, part_list_t *pl);
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
  char outfile[50],inputfile[50];
  char tag[20];
  char *wavefile;
  int nparts;
  double alpha, wei=0;
  part_t parts[N_MAX_PARTS],beam,X,recoil;
  part_list_t partList;
  int i,j,gotANevent=1,weighted=0,written=0,ntotal=999999999;
  int UseGampInput=1,UseGampOutput=1,UseNoWeighting=0;
  int checkChannel=False,checkMode=False,keepIt=True;
  FILE *outputfp=NULL;
  FILE *inputfp=NULL;
  double rho[2][2]={0.5,0.0,0.0,0.5};
  double emass=0, ubin=50, lbin=0;
	string string_lbin;
	stringstream convert1;
	convert1 << lbin;
	string_lbin = convert1.str();
	string string_ubin;
	stringstream convert2;
	convert2 << ubin;
	string outpfile;
	string_ubin = convert2.str();
  double rhoGJ[2][2]={0.5,0.0,0.0,0.5};
  double p1[4],p2[4],p3[4],egamma=9.0,t;
  double target_p[4];
  int UseLabParticles=0;
  int Use_Pure_GJ_Polarization=0;
  double P=0.0,Pvector[3]={0.0,0.0,0.0};
  double alpha_low=0.0, alpha_high=180.0;
  unsigned long  channel=0;
  int mode =0;

  if (argc == 1){
    PrintUsage(argv[0]);
    exit (0);
  }
  else {
    for (i=1; i<argc; i++) {
      argptr = argv[i];
      if ((*argptr == '-') && (strlen(argptr) > 1)) {
        argptr++;
        switch (*argptr) {
	case 'I':
	  UseGampInput=0;
	  break;
	case 'O':
	  UseGampOutput=0;
	  break;
	case 'K':
	  channel=atoi(++argptr);
	  checkChannel=True;
	  break;
	case 'M':
	  mode=atoi(++argptr);
	  checkMode=True;
	  break;

	case 'B':
	  Add_Beam=atof(++argptr);
	  fprintf(stderr, "Adding Beam(beam.p = %5.2f GeV) info to ascii input\n", 
		  Add_Beam);
	  break;
	case 'R':
	  /* Add Reoil to event */
	  Add_Recoil=1;
	  break;
	case 'L':
	  lbin=atof(++argptr);
	  fprintf(stderr, "Using lower bin limit of %f\n", lbin);
	  break;
	case 'U':
	  ubin=atof(++argptr);
	  fprintf(stderr, "Using upper bin limit of %f\n", ubin);
	  break;
	case 'x':
	  wei = 1;
	  break;
      	case 'a':
	  argptr++;
	  if(*argptr =='L'){
	    alpha_low=atof(++argptr);
	    fprintf(stderr, "Selecting alpha's > %f\n", alpha_low);
	  }
	  else if(*argptr =='U'){
	    alpha_high=atof(++argptr);
	    fprintf(stderr, "Selecting alpha's < %f\n", alpha_high);
	  }
	  else{
	    fprintf(stderr,"Unrecognized argument -%s\n\n",argptr);
	    PrintUsage(argv[0]);    
	    exit(-1);
	  }
	  break;
      	case 'g':
	  if(*(argptr+1)=='i')
	    UseGampInput=1;
	  else if(*(argptr+1)=='o')
	    UseGampOutput=1;
	  else{/*use both*/
	    UseGampInput=1;
	    UseGampOutput=1;
	  }
	  break;
	case 'N':
          ntotal=atoi(++argptr);
          break;   
        case 'o':
	  sprintf(outfile,"%s",++argptr);
	  if(!(outputfp = fopen(outfile,"w"))){
	    fprintf(stderr,"Failed to open output file!\n");
	    exit(-1);
	  }
          break;
        case 'i':
	  sprintf(inputfile,"%s",++argptr);
	  if(!(inputfp = fopen(inputfile,"r"))){
	    fprintf(stderr,"Failed to open input file!\n");
	    exit(-1);
	  }
          break;
	case 'd':
	  Debug=atoi(++argptr);
	  break;
        default:
          fprintf(stderr,"Unrecognized argument -%s\n\n",argptr);
          PrintUsage(argv[0]);    
          exit(-1);
          break;
	}
      }
    }
    
    if(P!=0 && (Pvector[0]+Pvector[1]) ==0){
      fprintf(stderr,"Must also choose beam polarization direction (-D option)!!\n");
      exit(-1);
    }
     
    if(!inputfp)
      inputfp=stdin;
  // if(!outputfp)
     // outputfp=stdout;
   
	//else{



	outpfile.append(string_lbin);
	outpfile.append(":");
	outpfile.append(string_ubin);
	outpfile.append(".gamp");
	outputfp=fopen(outpfile.c_str(),"r");//}

 
   gotANevent= UseGampInput
     ?getGampEvent(inputfp,&nparts,parts,&beam,&recoil)
     :getEvent(inputfp,&nparts,&alpha,&wei,parts,&beam);
   
   while(gotANevent>0 && (ntotal >0) ){  /* I have an event! */           
   
     if(Debug==1){ /* print parts */
       //fprintf(stderr,"beam p(x,y,z,t): %f %f %f %f\n",
       //      beam.p[0],beam.p[1],beam.p[2],beam.p[3]);
       for(i=0;i<nparts;i++)
	 fprintf(stderr,"part[%d] id= %d charge= %d mass= %f p(x,y,z,t)= %f %f %f %f\n",
		 i,parts[i].id,parts[i].charge,parts[i].mass,
		 parts[i].p[0],parts[i].p[1],parts[i].p[2],parts[i].p[3]);
       
     }

     //
     // Calculate  total meson mass
     //
     for(j=0;j<4;j++)
       X.p[j] = 0;  // zero the 4 vec
     for(i=0;i<nparts;i++)
       if(parts[i].id != 14 && parts[i].id != 13 
	  && parts[i].id != 3 && parts[i].id != 18) // not a p, n , lambda or e
	 for(j=0;j<4;j++)  // copy four-vects
	   X.p[j] += parts[i].p[j];
     emass = sqrt( X.p[3]*X.p[3] - 
		   (  X.p[0]*X.p[0] + X.p[1]*X.p[1] + X.p[2]*X.p[2]));

     if(Debug==1){    // print X 
       fprintf(stderr,"X p(x,y,z,t): %f %f %f %f\n",
	       X.p[0],X.p[1],X.p[2],X.p[3]);
       fprintf(stderr,"Emass: %f\n",emass);       
     }

     //
     // particle inventory
     //
     getList(nparts,parts,&partList);

     //
     // Check for cuts
     //
     if(keepIt)
       keepIt = ( !(checkChannel) || channel == checkEvent(&partList));
     if(keepIt)
       keepIt = ( !(checkMode) || mode == getMode(&partList));
     if(keepIt)
       keepIt = (emass>=lbin && emass<=ubin);
    

    
     if(keepIt){
       if(UseGampOutput)
	 writeGampEvent(outputfp,nparts,parts,&beam,&recoil);
       else
	 writeEvent(outputfp,nparts,&alpha,&wei,parts,&beam);
       
       if(Debug==3)
	 fprintf(stderr, "Event saved\n");
       written++; ntotal--;
     }
     
     gotANevent= UseGampInput
       ?getGampEvent(inputfp,&nparts,parts,&beam,&recoil)
       :getEvent(inputfp,&nparts,&alpha,&wei,parts,&beam);
     keepIt=True;
     if(!(++weighted %100))
       fprintf(stderr,"Events processed: %d\r",weighted);
   }
    

    fprintf(stderr,"\n");
    fprintf(stderr,"Events Read: %d    Events Written: %d\n\n\n",weighted,written);
    
    if(inputfp){
      fclose(inputfp);
      inputfp = fopen(inputfile,"r");
    }
    if(outputfp)
      fclose(outputfp);
  }
}

void zero_part_list(part_list_t *pl){
  //
  // Zero the particle counters
  //
   pl->nunknown=0;
   pl->ngamma=0;
   pl->npiplus=0;
   pl->npiminus=0;
   pl->npizero=0;
   pl->neta=0;
   pl->nKplus=0;
   pl->nKminus=0;
 //  pl->nlambda=0;
   pl->nproton=0;
   pl->nantiproton=0;
   pl->nneutron=0;
};

void  getList(int nparts, part_t *parts, part_list_t *pl){
  //
  // Loop over the particles and count them
  // 

  int i;
  unsigned long ret=0;
  zero_part_list(pl);
  
  for(i=0; i<nparts; i++)
    switch(parts[i].id){
    case 1: //gamma
      pl->ngamma++;
      break;
    case 7: //pi0
      pl->npizero++;
      break;
    case 8: //pi+
      pl->npiplus++;
      break;
    case 9: //pi-
      pl->npiminus++;
      break;
    case 11: //K+
      pl->nKplus++;
      break;
    case 12: //K-
      pl->nKminus++;
      break;
    case 17: //eta
      pl->neta++;
      break;
   // case 18: //lambda
     // pl->nlambda++;
      break;
    case 14: //proton
      pl->nproton++;
      break;
    case 13: //neutron
      pl->nneutron++;
      break;
    case 15: //antiProton
      pl->nantiproton++;
      break;    
    default:
      pl->nunknown++;
      
      
    }
  
  return;
}



int getMode(part_list_t *pl){
  //
  // 
  //
  int mode=0;
  
  if( pl->ngamma==0 && pl->npiplus==1 && pl->npiminus==1 && pl->npizero==0 
      && pl->neta==0 && pl->nKplus==0 && pl->nKminus==0 && pl->nproton==1 
      && pl->nantiproton==0 && pl->nneutron==0 && pl->nunknown==0)
    mode=1;
  else if( pl->ngamma==0 && pl->npiplus==1 && pl->npiminus==0 && pl->npizero==0 
	   && pl->neta==0 && pl->nKplus==0 && pl->nKminus==0 && pl->nproton==1 
	   && pl->nantiproton==0 && pl->nneutron==0 && pl->nunknown==0)
    mode=2;
  else if( pl->ngamma==0 && pl->npiplus==2 && pl->npiminus==1 && pl->npizero==0 
	   && pl->neta==0 && pl->nKplus==0 && pl->nKminus==0 && pl->nproton==0 
	   && pl->nantiproton==0 && pl->nneutron==0 && pl->nunknown==0)
    mode=3;
  else if( pl->ngamma==0 && pl->npiplus==1 && pl->npiminus==1 && pl->npizero==0 
	   && pl->neta==0 && pl->nKplus==1 && pl->nKminus==0 && pl->nproton==0 
	   && pl->nantiproton==0 && pl->nneutron==0 && pl->nunknown==0)
    mode=4;

  return mode;
}

unsigned long  checkEvent(part_list_t *pl){
  //
  // nGamma 1's, nPion 10's, nKaon 100's, nEta 1,000's, 
  // nProton 10,000's, nNeutron 100,000's, nAntiProton 1,000,000's
  //

  return 10000000*pl->nunknown 
    + 1000000*pl->nantiproton 
    + 100000*pl->nneutron
    + 10000*pl->nproton
    + 1000*pl->neta
    + 100*(pl->nKminus+pl->nKplus)
    + 10*(pl->npiminus + pl->npizero + pl->npiplus)
    + pl->ngamma;
}


int writeEvent(FILE *fp,int nparts, double *alpha, double *wei,part_t *P,part_t *beam)
{
  int runNo=900,i;
  static int num=0;

  /* write header */
  fprintf(fp,"%d %d %d %f %f %f %f ",runNo,num++,nparts,
	  beam->p[0],beam->p[1],beam->p[2],beam->p[3]);
  if(*wei == 1)
    fprintf(fp,"%f %f",*alpha,*wei);
  fprintf(fp,"\n");

  for(i=0;i<nparts;i++){ /* write particles */
    fprintf(fp,"%d %d %f\n",i+1,P[i].id,P[i].mass);
    fprintf(fp,"   %d %f %f %f %f\n",P[i].charge,P[i].p[0],P[i].p[1],P[i].p[2],P[i].p[3]); 
  }
}

int writeGampEvent(FILE *fp,int nparts,part_t *P,part_t *beam,part_t *recoil )
{
  int runNo=900,i;
  static int num=0;
  
  /* write header */
  fprintf(fp,"%d\n",nparts+1+Add_Recoil);/* nparts + beam  */
  fprintf(fp,"%d %d %f %f %f %f\n",beam->id,beam->charge,
	  beam->p[0],beam->p[1],beam->p[2],beam->p[3]);
  if(Add_Recoil){
    fprintf(fp,"%d %d %f %f %f %f\n",recoil->id,recoil->charge,
	    recoil->p[0],recoil->p[1],recoil->p[2],recoil->p[3]);
  }
  for(i=0;i<nparts;i++){ /* write particles */
    fprintf(fp,"%d %d %f %f %f %f\n",P[i].id,P[i].charge,
	    P[i].p[0],P[i].p[1],P[i].p[2],P[i].p[3]); 
  }
}

int getEvent(FILE *fp,int *nparts,double  *alpha, double *wei, part_t *P,part_t *beam)
{
  char line[2056];
  char *token;
  static int nread=0;
  int i;

  /* get the event header info */
  
  if(fgets(line,sizeof(line),fp)!=NULL){
    nread++;
    token=strtok(line," ");
    /* runNo = atoi(token); */
    
    token=strtok(NULL," ");
    /* conf = atof(token); */
    
    token=strtok(NULL," ");
    *nparts = atoi(token);

    /*
     * Assume a photon beam
     */
    beam->charge=0;
    beam->id=1;
    if(Add_Beam){
      beam->p[0]= 0;//p.x
      beam->p[1]= 0;//p.y
      beam->p[2]= Add_Beam;//p.z
      beam->p[3]= Add_Beam; //p.t
    }else{
      token=strtok(NULL," ");
      beam->p[0]= atof(token);
      token=strtok(NULL," ");
      beam->p[1]= atof(token);
      token=strtok(NULL," ");
      beam->p[2]= atof(token);
      token=strtok(NULL," ");
      beam->p[3]= atof(token);
    }
    /*Read alpha and weight of the event*/
    if( *wei == 1){
      token=strtok(NULL," ");
      *alpha=atof(token);
      token=strtok(NULL," ");
      *wei=atof(token);
    }
    else
      *alpha=1.0;

    /* get the particle information */
    for(i=0;i<*nparts;i++){ 
      if(fgets(line,sizeof(line),fp)!=NULL){
	
	token=strtok(line," ");
	/* particleNo[i] = atoi(token); */
	
	token=strtok(NULL," ");
	/* code[i] = atoi(token); */
	P[i].id = atoi(token); 

	token=strtok(NULL," ");
	/* mass[i] = atof(token); */
	
	
      } else return -2;

      /* get next line */
      if(fgets(line,sizeof(line),fp)!=NULL){
	
	token=strtok(line," ");
	P[i].charge = atoi(token); 
	if(Debug>2) 
	  fprintf(stderr,"Particle %d with charge = %d \n",i,P[i].charge);
	token=strtok(NULL," ");
	P[i].p[0] = atof(token);
	token=strtok(NULL," ");
	P[i].p[1] = atof(token);
	token=strtok(NULL," ");
	P[i].p[2] = atof(token);
	token=strtok(NULL," ");
	P[i].p[3] = atof(token);
	P[i].mass = sqrt(P[i].p[3]*P[i].p[3] - (P[i].p[0]*P[i].p[0] +
					  P[i].p[1]*P[i].p[1] + 
					  P[i].p[2]*P[i].p[2]) );
	if(Debug>2) 
	  fprintf(stderr,"\tThascii2gampe four momentum is %lf %lf %lf %lf \n",
		  P[i].p[0],P[i].p[1],P[i].p[2],P[i].p[3]);
	if(Debug>2) 
	  fprintf(stderr,"\tThe calculated mass is %lf\n",P[i].mass);
	
      } else return -2;/* bad input file */
    } /* end of for(i=0 ... four particles */
    return 1;/* it looks like a good event */
  } else return -1;/* there are no more events */
}


int getGampEvent(FILE *fp,int *nparts, part_t *P,part_t *beam,part_t *recoil)
{
  /*
   * Assume that the beam and the recoil are the first 2 parts
   *
   */
  char line[2056];
  char *token;
  static int nread=0;
  int i;

  /*
   * get the number of parts
   */
 
  if(fgets(line,sizeof(line),fp)!=NULL){
    nread++;
    token=strtok(line," ");
    *nparts = atoi(token) ; 
    if(Debug) 
      fprintf(stderr,"Reading gamp input. Found %d particles\n",*nparts);

    //
    // Get the beam  (this is the 1st gamp particle in the list)
    //
    (*nparts)--;
    if(fgets(line,sizeof(line),fp)!=NULL){
	token=strtok(line," ");
	beam->id= atoi(token);
	token=strtok(NULL," ");
	beam->charge=atoi(token);
	token=strtok(NULL," ");
	beam->p[0]= atof(token);
	token=strtok(NULL," ");
	beam->p[1]= atof(token);
	token=strtok(NULL," ");
	beam->p[2]= atof(token);
	token=strtok(NULL," ");
	beam->p[3]= atof(token);
	beam->mass = sqrt(beam->p[3]*beam->p[3] - (beam->p[0]*beam->p[0] +
						beam->p[1]*beam->p[1] + 
						beam->p[2]*beam->p[2]) );
	if(Debug==1) {
	  fprintf(stderr,"\tbeam:\tfour momentum: %lf %lf %lf %lf \n",
		  beam->p[0],beam->p[1],beam->p[2],beam->p[3]);
	  fprintf(stderr,"\t\t\tmass: %lf\n",beam->mass);
	}
      } else return -2;

    // 
    // get the particle information 
    //
    for(i=0;i < *nparts;i++){ 
      if(fgets(line,sizeof(line),fp)!=NULL){
	
	token=strtok(line," ");
	P[i].id= atoi(token);
	token=strtok(NULL," ");
	P[i].charge=atoi(token);
	token=strtok(NULL," ");
	P[i].p[0]= atof(token);
	token=strtok(NULL," ");
	P[i].p[1]= atof(token);
	token=strtok(NULL," ");
	P[i].p[2]= atof(token);
	token=strtok(NULL," ");
	P[i].p[3]= atof(token);
	P[i].mass = sqrt(P[i].p[3]*P[i].p[3] - (P[i].p[0]*P[i].p[0] +
						P[i].p[1]*P[i].p[1] + 
						P[i].p[2]*P[i].p[2]) );
	if(Debug==1) {
	  fprintf(stderr,"\tpart[%d]:\tfour momentum: %lf %lf %lf %lf \n",i,
		  P[i].p[0],P[i].p[1],P[i].p[2],P[i].p[3]);
	  fprintf(stderr,"\t\t\tmass: %lf\n",P[i].mass);
	}
	
      } else return -2;
    }

    return 1;/* it looks like a good event */
  } else return -1;/* there are no more events */
}






int PrintUsage(char *processName)
{
  
  fprintf(stderr,"%s usage: [switches]    \n",processName);
  fprintf(stderr,"\t-d<level> debug flag\n");
  fprintf(stderr,"\t\t level =: \n");
  fprintf(stderr,"\t\t\t1 -> debug output at input\n");
  fprintf(stderr,"\t-N# total number of events to save\n");
  fprintf(stderr,"\t-I read in ascii format files\n");
  fprintf(stderr,"\t-O write out ascii format files\n");
  fprintf(stderr,"\t-K<chan-num> Keep only these events.\n");
  fprintf(stderr,"\t\tchan-num: \n");
  fprintf(stderr,"\t\t\t nGamma 1's\n");
  fprintf(stderr,"\t\t\t nPion 10's\n");
  fprintf(stderr,"\t\t\t nKaon 100's\n");
  fprintf(stderr,"\t\t\t nEta 1,000's\n");
  fprintf(stderr,"\t\t\t nProton 10,000's\n");
  fprintf(stderr,"\t\t\t nNeutron 100,000's\n");
  fprintf(stderr,"\t\t\t nAntiProton 1,000,000's\n\n");
  fprintf(stderr,"\t-M<mode#> Keep only this channel mode.\n");
  fprintf(stderr,"\t\tmodes: \n");
  fprintf(stderr,"\t\t\t 1\tp pi+ pi-\n");
  fprintf(stderr,"\t\t\t 2\tp pi+ \n");
  fprintf(stderr,"\t\t\t 3\t2pi+ pi-\n");
  fprintf(stderr,"\t\t\t\n");
  
  fprintf(stderr,"\t-L<lowmass> lower limit on bin range(GeV)\n");  
  //fprintf(stderr,"\t-aU<highmass> upper limit on alpha range(default 180.0)\n");   fprintf(stderr,"\t-aL# lower limit on alpha range(default=0.0)\n");  
  fprintf(stderr,"\t-U<highmass> upper limit on bin range(GeV)\n"); 
  
  fprintf(stderr,"\t-o<name> The output file name(default stdout).\n");
 
  fprintf(stderr,"\t-i<name> The data input file(default stdin).\n");

  fprintf(stderr,"\t-x Input file HAS weights!(asciiformat).\n");
  fprintf(stderr,"\t-B<beam.p> Add beam info to the ascii input (GeV).\n");

  fprintf(stderr,"\t-h Print this help message\n\n");
}






