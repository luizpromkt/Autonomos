//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
// 1. SEPARAR O FILTRO DO RECEPTOR DO FILTRO DO ACELEROMETRO.
// 2. CRIAR O DEATH ZONE  CENTRALIZADO PARA DESABILITAR O MODULO AUTONOMO.
// 3. CHECAR QUAL ULTIMO ESTADO LATERAL DO ACELEROMETRO PARA ESCOLHA A DIREÇÃO DO CERVO PARA CORRIGIR O CURSO.
// 4. CRIAR AS 3 FUNÇÕES DE FLAPS E SE VIRAR COM OS CANAIS QUE O RÁDIO TEM.
// 5. RESOLVER O PROBLEMA DOS SERVOS ZERADO (PROVAVELMENTE MECANICA) NA INICIALIZAÇÃO DO ARDUINO.
// 6. HABILITAR FUNÇÃOI DE ALTITUDE MINIMA E MÁXIMA, COM ACIONAMENTO DE MOTOR
// 7. CRIAR A FEATURE DE CURVA PARA O LADO QUE O AILERON SUBIR E UM AUMENTO RELEVANTE DE SUSTENTAÇÃO.
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------



#include <Servo.h> //SERVO
#include <Wire.h> //VARIO



//SERVO
Servo AILERON_D;
Servo PROFUNDOR;
Servo AILERON_E;
int ch1; 
int ch2;
int ch5;
int ch6;



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

pressaoAnterior-pressao;

  //VARIO
  Wire.begin();
  Serial.begin(9600);
  setupSensor();
  pressure = getPressure();
  lowpassFast = lowpassSlow = pressure;



//SERVO
  AILERON_D.attach(5);
  PROFUNDOR.attach(4);
  AILERON_E.attach(3);
 AILERON_D.write(90);   
 PROFUNDOR.write(90);   
 AILERON_E.write(90); 



//RECEPTOR
  pinMode(9, INPUT); 
  pinMode(8, INPUT);
  pinMode(7, INPUT);
  pinMode(6, INPUT);
}


//LOOP PRINCIPAL
void loop()
{

 //RECEPTOR
 ch1 = pulseIn(9, HIGH, 400000); // AILERON D 
 ch2 = pulseIn(8, HIGH, 400000); // PROFUNDOR
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



//VARIO
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
 

//FILTRO SERVO E RECEPTOR.
if (c>=5) {c=0;fcompleto=1;}
  filtroX[c]=valX;
  filtroY[c]=valY;
  filtroZ[c]=valZ;
  filtroch1[c]=ch1;
  filtroch2[c]=ch2;
  filtroch6[c]=ch6;
   c++;
   
   if (fcompleto==1){
        for (int i=0; i <= 5; i++){
            somaX=somaX + filtroX[i];
            somaY=somaY + filtroY[i];
            somaZ=somaZ + filtroZ[i];
            somach1=somach1 + filtroch1[i];
            somach2=somach2 + filtroch2[i];
        }
    somaX=somaX / 6;
    somaY=somaY / 6;
    somaZ=somaZ / 6;
    somach1=somach1 / 6;
    somach2=somach2 / 6 ;
}
    valX=somaX; 
    valY=somaY; 
    valZ=somaZ;
    ch1=somach1;
    ch2=somach2;


//FILTRO VARIOMETRO    
if (d>=19) {d=0;fcompletod=1;}
    filtrop[d]=pressao;
    d++;
   
   if (fcompleto==1){
        for (int i=0; i <= 19; i++){
            somap=somap + filtrop[i];
            }
        somap=somap / 20;
        }

    pressao=somap;

 comparavario();

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



//ACELEROMETRO/SERVOR
// FALTA Checar qual é o ultimo estado lateral para saber como voltar sem invester o servo  
if (valZ < 300) {
   //  Serial.print("Capotado");
   if (ch6 <= 1200) {
    ch1=0;
    ch5=0;
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
  } else if ( (valZ < 360) && (valX > 380)) {
   //  Serial.print("Tombado para Direita");
     ch1=180;
     ch5=180;
  } else {
         if (ch6 <= 1200) {
              //  Serial.print("Normal");
              ch1 = map(valX, 260, 400, -50, 230);
              ch2 = map(valY, 260, 400, -50, 230);
              ch5 = ch1; 
        } 

  }

//SERVO SAIDA
 AILERON_D.write(ch1);   
 PROFUNDOR.write(ch2);   
 AILERON_E.write(ch5);    
 delay(15); 
}


//VARIO
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
  //  Serial.println(P);
  hpa=P; //100;

 
 // Serial.print("hpa");
 // Serial.print(hpa);
 altimetro=100900-hpa;// * 30; //0.305;
 // Serial.print("Dhpa");
 //Serial.print(altimetro);
 altimetro=altimetro * 30;
 // Serial.print("Pés");
 // Serial.print(altimetro);
 //altimetro=altimetro / 1000;
 altimetro=altimetro / 0.305;
 //Serial.print(altimetro);
 altimetro=altimetro / 100;
 // Serial.println(altimetro);
 // Serial.println(hpa);  
  return P;
}



//VARIO
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



//VARIO
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
//VARIO
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

/*
//VARIO
void ledOn()
{digitalWrite(led,1);}
//VARIO
void ledOff()
{digitalWrite(led,0);}
*/

void comparavario() {
static unsigned long delayPisca;
long diferenca;

if ( (millis() - delayPisca) < 1 ) {
      digitalWrite(led,1);
  }
  
else {
      digitalWrite(led,0);
    }

if ( (millis() - delayPisca) >= 2 ) {
    delayPisca = millis(); 
    diferenca=pressaoAnterior-pressao;
        // Serial.print(diferenca);
        // Serial.println(' ');
        // Serial.println(pressao);
        // Serial.println(' ');
        // Serial.println(hpa);
        if(diferenca > 1 or diferenca < -1 ){
            pressaoAnterior=pressao;
        //  Serial.print(diferenca);
        //  Serial.print(' ');
        //  Serial.println(pressao);
        //  Serial.println(' ');
        }
  }
}
