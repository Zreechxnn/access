#include <Servo.h>

#define SERVO_KAMAR 2
#define SERVO_RUMAH 3
#define TRIG_PIN 4
#define ECHO_PIN 5
#define LAMPU1 6
#define LAMPU2 7
#define TOUCH_PIN 8 
#define SPEAKER_PIN A1

int lagu1[] = {
    262, 294, 330, 349, 392, 330, 262
}; 
int durations1[] = {
    600, 300, 550, 600, 800, 300, 800
};
int lagu2[] = {
    392, 330, 392, 330, 330, 392, 349, 330, 349, 392,  
    294, 294, 330, 330, 349,349, 392, 349, 330, 294, 261
};

int durations2[] = {
    600, 600, 600, 350, 200, 200, 200, 200, 300, 600,  
    200, 200, 200, 200, 200, 400, 200, 200, 400, 400, 600 
};

Servo servoKamar;
Servo servoRumah;

bool rumahTerbuka = false;
bool kamarTerbuka = false;

void setup() {
  Serial.begin(9600);
  
  servoKamar.attach(SERVO_KAMAR);
  servoRumah.attach(SERVO_RUMAH);
  
  servoKamar.write(0);
  servoRumah.write(0);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LAMPU1, OUTPUT);
  pinMode(LAMPU2, OUTPUT);
  pinMode(TOUCH_PIN, INPUT);
  pinMode(SPEAKER_PIN, OUTPUT);

  Serial.println(F("Sistem Servo dan Sensor Jarak siap!"));
  Serial.println(F("Sentuh sensor untuk membuka atau menutup pintu rumah."));
  Serial.println(F("Gunakan perintah 'ON' atau 'OFF' untuk membuka/tutup pintu kamar."));
}

void loop() {
  if (digitalRead(TOUCH_PIN) == HIGH) {
    if (!rumahTerbuka) {
      servoRumah.write(90); 
      rumahTerbuka = true;
      Serial.println(F("Pintu rumah terbuka."));
    } else {
      servoRumah.write(0); 
      rumahTerbuka = false;
      Serial.println(F("Pintu rumah tertutup."));
    }

    delay(2000);

    long distance = getDistance();
    if (distance >= 0 && distance < 8) {
      digitalWrite(LAMPU1, HIGH); 
      digitalWrite(LAMPU2, LOW); 
    } else if (distance >= 8 && distance < 16){
      digitalWrite(LAMPU1, LOW); 
      digitalWrite(LAMPU2, HIGH); 
    } else {
      digitalWrite(LAMPU1, LOW); 
      digitalWrite(LAMPU2, LOW); 
    }
  }

  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "ON" && !kamarTerbuka) {
      servoKamar.write(90); 
      kamarTerbuka = true;
      Serial.println("Pintu kamar terbuka (Servo ON)");
    } else if (command == "OFF" && kamarTerbuka) {
      servoKamar.write(0);
      kamarTerbuka = false;
      Serial.println("Pintu kamar tertutup (Servo OFF)");
    } else if (command == "MUSIK1") {
      Serial.println("Memainkan Musik 1...");
      playMelody(lagu1, durations1, sizeof(lagu1) / sizeof(lagu1[0]));
    } else if (command == "MUSIK2") {
      Serial.println("Memainkan Musik 2...");
      playMelody(lagu2, durations2, sizeof(lagu2) / sizeof(lagu2[0]));
    }
  }
}

long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}

void playMelody(int melody[], int durations[], int melodyLength) {
  for (int i = 0; i < melodyLength; i++) {
    tone(SPEAKER_PIN, melody[i]); 
    delay(durations[i]);          
    noTone(SPEAKER_PIN);           
    delay(50);                    
  }
}
