#define DELAY 2000


class Motor {
    private:
        byte pinoMarrom;
        byte pinoBranco;
        byte pinoControle;

    public:
        Motor(byte pM, byte pB, byte pC) {
            this->pinoMarrom   = pM;
            this->pinoBranco   = pB;
            this->pinoControle = pC;

            pinMode(this->pinoMarrom, OUTPUT);
            pinMode(this->pinoBranco, OUTPUT);
            pinMode(this->pinoControle, OUTPUT);
        }

    void ligar(bool pinoM, bool pinoB, byte velocidade) {

            Serial.println("Pino Marrom:  (" + String(pinoM) + ")\t\t\t" + String(pinoM? HIGH : LOW));
            Serial.println("Pino Branco:  (" + String(pinoB) + ")\t\t\t" + String(pinoB? HIGH : LOW));
            Serial.println("Pino Laranja: (" + String(velocidade) + ")\t\t\t" + String(velocidade));

            digitalWrite(pinoMarrom, pinoM);
            digitalWrite(pinoBranco, pinoB);
            analogWrite(pinoControle, velocidade);
        }
};


Motor motorDireita (22, 24, 13);    // Azul (IN_1 ) : Verde (IN_2) : Roxo (EN_A)
Motor motorEsquerda(23, 25, 11);    // Amarelo (IN_3 ) : Branco (IN_4) : Laranja (EN_B)

void setup() {
    Serial.begin(9600);
}

void loop() {
    Serial.println("----------------------- Testando Motores -----------------------");
    Serial.println("Motor direita -> Frente:");
    motorDireita.ligar(LOW, HIGH, 255);
    delay(DELAY);
    
    Serial.println("Motor Esquerda -> Frente:");
    motorEsquerda.ligar(LOW, HIGH, 255);
    delay(DELAY);
   
}
