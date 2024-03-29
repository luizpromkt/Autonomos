//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
// 1. REVER FILTRO DO VARIO E CONFERIR A CALIBRAGEM DO DETECTOR DE MASSA TERMAL, LIMITAR O GANHO DE TEMPO DA SUSTENTAÇÃO EM 7.5s
// 2. CHAVE DE 3 POSIÇÕES PARA ESCOLHER SEM AUTOMAÇÃO / INFLUENCIA BAIXA DO ESTABILIZADOR / AUTOMAÇÃO COMPLETA COM CURVAS /
// 3. CHAVE PARA ATIVAR OU DESATIVAR FLAPERONS
// 4. CRIAR A FEATURE DE CURVA PARA O LADO QUE O AILERON SUBIR E UM AUMENTO RELEVANTE DE SUSTENTAÇÃO.
// 5. ATIVAR FLAPERONS COM CHAVE E DESLIGAR AUTOMATICAMENTE QUANDO MOTOR ESTIVER DESLIGADO E QUANDO ESTIVER FAZENDO CURVAS.
// 6. FAZER TESTES DE STRESS COM MOTOR LIGADO E ESTIMATIVA DE CONSUMO DE BATERIA.
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------


#include <Servo.h> //SERVO
#include <Wire.h> //VARIO



// Detector de Massa termal
const int ledPin =  LED_BUILTIN;
int ledState = LOW;             
unsigned long previousMillis = 0;        
const long interval = 500;          
int valor=10;






//SERVO
Servo AILERON_D;
Servo PROFUNDOR;
Servo AILERON_E;
Servo MOTOR;

int ch1; 
int ch2;
int ch3;
int ch5;
int ch6;
int tombo;



//FILTRO
int c;
int d;
int filtroX[20];
int filtroY[20];
int filtroZ[20];
int filtroch1[20];
int filtroch2[20];
int filtroch6[20];
int fcompleto=0;
int fcompletod=0;
int fcompletoe=0;
int somaX=0;
int somaY=0;
int somaZ=0;
int somach1=0;
int somach2=0;
int somach6=0;
long somap=0;
int filtrof;
long filtrop[20];
long pressao;
int serial; 
int e;


//ACELEROMETRO
#define pinAcelX A0
#define pinAcelY A1
#define pinAcelZ A2



//VARIO
const byte led = 13;
unsigned int calibrationData[7];
unsigned long time = 0;
float toneFreq, toneFreqLowpass, pressure, lowpassFast, lowpassSlow ;
int ddsAcc;
int AnguloX;
int AnguloY;
long hpa;
int altimetro=10090;
long pressaoAnterior;


//CONFIGURAÇÕES
void setup()
{


// Detector de Massa termal
pinMode(ledPin, OUTPUT);



 
pressaoAnterior-pressao;

  
  //VARIO DIY CRÉDITO
  Wire.begin();
  Serial.begin(9600);
  setupSensor();
  pressure = getPressure();
  lowpassFast = lowpassSlow = pressure;



//SERVO
  AILERON_D.attach(5);
  PROFUNDOR.attach(4);
  AILERON_E.attach(3);
  MOTOR.attach(2);
  AILERON_D.write(90);   
  PROFUNDOR.write(90);   
  AILERON_E.write(90); 



//RECEPTOR
  pinMode(10, INPUT); 
  pinMode(9, INPUT); 
  pinMode(8, INPUT);
  pinMode(7, INPUT);
  pinMode(6, INPUT);
}


//LOOP PRINCIPAL
void loop()
{

// Detector de Massa termal
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (valor >= 2) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

if (valor > 0) {
valor=valor-1;
}
    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }







 //RECEPTOR
 ch1 = pulseIn(9, HIGH, 400000); // AILERON D 
 ch2 = pulseIn(8, HIGH, 400000); // PROFUNDOR
 ch3 = pulseIn(10, HIGH, 400000); // MOTOR
 ch6 = pulseIn(6, HIGH, 400000); // AILERON E
 ch5 = ch1;
 // Serial.print("Servo Aileron D "); 
 // ch1 = map(ch1, 1100, 2000, 160, 20);
 // ch2 = map(ch2, 1100, 2000, 20, 160);
 // ch5 = map(ch5, 1100, 2000, 20, 160);
 // Serial.print(ch1);       
 // Serial.print(" / Servo Profundor ");
 // Serial.print(ch2);
 // Serial.print(" / Servo Aileron E ");
 // Serial.println(ch6);



