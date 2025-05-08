#include <Arduino.h>

// Capteurs de places
#define CAPTEUR_PLACE_0 33
#define CAPTEUR_PLACE_1 14
#define CAPTEUR_PLACE_2 27
#define CAPTEUR_PLACE_3 26
#define CAPTEUR_PLACE_4 25

// Capteurs d’étage
#define CAPTEUR_ETAGE_0 15
#define CAPTEUR_ETAGE_1 16
#define CAPTEUR_ETAGE_2 4

// Capteur de distribution
#define CAPTEUR_DISTRIBUTION 13

// Moteurs
#define MOTEUR_ROTATION_PWM 19
#define MOTEUR_ROTATION_DIR 18
#define MOTEUR_HAUTEUR_PWM 22
#define MOTEUR_HAUTEUR_DIR 21
#define MOTEUR_DISTRIBUTION 23

// Sauvegarde des places

struct Place
{
  int etage;
  int place;
  bool occupee;
  String id;
  String password;
};

Place place[] = {
  {CAPTEUR_ETAGE_0, CAPTEUR_PLACE_0, true, "Home", "admin"},
  {CAPTEUR_ETAGE_0, CAPTEUR_PLACE_1, false, "", ""},
  {CAPTEUR_ETAGE_0, CAPTEUR_PLACE_2, false, "", ""},
  {CAPTEUR_ETAGE_0, CAPTEUR_PLACE_3, false, "", ""},
  {CAPTEUR_ETAGE_0, CAPTEUR_PLACE_4, false, "", ""},
  {CAPTEUR_ETAGE_1, CAPTEUR_PLACE_0, false, "", ""},
  {CAPTEUR_ETAGE_1, CAPTEUR_PLACE_1, false, "", ""},
  {CAPTEUR_ETAGE_1, CAPTEUR_PLACE_2, false, "", ""},
  {CAPTEUR_ETAGE_1, CAPTEUR_PLACE_3, false, "", ""},
  {CAPTEUR_ETAGE_1, CAPTEUR_PLACE_4, false, "", ""},
  {CAPTEUR_ETAGE_2, CAPTEUR_PLACE_0, false, "", ""},
  {CAPTEUR_ETAGE_2, CAPTEUR_PLACE_1, false, "", ""},
  {CAPTEUR_ETAGE_2, CAPTEUR_PLACE_2, false, "", ""},
  {CAPTEUR_ETAGE_2, CAPTEUR_PLACE_3, false, "", ""},
  {CAPTEUR_ETAGE_2, CAPTEUR_PLACE_4, false, "", ""}
};


// Fonctions de déplacement
bool verticalPlace(int place){
  while(digitalRead(CAPTEUR_ETAGE_0) != LOW){
    digitalWrite(MOTEUR_HAUTEUR_DIR, LOW);
    ledcWrite(0,200);
  }
  ledcWrite(0, 0);
  
  while (digitalRead(place) != LOW) {
    digitalWrite(MOTEUR_ROTATION_DIR, HIGH);
    ledcWrite(1, 200);
  }
  ledcWrite(1, 0);
  return true;
}

bool horizontalEtage(int etage){  
  while (digitalRead(etage) != LOW) {
    digitalWrite(MOTEUR_HAUTEUR_DIR, HIGH);
    ledcWrite(0, 200);
  }
  ledcWrite(0, 0);
  return true;
}

bool returnToHome(){
  while (digitalRead(CAPTEUR_ETAGE_0) != LOW) {
    digitalWrite(MOTEUR_HAUTEUR_DIR, LOW);
    ledcWrite(0, 200);
  }
  ledcWrite(0, 0);

  while (digitalRead(CAPTEUR_PLACE_0) != LOW) {
    digitalWrite(MOTEUR_ROTATION_DIR, HIGH);
    ledcWrite(1, 200);
  }
  ledcWrite(1, 0);
  return true;
}

bool distribution(){
  while (digitalRead(CAPTEUR_DISTRIBUTION) != LOW) {
    digitalWrite(MOTEUR_DISTRIBUTION, 1);
  }
  digitalWrite(MOTEUR_DISTRIBUTION, 0);
  return true;
}

bool goToPlace(int index){
  verticalPlace(place[index].place);
  horizontalEtage(place[index].etage);
  distribution();
  returnToHome();
  return true;
}

int searchPlace(String id){
  for(int i = 0; i < sizeof(place)/ sizeof(place[0]); i++){
    if(place[i].id == id){
      return i;
    }
  }
  return -1;
}

