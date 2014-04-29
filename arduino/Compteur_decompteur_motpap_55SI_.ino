/*
  LiquidCrystal Library - 
  
  Le circuit:
 * FORK A to digital pin 2
 * FORK B to digital pin 1
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 10
 * LCD D5 pin to digital pin 9
 * LCD D6 pin to digital pin 8
 * LCD D7 pin to digital pin 7
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 
 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 
 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */
/*******************************************************

 Gestion de menu – v1
 Par Yersinia Programs

 http://yersiniaprograms.wordpress.com/
 

********************************************************/

/*******************************************************
Bobineuse V3

Ce programme gère l'affichage de la bobineuse et la gestion moteur
 Affiche sur l'écran LCD le nombre de spires et le nombre de spires par couche
 Gère l'avance du moteur pàp en fonction du diamètre du fil
 stocke le nombre de spire et le nombre de spire par couche 
 permet de rappeler les données enregistrées dans 10 zones de stockage de l'EEPROM
 
 CC by SA 
 Totof 2014
 
 Pour toute info complémentaire un lien ici
 http://www.audiyofan.org/forum/viewtopic.php?f=73&t=7176&start=105
 
 www.audiyofan.org
 
 ********************************************************/

// include the library code:
#include <LiquidCrystal.h>
#include <Stepper.h>
#include <EEPROM.h>

// Définition des PIN
#define Fourche_A  2     
#define Fourche_B  1 

// Définition des variables
// Mémorise l'état des sorties A et B
int StateA = 0; 
int StateB = 0;
// Mémorise l'état des sorties A et B antérieures
int StateA_anterieur = 0; 
int StateB_anterieur = 0; 

int Compteur = 0;
int Compteur_anterieur = 0;
int Compteur_impulsion = 0;
int Compteurspcouche = 0;
int Sensspirecouche = 0;
int Sensdebutcouche = 1;
int Senschangecote = 1;
int Avancechangecote = 0;
int Position = 0;

int lcd_keyA1     = 0;
boolean choixmenu = 0;

float Nbspire = 0;
float Pasvis = 0.80;// Défini le pas de la tige filetée actuellement 5mm de diamètre
int Sense = 0;// Défini le sens de rotation de l'arbre principal
int Steps = 0;// Déini le nombre de pas pour le mot pàp de l'avance quide fil

int val = 0;
int stepsMaped = 0;// Transforme la lecture analogique du potentiomètre 
// Mémorise le diamètre du fil
float fFil = 0;

// Modification des settings Arduino d'origine pour améliorer la conversion
// analogique vers numérique et ainsi augmenter la vitesse de traitement
// avec ces réglages une conversion prend 24 µs
// Define various ADC prescaler
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
int cumul_spire = 0;
int cumul =0;
float reste_compteur = 0;
int retour_compteur = 0;
int cumul_couche = 0;

int A = 0;
int B = 0;
int C = 0;
int D = 0;
int E = 0;
int F = 0;
int G = 0;
int H = 0;
int I = 0;
int J = 0;
int K = 0;
int L = 0;
int M = 0;
int X = 0;
int Y = 0;
int Z = 0;

boolean choix = 0;
int up;
int ancien_boutton;
int ancien_boutton_menu;
int droit;
int select;
int valeur;
int numerosauvegarde;


// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

// initialize the stepper library on pins 8 through 11:
// 48 is the number of steps per revolution,change this to fit the number of steps per revolution
// Réglage pour le moteur 55SI-25DAWC
// Brochage des fils en sortie de l'ULN2003 
// Pin 16 Rouge 55SI-25DAWC, Rose   ST35,
// Pin 15 Brun  55SI-25DAWC, Orange ST35,
// Pin 14 Jaune 55SI-25DAWC, Jaune  ST35,
// Pin 13 Bleu  55SI-25DAWC, Bleu   ST35,
Stepper myStepper(48,6,5,4,3);

