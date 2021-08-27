#include <ArduinoJson.h>

const int TAMANHO  = 200;
const int termisor = A0;
int tempValor 	  = 0;
int luxValor 	  = 1;
int ventValor 	  = 0;

int tempDesejada   = 0;
double Lout 		  = 0;
int velocidadeVento = 0;

// Parâmetros do termistor
const double beta   = 3600.0;
const double r0     = 10000.0;
const double t0     = 273.0 + 25.0;
const double rx     = r0 * exp(-beta / t0);

// Parâmetros do circuito
const double vcc    = 5.0;
const double R      = 10000.0;

// Constantes
const float pi = 3.14159265;     //Número de pi
int period = 5000;               //Tempo de medida(miliseconds)
int radius = 147;                //Raio do anemometro(mm)

// Variáveis Globais
volatile unsigned int counter = 0;        //Contador para o sensor  
unsigned int RPM = 0;   

void setup() {
  Serial.begin(115200);
  //O valor padrão de 1000ms é muito tempo
  Serial.setTimeout(100);
  pinMode(termisor, INPUT);
  pinMode(A3, INPUT);
  pinMode(2, INPUT);
}

void loop() {

 // FUNÇÕES
	temperatura();
	lux();
  	windvelocity();
  
  // PROGRAMA
  if (Serial.available() > 0) {
    //Lê o JSON disponível na porta serial:
    StaticJsonDocument<TAMANHO> json;
    deserializeJson(json, Serial);

    if (json.containsKey("temperatura")) {
      tempValor = json["temperatura"];
    }

    if (json.containsKey("iluminacao")) {
      luxValor = json["iluminacao"];
    }

	if (json.containsKey("vento")) {
      ventValor = json["vento"];
    }
  }

  StaticJsonDocument<TAMANHO> json; //cria o objeto Json
  //formato de leitura no node-red
  json["topic"] = "SharkWatch";
  json["temp"] = tempDesejada;
  json["lux"] = Lout;
  json["vent"] = velocidadeVento;
  serializeJson(json, Serial);
  Serial.println();
  delay(300);
}

// CALCULOS - TEMPERATURA
void temperatura()
{
 // Determina a resistência do termistor
  double v = (vcc * analogRead(termisor)) / (1024.0);
  double rt = (vcc * R) / v - R;

  // Calcula a temperatura
  double t = beta / log(rt / rx);
  tempDesejada = (round(t - 273.0));
}

// CALCULOS - LUMINOSIDADE
void lux()
{
 // Variaveis luminosidade
  double vout = (vcc * analogRead(A2)) / (1023);
  double Rldr = (10000 / ((vcc / vout) - 1));

  double Rdark = 127410;
  double alpha = 0.858;

  double Bruh = (Rdark / Rldr);
  double potencia = 1 / alpha;
  Lout = pow(Bruh, potencia);
}
