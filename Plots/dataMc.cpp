#include <iostream>
#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TPad.h>

void plotTwoHistograms(const char* file1_name, const char* file2_name, const char* hist1_path, const char* hist2_path) {
    // Ouverture des fichiers ROOT
    TFile file1(file1_name);
    TFile file2(file2_name);

    // Extraction des histogrammes
    TH1D *hist1 = (TH1D*)file1.Get(hist1_path);
    TH1D *hist2 = (TH1D*)file2.Get(hist2_path);

    hist1->Rebin(hist1->GetNbinsX()/30);
    hist2->Rebin(hist2->GetNbinsX()/30);

    // hist1->GetXaxis()->SetRangeUser(70, 110);
    // hist2->GetXaxis()->SetRangeUser(70, 110);

    hist1->SetStats(0);
    hist2->SetStats(0);

    // Création d'un nouvel histogramme pour les ratios
    TH1D *ratioHist = new TH1D("MC/Data", "MC/Data", hist1->GetNbinsX(), hist1->GetXaxis()->GetXmin(), hist1->GetXaxis()->GetXmax());

    // Calcul des ratios pour chaque bin
    for (int i = 1; i <= hist1->GetNbinsX(); ++i) {
        double binContentHist1 = hist1->GetBinContent(i);
        double binContentHist2 = hist2->GetBinContent(i);
        if (binContentHist2 != 0) {
            ratioHist->SetBinContent(i, binContentHist1 / binContentHist2);
        }
    }

    // Création d'un nouvel histogramme pour les ratios avec les incertitudes
    TH1D *ratioHist2 = new TH1D("ratioHist2", "Ratio hist2/hist2", hist2->GetNbinsX(), hist2->GetXaxis()->GetXmin(), hist2->GetXaxis()->GetXmax());

    // Calcul des ratios et des incertitudes pour chaque bin
    for (int i = 1; i <= hist2->GetNbinsX(); ++i) {
        double binContentHist2 = hist2->GetBinContent(i);
        double binErrorHist2 = hist2->GetBinError(i);
        if (binContentHist2 != 0) {
            ratioHist2->SetBinContent(i, 1.0);
            ratioHist2->SetBinError(i, binErrorHist2 / binContentHist2);
        }
    }

    // Création d'un canvas pour le tracé
    TCanvas *canvas = new TCanvas("canvas", "Histogram Comparison", 800, 800);

    // Diviser le canvas en deux zones
    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1);
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0, 1, 0.3);
    pad1->SetBottomMargin(0); // Pas de marge inférieure pour pad1
    pad2->SetTopMargin(0); // Pas de marge supérieure pour pad2
    pad1->Draw();
    pad2->Draw();

    
    pad1->cd();
    hist1->Draw("HIST");
    hist1->SetLineColor(kBlue);

    hist2->SetLineColor(kBlack);
    hist2->SetMarkerStyle(kFullDotLarge);
    hist2->SetMarkerSize(1);
    hist2->Draw("P0 SAME");
    

    // Ajuster l'échelle de l'axe y pour s'adapter à l'histogramme le plus grand
    double max_value = std::max(hist1->GetMaximum(), hist2->GetMaximum());
    hist1->SetMaximum(max_value * 1.1);
    hist2->SetMaximum(max_value * 1.1);

    // Legend
    TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(hist1, "D-Y", "l");
    legend->AddEntry(hist2, "Data (2022C)", "P");
    legend->Draw("SAME");


    // Tracer le ratio hist2/hist1 sous forme d'un histogramme dans la zone inférieure
    pad2->cd();
    // ratioHist->GetXaxis()->SetRangeUser(70, 110);
    // ratioHist2->GetXaxis()->SetRangeUser(70, 110);
    ratioHist->SetStats(0);
    ratioHist2->SetStats(0);
    ratioHist->Draw();
    ratioHist->SetLineColor(kBlue);
    ratioHist->SetMaximum(2.0); // Définir une limite supérieure arbitraire pour le ratio
    TLine *line = new TLine(ratioHist->GetXaxis()->GetXmin(), 1.0, ratioHist->GetXaxis()->GetXmax(), 1.0);
    line->SetLineColor(kBlack);
    line->SetLineStyle(2); // Ligne en pointillés
    line->Draw("same");
    // Ajouter les incertitudes au ratio hist2/hist2
    ratioHist2->SetFillColor(kBlack);
    ratioHist2->SetLineColor(kBlack);
    ratioHist2->SetMarkerStyle(kFullDotLarge);
    ratioHist2->Draw("P same"); // E2 pour remplissage avec incertitudes

    // Enregistrement du canvas en PDF
    std::string ext=".pdf";
    canvas->SaveAs(((std::string)hist1->GetName()+".pdf").c_str());
    canvas->SaveAs(((std::string)hist1->GetName()+".svg").c_str());
}

int dataMc() {
    const char* file1_name = "../python/MC.root";
    const char* file2_name = "../python/Data.root";
    // const char* hist1_path = "demo/Mll";
    // const char* hist2_path = "demo/Mll";
    const char* hist1_path = "demo/dr03HcalIsoTreshold";
    const char* hist2_path = "demo/dr03HcalIsoTreshold";

    plotTwoHistograms(file1_name, file2_name, hist1_path, hist2_path);

    hist1_path = "demo/dr04HcalIsoTreshold";
    hist2_path = "demo/dr04HcalIsoTreshold";

    plotTwoHistograms(file1_name, file2_name, hist1_path, hist2_path);

    hist1_path = "demo/Mll";
    hist2_path = "demo/Mll";

    plotTwoHistograms(file1_name, file2_name, hist1_path, hist2_path);

    return 0;
}