// initialisation du menu
char menu_text [7][8][17] = {
{
{"Nb de spires  =>"}, {"Millier       =>"}, {"Centaine      =>"}, {"Dizaine       =>"}, {"Unite         =>"}, {"Dizieme         "},             {"null"}, {"null"}
},   
{
{"Cumul compt   =>"}, {"Comptage      =>"}, {"Decomptage      "},             {"null"},             {"null"},             {"null"},             {"null"}, {"null"}
},
{
{"Spire/couche  =>"}, {"Centaine      =>"}, {"Dizaine       =>"}, {"Unite         =>"}, {"Debut Gauche  =>"}, {"Debut Droite    "},             {"null"}, {"null"}
},
{
{"Guide fil     =>"}, {"- 1/2 tour    =>"}, {"+ 1/2 tour    =>"}, {"- 1/8 tour    =>"}, {"+ 1/8 tour    =>"}, {"Change de cote  "},             {"null"}, {"null"}
},
{
{"Position init =>"}, {"Enregistrer   =>"}, {"Retour a 0    =>"}, {"- 10 tours    =>"}, {"+10 tours     =>"}, {"- 1 tour      =>"}, {"+ 1 tour      =>"}, {"null"}
},
{
{"Compteur      =>"}, {"+ 1/10        =>"}, {"- 1/10          "},             {"null"},             {"null"},             {"null"},             {"null"}, {"null"}
},      
{
{"Sauvegarde    =>"}, {"Choix numero  =>"}, {"Sauver        =>"}, {"Recharger       "},             {"null"},             {"null"},             {"null"}, {"null"}
}   
};

// Il faut mettre les enregistrements des valeurs dans un tableau à la place des 0
// Faire une fonction de parcours de tableau et de chargement des valeurs
// Puis faire un enregistrement directement dans le tableau qui donnera
// l’adresse de la case mémoire utilisé comme moyen de réutilisation
int menu_valeur [7][8][3] = {
{
{-21},  {0}, {0}, {0},  {0},  {0},  {-1},{-1}
},  
{
{-1}, {-2}, {-3}, {-1}, {-1}, {-1}, {-1},{-1}
},
{
{-22}, {0},  {0},  {0}, {-4}, {-5}, {-1},{-1}
},
{
{-1}, {-6}, {-7}, {-8}, {-9}, {-10},{-1},{-1}
},
{
{-1},{-11},{-12},{-13},{-14},{-15},{-16},{-1}
},
{
{-1}, {-17}, {-18}, {-1},{-1},{-1}, {-1},{-1}
},
{
{-1}, {0},{-19}, {-20}, {-1}, {-1}, {-1},{-1}
}
};
  
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int menu_config [98];

void setup() {
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  // lcd.print("Compteur");
  // Initialise les entrées A et B de l'encodeur
  pinMode(Fourche_A, INPUT); 
  pinMode(Fourche_B, INPUT);
  // set up the ADC
  ADCSRA &= ~PS_128;  // remove bits set by Arduino library
  // you can choose a prescaler from above.
  // PS_16, PS_32, PS_64 or PS_128
  ADCSRA |= PS_16;    
  // set our own prescaler to 16 
  // set the speed at 300 rpm:
  myStepper.setSpeed(300);
    
}

void loop() {
  
  Selection();
  
  if (choixmenu == 0)
    {
           
    Comptage();
    
    Compteimpulsion();
    
    AfficheCompteur();
    
    Retournerestecompteur ();
    
    Diamfil();
    
    AfficheDiamFil();
    
    Nombrespirecouche();
    
    Gestionstep();
        
    Avanceguidefil();
    
    Positioncourante();
           
    }
    
  if (choixmenu == 1)
    {
    
    Gestionboutons ();
    
    Gestionvaleur ();
    
    Affichagemenutext();
    
    delay(20);
    
    }
   
   
}

/*******************************************************
*******************************************************/

// Fonction de comptage
void Comptage()
{
    // Lecture des sorties A et B de l'encodeur. Avant chaque lecture,
  // mémorisation de l'état antérieur de chaque broche. Ce principe permet
  // la détection d'un front montant.
  StateA_anterieur= StateA;
  StateA = digitalRead(Fourche_A);
  StateB_anterieur= StateB;  
  StateB = digitalRead(Fourche_B);

  // Détection du front montant :
  // Etat actuel : haut, état antérieur : bas
  if ((StateA == HIGH) && (StateA_anterieur == LOW)) { 
    if (StateB == LOW) {
      Sense = 1;      
      }
    
    };  

  // Détection du front montant :
  // Etat actuel : haut, état antérieur : bas
  if ((StateB == HIGH) && (StateB_anterieur == LOW)) { 
    if (StateA == LOW) {
      Sense = -1;      
      };    
    }
}

