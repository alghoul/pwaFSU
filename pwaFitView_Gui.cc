
#include "pwaFitView_Gui.h"


const char *MyMainFrame::fSaveAsFileTypes[] = { 
  "gif"        , "*.gif" , 
  "ps"         , "*.ps"  ,
  "eps"        , "*.eps" ,
  "C source"   , "*.C"   , 
  "root file" , "*.root", 
  //  "ps"         , "*"     , 
  0       , 0};




void MyMainFrame::DoSelect()
{
	Printf("Slot DoSelect()");
}

//  int numberFitBins

MyMainFrame::MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h, PwaFitResults *afitResults, char *fitfile ) :
TGMainFrame(p, w, h)
{

  fFitResults = afitResults;
  fCurrentCanvas =0;
  fNumberCanvases = 3;

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
  fListBox = new TGListBox(hTopFrame, 100);
  fSelected = new TList;
  char tmp[20];
  for (int i = 0; i < fFitResults->fitResult[0]->numberWaves() ; ++i) {
    cerr << "Adding : " << fFitResults->fitResult[0]->iWave[i].name << endl;
    fListBox->AddEntry(fFitResults->fitResult[0]->iWave[i].name.c_str(), i+1);
    //sprintf(tmp, "Entry %i", i+1);
    //    fListBox->AddEntry(tmp, i+1);
  }
  fListBox->Resize(100,150);
  fListBox->SetMultipleSelections();
  hTopFrame->AddFrame(fListBox, new TGLayoutHints(kLHintsTop | kLHintsLeft |
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

  
  fButtonGroup->Show();
  
  hTopFrame->AddFrame(fButtonGroup, new TGLayoutHints( kLHintsCenterY,
						      1, 1, 1, 1));
  



 
  // embedded canvas
  //  fECanvas = new TRootEmbeddedCanvas("Ecanvas",hTopFrame,416,368);
  fECanvas = new TRootEmbeddedCanvas("Ecanvas",hTopFrame,510,410 );
  hTopFrame->AddFrame(fECanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY,2,2,2,2));
  //fECanvas->MoveResize(240,16,416,368);
  
  AddFrame(hTopFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY|kLHintsRight, 2, 2, 5, 1));
  
  
	
  // Create a horizontal frame containing button(s)
  TGHorizontalFrame *hBottomFrame = new TGHorizontalFrame(this, 150, 60, kFixedWidth);
  // Reset Button
  TGTextButton *fResetButton = new TGTextButton(hBottomFrame, "&Reset Selections");
  fResetButton->SetToolTipText("Click here to clear wave selections");
  fResetButton->Connect("Pressed()", "MyMainFrame", this, "ClearSelectedWaves()");
  hBottomFrame->AddFrame(fResetButton, new TGLayoutHints(kLHintsExpandX, 5, 5, 3, 4));
  
/*  //Add Fits
  TGTextButton *fAddButton = new TGTextButton(hBottomFrame, "&Add");
  fAddButton->SetToolTipText("Click here to Add a fit file");
  fAddButton->Connect("Pressed()", "MyMainFrame", this, "AddFit()");
  hBottomFrame->AddFrame(fAddButton, new TGLayoutHints(kLHintsExpandX, 5, 5, 3, 4));  
*/

  // Plot Button
  TGTextButton *fPlotButton = new TGTextButton(hBottomFrame, "&Plot");
  fPlotButton->SetToolTipText("Click here to generate plot");
  fPlotButton->Connect("Pressed()", "MyMainFrame", this, "PlotSelected()");
  hBottomFrame->AddFrame(fPlotButton, new TGLayoutHints(kLHintsExpandX, 5, 5, 3, 4));
  // Exit Button
  TGTextButton *exit = new TGTextButton(hBottomFrame, "&Exit ");
  exit->Connect("Pressed()", "MyMainFrame", this, "DoExit()");
  hBottomFrame->AddFrame(exit, new TGLayoutHints(kLHintsExpandX, 5, 5, 3, 4));
  AddFrame(hBottomFrame, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 1));
	
  // Set a name to the main frame   
 std::string fitin = std::string(fitfile);
  SetWindowName(fitfile);
  MapSubwindows();
  
  // Initialize the layout algorithm via Resize()
  Resize(GetDefaultSize());
  
  // Map main frame
  MapWindow();
  fListBox->Select(1);


}