bool searchID(String id){
  for(int i = 0; i < sizeof(place)/ sizeof(place[0]); i++){
    if(place[i].occupee && place[i].id == id){
      return true;
    }
  }
  return false;
}

bool verifyPWD(int index, String pwd){
  if (place[index].password == pwd)
  {
    return true;
  }
  return false;
}

int getEmptyPlace(){
  for(int i = 0; i < sizeof(place)/ sizeof(place[0]); i++){
    if (place[i].occupee == false)
    {
      return i;
    }
  }
  return -1;
}

bool verifyId(String id){
  for(int i = 0; i < sizeof(place)/ sizeof(place[0]); i++){
    if (place[i].occupee && place[i].id == id) {
      return false; // L'id est déjà utilisé
    }
  }
  return true; // L'id est libre
}

String readLine() {
  String input = "";
  while (true) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '\n' || c == '\r') {
        if (input.length() > 0) return input;  // Évite les retours vides
      } else {
        input += c;
      }
    }
  }
}

void setup() {
  // Capteurs
  pinMode(CAPTEUR_PLACE_0, INPUT_PULLUP);
  pinMode(CAPTEUR_PLACE_1, INPUT_PULLUP);
  pinMode(CAPTEUR_PLACE_2, INPUT_PULLUP);
  pinMode(CAPTEUR_PLACE_3, INPUT_PULLUP);
  pinMode(CAPTEUR_PLACE_4, INPUT_PULLUP);
  
  pinMode(CAPTEUR_ETAGE_0, INPUT_PULLUP);
  pinMode(CAPTEUR_ETAGE_1, INPUT_PULLUP);
  pinMode(CAPTEUR_ETAGE_2, INPUT_PULLUP);
  
  pinMode(CAPTEUR_DISTRIBUTION, INPUT_PULLUP);
  
  // Moteurs
  pinMode(MOTEUR_ROTATION_PWM, OUTPUT);
  pinMode(MOTEUR_ROTATION_DIR, OUTPUT);
  pinMode(MOTEUR_HAUTEUR_PWM, OUTPUT);
  pinMode(MOTEUR_HAUTEUR_DIR, OUTPUT);
  pinMode(MOTEUR_DISTRIBUTION, OUTPUT);

  Serial.begin(115200);
  Serial.println("Parking initialisé");

  ledcAttachPin(MOTEUR_HAUTEUR_PWM, 0);
  ledcSetup(0,1000,8);
  ledcAttachPin(MOTEUR_ROTATION_PWM, 1);
  ledcSetup(1,1000,8);
}

void loop() {
  Serial.println("Deposer une voiture : 1\nRecuperer une voiture : 2");
  while (!Serial.available());
  String choix = readLine();

  if (choix == "1") {
    Serial.println("Saisissez votre plaque d'immatriculation :");
    String id = readLine();
    while (!verifyId(id)) {
      Serial.println("Immatriculation deja existante, reessayez :");
      id = readLine();
    }

    Serial.println("Saisissez un mot de passe :");
    String pwd = readLine();

    int placenb = getEmptyPlace(); 
    if (placenb != -1) {
      place[placenb].id = id;
      place[placenb].password = pwd;
      Serial.println("Garage en cours...");
      
      if (goToPlace(placenb)) {
        Serial.println("Voiture garée, merci !");
        place[placenb].occupee = true;
      } else {
        Serial.println("Erreur lors du placement.");
      }

    } else {
      Serial.println("Aucune place disponible");
    }
  }
  else if (choix == "2") {
    Serial.println("Saisissez votre plaque d'immatriculation :");
    String id = readLine();
    while (!searchID(id)) {
      Serial.println("Plaque inconnue, reessayez :");
      id = readLine();
    }

    Serial.println("Saisissez votre mot de passe :");
    String pwd = readLine();
    while (!verifyPWD(searchPlace(id), pwd)) {
      Serial.println("Mauvais mot de passe, reessayez :");
      pwd = readLine();
    }

    Serial.println("Votre voiture arrive...");
    if (goToPlace(searchPlace(id))) {
      place[searchPlace(id)].occupee = false;
      Serial.println("Vous pouvez recuperer votre voiture, bonne journée !");
      delay(3000);
    } else {
      Serial.println("Erreur lors de la récupération.");
    }
  }

  Serial.println("----- Fin d'opération -----\n");
  delay(500);  // Pause pour laisser le temps à l'utilisateur de lire
}