/*******************************************************
*******************************************************/

// Fonction comptage des impulsions
void Compteimpulsion()
{
  A = abs(Compteur_impulsion);
  
  if (A<10)
  {
   Compteur_impulsion = Compteur_impulsion + Sense;
  }
  else 
  {
   Compteur_impulsion = 0;
  } 

}

/*******************************************************
*******************************************************/

// Fonction d'affichage du compteur
void AfficheCompteur()
{
  if (menu_config [0] == 0)
  {
  Compteur = Compteur + Sense;
  Nbspire=Compteur*0.1;
  // set the cursor to column 12, line 0
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0,0);
  lcd.print("Compteur   ");
  lcd.setCursor(9,0);
  // reset the screen:
  lcd.print("       ");
  // set the cursor to column 12, line 0
  Affichenombre(); 
  }
  else if (menu_config [0] == 1)
  {
  cumul_spire = cumul_spire - Sense;
  Nbspire=cumul_spire*0.1;
  // set the cursor to column 12, line 0
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0,0);
  lcd.print("Reste     ");
  lcd.setCursor(9,0);
  // reset the screen:
  lcd.print("       ");
  // set the cursor to column 12, line 0
  Affichenombre(); 
  }
}

/*******************************************************
*******************************************************/

// Fonction de paramétrage de l'affichage du nombre de spires
void Affichenombre()
{
  if ((Nbspire <= -10) && (Nbspire > -100))
  {
   lcd.setCursor(11,0);
   lcd.print(Nbspire,1); 
  }
  if ((Nbspire < 0) && (Nbspire > -10))
  {
   lcd.setCursor(12,0);
   lcd.print(Nbspire,1); 
  }
  if (Nbspire >= 0)
  {
   lcd.setCursor(13,0);
   lcd.print(Nbspire,1); 
  }
  if (Nbspire >= 10)
  {
   lcd.setCursor(12,0);
   lcd.print(Nbspire,1); 
  }
  if (Nbspire >= 100)
  {
   lcd.setCursor(11,0);
   lcd.print(Nbspire,1);
  }
  if (Nbspire >= 1000)
  {
   lcd.setCursor(10,0);
   lcd.print(Nbspire,1);
   delay(5); 
  }
}

/*******************************************************
*******************************************************/

// Fonction de paramétrage du diamètre du fil
void Diamfil()
{
  val=analogRead(A0);
  stepsMaped=map(val, 0, 1023, 0, 1008);
      
  // Doit etre calculé par rapport à la tige filetée 
  // 1 tour de tige filetée en pas de 1 revient à une avance de 1 mm
  // Soit 1/48 (par rapport au moteur)steps fois le nombre de steps d'avance
  // pour faire 1 tour complet du motoréducteur 
  // ici le codeur comporte 10 pulses par tour donc 0.02803333 * 0.1
  fFil = stepsMaped*Pasvis*0.002083333;
}

/*******************************************************
*******************************************************/

// Fonction d'affichage du diamètre du fil
void AfficheDiamFil()
{
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0,1);
  // print the number of seconds since reset:
  lcd.print("Diam fil");
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(11,1);
  lcd.print(fFil,3); 
}

/*******************************************************
*******************************************************/

// Fonction D'avance du guide fil 
void Avanceguidefil()
{
  // revolution  in one direction or the other:
  Steps = (Sense*Sensspirecouche*Sensdebutcouche*B);
  myStepper.step(Steps);
  Sense = 0;
  B = 0;
  C = 0;
  D = 0;
}

/*******************************************************
*******************************************************/