MyMainFrame::~MyMainFrame()
{
	// Clean up main frame...
	Cleanup();
	if (fSelected) {
		fSelected->Delete();
		delete fSelected;
	}
}

void MyMainFrame::HandleButtons()
{
	// Handle check button.
	Int_t id;
	TGButton *btn = (TGButton *) gTQSender;
	id = btn->WidgetId();
	
	printf("HandleButton: id = %d\n", id);
	/*
	if (id == 5){
		fYieldButton->SetState(kTRUE);
		fPhaseButton->SetState(kFALSE);
	} else {
		fYieldButton->SetState(kFALSE);
		fPhaseButton->SetState(kTRUE);
	}
	*/
}


void MyMainFrame::PlotLikely(){
  TCanvas *fcc = fECanvas->GetCanvas(); 
  fcc->cd( CurrentCanvas() );  
  
  float *binData = new float[ fFitResults->numberBins ];
  float *binDataError = new float[ fFitResults->numberBins ];
  float *xData = new float[ fFitResults->numberBins ];
  float *xDataError = new float[ fFitResults->numberBins ];
  float binSize = ( fFitResults->maxBin() - fFitResults->minBin() ) / ( float( fFitResults->numberBins -1 ) );
  float minY=0, maxY=0;

  for( int i = 0; i < fFitResults->numberBins; i++ ){
    binData[i] = fFitResults->fitResult[i]->minusLogLikelihood() ;
    if( binData[i] > maxY ) maxY = binData[i];
    if( binData[i] < minY ) minY = binData[i];
    binDataError[i] = 0;
    xData[i] = fFitResults->minBin() + (float(i) + 0.5 ) * binSize ;
    xDataError[i] = 0;
  }
  maxY = maxY * 1.1; // add 10% to range for plotting
  
  // draw a frame to define the range
  TH1F *hr = fcc->DrawFrame( fFitResults->minBin() - binSize, minY, fFitResults->maxBin() + binSize, maxY);
  hr->SetXTitle("Mass    [GeV]");
  //hr->SetYTitle("Y title");
        
  TGraphErrors *graph = new TGraphErrors( fFitResults->numberBins,xData,binData,xDataError,binDataError);
  string hTitle("-LogLikelihood");
  graph->SetTitle( hTitle.c_str() );
  graph->SetMarkerStyle(8);
  
  delete [] binData;
  delete [] xData;
  delete [] binDataError;
  delete [] xDataError;
  
  graph->Draw("APC");
  fcc->Update();
}




