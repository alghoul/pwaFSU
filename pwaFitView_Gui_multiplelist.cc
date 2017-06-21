
#include "pwaFitView_Gui.h"
#include "custom.h"
#include <fstream>


const char *MyMainFrame::fSaveAsFileTypes[] = { 
  "gif"        , "*.gif" , 
  "ps"         , "*.ps"  ,
  "eps"        , "*.eps" ,
  "C source"   , "*.C"   , 
  "png"        , "*.png" ,
  "root file" , "*.root", 
  //  "ps"         , "*"     , 
  0       , 0};




void MyMainFrame::DoSelect()
{
	Printf("Slot DoSelect()");
}

//  int numberFitBins
	


MyMainFrame::MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h, UInt_t m ,PwaFitResults *ofitResults, char *fitlist ) :
TGMainFrame(p, w, h)
{





  fFitResults = ofitResults;
  int filesnumber=0;
  filesnumber = m;
  fCurrentCanvas =0;
  fNumberCanvases = 1;



  // MENU

  fMenuDock = new TGDockableFrame(this);
  AddFrame(fMenuDock, new TGLayoutHints(kLHintsExpandX, 0, 0, 1, 0));
  fMenuDock->SetWindowName("Menu Dock");

  TGLayoutHints *fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX);
  TGLayoutHints *fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
  TGLayoutHints *fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);
  
  // File Menu
  TGPopupMenu *fMenuFile = new TGPopupMenu( gClient->GetRoot() );
  fMenuFile->AddEntry("E&xit", M_FILE_EXIT );
  fMenuFile->AddEntry("Close", M_FILE_EXIT );
  fMenuFile->AddSeparator();
  fMenuFile->AddEntry("&Print", M_FILE_PRINT);
  fMenuFile->AddEntry("P&rint setup...", M_FILE_PRINTSETUP);
  
  //Canvas Menu
  TGPopupMenu *fMenuCanvas = new TGPopupMenu( gClient->GetRoot() );
  fMenuCanvas->AddEntry("Save as...", M_CANVAS_SAVEAS);
  fMenuCanvas->AddEntry("&1x1", M_CANVAS_1X1);
  fMenuCanvas->AddEntry("&1x2", M_CANVAS_1X2);
  fMenuCanvas->AddEntry("&1x3", M_CANVAS_1X3);
  fMenuCanvas->AddEntry("&2x2", M_CANVAS_2X2);
  fMenuCanvas->AddEntry("&2x3", M_CANVAS_2X3);
  fMenuCanvas->AddEntry("&3x3", M_CANVAS_3X3);
  fMenuCanvas->AddEntry("&3x2", M_CANVAS_3X2);


  // Help Menu
  TGPopupMenu *fMenuHelp = new TGPopupMenu(gClient->GetRoot());
  fMenuHelp->AddEntry("&Help...", M_HELP_HELP);
  fMenuHelp->AddSeparator();
  fMenuHelp->AddEntry("&About", M_HELP_ABOUT);


  fMenuFile->Connect("Activated(Int_t)", "MyMainFrame", this,"HandleMenu(Int_t)");
  //fMenuFile->Connect("PoppedUp()", "MyMainFrame", this, "HandlePopup()");
  //fMenuFile->Connect("PoppedDown()", "MyMainFrame", this, "HandlePopdown()");
  fMenuCanvas->Connect("Activated(Int_t)", "MyMainFrame", this, "HandleMenu(Int_t)");
  fMenuHelp->Connect("Activated(Int_t)", "MyMainFrame", this, "HandleMenu(Int_t)");
  

  // MenuBar
  fMenuBar = new TGMenuBar(fMenuDock, 1, 1, kHorizontalFrame);
  fMenuBar->AddPopup("&File", fMenuFile, fMenuBarItemLayout);
  fMenuBar->AddPopup("&Canvas", fMenuCanvas, fMenuBarItemLayout);
  fMenuBar->AddPopup("&Help", fMenuHelp, fMenuBarHelpLayout);
  
  fMenuDock->AddFrame(fMenuBar, fMenuBarLayout);


 


  //TGHorizontalFrame *hTopFrame = new TGHorizontalFrame(this, 800, 800, kFixedWidth);