// Fonction de gestion de la découpe du nombre de steps pour 
// remplir une révolution de la broche principale au step près
void Gestionstep()
{
 if ( A <= 7 )
    {
      B = int(stepsMaped*0.01);
    }
    else if ( A == 8 )
    {
      C = int((stepsMaped*0.1)-(int(stepsMaped*0.01)*8));
      B = int(C*0.5);
    }
    else if ( A == 9 )
    {
      C = int(int((stepsMaped*0.1)-(int(stepsMaped*0.01)*8))*0.5);
      D = (int(stepsMaped*0.01))*8;
      B = ((int(stepsMaped*0.1))-D)-C;            
    } 
}

/*******************************************************
*******************************************************/

// Fonction qui enregistre la position du guide fil 
void Positioncourante()
{
  Position = Position + Steps;
}

/*******************************************************
*******************************************************/
// Fonction pour entrer ou sortir du menu
int Selection()
{
  lcd_keyA1 = digitalRead(A1);
  if (ancien_boutton_menu != lcd_keyA1) // permet d’in/décrémenter que de 1 lors de l’appuie d’un bouton
  {
  if (lcd_keyA1 == HIGH)
  {
    choixmenu = !choixmenu;
  }
  ancien_boutton_menu = lcd_keyA1;
  }
}

/*******************************************************
*******************************************************/

// Partie qui gère l'affichage du menu


// Fonction qui gère la lecture des boutons et qui retourne 
// le bouton appuyés
int Lecture_LCD_bouttons()
{
adc_key_in = analogRead(A2);      // read the value from the sensor
// my buttons when read are centered at these valies: 0, 144, 329, 504, 741
// we add approx 50 to those values and check to see if we are close
if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
//lcd.setCursor(0,0);
//lcd.print(adc_key_in);
// For V1.0 us this threshold
if (adc_key_in < 50)   return btnRIGHT;
if (adc_key_in > 150 && adc_key_in < 250)  return btnUP;
if (adc_key_in > 375 && adc_key_in < 475)  return btnDOWN;
if (adc_key_in > 565 && adc_key_in < 665)  return btnLEFT;
if (adc_key_in > 795 && adc_key_in < 895)  return btnSELECT;

}

/*******************************************************
*******************************************************/

// Fonction gestion des retours boutons
void Gestionboutons ()
{
 lcd_key = Lecture_LCD_bouttons();
 if (ancien_boutton != lcd_key) // permet d’in/décrémenter que de 1 lors de l’appuie d’un bouton
 {
 switch (lcd_key)
 {
 case btnRIGHT:
 {
 if (choix == 0)
 {
 if (droit < 7)
 {
 droit++;
 }
 else 
 {
 droit=0; 
 }
 }
 
 if (choix == 1)
 {
 if (valeur <= 8)
 {
   valeur++;
 }
 else
 {
   valeur = 0;
 }  
 }
 }
 
 break;
 

 case btnLEFT:
 {
 if (choix == 0)
 {
 if (droit > 0) 
 {
 droit--;
 }
 else
 { }
 }
 if (choix == 1)
 {
 if (valeur >= 1)
 {
   valeur--;
 }
 else
 {
   valeur = 9;
 }  
 }
 }
 
 break;

 case btnUP:
 {
 if (up < 6)
 {
 up++;
 }
 else 
 {
 up=0; 
 }
 }
 
 break;

 case btnDOWN:
 {
 if (up > 0) 
 {
 up--;
 }
 else
 { }
 }
 
 break;

 case btnSELECT:
 {
 delay(100); // delay to debounce switch
 choix = !choix;
 }
 break;

 case btnNONE:
 { }
 
 break;
 
 }
 ancien_boutton = lcd_key;
 }
 if (strcmp(menu_text[up][droit], "null") == 0) // le test pour savoir si il y a un sous menu
 {
 droit = 0;
 } 
 else { }
}

/*******************************************************
*******************************************************/

// Fonction qui gère l'affichage du menu 

void Affichagemenutext()
{
 lcd.setCursor(0,0); lcd.print("               ");
 lcd.setCursor(0,0); lcd.print(menu_text[up][droit]);
 delay(10); 
}


/*******************************************************
*******************************************************/

// Fonction qui gère l'effacement de la seconde ligne

void Effaceligne2 ()
{
 lcd.setCursor(0,1);
 lcd.print("                ");
}

/*******************************************************
*******************************************************/

// Fonction qui gère le menu valeur

