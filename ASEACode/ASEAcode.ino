#include <Servo.h>
#include <LiquidCrystal.h>
#include <DHT.h>

#define DHTPIN 10     // Pin collegato al sensore DHT
#define DHTTYPE DHT11 // Tipo di sensore DHT

DHT dht(DHTPIN, DHTTYPE);

const int trigPin = 12; // Pin trigger del sensore
const int echoPin = 13; // Pin echo del sensore
const int moteur = 11;  // Pin del servo motore
const int buttonPin = 9; // Pulsante per cambiare opzione

// LCD
LiquidCrystal lcd(3, 4, 5, 6, 7, 8);
Servo myservo;

// Variabili
int selectedOption = 0; // Opzione selezionata
bool buttonPressed = false;
unsigned long lastDebounceTime = 0; // Per evitare rimbalzi del pulsante
const unsigned long debounceDelay = 300;
int pos = 0; // Posizione del servo motore

void setup() {
    // Configura LCD
    lcd.begin(16, 2);
    lcd.clear();

    pinMode(buttonPin, INPUT);

    // Configura il servo e i pin del sensore
    myservo.attach(moteur);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    // Configura il contrasto (opzionale)
    analogWrite(2, 25);

    // Inizializzazione del sensore di temperatura
    dht.begin();

    // Comunicazione seriale per debug
    Serial.begin(9600);
    Serial.println("Sistema pronto. Premi il pulsante per scorrere.");

    // Mostra il menu iniziale
    displayMenu();
}

void loop() {
    int buttonState = digitalRead(buttonPin); // Leggi lo stato del pulsante

    if (buttonState == HIGH && !buttonPressed) { 
        delay(50); // Stabilizza il pulsante
        if (digitalRead(buttonPin) == HIGH) { // Conferma il pulsante premuto
            buttonPressed = true; // Imposta stato premuto

            // Cambia opzione manualmente
            selectedOption++;
            if (selectedOption > 3) selectedOption = 0;

            // Mostra il menu o esegue un'azione
            if (selectedOption == 0) {
                displayMenu();
            }
            else {
                executeOption(selectedOption);
            }
        }
    }

    // Controllo del rilascio del pulsante
    if (buttonState == LOW && buttonPressed) { 
        buttonPressed = false; // Resetta stato premuto
    }
}

void displayMenu() 
{ 
    lcd.clear(); 
    lcd.setCursor(0, 0); 
    // METTERE LA CODIFICA ASCII 
    // (MENU: 1.Temp)
    lcd.write(32); 
    lcd.write(77); 
    lcd.write(69); 
    lcd.write(78); 
    lcd.write(85); 
    lcd.write(58);
    lcd.write(32);
    lcd.write(32);
    lcd.write(32);
    lcd.write(49); 
    lcd.write(46); 
    lcd.write(84); 
    lcd.write(101);
    lcd.write(109); 
    lcd.write(112); 
    lcd.write(32); 
    //2.Umi 3.Radar 
    lcd.setCursor(0, 1);
    lcd.write(32); 
    lcd.write(50); 
    lcd.write(46); 
    lcd.write(85); 
    lcd.write(109); 
    lcd.write(105);
    lcd.write(100);
    lcd.write(32); 
    lcd.write(51); 
    lcd.write(46); 
    lcd.write(82); 
    lcd.write(97); 
    lcd.write(100); 
    lcd.write(97); 
    lcd.write(114); 
    Serial.println("Menu visualizzato.");
}  

void executeOption(int option) {
    switch (option) {
        case 1:
            displayTemp();
            break;
        case 2:
            displayHumidity();
            break;
        case 3:
            displayRadar();
            break;
        default:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Errore opzione!");
    }
}

void displayTemp() {
    float t = dht.readTemperature(); // Legge la temperatura

    if (isnan(t)) {
        lcd.clear();
        lcd.print("Errore sensore");
        return;
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: " + String(t) + " C");
    delay(500);
}

void displayHumidity() {
    float h = dht.readHumidity();

    if (isnan(h)) {
        lcd.clear();
        lcd.print("Errore sensore");
        return;
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Umid: " + String(h) + " %");

    delay(500);
}

void displayRadar() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scansione...");

    for (pos = 0; pos <= 180; pos += 2) {
        myservo.write(pos);

        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);

        float duration = pulseIn(echoPin, HIGH, 30000);
        float distance = (duration > 0) ? duration * 0.034 / 2 : -1;

        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        if (distance < 2 || distance > 400 || distance < 0) {
            lcd.print("Fuori range");
        }
        else if (distance < 10) { // Se un oggetto è vicino
            lcd.print("ATTENZIONE! Ostacolo!");
        }
        else {
            lcd.print("Dist: " + String(distance) + " cm");
        }

        delay(100);
    }

    // Movimento indietro
    pos = 180;
    while (pos >= 0) {

        myservo.write(pos);

        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);

        float duration = pulseIn(echoPin, HIGH, 30000);
        float distance = (duration > 0) ? duration * 0.034 / 2 : -1;

        lcd.setCursor(0, 1);
        lcd.print("                "); // Pulisce la riga
        lcd.setCursor(0, 1);
        if (distance < 2 || distance > 400 || distance < 0) {
            lcd.print("Fuori range");
        }
        else if (distance < 10) { // Se un oggetto è vicino
            lcd.print("ATTENZIONE! Ostacolo!");
        }
        else {
            lcd.print("Dist: " + String(distance) + " cm");
        }

        delay(15); // Ritardo per movimento fluido
        pos -= 1; // Decremento di pos
    }

    delay(2000);
}