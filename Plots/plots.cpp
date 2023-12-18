#include <TFile.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TString.h>

void plotsGen(TString histLoc, TString histAod) {
    // Ouvrir le fichier ROOT
    TFile* file = new TFile("../python/histodemo.root", "READ");

    // Accéder au dossier "demo"
    TDirectory* demoFolder = file->GetDirectory("demo");
    TList* keys = demoFolder->GetListOfKeys();

    //Parcourir la liste des clés et afficher les noms des objets
    TIter next(keys);
    TKey* key;
    while ((key = dynamic_cast<TKey*>(next()))) {
        TObject* obj = key->ReadObj();
        std::cout << "Nom de l'objet : " << obj->GetName() << "Et la classe :"<<obj->ClassName()<< std::endl;
    }

    // Lire les histogrammes à l'intérieur du dossier
    TH1D* histo1 = dynamic_cast<TH1D*>(demoFolder->Get(histLoc));
    histo1->SetLineColor(kBlue);
    TH1D* histo2 = dynamic_cast<TH1D*>(demoFolder->Get(histAod));
    histo1->SetLineColor(kRed);
    std::cout<<histLoc<<" / "<<histAod<<std::endl;

    // Créer une nouvelle toile
    TCanvas* canvas = new TCanvas("canvas", "Neutral Isolation (No cut)", 800, 600);

    // Activer l'échelle logarithmique sur l'axe des y
    canvas->SetLogy();

    // Dessiner le premier histogramme
    histo1->Draw();
    histo1->SetStats(0);
    histo1->SetTitle("Neutral Isolation");
    histo1->GetXaxis()->SetTitle("Isolation");
    histo1->GetYaxis()->SetTitle("Events (log)");

    // Superposer le deuxième histogramme
    histo2->Draw("SAME");
    histo2->SetStats(0);

    // Ajouter une légende
    TLegend* legend = new TLegend(0.6, 0.7, 0.8, 0.8);
    legend->AddEntry(histo1, "Local computation", "l");
    legend->AddEntry(histo2, "MiniAod value", "l");
    legend->SetBorderSize(0);
    legend->SetTextSize(0.04);
    legend->SetLineWidth(5);
    legend->Draw();

    // Afficher la toile
    canvas->Update();
    canvas->SaveAs(histLoc+".png");

    // Attendre que l'utilisateur ferme la fenêtre graphique
    canvas->WaitPrimitive();
}

void plots(){
    std::cout<<"Starting"<<std::endl;
    plotsGen("PfNeutralIso", "PfNeutralAodIso");
    plotsGen("HcalNeutralIso", "HcalNeutralAodIso");
}