void Gestionvaleur ()
{
 if (*menu_valeur[up][droit] >= 0)// Savoir si il y a une valeur à modifier
  {
    if (choix== 1)
    {
    (*menu_valeur[up][droit]) = valeur;
    lcd.setCursor(0,1); lcd.print("<=            =>");
    lcd.setCursor(7,1);
    lcd.print(valeur);
    }
    else
    {
    Effaceligne2 ();
    valeur = (*menu_valeur[up][droit]);
    }
  }
 if (*menu_valeur[up][droit] == -1)
  {
   Effaceligne2 ();
  }
 if (*menu_valeur[up][droit] == -2)// Savoir si on décompte
  {
    if (choix == 1)
     {
     menu_config [0] = 0;
     choix = !choix;
     lcd.setCursor(0,1);
     lcd.print("    Comptera    ");
     delay(1000);
     Effaceligne2 ();
     }    
  }
 if (*menu_valeur[up][droit] == -3)// Savoir si on compte
  {
    if (choix == 1)
     {
     menu_config [0] = 1;
     choix = !choix;
     lcd.setCursor(0,1);
     lcd.print("   Decomptera   ");
     delay(1000);
     Effaceligne2 ();
     }    
  }
 if (*menu_valeur[up][droit] == -4)// Savoir si on commence à gauche
  {
    if (choix == 1)
     {
     Sensdebutcouche = -1;
     choix = !choix;
     lcd.setCursor(0,1);
     lcd.print("Debute a gauche ");
     delay(1000);
     Effaceligne2 ();
     }    
  }
 if (*menu_valeur[up][droit] == -5)// Savoir si on commence à droite
  {
    if (choix == 1)
     {
     Sensdebutcouche = 1;
     choix = !choix;
     lcd.setCursor(0,1);
     lcd.print("Debute a droite ");
     delay(1000);
     Effaceligne2 ();
     }    
  }
 if (*menu_valeur[up][droit] == -6)// Avance de 24 steps
  {
    if (choix == 1)
     {
     lcd.setCursor(0,1);
     lcd.print("    -1/2 tour   ");
     myStepper.step(-24*Sensdebutcouche);
     choix = !choix;
     delay(500);
     Effaceligne2 ();
     }    
  }
 if (*menu_valeur[up][droit] == -7)// Recule de 24 steps
  {
    if (choix == 1)
     {
     lcd.setCursor(0,1);
     lcd.print("   +1/2 tour    ");
     myStepper.step(24*Sensdebutcouche);
     choix = !choix;
     delay(500);
     Effaceligne2 ();
     }    
  }
 if (*menu_valeur[up][droit] == -8)// Avance de 6 steps
  {
    if (choix == 1)
     {
     lcd.setCursor(0,1);
     lcd.print("    -1/8 tour   ");
     myStepper.step(-6*Sensdebutcouche);
     choix = !choix;
     delay(500);
     Effaceligne2 ();
     }    
  }
 if (*menu_valeur[up][droit] == -9)// Recule de 6 steps
  {
    if (choix == 1)
     {
     lcd.setCursor(0,1);
     lcd.print("    +1/8 tour   ");
     myStepper.step(6*Sensdebutcouche);
     choix = !choix;
     delay(500);
     Effaceligne2 ();
     }    
  }
 if (*menu_valeur[up][droit] == -10)// Change de côté
  {
    if (choix == 1)
     {
     Diamfil();
     Avancechangecote = int(Sensdebutcouche*Senschangecote*cumul_couche*stepsMaped*0.1);
     myStepper.step(Avancechangecote);
     choix = !choix;
     Senschangecote = -Senschangecote;
     lcd.setCursor(0,1);
     lcd.print(" Change de cote ");
     delay(500);
     Effaceligne2 ();
     }    
  }
 if (*menu_valeur[up][droit] == -11)// Enregistre la position initiale
  {
    if (choix == 1)
     {
     Position = 0;
     choix = !choix;
     lcd.setCursor(0,1);
     lcd.print("  Enregistree  ");
     delay(500);
     Effaceligne2 ();
     }    
  }
 if (*menu_valeur[up][droit] == -12)// Retourne à la position initiale
  {
    if (choix == 1)
     {
     choix = !choix;
     lcd.setCursor(0,1);
     lcd.print("  Retour à 0  ");
     myStepper.step(-Position);
     Compteur = 0;
     Effaceligne2 ();
     }    
  }
 if (*menu_valeur[up][droit] == -13)// Recule de 10 tours
  {
    if (choix == 1)
     {
     lcd.setCursor(0,1);
     lcd.print("    -10 tours   ");
     myStepper.step(-480*Sensdebutcouche);
     choix = !choix;
     Effaceligne2 ();
     }    
  }
 if (*menu_valeur[up][droit] == -14)// Avance de 10 tours
  {
    if (choix == 1)
     {
     lcd.setCursor(0,1);
     lcd.print("    +10 tours   ");
     myStepper.step(480*Sensdebutcouche);
     choix = !choix;
     Effaceligne2 ();
     }    
  }
 if (*menu_valeur[up][droit] == -15)// Recule de 1 tour
  {
    if (choix == 1)
     {
     lcd.setCursor(0,1);
     lcd.print("     -1 tour    ");
     myStepper.step(-48*Sensdebutcouche);
     choix = !choix;
     delay(500);
     Effaceligne2 ();
     }    
  }
 if (*menu_valeur[up][droit] == -16)// Avance de 1 tour
  {
    if (choix == 1)
     {
     lcd.setCursor(0,1);
     lcd.print("     +1 tour    ");
     myStepper.step(48*Sensdebutcouche);
     choix = !choix;
     delay(500);
     Effaceligne2 ();
     }    
  }
 if (*menu_valeur[up][droit] == -17)// Avance le compteur de 0.1 tour
  {
    if (choix == 1)
     {
     Compteur = Compteur + 1;
     choix = !choix;
     choixmenu = !choixmenu;
     lcd.setCursor(0,1);
     lcd.print("    +0.1 spire  ");
     delay(500);
     Effaceligne2 ();
     }    
  }
 if (*menu_valeur[up][droit] == -18)// Recule le compteur de 0.1 tour
  {
    if (choix == 1)
     {
     Compteur = Compteur - 1;
     choix = !choix;
     choixmenu = !choixmenu;
     lcd.setCursor(0,1);
     lcd.print("    -0.1 spire  ");
     delay(500);
     Effaceligne2 ();
     }    
  }
 if (*menu_valeur[up][droit] == -19)// Sauve en EEPROM
  {
    if (choix == 1)
     {
     Choixsauvegarde();
     Sauve();
     choix = !choix;
     lcd.setCursor(0,1);
     lcd.print("Sauve en EEPROM ");
     delay(1000);
     lcd.setCursor(0,1);
     lcd.print("Donnee          ");
     lcd.setCursor(8,1);
     lcd.print(numerosauvegarde);
     delay(1000);
     Effaceligne2 ();
     }    
  }
 if (*menu_valeur[up][droit] == -20)// Recharge depuis l'EEPROM
  {
    if (choix == 1)
     {
     Choixsauvegarde();
     Recharge();
     choix = !choix;
     lcd.setCursor(0,1);
     lcd.print("Charge donnees  ");
     delay(1000);
     lcd.setCursor(0,1);
     lcd.print("Stockee en      ");
     lcd.setCursor(12,1);
     lcd.print(numerosauvegarde);
     delay(1000);
     Effaceligne2 ();
     }    
  }
 if (*menu_valeur[up][droit] == -21)
  {
  cumul_spire = ((*menu_valeur[0][1])*10000)+((*menu_valeur[0][2])*1000)+((*menu_valeur[0][3])*100)+((*menu_valeur[0][4])*10)+(*menu_valeur[0][5]);
  reste_compteur = cumul_spire*0.1;
  Effaceligne2 ();
  lcd.setCursor(0,1);
  lcd.print(reste_compteur,1);
  lcd.setCursor(6,1);
  lcd.print("           ");
  }
 if (*menu_valeur[up][droit] == -22)
  {
  cumul_couche = ((*menu_valeur[2][1])*100)+((*menu_valeur[2][2])*10)+(*menu_valeur[2][3]);
  Effaceligne2 ();
  lcd.setCursor(0,1);
  lcd.print(cumul_couche);
  lcd.setCursor(4,1);
  lcd.print("            ");
  }
}

