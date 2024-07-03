#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <iostream>
#include "TLorentzVector.h"

TGraph* drawRoc(const char* fileName, const char* treeName, const char* isoName) {
    TFile *file = TFile::Open(fileName, "READ");
    TTree *tree = dynamic_cast<TTree*>(file->Get(treeName));
    TBranch *b_true = tree->GetBranch("isTrue");
    TBranch *b_dr03HcalIso = tree->GetBranch(isoName);

    bool isTrue;
    double dr03HcalIso;

    b_true->SetAddress(&isTrue);
    b_dr03HcalIso->SetAddress(&dr03HcalIso);

    int nPoint = 1000;
    // To change in fonction of the output
    float isoMax = 18;
    float isoMin = 0;
    float step = (isoMax-isoMin)/nPoint;
    double signalEff[nPoint];
    double backgroundEff[nPoint];
    for(int i = 0; i<nPoint; i++){
        signalEff[i]=0;
        backgroundEff[i]=0;
    }
    int nEntries = tree->GetEntries();

    float nTrue=0;
    float nNonTrue=0;
    for (int i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);
        
        int pos = (dr03HcalIso-isoMin)/step;
        if(dr03HcalIso>=isoMax) pos = nPoint-1;
        if(dr03HcalIso<=isoMin) pos = 0;
        if(isTrue){
            signalEff[pos]++;
            nTrue++;
        }
        if(!isTrue){
            backgroundEff[pos]++;
            nNonTrue++;
        }        
    }
    signalEff[0]*=1/nTrue;
    backgroundEff[0]*=1/nNonTrue;
    for(int i = 1; i<nPoint; i++){
        // cout<<"bg : "<<backgroundEff[i]<<" / n : "<<nNonTrue<<endl;
        // cout<<"bg : "<<backgroundEff[nPoint-1]<<" / n : "<<nNonTrue<<endl;
        signalEff[i]=(signalEff[i-1]*nTrue+signalEff[i])/nTrue;
        backgroundEff[i]=(backgroundEff[i-1]*nNonTrue+backgroundEff[i])/nNonTrue;
        //cout<<i<<" / "<<backgroundEff[i]<<endl;
    }

    TGraph *rocCurve = new TGraph(nPoint, backgroundEff, signalEff);
    rocCurve->SetTitle("ROC Curve");
    rocCurve->GetXaxis()->SetTitle("Background Efficiency");
    rocCurve->GetYaxis()->SetTitle("Signal Efficiency");
    rocCurve->SetMarkerStyle(20);
    rocCurve->SetMarkerSize(0.5);
    
    delete tree;
    delete file;
    return rocCurve;
}

int rocCurve() {
    TCanvas *c = new TCanvas("c", "ROC Curve", 800, 600);

    TGraph *dr03RocCurve = drawRoc("../python/test.root", "demo/outputTree;2", "dr03HcalNeutralIso");
    dr03RocCurve->SetLineColor(kBlue);
    dr03RocCurve->Draw("AL");

    TGraph *dr04RocCurve = drawRoc("../python/test.root", "demo/outputTree;2", "dr04HcalNeutralIso");
    dr04RocCurve->SetLineColor(kRed);
    dr04RocCurve->Draw("AL same");

    TLegend *legend = new TLegend(0.7, 0.1, 0.9, 0.3);
    legend->AddEntry(dr03RocCurve, "dr03", "l");
    legend->AddEntry(dr04RocCurve, "dr04", "l");
    legend->Draw("same");

    c->SetGrid();
    c->Draw();
    c->SaveAs("rocCurve.pdf");
    c->SaveAs("rocCurve.svg");
    return 0;
}
