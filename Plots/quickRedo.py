import ROOT

# Chargement du fichier ROOT
fichier = ROOT.TFile("test.root")

# Récupération de l'arbre (tree) à partir du fichier
arbre = fichier.Get("demo/outputTree")

# Création d'un histogramme à partir de l'arbre
histogramme = ROOT.TH1F("NeutralHcalIso", "NeutralHcalIso", 180, 0, 15)

# Boucle sur les événements de l'arbre
fourvec = ROOT.TLorentzVector(0, 0, 0, 0)
for evenement in arbre:
    el = evenement.ele
    fourvec += el
    if fourvec.M() < 50:
        continue

    histogramme.Fill(valeur)

# Dessiner l'histogramme
histogramme.Draw()

# Afficher le canvas
ROOT.gPad.Print("nom_de_l_histogramme.png")

# Fermer le fichier ROOT
fichier.Close()