/*******************************************************
*******************************************************/

// Fonction qui retourne le compteur ou décompteur vers le menu
// vers le tableau des valeurs 

void Retournerestecompteur ()
{ 
  if (menu_config [0] == 0)
  {
   retour_compteur = Compteur; 
  }
  else if (menu_config [0] == 1)
  {
   retour_compteur = cumul_spire; 
  }
  
  E = int (retour_compteur*0.0001);
  (*menu_valeur[0][1]) = E;
  F = retour_compteur - (10000 * E);
  G = int ( F*0.001 );
  (*menu_valeur[0][2]) = G;
  H = F - ( 1000 * G );
  I = int ( H * 0.01 );
  (*menu_valeur[0][3]) = I;
  J = H - ( 100 * I );
  K = int ( J * 0.1 );
  (*menu_valeur[0][4]) = K;
  L = J - ( 10 * K );
  (*menu_valeur[0][5]) = L;
}

/*******************************************************
*******************************************************/

// Fonction qui gère le sens du guide fil en fonction du nombre de spire par couche

void Nombrespirecouche()
{
  Compteurspcouche = Compteurspcouche + Sense;
  X = Compteurspcouche;
  Y = cumul_couche*20;
  
  if (X<Y)
  {
  if (X == 0 && X < (Y*0.5))
  {
   Sensspirecouche = 1; 
  }
  if (X >= (Y*0.5) && X < Y)
  {
   Sensspirecouche = -1; 
  }
  }
  if (X==Y)
  {
   Compteurspcouche = 0;  
  }
}