//ACELEROMETRO
  int valX = analogRead(pinAcelX);
  int valY = analogRead(pinAcelY);
  int valZ = analogRead(pinAcelZ);



//VARIO DIY CRÉDITO
 pressure = getPressure();
 lowpassFast = lowpassFast + (pressure - lowpassFast) * 0.1;
 lowpassSlow = lowpassSlow + (pressure - lowpassSlow) * 0.05;
 toneFreq = (lowpassSlow - lowpassFast) * 50;
 toneFreqLowpass = toneFreqLowpass + (toneFreq - toneFreqLowpass) * 0.1;
 toneFreq = constrain(toneFreqLowpass, -500, 500);
 ddsAcc += toneFreq * 100 + 2000;
 pressao=pressure;

// Serial.println(pressao);
// Serial.print(" | Vario | ");
// Serial.println(toneFreq);
// Serial.println(" ");
// Serial.println(pressure);
// if (toneFreq >=10) {ledOn();}
// else {ledOff();}
 
//FILTRO ACELEROMETRO .
if (c>=9) {c=0;fcompleto=1;}
  filtroX[c]=valX;
  filtroY[c]=valY;
  filtroZ[c]=valZ;
   c++;
   
   if (fcompleto==1){
        for (int i=0; i <= 9; i++){
            somaX=somaX + filtroX[i];
            somaY=somaY + filtroY[i];
            somaZ=somaZ + filtroZ[i];
            }
    somaX=somaX / 10;
    somaY=somaY / 10;
    somaZ=somaZ / 10;
}
    valX=somaX; 
    valY=somaY; 
    valZ=somaZ;


//FILTRO VARIOMETRO    
if (d>=19) {d=0;fcompletod=1;}
    filtrop[d]=pressao;
    d++;
   
   if (fcompletod==1){
        for (int i=0; i <= 19; i++){
            somap=somap + filtrop[i];
            }
        somap=somap / 20;
        }
    pressao=somap;
    comparavario();


//FILTRO RECEPTOR    
if (e>=2) {e=0;fcompletoe=1;}
  filtroch1[e]=ch1;
  filtroch2[e]=ch2;
  filtroch6[e]=ch6;
    e++;
   
   if (fcompletoe==1){
        for (int i=0; i <= 2; i++){
            somach1=somach1 + filtroch1[i];
            somach2=somach2 + filtroch2[i];
            }
    somach1=somach1 / 3;
    somach2=somach2 / 3;
        }
        
    ch1=somach1;
    ch2=somach2;
    

 // Serial.print("X Aileron: ");
   AnguloX = map(valX, 260, 400, -90, 90);
 // Serial.print(valX);
 // Serial.print(AnguloX);
 // Serial.print("  Y Profundor: ");
   AnguloY = map(valY, 260, 400, -90, 90);
 // Serial.print(valY);
 // Serial.print(AnguloY);
 // Serial.print("  Z: ");
 // Serial.print(valZ);
 // Serial.print(" ");
 // Serial.print(ch1);
 // Serial.print(" ");
 // Serial.println(ch2);
  
 //Death Zone
if ( ch1 >= 1450 && ch1 <= 1520 && ch2 >= 1500 && ch2 <= 1590 )

{
//ACELEROMETRO/SERVOR

if (valZ < 300) {
     Serial.print(tombo);
   if (ch6 <= 1200) {

    if (tombo==2){
    ch1=180;
    ch5=180;
      }
else if(tombo==1)  
    {
    ch1=0;
    ch5=0;
   }
      
        } 
  } else if ( (valZ < 360) && (valY < 270)) {
   //  Serial.print("Peso de cauda");
     ch2=0;
  } else if ( (valZ < 360) && (valY > 380)) {
   //  Serial.print("Peso de Bico");
     ch2=180;
  } else  if ( (valZ < 360) && (valX < 270)) {
   //  Serial.print("Tombado para Esquerda");
     ch1=0;
     ch5=0;
     tombo=1;
  } else if ( (valZ < 360) && (valX > 380)) {
   //  Serial.print("Tombado para Direita");
     ch1=180;
     ch5=180;
         tombo=2;
  } else {
         if (ch6 <= 1200) {
              //  Serial.print("Normal");
              ch1 = map(valX, 260, 400, -50, 230);
              ch2 = map(valY, 260, 400, -50, 230);
              ch5 = ch1; 
        } 
  }

} //Death Zone