void MyMainFrame::PlotPhase(TList *selectedList ){
  TCanvas *fcc = fECanvas->GetCanvas(); 
  fcc->cd( CurrentCanvas() );  
  
  float *binData = new float[ fFitResults->numberBins ];
  float *binDataPlus = new float[ fFitResults->numberBins ];  // values plus 2PI
  float *binDataMinus = new float[ fFitResults->numberBins ]; // values minus 2PI

  float *binDataError = new float[ fFitResults->numberBins ];
  float *xData = new float[ fFitResults->numberBins ];
  float *xDataError = new float[ fFitResults->numberBins ];
  float binSize = ( fFitResults->maxBin() - fFitResults->minBin() ) / ( float( fFitResults->numberBins -1 ) );
  float minY=0, maxY=0;
  
  string hTitle;
  string minus(" - ");
  
  
  //  GetPhaseData( selectedList, binData, BinDataError );
  int nWaves =  selectedList->GetEntries();
  int *waveIndex = new int[nWaves];
  
  if( nWaves != 2 ) cerr << "SCREAM ERROR!!  MyMainFrame::PlotPhase() "<< endl;
  
  for( int n = 0; n < nWaves; n++){
    waveIndex[n] = fFitResults->fitResult[0]->getWaveIndex(((TGTextLBEntry*)(fSelected->At(n)))->GetTitle() );
    hTitle.append( ((TGTextLBEntry*)(fSelected->At(n)))->GetTitle() );
    hTitle.append( minus );
  }

  if(  fFitResults->fitResult[0]->isInterfering( waveIndex[0], waveIndex[1] ) ){
    //    double *binData = new double[ fFitResults->numberBins ];
    //double *binDataError = new double[ fFitResults->numberBins ];
    
    hTitle.resize( hTitle.size() - 3 ); //trim off last minus
    cerr << "PhaseDif of : " << hTitle << endl;
    
    for( int i = 0; i < fFitResults->numberBins; i++ ){
      binData[i] = fFitResults->fitResult[i]->phaseDif( waveIndex[0], waveIndex[1] );
      binDataError[i] = fFitResults->fitResult[i]->phaseDifError( waveIndex[0], waveIndex[1] );
      if( binData[i] > maxY ) maxY = binData[i];
      if( binData[i] < minY ) minY = binData[i]; 
      xData[i] = fFitResults->minBin() + (float(i) + 0.5 ) * binSize ;
      xDataError[i] = 0;
      
      binDataPlus[i] = binData[i] + 2.0 * M_PI;
      binDataMinus[i] = binData[i] - 2.0 * M_PI;
    }
    
    maxY = M_PI * 1.25; // increase range for plotting
    minY = -maxY;
    
    // draw a frame to define the range
    TH1F *hr = fcc->DrawFrame( fFitResults->minBin() - binSize, minY, fFitResults->maxBin() + binSize, maxY);
    hr->SetXTitle("Mass    [GeV]");
    //hr->SetYTitle("Y title");
        
    
    TGraphErrors *graph = new TGraphErrors( fFitResults->numberBins,xData,binData,xDataError,binDataError);
    TGraphErrors *graphPlus = new TGraphErrors( fFitResults->numberBins,xData,binDataPlus,xDataError,binDataError);
    TGraphErrors *graphMinus = new TGraphErrors( fFitResults->numberBins,xData,binDataMinus,xDataError,binDataError);
    graph->SetTitle( hTitle.c_str() );
    graph->SetMarkerStyle(8);
    graphPlus->SetMarkerStyle(8);
    graphPlus->SetMarkerColor(kGreen);
    graphMinus->SetMarkerStyle(8);
    graphMinus->SetMarkerColor(kGreen);

    graph->Draw("AP");
    graphPlus->Draw("P");
    graphMinus->Draw("P");

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





void MyMainFrame::PlotYield(TList *selectedList ){
  TCanvas *fcc = fECanvas->GetCanvas(); 
  fcc->cd( CurrentCanvas() );  
  
  string hTitle;
  string comma(", ");
  float *binData = new float[ fFitResults->numberBins ];
  float *binDataError = new float[ fFitResults->numberBins ];
  float *xData = new float[ fFitResults->numberBins ];
  float *xDataError = new float[ fFitResults->numberBins ];
  float binSize = ( fFitResults->maxBin() - fFitResults->minBin() ) / ( float( fFitResults->numberBins - 1 ) );
  float minY=0, maxY=0;  

  //  GetYieldData( selectedList, binData, BinDataError );
  int nWaves =  selectedList->GetEntries();
  int *waveIndex = new int[nWaves];
  for( int n = 0; n < nWaves; n++){
    waveIndex[n] = fFitResults->fitResult[0]->getWaveIndex(((TGTextLBEntry*)(fSelected->At(n)))->GetTitle() );
    hTitle.append( ((TGTextLBEntry*)(fSelected->At(n)))->GetTitle() );
    hTitle.append( comma );
  }
  hTitle.resize( hTitle.size() - 2 ); //trim off last comma
  
  for( int i = 0; i < fFitResults->numberBins; i++ ){
    binData[i] = fFitResults->fitResult[i]->yield( nWaves, waveIndex );
    //binDataError[i] = 0;
    binDataError[i] = fFitResults->fitResult[i]->yieldError( nWaves, waveIndex ); //remove the errors from y axis.
    if( binData[i] > maxY ) maxY = binData[i];
    if( binData[i] < minY ) minY = 0;
    xData[i] = fFitResults->minBin() + (float(i) + 0.5 ) * binSize ;
    xDataError[i] = 0;
  }
  maxY = maxY * 1.1; // add 10% to range for plotting
  
  // draw a frame to define the range
  //cerr << "\nNumber of bins is " << fFitResults->numberBins << endl;
  //cerr << "\nminBin is " << fFitResults->minBin() << " maxBin is " << fFitResults->maxBin() << endl;
  TH1F *hr = fcc->DrawFrame( fFitResults->minBin() - binSize, minY, fFitResults->maxBin() + binSize, maxY);
  hr->SetXTitle("Mass    [GeV]");
  //hr->SetYTitle("Y title");

  // fill graph
  TGraphErrors *graph = new TGraphErrors( fFitResults->numberBins,xData,binData,xDataError,binDataError);
  graph->Print();
  graph->SetTitle( hTitle.c_str() );
  graph->SetMarkerStyle(8);
  //graph->GetYaxis()->SetLimits(0,maxY);

  delete [] binData;
  delete [] binDataError;
  delete [] xData;
  delete [] xDataError;


  graph->Draw("AP");
  graph->GetYaxis()->SetRangeUser(0,maxY); //added to enforce keeping the graph within range
  fcc->Update();

}




void MyMainFrame::PlotSelected(){
	// Writes selected entries in TList if multiselection.
  bool isPlotting = false;
  fSelected->Clear();
  
  if (fListBox->GetMultipleSelections()) {
    fListBox->GetSelectedEntries(fSelected);
    
    if( fPhaseButton->IsOn() )
      if( fSelected->GetEntries() != 2 ){
	Printf("\aSelect two and only two waves\n");
	static char mTitle[] = "User Error";
	static char mMsg[] = "PHASE DIFFERENCE ERROR:: Select two interfering waves.";
	SendMsg(mTitle, mMsg);
      } else{
	isPlotting = true;
	Printf("Phase Difference of:\n");
	fSelected->ls();
	PlotPhase( fSelected );
      }
    else if( fYieldButton->IsOn() ){ // Yield Button is on
      if( fSelected->GetEntries() > 0 ){
	isPlotting = true;
	Printf("Yield of: %d waves\n", fSelected->GetEntries() );
	cerr << "The TList is a: " <<  fSelected->GetEntries() << endl;
	cerr << "The TList is a: " << ((TGTextLBEntry*)(fSelected->At(0)))->GetTitle() << endl;
	fSelected->ls();
	PlotYield( fSelected );
      } else{
	static char mTitle[] = "User Error";
	static char mMsg[] = "YIELD ERROR:: YOU MUST SELECT AT LEAST ONE WAVE";
	SendMsg(mTitle, mMsg);
	Printf("Must select at least one wave...\n");
      }
    } else { // Likelihood button is on
      
      PlotLikely();

    }
    
  } else {
    Printf("Code error: ListBox should be Multi Selectable!!\n");
  }
  

}

/*
void MyMainFrame::Add(){
	bool isPlotting = false;
	fSelected->Clear();
*/






void MyMainFrame::PrintSelected(){
	// Writes selected entries in TList if multiselection.
  bool isPlotting = false;
  fSelected->Clear();
  
  if (fListBox->GetMultipleSelections()) {
    fListBox->GetSelectedEntries(fSelected);
    
    if( fPhaseButton->IsOn() )
      if( fSelected->GetEntries() != 2 )
	Printf("Select two and only two waves\n");
      else{
	isPlotting = true;
	Printf("Phase Difference of:\n");
	fSelected->ls();
      }
    else{ // Yield Button is on
      if( fSelected->GetEntries() > 0 ){
	isPlotting = true;
	Printf("Yield of: %d waves\n", fSelected->GetEntries() );
	cerr << "The TList is a: " <<  fSelected->GetEntries() << endl;
	cerr << "The TList is a: " << ((TGTextLBEntry*)(fSelected->At(0)))->GetTitle() << endl;
	fSelected->ls();
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
  fListBox->SetMultipleSelections( kFALSE );
  fListBox->SetMultipleSelections( kTRUE );
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