/*******************************************************
*******************************************************/

// Fonction qui prépare la sauvegarde des valeurs

void Sauve()
{
 menu_config [Z+1] = (*menu_valeur[0][1]);
 menu_config [Z+2] = (*menu_valeur[0][2]);
 menu_config [Z+3] = (*menu_valeur[0][3]);
 menu_config [Z+4] = (*menu_valeur[0][4]);
 menu_config [Z+5] = (*menu_valeur[0][5]);
 menu_config [Z+6] = (*menu_valeur[2][1]);
 menu_config [Z+7] = (*menu_valeur[2][2]);
 menu_config [Z+8] = (*menu_valeur[2][3]);
 for (int i = 1; i < 9; i++)
 {
  EEPROM.write((Z+i),(menu_config [(Z+i)]));
 }
}

/*******************************************************
*******************************************************/

// Fonction qui prépare la recharge des valeurs

void Recharge()
{
  (*menu_valeur[0][1]) = EEPROM.read(Z+1);
  (*menu_valeur[0][2]) = EEPROM.read(Z+2);
  (*menu_valeur[0][3]) = EEPROM.read(Z+3);
  (*menu_valeur[0][4]) = EEPROM.read(Z+4);
  (*menu_valeur[0][5]) = EEPROM.read(Z+5);
  (*menu_valeur[2][1]) = EEPROM.read(Z+6);
  (*menu_valeur[2][2]) = EEPROM.read(Z+7);
  (*menu_valeur[2][3]) = EEPROM.read(Z+8);
}

/*******************************************************
*******************************************************/

// Fonction qui selectionne un numéro de sauvegarde

void Choixsauvegarde ()
{
 numerosauvegarde =  (*menu_valeur[6][1]);
 switch (numerosauvegarde)
  {
  case 0:
  {
   Z = 0;
  }
 
  break;
  
  case 1:
  {
   Z = 10;
  }
 
  break;
  case 2:
  {
   Z = 20;
  }
 
  break;
  case 3:
  {
   Z = 30;
  }
 
  break;
  case 4:
  {
   Z = 40;
  }
 
  break;
  case 5:
  {
   Z = 50;
  }
 
  break;
  case 6:
  {
   Z = 60;
  }
 
  break;
  case 7:
  {
   Z = 70;
  }
 
  break;
  case 8:
  {
   Z = 80;
  }
 
  break;
  case 9:
  {
   Z = 90;
  }
 
  break;
  }
}