TGHorizontalFrame *hTopFrame = new TGHorizontalFrame(this, 800, 800);
	// Create top frame
	
  //
  // listBox
  //

  ifstream fitlistf;
	std::string fitfilename[10];
 	fitlistf.open( fitlist , ios::in);
	int filenumber;
	fitlistf>>filenumber;
	std::string shortffname[10];
	const char* fitfilechar[10]={};
  fListFBox = new TGListBox(hTopFrame, 100);
  fFSelected = new TList;
  char tmp2[20];
  for (int i = 0; i < filenumber ; ++i) {
	fitlistf>>fitfilename[i];

	shortffname[i] =  fitfilename[i].substr(fitfilename[i].find_last_of("/\\") + 1 );
	//cout<<shortffname<<endl;

		fitfilechar[i] = shortffname[i].c_str();
    cerr << "Adding : " << shortffname[i] << endl;
    fListFBox->AddEntry(fitfilechar[i], i+1);
    //sprintf(tmp, "Entry %i", i+1);
    //    fListBox->AddEntry(tmp, i+1);
  }
  fListFBox->Resize(100,150);
  //fListFBox->SetMultipleSelections();
  hTopFrame->AddFrame(fListFBox, new TGLayoutHints(kLHintsTop | kLHintsLeft |
						  kLHintsExpandY, 
						  5, 5, 5, 5));
	

  fListWBox = new TGListBox(hTopFrame, 100);
  fWSelected = new TList;
  char tmp[20];
    
  fListWBox->Resize(100,150);
  fListWBox->SetMultipleSelections();
 hTopFrame->AddFrame(fListWBox, new TGLayoutHints(kLHintsTop | kLHintsLeft |
						  kLHintsExpandY, 
						  5, 5, 5, 5));


  TGVButtonGroup *fButtonGroup = new TGVButtonGroup(hTopFrame, "plot type");
  fYieldButton = new TGRadioButton(fButtonGroup,"yield", 5);
  fYieldButton->SetToolTipText("generate yield for one or more waves");
  fButtonGroup->AddFrame(fYieldButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  //fYieldButton->MoveResize(40,416,113,19);
  fYieldButton->Connect("Clicked()", "MyMainFrame", this, "HandleButtons()");
  fYieldButton->SetOn();
  
  fPhaseButton = new TGRadioButton(fButtonGroup,"phase",6);  
  fPhaseButton->SetToolTipText("generate phase difference between two interfering waves");
  fButtonGroup->AddFrame(fPhaseButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fPhaseButton->Connect("Clicked()", "MyMainFrame", this, "HandleButtons()");

  fLikelyButton = new TGRadioButton(fButtonGroup,"likely",7);  
  fLikelyButton->SetToolTipText("generate a plot of the -LogLikelihood vs mass");
  fButtonGroup->AddFrame(fLikelyButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fLikelyButton->Connect("Clicked()", "MyMainFrame", this, "HandleButtons()");

  fBWButton = new TGCheckButton(fButtonGroup,"Fit to BW",8);  
  fBWButton->SetToolTipText("plot a Breit-Wigner");
  fButtonGroup->AddFrame(fBWButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fBWButton->Connect("Clicked()", "MyMainFrame", this, "HandleButtons()"); 

/*
  MassTextBox = new TGTextEntry(fButtonGroup,"BW Mass",9); 
  MassTextBox->SetToolTipText("Insert mass of Breit-Wigner");
  //MassTextBox->SetTitle("BW Mass");
  fButtonGroup->AddFrame(MassTextBox, new TGLayoutHints( kLHintsTop,3,3,3,3));  

  WidthTextBox = new TGTextEntry(fButtonGroup,"BW Width",10); 
  WidthTextBox->SetToolTipText("Insert width of Breit-Wigner");
  //WidthTextBox->SetTitle("BW Width");
  fButtonGroup->AddFrame(WidthTextBox, new TGLayoutHints( kLHintsTop,3,3,3,3));

  AmpTextBox = new TGTextEntry(fButtonGroup,"BW Amplitude",11); 
  AmpTextBox->SetToolTipText("Insert Amplitude of Breit-Wigner");
  //WidthTextBox->SetTitle("BW Width");
  fButtonGroup->AddFrame(AmpTextBox, new TGLayoutHints( kLHintsTop,3,3,3,3));*/

  FHSlider = new TGDoubleHSlider(fButtonGroup,100,kDoubleScaleBoth,13); 
  FHSlider->Connect("PositionChanged()", "MyMainFrame", this, "DoSlider()");
  FHSlider->SetRange(1.1,2.2);
  FHSlider->SetPosition(1.1, 2.2);
  //WidthTextBox->SetTitle("BW Width");
  fButtonGroup->AddFrame(FHSlider, new TGLayoutHints(kLHintsExpandX,  5, 5, 5, 5));


  fAWFButton = new TGCheckButton(fButtonGroup,"Add Files",8);  
  fAWFButton->SetToolTipText("Plot the combination of two files");
  fButtonGroup->AddFrame(fAWFButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fAWFButton->Connect("Clicked()", "MyMainFrame", this, "HandleButtons()"); 

  fButtonGroup->Show();
  hTopFrame->AddFrame(fButtonGroup, new TGLayoutHints( kLHintsTop,
						      0,0,0,0));


 
  // embedded canvas
  //  fECanvas = new TRootEmbeddedCanvas("Ecanvas",hTopFrame,416,368);
	if(filenumber<=3){
  fECanvas = new TRootEmbeddedCanvas("Ecanvas",hTopFrame,filenumber*300,300 );
  fECanvas->GetCanvas()->Divide(filenumber,1);
  fECanvas->GetCanvas()->SetFillColor(kWhite);  
		}
	if(filesnumber>3){
		if(filenumber%2 != 0){
  fECanvas = new TRootEmbeddedCanvas("Ecanvas",hTopFrame,((filenumber+1)/2)*300,600 );
  fECanvas->GetCanvas()->Divide((filenumber+1)/2,2); 
  fECanvas->GetCanvas()->SetFillColor(kWhite);  
				}
		if(filenumber%2 ==0){
  fECanvas = new TRootEmbeddedCanvas("Ecanvas",hTopFrame,(filenumber/2)*300,600 );
  fECanvas->GetCanvas()->Divide(filenumber/2,2);
  fECanvas->GetCanvas()->SetFillColor(kWhite);  
				}
			}

fECanvas->GetCanvas()->SetFillColor(kWhite);
  fECanvas->GetCanvas()->SetBorderSize(0);


  hTopFrame->AddFrame(fECanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY,2,2,2,2));
  //fECanvas->MoveResize(240,16,416,368);
  
  AddFrame(hTopFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY|kLHintsRight, 2, 2, 5, 1));
  
  
	
  // Create a horizontal frame containing button(s)
  TGHorizontalFrame *hBottomFrame = new TGHorizontalFrame(this, 180, 70, kFixedWidth);
  // Reset Button
  

////////////////// *********************************************************////////////  
  //Reset Fit files button
  TGTextButton *fResetFButton = new TGTextButton(hBottomFrame, "&Reset File Selections");
  fResetFButton->SetToolTipText("Click here to clear wave selections");
  fResetFButton->Connect("Pressed()", "MyMainFrame", this, "ClearSelectedFiles()");
  hBottomFrame->AddFrame(fResetFButton, new TGLayoutHints(kLHintsExpandX, 5, 5, 3, 4));


  TGTextButton *fResetWButton = new TGTextButton(hBottomFrame, "&Reset Waves Selections");
  fResetWButton->SetToolTipText("Click here to clear wave selections");
  fResetWButton->Connect("Pressed()", "MyMainFrame", this, "ClearSelectedWaves()");
  hBottomFrame->AddFrame(fResetWButton, new TGLayoutHints(kLHintsExpandX, 5, 5, 3, 4));


  // Add waves of corresponding file
  TGTextButton *fAddButton = new TGTextButton(hBottomFrame, "&Add Waves");
  fAddButton->SetToolTipText("Click here to Add the Corresponding Waves");
  fAddButton->Connect("Pressed()", "MyMainFrame", this, "AddFileWaves()");
  hBottomFrame->AddFrame(fAddButton, new TGLayoutHints(kLHintsExpandX, 5, 5, 3, 4));
/////////////////////////////*************************************************///////////////


  // Plot Button
  TGTextButton *fPlotButton = new TGTextButton(hBottomFrame, "&Plot");
  fPlotButton->SetToolTipText("Click here to generate plot");
  fPlotButton->Connect("Pressed()", "MyMainFrame", this, "PlotSelected()");
  hBottomFrame->AddFrame(fPlotButton, new TGLayoutHints(kLHintsExpandX, 5, 5, 3, 4));


  TGTextButton *fSaveButton = new TGTextButton(hBottomFrame, "&Save");
  fSaveButton->SetToolTipText("Click here to save plot");
  fSaveButton->Connect("Pressed()", "MyMainFrame", this, "Save()");
  hBottomFrame->AddFrame(fSaveButton, new TGLayoutHints(kLHintsExpandX, 5, 5, 3, 4));


  // Exit Button
  TGTextButton *exit = new TGTextButton(hBottomFrame, "&Exit ");
  exit->Connect("Pressed()", "MyMainFrame", this, "DoExit()");
  hBottomFrame->AddFrame(exit, new TGLayoutHints(kLHintsExpandX, 5, 5, 3, 4));
  AddFrame(hBottomFrame, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 1));
	
  // Set a name to the main frame   
 //std::string fitin = std::string(fitfile);
  SetWindowName("sum");
  MapSubwindows();
  
  // Initialize the layout algorithm via Resize()
  Resize(GetDefaultSize());
  
  // Map main frame
  MapWindow();
  fListWBox->Select(1);
  fListFBox->Select(1);

}

MyMainFrame::~MyMainFrame()
{
	// Clean up main frame...
	Cleanup();
	if (fWSelected) {
		fWSelected->Delete();
		delete fWSelected;
	}
}

void MyMainFrame::HandleButtons()
{
	// Handle check button.
	Int_t id;
	TGButton *btn = (TGButton *) gTQSender;
	id = btn->WidgetId();
	
	printf("HandleButton: id = %d\n", id);
	
	if (id == 5){
		fYieldButton->SetOn(kTRUE);
		fPhaseButton->SetOn(kFALSE);
		fLikelyButton->SetOn(kFALSE);
	} 

	if (id == 6) {
		fYieldButton->SetOn(kFALSE);
		fPhaseButton->SetOn(kTRUE);
		fLikelyButton->SetOn(kFALSE);
	}
	
	if (id == 7) {
		fYieldButton->SetOn(kFALSE);
		fPhaseButton->SetOn(kFALSE);
		fLikelyButton->SetOn(kTRUE);
	}
}


void MyMainFrame::AddFileWaves()
	{
			fListWBox->RemoveAll();
  fListWBox->Resize(100,350);
		int j=  fListFBox->GetSelected();
		//cout<<"this is j "<<fListFBox->GetSelected()<<endl;

	for (int i = 0; i < fFitResults->fitResult[j-1][0]->numberWaves() ; ++i) {
    cerr << "Adding : " << fFitResults->fitResult[j-1][0]->iWave[i].name << endl;
    fListWBox->AddEntry(fFitResults->fitResult[j-1][0]->iWave[i].name.c_str(), i+1);
    //sprintf(tmp, "Entry %i", i+1);
    //    fListBox->AddEntry(tmp, i+1);
  }
  fListWBox->Resize(100,400);
  fListWBox->SetMultipleSelections();

}


void MyMainFrame::PlotLikely(){
if( fAWFButton->IsOn() ){
  TCanvas *fcc = fECanvas->GetCanvas(); 
  int j=  fListFBox->GetSelected();
  fcc->cd( j );  
  float *binData = new float[ (fFitResults->numberBins[0] + fFitResults->numberBins[1])  ];
  float *binDataError = new float[ (fFitResults->numberBins[0] + fFitResults->numberBins[1])  ];
  float *xData = new float[ (fFitResults->numberBins[0] + fFitResults->numberBins[1])  ];
  float *xDataError = new float[ (fFitResults->numberBins[0] + fFitResults->numberBins[1])  ];
//cout<< "j  is" << j<<endl;

	//cout<<"*********check here*********"<<fFitResults->maxBin(j-1)<<endl;
  float binSize = ( fFitResults->maxBin(1) - fFitResults->minBin(0) ) / ((fFitResults->numberBins[0] + fFitResults->numberBins[1])/100 - 1) ;
  float minY=0, maxY=0;  
  float miny, fitnumber=0,bindata=0;  
	int top;
	top = (fFitResults->numberBins[0] + fFitResults->numberBins[1])/22;
	  for( int k = 0; k < 22; k++ ){ 

  for( int i = 0; i < (fFitResults->numberBins[0] + fFitResults->numberBins[1])/22 ; i++ ){
if((top*k)+i<fFitResults->numberBins[0]){j=1;}
if((top*k)+i>=fFitResults->numberBins[0]){j=2;}
		if(k%2 == 0 ){

    binData[(top*k)+i] = fFitResults->fitResult[j-1][(top*k)+i-((j-1)*(fFitResults->numberBins[0]))]->minusLogLikelihood();
    if( binData[(top*k)+i] > maxY ) maxY = binData[(top*k)+i];
    if( binData[(top*k)+i] < minY ) minY = binData[(top*k)+i];
    miny=binData[(top*k)]; if(binData[(top*k)+i] < miny){miny=binData[(top*k)+i];fitnumber=i;}
    binDataError[(top*k)+i] = 0;
    xData[(top*k)+i] = fFitResults->minBin(0) + ((float(k)/2) + 0.5 ) * binSize*2 ;
    xDataError[(top*k)+i] = 0;	} // end of if i%2==0

		if(k%2 != 0 ){
   binData[(top*k)+i] = fFitResults->fitResult[j-1][(top*k)+i-((j-1)*(fFitResults->numberBins[0]))]->minusLogLikelihood(); 
    if( binData[(top*k)+i] > maxY ) maxY = binData[(top*k)+i];
    if( binData[(top*k)+i] < minY ) minY = binData[(top*k)+i]; 
    miny=binData[(top*k)]; if(binData[(top*k)+i] < miny){miny=binData[(top*k)+i];fitnumber=i;}
    binDataError[(top*k)+i] = 0;
    xData[(top*k)+i] = fFitResults->minBin(0) + binSize + ((float(k-1)/2) + 0.5 ) * binSize*2 ;
    xDataError[(top*k)+i] = 0;  }// end of if i%2!=0

  } //end of for i loop
//output the lowest likelihood fit_1100-1200_0.fit
		if(k%2 == 0 ){
bindata=fFitResults->minBin(0) + ((float(k)/2) + 0.5 ) * binSize*2 ;
//cout<<"Bin: "<<bindata<<"          Minimum likelihood: "<<miny<<"        from fit: "<<fitnumber<<endl;
cout<<"/lustre/expphy/volatile/clas/clasg12/alghoul/pwafit_jb_v2v/data/"<<(bindata-0.05)*1000<<":"<<(bindata+0.05)*1000<<"/"<<"fit_"<<(bindata-0.05)*1000<<"-"<<(bindata+0.05)*1000<<"_"<<fitnumber<<".fit"<<endl;
}
		if(k%2 != 0 ){
bindata=fFitResults->minBin(0) + binSize + ((float(k-1)/2) + 0.5 ) * binSize*2 ;
//cout<<"Bin: "<<bindata<<"          Minimum likelihood: "<<miny<<"        from fit: "<<fitnumber<<endl;
cout<<"/lustre/expphy/volatile/clas/clasg12/alghoul/pwafit_jb_v2v/data/"<<(bindata-0.05)*1000<<":"<<(bindata+0.05)*1000<<"/"<<"fit_"<<(bindata-0.05)*1000<<"-"<<(bindata+0.05)*1000<<"_"<<fitnumber<<".fit"<<endl;
}

}
  maxY = maxY * 1.1; // add 10% to range for plotting
  
  // draw a frame to define the range
    TH1F *hr = fcc->DrawFrame( fFitResults->minBin(0) - binSize, minY, fFitResults->maxBin(1) + binSize, maxY);
   // hr->SetXTitle("Mass    [GeV]");
    //hr->SetYTitle("Y title");
        
     
    TGraphErrors *graph = new TGraphErrors( (fFitResults->numberBins[0] + fFitResults->numberBins[1])  ,xData,binData,xDataError,binDataError);
  string hTitle("-LogLikelihood");
  
  graph->SetTitle( hTitle.c_str() );
  custom(graph,fcc,j-1);

  delete [] binData;
  delete [] xData;
  delete [] binDataError;
  delete [] xDataError;
  
  graph->Draw("AP");
  fcc->Update();
}//if

else{
  TCanvas *fcc = fECanvas->GetCanvas(); 
  int j=  fListFBox->GetSelected();
  fcc->cd( j );  
  float *binData = new float[ fFitResults->numberBins[j-1]  ];
  float *binDataError = new float[ fFitResults->numberBins[j-1]  ];
  float *xData = new float[ fFitResults->numberBins[j-1]  ];
  float *xDataError = new float[ fFitResults->numberBins[j-1]  ];
  float binSize = ( fFitResults->maxBin(j-1) - fFitResults->minBin(j-1) ) / ( float( fFitResults->numberBins[j-1]  -1 ) );
  float minY=0, maxY=0;

  for( int i = 0; i < fFitResults->numberBins[j-1] ; i++ ){
    binData[i] = fFitResults->fitResult[j-1][i]->minusLogLikelihood() ;
    if( binData[i] > maxY ) maxY = binData[i];
    if( binData[i] < minY ) minY = binData[i];
    binDataError[i] = 0;
    xData[i] = fFitResults->minBin(j-1) + (float(i) + 0.5 ) * binSize ;
    xDataError[i] = 0;
  }
  maxY = maxY * 1.1; // add 10% to range for plotting
  
  // draw a frame to define the range
  TH1F *hr = fcc->DrawFrame( fFitResults->minBin(j-1) - binSize, minY, fFitResults->maxBin(j-1) + binSize, maxY);
  hr->SetXTitle("Mass    [GeV]");
  //hr->SetYTitle("Y title");
        
  TGraphErrors *graph = new TGraphErrors( fFitResults->numberBins[j-1] ,xData,binData,xDataError,binDataError);
  string hTitle("-LogLikelihood");
  
  graph->SetTitle( hTitle.c_str() );
  custom(graph,fcc,j);

  delete [] binData;
  delete [] xData;
  delete [] binDataError;
  delete [] xDataError;
  
  graph->Draw("APC");
  fcc->Update();
}//else
}




void MyMainFrame::PlotPhase(TList *selectedList ){
  TCanvas *fcc = fECanvas->GetCanvas(); 
  int j=  fListFBox->GetSelected();
  fcc->cd( j );   
  
  float *binData = new float[ fFitResults->numberBins[j-1]  ];
  float *binDataPlus = new float[ fFitResults->numberBins[j-1]  ];  // values plus 2PI
  float *binDataMinus = new float[ fFitResults->numberBins[j-1]  ]; // values minus 2PI

  float *binDataError = new float[ fFitResults->numberBins[j-1]  ];
  float *xData = new float[ fFitResults->numberBins[j-1]  ];
  float *xDataError = new float[ fFitResults->numberBins[j-1]  ];
  float binSize = ( fFitResults->maxBin(j-1) - fFitResults->minBin(j-1) ) / ( float( fFitResults->numberBins[j-1]  -1 ) );
  float minY=0, maxY=0;
  
  string hTitle;
  string minus(" - ");
  std::string FileSeparator =" in ";
  
  //  GetPhaseData( selectedList, binData, BinDataError );
  int nWaves =  selectedList->GetEntries();
  int *waveIndex = new int[nWaves];
  
  if( nWaves != 2 ) cerr << "SCREAM ERROR!!  MyMainFrame::PlotPhase() "<< endl;

  for( int n = 0; n < nWaves; n++){
    waveIndex[n] = fFitResults->fitResult[j-1][0]->getWaveIndex(((TGTextLBEntry*)(fWSelected->At(n)))->GetTitle() );
    hTitle.append( ((TGTextLBEntry*)(fWSelected->At(n)))->GetTitle() );

    hTitle.append( minus );
  }

  if(  fFitResults->fitResult[j-1][0]->isInterfering( waveIndex[0], waveIndex[1] ) ){
    //    double *binData = new double[ fFitResults->numberBins[j-1]  ];
    //double *binDataError = new double[ fFitResults->numberBins[j-1]  ];
    
    hTitle.resize( hTitle.size() - 3 ); //trim off last minus
    hTitle.append(FileSeparator);
    cerr << "PhaseDif of : " << hTitle << endl;

    for( int i = 0; i < fFitResults->numberBins[j-1] ; i++ ){
      binData[i] = fFitResults->fitResult[j-1][i]->phaseDif( waveIndex[0], waveIndex[1] );
//cout<<"checking here "<<endl;
      binDataError[i] = fFitResults->fitResult[j-1][i]->phaseDifError( waveIndex[0], waveIndex[1] );
      if( binData[i] > maxY ) maxY = binData[i];
      if( binData[i] < minY ) minY = binData[i]; 
      xData[i] = fFitResults->minBin(j-1) + (float(i) + 0.5 ) * binSize ;
      xDataError[i] = 0;
      
     // binDataPlus[i] = binData[i] + 2.0 * M_PI;
      //binDataMinus[i] = binData[i] - 2.0 * M_PI;
    }
    
    maxY = M_PI * 1.25; // increase range for plotting
    minY = -maxY;
    
    // draw a frame to define the range
    TH1F *hr = fcc->DrawFrame( fFitResults->minBin(j-1) - binSize, minY, fFitResults->maxBin(j-1) + binSize, maxY);
    hr->SetXTitle("Mass    [GeV]");
    //hr->SetYTitle("Y title");
        
    
    TGraphErrors *graph = new TGraphErrors( fFitResults->numberBins[j-1] ,xData,binData,xDataError,binDataError);
   // TGraphErrors *graphPlus = new TGraphErrors( fFitResults->numberBins[j-1] ,xData,binDataPlus,xDataError,binDataError);
   // TGraphErrors *graphMinus = new TGraphErrors( fFitResults->numberBins[j-1] ,xData,binDataMinus,xDataError,binDataError);
    graph->SetTitle( hTitle.c_str() );
    graph->SetMarkerStyle(8);
    //graphPlus->SetMarkerStyle(8);
    //graphPlus->SetMarkerColor(kGreen);
    //graphMinus->SetMarkerStyle(8);
    //graphMinus->SetMarkerColor(kGreen);
    graph->GetYaxis()->SetRangeUser(-5,5);
    graph->Draw("AP");
  custom(graph,fcc,j);

    //graphPlus->Draw("P");
    //graphMinus->Draw("P");

    fcc->Update();
  } else {
    static char mTitle[] = "User Error";
    static char mMsg[] = "PHASE-DIF ERROR:: WAVES MUST BE INTERFERING IN ORDER TO PLOT PHASE DIFFERENCE";
    SendMsg(mTitle, mMsg);
    cerr << "\aWAVES MUST BE INTERFERING IN ORDER TO PLOT PHASE DIFFERENCE\a" << endl;
  }
  delete [] binData;
  delete [] xData;
  delete [] binDataError;
  delete [] xDataError;





	if( fAWFButton->IsOn() ){

  TCanvas *fcc = fECanvas->GetCanvas(); 
  int j=  fListFBox->GetSelected();
  fcc->cd( j );  
  float *binData = new float[ (fFitResults->numberBins[0] + fFitResults->numberBins[1])  ];
  float *binDataError = new float[ (fFitResults->numberBins[0] + fFitResults->numberBins[1])  ];
  float *xData = new float[ (fFitResults->numberBins[0] + fFitResults->numberBins[1])  ];
  float *xDataError = new float[ (fFitResults->numberBins[0] + fFitResults->numberBins[1])  ];
//cout<< "j  is" << j<<endl;

	//cout<<"*********check here*********"<<fFitResults->maxBin(j-1)<<endl;
  float binSize = ( fFitResults->maxBin(1) - fFitResults->minBin(0) ) / ((fFitResults->numberBins[0] + fFitResults->numberBins[1])/100 - 1) ;
  float minY=0, maxY=0;  

  //  GetYieldData( selectedList, binData, BinDataError );
  int nWaves =  selectedList->GetEntries();
  int *wavenex = new int[nWaves];
  int *waveIndex = new int[nWaves];
  int *index = new int[nWaves];
  if( nWaves != 2 ) cerr << "SCREAM ERROR!!  MyMainFrame::PlotPhase() "<< endl;

  for( int n = 0; n < nWaves; n++){
if (j=1){
    waveIndex[n] = fFitResults->fitResult[j-1][0]->getWaveIndex(((TGTextLBEntry*)(fWSelected->At(n)))->GetTitle() );
	//wavenex[n]=waveIndex[n];
cout<<"index of lower mass"<<waveIndex[n]<<endl;  
 }
if (j=2){
    index[n] = fFitResults->fitResult[j-1][0]->getWaveIndex(((TGTextLBEntry*)(fWSelected->At(n)))->GetTitle() );
	//wavenex[n]=index[n];
cout<<"index of higher mass"<<index[n]<<endl;  
 }
    hTitle.append( ((TGTextLBEntry*)(fWSelected->At(n)))->GetTitle() );

    hTitle.append( minus );
	}

  if(  fFitResults->fitResult[1][0]->isInterfering( index[0], index[1] ) ){
    //    double *binData = new double[ fFitResults->numberBins[j-1]  ];
    //double *binDataError = new double[ fFitResults->numberBins[j-1]  ];
    
    hTitle.resize( hTitle.size() - 3 ); //trim off last minus
    hTitle.append(FileSeparator);
    cerr << "PhaseDif of : " << hTitle << endl;
	int top;
	top = (fFitResults->numberBins[0] + fFitResults->numberBins[1])/22;
	  for( int k = 0; k < 22; k++ ){ 

  for( int i = 0; i < (fFitResults->numberBins[0] + fFitResults->numberBins[1])/22 ; i++ ){
if((top*k)+i<fFitResults->numberBins[0]){j=1;wavenex=waveIndex;cout<<"low j "<<wavenex[nWaves]<<endl;}
if((top*k)+i>=fFitResults->numberBins[0]){j=2;wavenex=index;cout<<"high j "<<wavenex[nWaves]<<endl;}
		if(k%2 == 0 ){
cout<<" this is the term " << i-((j-1)*(fFitResults->numberBins[0]))<<endl;
    binData[(top*k)+i] = fFitResults->fitResult[j-1][(top*k)+i-((j-1)*(fFitResults->numberBins[0]))]->phaseDif( wavenex[0], wavenex[1] );
    //binDataError[i] = 0;
    binDataError[(top*k)+i] = fFitResults->fitResult[j-1][(top*k)+i-((j-1)*(fFitResults->numberBins[0]))]->phaseDiffError( wavenex[0], wavenex[1] ); //remove the errors from y axis.
    if( binData[(top*k)+i] > maxY ) maxY = binData[(top*k)+i];
    if( binData[(top*k)+i] < minY ) minY = binData[(top*k)+i];
    xData[(top*k)+i] = fFitResults->minBin(0) + ((float(k)/2) + 0.5 ) * binSize*2 ;
    xDataError[(top*k)+i] = 0;

		}
		if(k%2 != 0 ){

    binData[(top*k)+i] = fFitResults->fitResult[j-1][(top*k)+i-((j-1)*(fFitResults->numberBins[0]))]->phaseDif( wavenex[0], wavenex[1] );
    //binDataError[i] = 0;
    binDataError[(top*k)+i] = fFitResults->fitResult[j-1][(top*k)+i-((j-1)*(fFitResults->numberBins[0]))]->phaseDiffError( wavenex[0], wavenex[1] ); //remove the errors from y axis.
    if( binData[(top*k)+i] > maxY ) maxY = binData[(top*k)+i];
    if( binData[(top*k)+i] < minY ) minY = binData[(top*k)+i];
    xData[(top*k)+i] = fFitResults->minBin(0) + binSize + ((float(k-1)/2) + 0.5 ) * binSize*2 ;
    xDataError[(top*k)+i] = 0;
	}

      
}	
}			
    maxY = M_PI * 1.25; // increase range for plotting
    minY = -maxY;
    
    // draw a frame to define the range
    TH1F *hr = fcc->DrawFrame( fFitResults->minBin(0) - binSize, minY, fFitResults->maxBin(1) + binSize, maxY);
    hr->SetXTitle("Mass    [GeV]");
    //hr->SetYTitle("Y title");
        
    
    TGraphErrors *graph = new TGraphErrors( (fFitResults->numberBins[0] + fFitResults->numberBins[1])  ,xData,binData,xDataError,binDataError);
   // TGraphErrors *graphPlus = new TGraphErrors( fFitResults->numberBins[j-1] ,xData,binDataPlus,xDataError,binDataError);
   // TGraphErrors *graphMinus = new TGraphErrors( fFitResults->numberBins[j-1] ,xData,binDataMinus,xDataError,binDataError);
  //hTitle.resize( hTitle.size()/2 ); 
  graph->SetTitle( hTitle.c_str() );
    graph->SetMarkerStyle(8);
    //graphPlus->SetMarkerStyle(8);
    //graphPlus->SetMarkerColor(kGreen);
    //graphMinus->SetMarkerStyle(8);
    //graphMinus->SetMarkerColor(kGreen);
    graph->GetYaxis()->SetRangeUser(-5,5);

  custom(graph,fcc,j-1);
    graph->Draw("AP");

    //graphPlus->Draw("P");
    //graphMinus->Draw("P");

    fcc->Update();
  } else {
    static char mTitle[] = "User Error";
    static char mMsg[] = "PHASE-DIF ERROR:: WAVES MUST BE INTERFERING IN ORDER TO PLOT PHASE DIFFERENCE";
    SendMsg(mTitle, mMsg);
    cerr << "\aWAVES MUST BE INTERFERING IN ORDER TO PLOT PHASE DIFFERENCE\a" << endl;
  }
  delete [] binData;
  delete [] xData;
  delete [] binDataError;
  delete [] xDataError;
}

}//end of phasediff function





void MyMainFrame::PlotYield(TList *selectedList ){



	if( fAWFButton->IsOn() ){

  TCanvas *fcc = fECanvas->GetCanvas(); 

  int j=  fListFBox->GetSelected();
  fcc->cd( j );   
  string hTitle;
  string comma(", ");
  float *binData = new float[ (fFitResults->numberBins[0] + fFitResults->numberBins[1])  ];
  float *binDataError = new float[ (fFitResults->numberBins[0] + fFitResults->numberBins[1])  ];
  float *xData = new float[ (fFitResults->numberBins[0] + fFitResults->numberBins[1])  ];
  float *xDataError = new float[ (fFitResults->numberBins[0] + fFitResults->numberBins[1])  ];
//cout<< "j  is" << j<<endl;

	//cout<<"*********check here*********"<<fFitResults->maxBin(j-1)<<endl;
  float binSize = ( fFitResults->maxBin(1) - fFitResults->minBin(0) ) / (((fFitResults->numberBins[0] + fFitResults->numberBins[1])/100) -1) ;
  float minY=0, maxY=0;  

  //  GetYieldData( selectedList, binData, BinDataError );
  int nWaves =  selectedList->GetEntries();
  int *wavenex = new int[nWaves];
  int *waveIndex = new int[nWaves];
  int *index = new int[nWaves];
cout << "nWaves "<<nWaves<<endl;
  for( int n = 0; n < nWaves; n++){
if (j=1){
    waveIndex[n] = fFitResults->fitResult[j-1][0]->getWaveIndex(((TGTextLBEntry*)(fWSelected->At(n)))->GetTitle() );
	//wavenex[n]=waveIndex[n];
cout<<"index of lower mass"<<waveIndex[n]<<endl;  
 }
if (j=2){
    index[n] = fFitResults->fitResult[j-1][0]->getWaveIndex(((TGTextLBEntry*)(fWSelected->At(n)))->GetTitle() );
	//wavenex[n]=index[n];
cout<<"index of higher mass"<<index[n]<<endl;  
 }

	//cout<<"this is the waveindex " << waveIndex[n]<<endl;
    hTitle.append( ((TGTextLBEntry*)(fWSelected->At(n)))->GetTitle() );   

    hTitle.append( comma );
  }

  hTitle.resize( hTitle.size() - 2 ); //trim off last comma

//if(j=1){wavenex=waveIndex;cout<<"low j"<<j<<endl;}
//if(j=2){wavenex=index;cout<<"high j"<<j<<endl;}
	int top=0;
	top = (fFitResults->numberBins[0] + fFitResults->numberBins[1])/22;
	  for( int k = 0; k < 22; k++ ){ 
  for( int i = 0; i < (fFitResults->numberBins[0] + fFitResults->numberBins[1])/22 ; i++ ){
if((top*k)+i<fFitResults->numberBins[0]){j=1;wavenex=waveIndex;cout<<"low j "<<wavenex[nWaves]<<endl;}
if((top*k)+i>=fFitResults->numberBins[0]){j=2;wavenex=index;cout<<"high j "<<wavenex[nWaves]<<endl;}
		if(k%2 == 0 ){
  for( int n = 0; n < nWaves; n++){
	if(wavenex[n] == -1){
    binData[(top*k)+i] = 0;
    //binDataError[i] = 0;
    binDataError[(top*k)+i] = 0; //remove the errors from y axis.
    xData[(top*k)+i] = 1.1 + ((float(k)/2) + 0.5 ) * binSize*2 ;
    xDataError[(top*k)+i] = 0;}
else{


cout<<" this is the term " << (top*k)+i-((j-1)*(fFitResults->numberBins[0]))<<endl;
    binData[(top*k)+i] = fFitResults->fitResult[j-1][(top*k)+i-((j-1)*(fFitResults->numberBins[0]))]->yield( nWaves, wavenex );
    //binDataError[i] = 0;
    binDataError[(top*k)+i] = fFitResults->fitResult[j-1][(top*k)+i-((j-1)*(fFitResults->numberBins[0]))]->yieldError( nWaves, wavenex ); //remove the errors from y axis.
    if( binData[(top*k)+i] > maxY ) maxY = binData[(top*k)+i];
    if( binData[(top*k)+i] < minY ) minY = 0;
    xData[(top*k)+i] = 1.1 + ((float(k)/2) + 0.5 ) * binSize*2 ;
    xDataError[(top*k)+i] = 0;

}
}
		}
		if(k%2 != 0 ){
  for( int n = 0; n < nWaves; n++){
   if(wavenex[n] == -1){
    binData[(top*k)+i] = 0;
    //binDataError[i] = 0;
    binDataError[(top*k)+i] = 0; //remove the errors from y axis.
    xData[(top*k)+i] = 1.1 + binSize + ((float(k-1)/2) + 0.5 ) * binSize*2 ;
    xDataError[(top*k)+i] = 0;}

else{

    binData[(top*k)+i] = fFitResults->fitResult[j-1][(top*k)+i-((j-1)*(fFitResults->numberBins[0]))]->yield( nWaves, wavenex );
    //binDataError[i] = 0;
    binDataError[(top*k)+i] = fFitResults->fitResult[j-1][(top*k)+i-((j-1)*(fFitResults->numberBins[0]))]->yieldError( nWaves, wavenex ); //remove the errors from y axis.
    if( binData[(top*k)+i] > maxY ) maxY = binData[(top*k)+i];
    if( binData[(top*k)+i] < minY ) minY = 0;
    xData[(top*k)+i] = 1.1 + binSize + ((float(k-1)/2) + 0.5 ) * binSize*2 ;
    xDataError[(top*k)+i] = 0;
	}   
}   	
			}
  }
}
  maxY = maxY * 1.1; // add 10% to range for plotting
  TH1F *hr = fcc->DrawFrame( fFitResults->minBin(0) - binSize, minY, fFitResults->maxBin(1) + binSize, maxY);

  //hr->SetXTitle("Mass    [GeV]");
  //hr->SetYTitle("Y title");
  // fill graph
  TGraphErrors *graph = new TGraphErrors( (fFitResults->numberBins[0] + fFitResults->numberBins[1])  ,xData,binData,xDataError,binDataError);
cout<<"maxy is "<<binData[100]<<" binsize "<<binSize<<" "<<fFitResults->minBin(0)<<" "<<fFitResults->maxBin(1)<<endl;


 // hTitle.resize( hTitle.size()/ 2 ); 
  graph->SetTitle( hTitle.c_str() );
  
  //graph->SetMarkerStyle(8);
  custom(graph,fcc,j-1);
  graph->Print();
  gPad->SetFrameBorderMode(0);
  //graph->GetYaxis()->SetLimits(0,maxY);

  delete [] binData;
  delete [] binDataError;
  delete [] xData;
  delete [] xDataError;

  graph->Draw("AP");

  //graph->GetYaxis()->SetRangeUser(0,maxY); //added to enforce keeping the graph within range
  fcc->Update();

if( fBWButton->IsOn() ){

	double lowmass = FHSlider->GetMinPosition();
	double highmass = FHSlider->GetMaxPosition();
	//double bwwidth = atof(WidthTextBox->GetText());
	//double amplitude = atof(AmpTextBox->GetText());
	TF1 *signalFcn = new TF1("signalFcn",BW,lowmass,highmass,3);
   	signalFcn->SetLineColor(kRed);
  	signalFcn->SetParameter(0,10000);
  	signalFcn->SetParameter(1,1);
  	signalFcn->SetParameter(2,1.1); 
	signalFcn->SetParNames("Amp","Mass","Width");
        graph->Fit("signalFcn","R");
        Double_t par[3];

        signalFcn->GetParameters(par);
        signalFcn->SetParameters(&par[0]);

        custom(graph,fcc,j-1);
	graph->Draw("AP");
   	signalFcn->Draw("same");
TPaveStats *st = (TPaveStats*)graph->FindObject("stats");

st->SetShadowColor(0);
st->SetX1NDC(0.6394094);
st->SetX2NDC(0.9992714);
st->SetY1NDC(0.7558458);
st->SetY2NDC(0.9953293);
st->SetFillColor(kWhite);
  	//graph->GetYaxis()->SetRangeUser(0,maxY); //added to enforce keeping the graph within range
  	fcc->Update(); 
		} 
 else{
  custom(graph,fcc,j-1);    
  graph->Draw("AP");
  gPad->SetFrameBorderMode(0);
  //graph->GetYaxis()->SetRangeUser(0,maxY); //added to enforce keeping the graph within range
  fcc->Update();
}


}
else{

  TCanvas *fcc = fECanvas->GetCanvas(); 
  int j=  fListFBox->GetSelected();
  fcc->cd( j );   
  string hTitle;
  string comma(", ");
	//cout<< "this is what I'm getting" << fFitResults->numberBins[j-1] <<endl;
  float *binData = new float[ fFitResults->numberBins[j-1]  ];
  float *binDataError = new float[ fFitResults->numberBins[j-1]  ];
  float *xData = new float[ fFitResults->numberBins[j-1]  ];
  float *xDataError = new float[ fFitResults->numberBins[j-1]  ];
cout<< "j  is" << j<<endl;

  float binSize = ( fFitResults->maxBin(j-1) - fFitResults->minBin(j-1) ) / ( float( fFitResults->numberBins[j-1]  - 1 ) );
  float minY=0, maxY=0;  

  //  GetYieldData( selectedList, binData, BinDataError );
  int nWaves =  selectedList->GetEntries();

  int *waveIndex = new int[nWaves];
cout << "nWaves "<<nWaves<<endl;
  for( int n = 0; n < nWaves; n++){
    waveIndex[n] = fFitResults->fitResult[j-1][0]->getWaveIndex(((TGTextLBEntry*)(fWSelected->At(n)))->GetTitle() );

	cout<<"this is the waveindex " << waveIndex[n]<<endl;
    hTitle.append( ((TGTextLBEntry*)(fWSelected->At(n)))->GetTitle() );   

    hTitle.append( comma );
  }

  hTitle.resize( hTitle.size() - 2 ); //trim off last comma



  for( int i = 0; i < fFitResults->numberBins[j-1] ; i++ ){
		if(i%2 == 0 ){
  for( int n = 0; n < nWaves; n++){
	if(waveIndex[n] == -1){
    binData[i] = 0;
    //binDataError[i] = 0;
    binDataError[i] = 0; //remove the errors from y axis.
    xData[i] = 0;
    xDataError[i] = 0;}
else{
    binData[i] = fFitResults->fitResult[j-1][i]->yield( nWaves, waveIndex );
    //binDataError[i] = 0;
    binDataError[i] = fFitResults->fitResult[j-1][i]->yieldError( nWaves, waveIndex ); //remove the errors from y axis.
    if( binData[i] > maxY ) maxY = binData[i];
    if( binData[i] < minY ) minY = 0;
    xData[i] = fFitResults->minBin(j-1) + ((float(i)/2) + 0.5 ) * binSize*2 ;
    xDataError[i] = 0;
}
}
		}
		if(i%2 != 0 ){
  for( int n = 0; n < nWaves; n++){
   if(waveIndex[n] == -1){
    binData[i] = 0;
    //binDataError[i] = 0;
    binDataError[i] = 0; //remove the errors from y axis.
    xData[i] = 0;
    xDataError[i] = 0;}

else{
    binData[i] = fFitResults->fitResult[j-1][i]->yield( nWaves, waveIndex );
    //binDataError[i] = 0;
    binDataError[i] = fFitResults->fitResult[j-1][i]->yieldError( nWaves, waveIndex ); //remove the errors from y axis.
    if( binData[i] > maxY ) maxY = binData[i];
    if( binData[i] < minY ) minY = 0;
    xData[i] = fFitResults->minBin(j-1) + binSize + ((float(i-1)/2) + 0.5 ) * binSize*2 ;
    xDataError[i] = 0;
	}
}	
			}
  }
  maxY = maxY * 1.1; // add 10% to range for plotting


	TH1F *hr = fcc->DrawFrame( fFitResults->minBin(j-1) - binSize, minY, fFitResults->maxBin(j-1) + binSize, maxY);
 // hr[j-1]->SetXTitle("Mass    [GeV]");
  //hr->SetYTitle("Y title");

  // fill graph
  TGraphErrors *graph = new TGraphErrors( fFitResults->numberBins[j-1] ,xData,binData,xDataError,binDataError);


  graph->SetTitle( hTitle.c_str() );
  
  //graph->SetMarkerStyle(8);
  custom(graph,fcc,j);
  graph->Print();
  //graph->GetYaxis()->SetLimits(0,maxY);
  gPad->SetFrameBorderMode(0);
  delete [] binData;
  delete [] binDataError;
  delete [] xData;
  delete [] xDataError;
   graph->Draw("AP");
  fcc->Update();


	if( fBWButton->IsOn() ){

	double lowmass = FHSlider->GetMinPosition();
	double highmass = FHSlider->GetMaxPosition();
	//double bwwidth = atof(WidthTextBox->GetText());
	//double amplitude = atof(AmpTextBox->GetText());
	TF1 *signalFcn = new TF1("signalFcn",BW,lowmass,highmass,3);
   	signalFcn->SetLineColor(kBlue);
  	signalFcn->SetParameter(0,10000);
  	signalFcn->SetParameter(1,1);
  	signalFcn->SetParameter(2,1.1);
	signalFcn->SetParNames("Amp","Mass","Width");
        graph->Fit("signalFcn","R");
        Double_t par[3];

        signalFcn->GetParameters(par);
        signalFcn->SetParameters(&par[0]);
	signalFcn->SetParNames("Amp","Mass","Width");
        custom(graph,fcc,j);
	graph->Draw("AP");
   	signalFcn->Draw("same");
TPaveStats *st = (TPaveStats*)graph->FindObject("stats");

st->SetShadowColor(0);
st->SetX1NDC(0.6394094);
st->SetX2NDC(0.9992714);
st->SetY1NDC(0.7558458);
st->SetY2NDC(0.9953293);
st->SetFillColor(kWhite);
  	//graph->GetYaxis()->SetRangeUser(0,maxY); //added to enforce keeping the graph within range
  	fcc->Update(); 
		} 
 else{
  custom(graph,fcc,j);
  graph->Draw("AP");
  gPad->SetFrameBorderMode(0);
  //graph->GetYaxis()->SetRangeUser(0,maxY); //added to enforce keeping the graph within range
  fcc->Update();
}
}
}



void MyMainFrame::PlotSelected(){
	// Writes selected entries in TList if multiselection.
  bool isPlotting = false;
  fWSelected->Clear();
  
  if (fListWBox->GetMultipleSelections()) {
    fListWBox->GetSelectedEntries(fWSelected);
    
    if( fPhaseButton->IsOn() )
      if( fWSelected->GetEntries() != 2 ){
	Printf("\aSelect two and only two waves\n");
	static char mTitle[] = "User Error";
	static char mMsg[] = "PHASE DIFFERENCE ERROR:: Select two interfering waves.";
	SendMsg(mTitle, mMsg);
      } else{
	isPlotting = true;
	Printf("Phase Difference of:\n");
	fWSelected->ls();
	PlotPhase( fWSelected );
      }
    else if( fYieldButton->IsOn() ){ // Yield Button is on
      if( fWSelected->GetEntries() > 0 ){
	isPlotting = true;
	Printf("Yield of: %d waves\n", fWSelected->GetEntries() );
	cerr << "The TList is a: " <<  fWSelected->GetEntries() << endl;
	cerr << "The TList is a: " << ((TGTextLBEntry*)(fWSelected->At(0)))->GetTitle() << endl;
	fWSelected->ls();
	PlotYield( fWSelected );
      } else{
	static char mTitle[] = "User Error";
	static char mMsg[] = "YIELD ERROR:: YOU MUST SELECT AT LEAST ONE WAVE";
	SendMsg(mTitle, mMsg);
	Printf("Must select at least one wave...\n");
      }
    } else if( fLikelyButton->IsOn() ){ // Likelihood button is on
      
      PlotLikely();

    }
         //if( fBWButton->IsOn() ) { 
	//PlotBW(); // Likelihood button is on
	//}
  } else {
    Printf("Code error: ListBox should be Multi Selectable!!\n");
  }
  

}



void MyMainFrame::PlotBW(){

  TCanvas *fcc = fECanvas->GetCanvas(); 
  int j=  fListFBox->GetSelected();
  fcc->cd( j );
cout<<"this is to try BW"<<endl; 
   TF1 *signalFcn = new TF1("signalFcn","TMath::Landau(x,[0],[1],0)",1.1,2);
   signalFcn->SetLineColor(kBlue);  
  signalFcn->SetParameter(0,0.2);
  signalFcn->SetParameter(1,0.3);
 // signalFcn->SetParameter(2,1.3);
  signalFcn->Draw();
   // fcc->Update();
}


void MyMainFrame::AddWaveFiles(){

  TCanvas *fcc = fECanvas->GetCanvas(); 
  int j=  fListFBox->GetSelected();
  fcc->cd( j );   
  fListFBox->SetMultipleSelections();
  fListFBox->GetSelectedEntries(fFSelected);
  int k=  fFSelected->GetEntries();
  fcc->Divide(k,k);
}



void MyMainFrame::DoSlider()
{
   // Handle slider widgets.

   Int_t id;
   int locVar;
   /*TGFrame *frm = (TGFrame *) gTQSender;
   if (frm->IsA()->InheritsFrom(TGSlider::Class())) {
      TGSlider *sl = (TGSlider*) frm;
      id = sl->WidgetId();
   } else {
      TGDoubleSlider *sd = (TGDoubleSlider *) frm;
      id = sd->WidgetId();
   }
         WhichButton=-100; //  i.e. not a button event
         locVar=pos;
         SliderReading=((double)locVar)/100.0;  //  double

         WhichSlider=-200+(int)id;
         Glauber=0;  // If any of state button pressed,
                     // The wavefunction is not Glauber type

         if (WhichSlider > 50)
         {
			 WhichSlider=100;
			 Glauber=1;  // If the energy slider is moved, Energy evaluated
                         // The wavefunction will be of Glauber type

			 SliderReading=((double)locVar)/300.0;  //  double
	     }
   Restart();
*/

}



void MyMainFrame::PrintSelected(){
	// Writes selected entries in TList if multiselection.
  bool isPlotting = false;
  fWSelected->Clear();
  
  if (fListWBox->GetMultipleSelections()) {
    fListWBox->GetSelectedEntries(fWSelected);
    
    if( fPhaseButton->IsOn() )
      if( fWSelected->GetEntries() != 2 )
	Printf("Select two and only two waves\n");
      else{
	isPlotting = true;
	Printf("Phase Difference of:\n");
	fWSelected->ls();
      }
    else{ // Yield Button is on
      if( fWSelected->GetEntries() > 0 ){
	isPlotting = true;
	Printf("Yield of: %d waves\n", fWSelected->GetEntries() );
	cerr << "The TList is a: " <<  fWSelected->GetEntries() << endl;
	cerr << "The TList is a: " << ((TGTextLBEntry*)(fWSelected->At(0)))->GetTitle() << endl;
	fWSelected->ls();
      } else{
	Printf("Must select at least one wave...\n");
      }
    }


  } else {
    Printf("Code error: ListBox should be Multi Selectable!!\n");
  }
  
  // Draws function graphics in randomly choosen interval  
  if( isPlotting ){
    TF1 *f1 = new TF1("f1","sin(x)/x",0,gRandom->Rndm()*10); 
    f1->SetFillColor(19); 
    f1->SetFillStyle(1);
    f1->SetLineWidth(3);
    TCanvas *fcc = fECanvas->GetCanvas(); 
    fcc->cd();
    f1->Draw();

    fcc->Update();
  } 	
}





void MyMainFrame::ClearSelectedWaves(){
  fListWBox->SetMultipleSelections( kFALSE );
  fListWBox->SetMultipleSelections( kTRUE );
}

void MyMainFrame::ClearSelectedFiles(){
  fListFBox->SetMultipleSelections( kFALSE );
 // fListFBox->SetMultipleSelections( kTRUE );
}




void MyMainFrame::SaveSelected(TList *selectedList){


  int nWaves =  selectedList->GetEntries();
  int *waveIndex = new int[nWaves];
  int j=  fListFBox->GetSelected();
  string cTitle;
  string comma(", ");
  string separator(":");
  string cYield("-yield.png");
  string cPhase("-phase.png");
  for( int n = 0; n < nWaves; n++){
    waveIndex[n] = fFitResults->fitResult[j-1][0]->getWaveIndex(((TGTextLBEntry*)(fWSelected->At(n)))->GetTitle() );
    cTitle.append( ((TGTextLBEntry*)(fWSelected->At(n)))->GetTitle() );   

    cTitle.append( separator );
  }
    cTitle.resize( cTitle.size() - 1 ); //trim off last comma
    
   if( fPhaseButton->IsOn() ){

   cTitle.append( cPhase );
  fECanvas->GetCanvas()->SaveAs(cTitle.c_str());
       }

  if( fYieldButton->IsOn() ){

   cTitle.append( cYield );
  fECanvas->GetCanvas()->SaveAs(cTitle.c_str());
       }

}




void MyMainFrame::Save(){

  bool isPlotting = false;
  fWSelected->Clear();
  
  if (fListWBox->GetMultipleSelections()) {
    fListWBox->GetSelectedEntries(fWSelected);
    
    if( fPhaseButton->IsOn() )
      if( fWSelected->GetEntries() != 2 ){
	Printf("\aSelect two and only two waves\n");
	static char mTitle[] = "User Error";
	static char mMsg[] = "PHASE DIFFERENCE ERROR:: Select two interfering waves.";
	SendMsg(mTitle, mMsg);
      } else{
	isPlotting = true;
	Printf("Phase Difference of:\n");
	fWSelected->ls();
	SaveSelected( fWSelected );
      }
    else if( fYieldButton->IsOn() ){ // Yield Button is on
      if( fWSelected->GetEntries() > 0 ){
	isPlotting = true;
	Printf("Yield of: %d waves\n", fWSelected->GetEntries() );
	cerr << "The TList is a: " <<  fWSelected->GetEntries() << endl;
	cerr << "The TList is a: " << ((TGTextLBEntry*)(fWSelected->At(0)))->GetTitle() << endl;
	fWSelected->ls();
	SaveSelected( fWSelected );
      } else{
	static char mTitle[] = "User Error";
	static char mMsg[] = "YIELD ERROR:: YOU MUST SELECT AT LEAST ONE WAVE";
	SendMsg(mTitle, mMsg);
	Printf("Must select at least one wave...\n");
      }
    } else { // Likelihood button is on
      
        fECanvas->GetCanvas()->SaveAs("Likelyhood-function.png");

    }
    
  } else {
    Printf("Code error: ListBox should be Multi Selectable!!\n");
  }
  
}






void MyMainFrame::HandleMenu( int id ){
  
  switch( id ) {
  case  M_FILE_EXIT:
    Printf("M_FILE_EXIT\n");
    DoExit();
    break;
  case  M_CANVAS_SAVEAS:
    {
    Printf("M_CANVAS_SAVEAS\n");
    static TString dir(".");
    TGFileInfo fi;
    fi.fFileTypes = fSaveAsFileTypes;
    fi.fIniDir    = StrDup(dir);
    new TGFileDialog(fClient->GetRoot(), this, kFDSave, &fi);
    cerr << "Saving canvas as: " << SaveAsFile.fFilename << endl;
    (fECanvas->GetCanvas())->SaveAs(fi.fFilename);
    }
    break;

  case M_CANVAS_1X1:
    SetCanvasDivision(1,1);
    break;
  case M_CANVAS_1X2:
    SetCanvasDivision(1,2);
    break;
  case M_CANVAS_1X3:
    SetCanvasDivision(1,3);
    break;
  case M_CANVAS_2X2:
    SetCanvasDivision(2,2);
    break;
  case M_CANVAS_2X3:
    SetCanvasDivision(2,3);
    break;
  case M_CANVAS_3X3:
    SetCanvasDivision(3,3);
    break;
  case M_CANVAS_3X2:
    SetCanvasDivision(3,2);
    break;
  case M_HELP_HELP:
    {
      char aTitle[] = "HELP";
      char aMsg[] = 
	"Documentation along with \n"
	"online wiki support is coming...\n";
      int j;
            
      new TGMsgBox(gClient->GetRoot(), this,
		   aTitle, aMsg,
		   kMBIconAsterisk, kMBOk, &j);
    }
    break;



  case M_HELP_ABOUT:
    {
      char aTitle[] = "ABOUT";
      char aMsg[] = 
	"pwaFitView - View PWA Fit Results \n" 
	"authored by Paul Eugenio \n"
	"Experimental Hadronic Physics\n"
	"Florida State University\n"
	"Tallahassee, FL, 32306, USA\n"
	"(C) Copyright 2011";
      int j;
            
      new TGMsgBox(gClient->GetRoot(), this,
		   aTitle, aMsg,
		   kMBIconAsterisk, kMBOk, &j);
    }
    break;
  default:
    Printf("Item not yet implimented\n");
    
  }
  
}

void MyMainFrame::SetCanvasDivision( int aRows, int aCols){
  fNumberCanvases = aRows * aCols;
  
  TCanvas * canvas = fECanvas->GetCanvas();
  canvas->cd();
  canvas->SetFillColor(0);
  canvas->SetBorderMode(kFALSE);
  canvas->Clear("");
  canvas->Divide( aCols, aRows );
  //ClearTGraphStack();
  for (int i = 1; i <= fNumberCanvases; i++){
    canvas->cd(i);
    gPad->SetFillColor(0);
    gPad->SetBorderMode(kFALSE);
    gPad->SetFrameBorderMode(kFALSE);
    gPad->Clear("");
  }
  
  if( fNumberCanvases > 1 )
    fCurrentCanvas = 3;
  else 
    fCurrentCanvas = 0;
  canvas->Update();
}


int MyMainFrame::CurrentCanvas(){
  
    int val  =   fCurrentCanvas;
    if( fNumberCanvases > 1){
      fCurrentCanvas++;
      if( fCurrentCanvas > fNumberCanvases )
	fCurrentCanvas = 1;
    }else fCurrentCanvas = 0;
    cerr << "current canvas is " << fCurrentCanvas;
    return val;
}

void MyMainFrame::DoExit()
{
	Printf("Slot DoExit()");
	gApplication->Terminate(0);
}


void MyMainFrame::SendMsg(char *aTitle, char *aMsg){
  char tTitle[] = "test Title";
  char tMsg[] = "test Messsage";
  int retval;

 new TGMsgBox(gClient->GetRoot(), this,
                aTitle, aMsg,
                kMBIconStop, 0, &retval);
  cerr << "Sending MsGBox" << endl;

}


