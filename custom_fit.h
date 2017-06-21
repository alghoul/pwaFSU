#include "Riostream.h"
#include "TROOT.h"
#include "TVirtualPad.h"
#include "TFrame.h"
#include "TStyle.h"
#include "TPaveText.h"
#include "TH1.h"
#include "TPaveStats.h"

Double_t bg(Double_t *x, Double_t *par) {
   return par[0] + par[1]*x[0] + par[2]*x[0]*x[0];
}

Double_t bw(Double_t *x, Double_t *par) {
  return (0.5*par[0]*par[1]/TMath::Pi()) / 
    TMath::Max( 1.e-10,(x[0]-par[2])*(x[0]-par[2]) 
   + .25*par[1]*par[1]);
}

Double_t mybw(Double_t* x, Double_t* par)
{
  Double_t arg1 = 14.0/22.0; // 2 over pi
  Double_t arg2 = par[1]*par[1]*par[2]*par[2]; //Gamma=par[1]  M=par[2]
  Double_t arg3 = ((x[0]*x[0]) - (par[2]*par[2]))*((x[0]*x[0]) - (par[2]*par[2]));
  Double_t arg4 = x[0]*x[0]*x[0]*x[0]*((par[1]*par[1])/(par[2]*par[2]));
  return par[0]*arg1*arg2/(arg3 + arg4);
}

Double_t BW(Double_t* x, Double_t* par)
{

        Double_t arg1 = 2*TMath::Power(2,0.5)/TMath::Pi();
        Double_t arg2 = TMath::Power(par[1]*par[1]*((par[1]*par[1])+(par[2]*par[2])),0.5);
        Double_t arg3 = (arg1*par[1]*par[2]*arg2)/TMath::Power((par[1]*par[1])+arg2,0.5);
        Double_t arg4 = arg3/(TMath::Power((x[0]*x[0])-(par[1]*par[1]),2)+(par[1]*par[1]*par[2]*par[2]));
        return par[0]*arg4;
}


Double_t fit(Double_t *x, Double_t *par) {
  return BW(x,par) + bg(x,&par[3]);
}




void custom(TGraphErrors *h1 , TCanvas *c , int j)
{

TCanvas *cc = (TCanvas*)c;
TGraphErrors *hh = (TGraphErrors*)h1;
cc->SetFillColor(kWhite);
cc->SetHighLightColor(0);
gPad->SetFrameFillColor(0);
gPad->SetFillColor(kWhite);
gPad->SetGridx();
gPad->SetGridy();
gPad->SetBorderSize(0);
gPad->SetBorderMode(0);
gPad->SetLeftMargin(0.1529659);
gPad->SetRightMargin(0.04800921);
gPad->SetTopMargin(0.07556298);
gPad->SetBottomMargin(0.1261436);
gPad->SetFrameFillColor(0);
gStyle->SetTitleX(0.2639085);
gStyle->SetTitleY(0.9854328);
gStyle->SetTitleBorderSize(0);
gStyle->SetTitleFillColor(0);
gStyle->SetTitleFont(102);
//gStyle->SetFrameBorderMode(0);
//gStyle->SetFrameLineColor(0);
//gStyle->SetFrameLineWidth(0);
//gStyle->SetFrameLineStyle(0);
//gStyle->SetOptStat("KMe");
gStyle->SetOptFit(1111);


//cc->SetFrameBorderMode(0);
cc->Draw();




hh->SetFillColor(0);
hh->SetMarkerStyle(21);
hh->SetMarkerColor(kBlack);
hh->SetLineColor(kBlack);
hh->SetMarkerColor(4);
//hh->SetLineColor(4);
hh->SetLineWidth(2);
hh->SetMarkerSize(0.8);
hh->GetXaxis()->SetTitle("M(K^{+}#pi^{+}#pi^{-}) [GeV/c^{2}]");
hh->GetXaxis()->CenterTitle(true);
hh->GetXaxis()->SetLabelFont(102);
hh->GetXaxis()->SetLabelSize(0.05);
hh->GetXaxis()->SetTitleSize(0.06);
hh->GetXaxis()->SetTickLength(0.04);
hh->GetXaxis()->SetTitleOffset(0.87);
hh->GetXaxis()->SetTitleFont(102);
//hh->GetYaxis()->SetTitle("Events/100 [MeV/c^{2}]");
hh->GetYaxis()->CenterTitle(true);
hh->GetYaxis()->SetLabelFont(102);
hh->GetYaxis()->SetLabelSize(0.05);
hh->GetYaxis()->SetTitleSize(0.06);
hh->GetYaxis()->SetTitleOffset(1.02);
hh->GetYaxis()->SetTitleFont(102);

TPaveStats *st = (TPaveStats*)hh->FindObject("stats");

st->SetShadowColor(0);
st->SetX1NDC(0.6179118);
st->SetX2NDC(0.9777113);
st->SetY1NDC(0.683787);
st->SetY2NDC(0.9236326);
st->SetFillColor(kWhite);

gPad->Modified();
gPad->Update();
TFrame *fr = (TFrame*)gPad->GetFrame();
fr->SetBorderMode(kFALSE);
gPad->Modified();
//gPad->Update();
}


/*
Double_t bw(Double_t *x, Double_t *par) {

 Double_t bw = par[0]*par[2]/((par[1])*(par[1]) + par[2]*par[2]/4);
 return bw/(2*TMath::Pi());
}
*/

