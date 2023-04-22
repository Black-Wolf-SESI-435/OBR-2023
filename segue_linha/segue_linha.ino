/* @Giroscopio
#include <MPU6050_tockn.h>
#include <Wire.h>
*/

// Motor directions
#define FRENTE 1
#define RE    -1
#define PARA   0

#define ESQUERDA   0
#define DIREITA    1
#define PRETO      0
#define BRANCO     1

#define NTESTES    125
#define VELOCIDADE 120
#define VEL_GIRO   90


byte buzzer_pino = 2;

byte pinosEnergia[2] = {37, 35}; // Vermelhos
/* Os cabos ligados no pinos digitais que não são da ponte H - os pinos da ponte H são
 * os cabos brancos e marrons - são cabos de energia para os sensores e para o buzzer
 * e devem ser ligados como positivo. Esses código é feito no setup().
*/

void tocar (bool musica) {
    if (musica) {
        for (int hz = 1400; hz >= 700; hz -= 100) {
            tone(buzzer_pino, hz, 200);
            delay(250);
            noTone(buzzer_pino);
        }
        delay(100);
    }
    else {
        for (byte x = 0; x < 3; x ++) {
            tone(buzzer_pino, 500, 300);
            delay(400);
            tone(buzzer_pino, 3000, 300);
            delay(400);
        }
    }
    noTone(buzzer_pino);
}

class Sensor {
    public:
        byte pinoEntrada;
        unsigned short threshold;

        Sensor(byte pino) {
            this->pinoEntrada = pino;
            this->threshold = 0;
        }

        byte ler() {
            return (analogRead(this->pinoEntrada) > this->threshold);
        }
};

// @Motor
struct Motor {
    byte pinoControle_1 = 0;
    byte pinoControle_2 = 0;
    byte pinoVelocidade = 0;
};

struct Motor motorDireita;
struct Motor motorEsquerda;

void controlar_robo (byte roda_direita, byte roda_esquerda, byte velocidade) {
    // debug
    Serial.print("Controlar Robo:");
    Serial.print(" direita: " + String(roda_direita));
    Serial.print(" esquerda: " + String(roda_esquerda));
    Serial.print(" velocidade: " + String(velocidade));

    // liga o motor da direita
    digitalWrite(motorDireita.pinoControle_1, roda_direita == RE? HIGH : LOW);
    digitalWrite(motorDireita.pinoControle_2, roda_direita == FRENTE? HIGH : LOW);

    // liga o motor da esquerda
    digitalWrite(motorEsquerda.pinoControle_1, roda_esquerda == RE? HIGH : LOW);
    digitalWrite(motorEsquerda.pinoControle_2, roda_esquerda == FRENTE? HIGH : LOW);

    // controla a velocidade
    analogWrite(motorDireita.pinoVelocidade, velocidade);
    analogWrite(motorEsquerda.pinoVelocidade, velocidade);
}

Sensor sensorExEsquerda(A0);    // Amarelo
Sensor sensorEsquerda  (A1);    // Azul
Sensor sensorDireita   (A2);    // Roxo
Sensor sensorExDireita (A3);    // Verde

Sensor sensorCorDireita (A4);
Sensor sensorCorEsquerda(A5);

/* @Giroscopio
MPU6050 mpu6050(Wire);          // Giroscopio
*/

void calibrar() {
    Serial.println("========================================");
    Serial.println("Coloque todos os sensores na cor BRANCA\n");
    
    tocar(1);
    delay(2000);

    // Calibragem Branco
    unsigned long media_brancoD  = 0;
    unsigned long media_brancoE  = 0;
    unsigned long media_brancoDD = 0;
    unsigned long media_brancoEE = 0;

    for (short teste = 1; teste <= NTESTES; teste++) {
        media_brancoD  += analogRead( sensorDireita.pinoEntrada    );
        media_brancoE  += analogRead( sensorEsquerda.pinoEntrada   );
        media_brancoDD += analogRead( sensorExDireita.pinoEntrada  );
        media_brancoEE += analogRead( sensorExEsquerda.pinoEntrada );
    }

    tocar(0);
    delay(2000);

    Serial.println("========================================");
    Serial.println("Coloque todos os sensores na cor PRETA\n");
    
    tocar(1);
    delay(2000);


    // Calibragem Preto
    unsigned long media_pretoD  = 0;
    unsigned long media_pretoE  = 0;
    unsigned long media_pretoDD = 0;
    unsigned long media_pretoEE = 0;

    for (short teste = 1; teste <= NTESTES; teste++) {
        media_pretoD  += analogRead( sensorDireita.pinoEntrada    );
        media_pretoE  += analogRead( sensorEsquerda.pinoEntrada   );
        media_pretoDD += analogRead( sensorExDireita.pinoEntrada  );
        media_pretoEE += analogRead( sensorExEsquerda.pinoEntrada );
    }

    sensorDireita.threshold    = round( (media_brancoD  + media_pretoD  ) / (NTESTES * 2));
    sensorEsquerda.threshold   = round( (media_brancoE  + media_pretoE  ) / (NTESTES * 2));
    sensorExDireita.threshold  = round( (media_brancoDD + media_pretoDD ) / (NTESTES * 2));
    sensorExEsquerda.threshold = round( (media_brancoEE + media_pretoEE ) / (NTESTES * 2));

    Serial.println("========================================");
    Serial.println("Sensor Direita     | " + String(sensorDireita.threshold));
    Serial.println("Sensor Esquerda    | " + String(sensorEsquerda.threshold));
    Serial.println("Sensor Ex Direita  | " + String(sensorExDireita.threshold));
    Serial.println("Sensor Ex Esquerda | " + String(sensorExEsquerda.threshold));
    Serial.println("========================================");
    tocar(0);
}

