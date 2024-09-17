#include <TFile.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TString.h>
#include <vector>

TString year="2024";
TString rootFileName = "/eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/DYto2L_M-50_TuneCP5_13p6TeV_pythia8/EGamma_MC_22G/240915_094228/mc2024_run2022GminiAod_reducedEgamma.root";

void plotsGen(TString histLoc, TString histAod) {
    // Ouvrir le fichier ROOT
    TFile* file = new TFile("../python/mc2024miniAod.root", "READ");

    // Accéder au dossier "demo"
    // TDirectory* demoFolder = file->GetDirectory("demo");
    // TList* keys = demoFolder->GetListOfKeys();

    // //Parcourir la liste des clés et afficher les noms des objets
    // TIter next(keys);
    // TKey* key;
    // while ((key = dynamic_cast<TKey*>(next()))) {
    //     TObject* obj = key->ReadObj();
    //     std::cout << "Nom de l'objet : " << obj->GetName() << "Et la classe :"<<obj->ClassName()<< std::endl;
    // }

    // Lire les histogrammes à l'intérieur du dossier
    TH1D* histo1 = dynamic_cast<TH1D*>(file->Get(histLoc));
    histo1->SetLineColor(kBlue);
    TH1D* histo2 = dynamic_cast<TH1D*>(file->Get(histAod));
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
    canvas->SaveAs(histLoc+".pdf");

    // Attendre que l'utilisateur ferme la fenêtre graphique
    canvas->WaitPrimitive();
}

void plotsGen1(TString histLoc) {
    // Ouvrir le fichier ROOT
    TFile* file = new TFile("../python/mc2024miniAod.root", "READ");

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

    // Ajouter une légende
    TLegend* legend = new TLegend(0.6, 0.7, 0.8, 0.8);
    legend->AddEntry(histo1, "RecHit", "l");
    legend->SetBorderSize(0);
    legend->SetTextSize(0.04);
    legend->SetLineWidth(5);
    legend->Draw();

    // Afficher la toile
    canvas->Update();
    canvas->SaveAs(histLoc+".png");
    canvas->SaveAs(histLoc+".pdf");

    // Attendre que l'utilisateur ferme la fenêtre graphique
    canvas->WaitPrimitive();
}

void plotsGenList(std::vector <TString> list, std::vector <TString> legendname, TString title) {
    // Ouvrir le fichier ROOT
    EColor color[5] = {kBlue, kRed, kGreen, kBlack, kCyan};
    TFile* file = new TFile(rootFileName, "READ");

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

    std::vector <TH1D*> histo;

    for(int i=0; i<list.size(); i++){
        // Lire les histogrammes à l'intérieur du dossier
        histo.push_back(dynamic_cast<TH1D*>(demoFolder->Get(list[i])));
        histo[i]->SetLineColor(color[i]);

        std::cout<<"Histo "<<list[i]<<"integral is : "<<histo[i]->Integral()<<std::endl;
    }

    double maxVal = 0;
    for (int i = 0; i < histo.size(); i++) {
        double histMax = histo[i]->GetMaximum();
        if (histMax > maxVal) {
            maxVal = histMax;
        }
    }

    // Créer une nouvelle toile
    TCanvas* canvas = new TCanvas("canvas", "Neutral Isolation (No cut)", 800, 600);

    // Activer l'échelle logarithmique sur l'axe des y
    canvas->SetLogy();

    // Dessiner le premier histogramme
    TH1D* histo1 = histo[0];
    histo1->Draw("HIST");
    histo1->SetStats(0);
    histo1->SetTitle(title);
    histo1->GetXaxis()->SetTitle("Isolation");
    histo1->GetYaxis()->SetTitle("Events (log)");
    histo1->SetMaximum(maxVal * 1.2);
    TLegend* legend = new TLegend(0.4, 0.8, 0.8, 0.9);
    legend->SetFillStyle(0);   // Transparence du fond
    legend->SetTextFont(42);   // Police claire et lisible
    legend->AddEntry(histo1, legendname[0], "l");

    // Superposer le deuxième histogramme
    for (int i=1; i<histo.size(); i++){
        histo[i]->Draw("HIST SAME");
        histo[i]->SetStats(0);
        legend->AddEntry(histo[i], legendname[i], "l");
    }

    // Ajouter une légende
    legend->SetBorderSize(0);
    legend->SetTextSize(0.04);
    legend->SetLineWidth(5);
    legend->Draw();

    // Afficher la toile
    canvas->Update();
    canvas->SaveAs(year+"_"+title+".png");
    canvas->SaveAs(year+"_"+title+".pdf");
    canvas->SaveAs(year+"_"+title+".svg");

    // Attendre que l'utilisateur ferme la fenêtre graphique
    canvas->WaitPrimitive();
}


void plots(){
    std::cout<<"Starting"<<std::endl;
    // plotsGen("PfNeutralIso", "PfNeutralAodIso");
    // plotsGen("HcalNeutralIso", "HcalNeutralAodIso");
    // plotsGen1("NofRecHit");
    std::cout<<"Starting the list"<<std::endl;
    std::vector<TString> name = {"HcalTowerIsodr03", "HcalTowerIsodr028", "HcalNeutralAodIso"};
    std::vector<TString> legend = {"Hcal Tower Isolation dR03", "Hcal Tower Isolation dR028", "Stored Hcal Tower Isolation"};
    TString title = "reducedEgamma";
    plotsGenList(name, legend, title);
}