//SERVO SAIDA
 AILERON_D.write(ch1);   
 PROFUNDOR.write(ch2);   
 AILERON_E.write(ch5);
 MOTOR.write(ch3);
 delay(15); 
}

//VARIO DIY CRÉDITO
long getPressure()
{
  long D1, D2, dT, P;
  float TEMP;
  long altimetro; 
  int64_t OFF, SENS;
  D1 = getData(0x48, 10);
  D2 = getData(0x50, 1);
  dT = D2 - ((long)calibrationData[5] << 8);
  TEMP = (2000 + (((int64_t)dT * (int64_t)calibrationData[6]) >> 23)) / (float)100;
  OFF = ((unsigned long)calibrationData[2] << 16) + (((int64_t)calibrationData[4] * dT) >> 7);
  SENS = ((unsigned long)calibrationData[1] << 15) + (((int64_t)calibrationData[3] * dT) >> 8);
  P = (((D1 * SENS) >> 21) - OFF) >> 15;
  // Serial.println(TEMP);
  // Serial.println(P);
  hpa=P; //100;

 // Serial.print("hpa");
 // Serial.print(hpa);
 altimetro=100900-hpa;// * 30; //0.305;
 // Serial.print("Dhpa");
 //Serial.print(altimetro);
 altimetro=altimetro * 30;
 // Serial.print("Pés");
 // Serial.print(altimetro);
 // altimetro=altimetro / 1000;
 altimetro=altimetro / 0.305;
 //Serial.print(altimetro);
 altimetro=altimetro / 100;
 // Serial.println(altimetro);
 // Serial.println(hpa);  
  return P;
}
//VARIO DIY CRÉDITO
long getData(byte command, byte del)
{
  long result = 0;
  twiSendCommand(0x77, command);
  delay(del);
  twiSendCommand(0x77, 0x00);
  Wire.requestFrom(0x77, 3);
  if(Wire.available()!=3) Serial.println("Error: raw data not available");
  for (int i = 0; i <= 2; i++)
  {
    result = (result<<8) | Wire.read(); 
  }
  return result;
}
//VARIO DIY CRÉDITO
void setupSensor()
{
  twiSendCommand(0x77, 0x1e);
  delay(100);
  for (byte i = 1; i <=6; i++)
  {
    unsigned int low, high;
    twiSendCommand(0x77, 0xa0 + i * 2);
    Wire.requestFrom(0x77, 2);
    if(Wire.available()!=2) Serial.println("Error: calibration data not available");
    high = Wire.read();
    low = Wire.read();
    calibrationData[i] = high<<8 | low;
    Serial.print("calibration data #");
    Serial.print(i);
    Serial.print(" = ");
    Serial.println( calibrationData[i] ); 
  }
}
//VARIO DIY CRÉDITO
void twiSendCommand(byte address, byte command)
{
  Wire.beginTransmission(address);
  if (!Wire.write(command)) Serial.println("Error: write()");
  if (Wire.endTransmission()) 
  {
    Serial.print("Error when sending command: ");
    Serial.println(command, HEX);
  }
}

//Detector de massa termal
void comparavario() {
static unsigned long delayPisca;
long diferenca;
/*
if ( (millis() - delayPisca) < 1 ) {
      digitalWrite(led,1);
  }
  
else {
      digitalWrite(led,0);
    }
*/
if ( (millis() - delayPisca) >= 1 ) {
    delayPisca = millis(); 
    diferenca=pressaoAnterior-pressao;
        // Serial.print(diferenca);
        // Serial.println(' ');
        // Serial.println(pressao);
        // Serial.println(' ');
        // Serial.println(hpa);
        if(diferenca > 2 or diferenca < -2 ){
            pressaoAnterior=pressao;
          Serial.print(diferenca);
          Serial.print(' ');
          Serial.print(pressao);
          Serial.print(' ');
        
if (diferenca > 0) { valor=valor+diferenca+1; }
if (diferenca < 0) { valor=valor-2; }
if (valor < 0) { valor=0; }
        }

Serial.println(valor);

  }
}