/* @Giroscopio
void girar (bool direcao, int angulo) {
    // Para o Robo
    controlar_robo(PARA, PARA, 0);

    mpu6050.update();
    int anguloInicial = round(mpu6050.getAngleZ());
    int anguloAlvo    = anguloInicial + (direcao? - angulo : angulo);
    int leitura       = anguloInicial;

    // Gira o Robo
    if (direcao == DIREITA) { // Gira para direita
        controlar_robo(RE, FRENTE, VEL_GIRO);
    } else { // Gira para esquerda
        controlar_robo(FRENTE, RE, VEL_GIRO);
    }
    
    while (leitura != anguloAlvo) {
        mpu6050.update();
        leitura = round(mpu6050.getAngleZ());
    
        Serial.print("Angulo: " + String(leitura));
    
    }
    
    // Para o Robo
    controlar_robo(PARA, PARA, 0);
}
*/

void setup() {
    byte output_pins[6] = {12, 13, 50, 51, 52, 53};

    for (int i = 0; i < sizeof(output_pins); i ++) {
        pinMode(output_pins[i], OUTPUT);
        digitalWrite(output_pins[i], HIGH);
    }

    pinMode(buzzer_pino, OUTPUT);

    motorDireita.pinoControle_1 = 52;  // Marrom: IN_1
    motorDireita.pinoControle_2 = 53;  // Branco: IN_3
    motorDireita.pinoVelocidade = 13;  // Laranja EN_A

    motorEsquerda.pinoControle_1 = 50;  // Marrom: IN_1
    motorEsquerda.pinoControle_2 = 51;  // Branco: IN_3
    motorEsquerda.pinoVelocidade = 12;  // Laranja EN_A

    for (int i = 0; i < sizeof(pinosEnergia); i ++) {
        pinMode(pinosEnergia[i], OUTPUT);
        digitalWrite(pinosEnergia[i], HIGH);
    }

    Serial.begin(9600);

    /* @Giroscopio
    Wire.begin();
    mpu6050.begin();
    // Calibragem do giroscopio
    tocar(1);
    mpu6050.calcGyroOffsets(true); // delay de 3 s antes da calibragem e 3 s depois
    tocar(0);
    */

    calibrar();
}

void loop() {
    /* @Giroscopio
    mpu6050.update(); // Calcula o angulo do giroscopio
    */
    
    // Segue Linha
    byte leituraXD = sensorExDireita.ler();
    byte leituraD  = sensorDireita.ler();
    byte leituraE  = sensorEsquerda.ler();
    byte leituraXE = sensorExEsquerda.ler();

    Serial.print( "ExD: " ); Serial.print  (leituraXD);
    Serial.print( "\tD: " ); Serial.print  (leituraD);
    Serial.print( "\tE: " ); Serial.print  (leituraE);
    Serial.print("\tExE: "); Serial.println(leituraXE);

    if (leituraD == PRETO and leituraE == PRETO) {
        controlar_robo(FRENTE, FRENTE, VELOCIDADE);
    }
    else if (leituraD == BRANCO and leituraE == PRETO) {
        controlar_robo(RE, FRENTE, VELOCIDADE);
    }
    else if (leituraD == PRETO and leituraE == BRANCO) {
        controlar_robo(FRENTE, RE, VELOCIDADE);
    }
    else if (leituraD == BRANCO and leituraE == BRANCO) {
        controlar_robo(FRENTE, FRENTE, VELOCIDADE);
    }
}