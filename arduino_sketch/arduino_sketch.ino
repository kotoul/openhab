const int DELAY = 200;
const int SECOND = 1000/DELAY; // pocet iteraci do sekundy
int clock = 0; // pocita sekundy

const int odparMax = 10;
int odpar = 0;

boolean kvasnice = false;

const char delim = '_';

char inString[3];

typedef struct Value {
  char name[4];
  int work; // hodnota pri zapnuti bude bud klesat (zaporna hodnota) nebo stoupat (kladna)
  int value;
  int counter; // pocita sekundy
  int on; // za kolik sekund se zvedne hodnota o work
  int off; // za kolik sekund se snizi hodnota o work
  int maxVal;
} Value;

typedef struct Switch {
  char name[4];
  boolean value;
} Switch;

//water
Value water_o =          { "---",  1, 499, 0, 1,  1,  999 };
Value water_t =          { "---",  1, 15, 0, 1,  1,  100 };

//outs
Value RMVP_teplota =     { "rmt",  1, 0, 0, 1,  30, 200 };
Value RMVP_objem =       { "rmo",  1, 0, 0, 1,  1,  100 };
Value SK_teplota =       { "skt",  1, 0, 0, 1,  30, 200 };
Value SK_objem =         { "sko",  1, 0, 0, 1,  1,  100 };
Value Chladic_teplota =  { "cht", -1, 0, 0, 2,  30, 40 };
Value Spilka_teplota =   { "spt", -1, 0, 0, 4,  50, 40 };
Value Spilka_objem =     { "spo",  1, 0, 0, 1,  1,  100 };
Value Spilka_sachmetr =  { "sps",  1, 0, 0, 10, 10, 100 };
Value Sklep_teplota =    { "slt", -1, 0, 0, 4,  50, 40 };
Value Sklep_objem =      { "slo",  1, 0, 0, 1,  1,  100 };

//ins
Switch RMVP_ohrev =       { "ro", false };
Switch RMVP_michani =     { "rm", false };
Switch RMVP_poklice =     { "rp", false };
Switch RMVP_slad =        { "rn", false };
Switch Chladic_zapnuto =  { "cc", false };
Switch Spilka_klima =     { "sc", false };
Switch Sklep_klima =      { "sl", false };
Switch Cerpadlo_varna =   { "cv", false };
Switch Cerpadlo_spilka =  { "cs", false };
Switch Ventil_voda_SK =   { "vs", false };
Switch Ventil_voda_RMVP = { "vr", false };
Switch Ventil_SKi =       { "si", false };
Switch Ventil_RMVPi =     { "ri", false };
Switch Ventil_SKo =       { "so", false };
Switch Ventil_RMVPo =     { "rv", false };
Switch Ventil_SKo_s =     { "ss", false };
Switch Ventil_RMVP_h =    { "rh", false };
Switch Ventil_mladina =   { "ml", false };
Switch Ventil_kvasnice =  { "kv", false };
Switch Ventil_spilka =    { "sp", false };

void doLiquid(Value& from, Value& to, Value& temp_from, Value& temp_to);
void doTemperature(Value& val, boolean on);
void sendMsg(char head[], int message);
boolean isSecond();

void setup() { 
  Serial.begin(57600);
}

void loop() {
  delay(DELAY);
  
  if (isSecond()) {
    // teploty
    doTemperature(RMVP_teplota, RMVP_ohrev.value);
    doTemperature(SK_teplota, false);
    doTemperature(Chladic_teplota, Chladic_zapnuto.value);
    doTemperature(Spilka_teplota, Spilka_klima.value);
    doTemperature(Sklep_teplota, Sklep_klima.value);
    
    // objemy - cerpani
    if (Ventil_voda_RMVP.value || RMVP_slad.value) { // slad funguje jako voda
      doLiquid(water_o, RMVP_objem, water_t, RMVP_teplota);
    }
    else if (Ventil_voda_SK.value) {
      doLiquid(water_o, SK_objem, water_t, SK_teplota);
    }
    else if (Ventil_SKi.value && (Ventil_RMVPo.value || Ventil_RMVP_h.value) && Cerpadlo_varna.value) {
      doLiquid(RMVP_objem, SK_objem, RMVP_teplota, SK_teplota);
    }
    else if ((Ventil_SKo.value || Ventil_SKo_s.value) && Ventil_RMVPi.value && Cerpadlo_varna.value) {
      doLiquid(SK_objem, RMVP_objem, SK_teplota, RMVP_teplota);
    }
    else if ((Ventil_RMVPo.value || Ventil_RMVP_h.value) && Ventil_mladina.value && Cerpadlo_varna.value) {
      doLiquid(RMVP_objem, Spilka_objem, water_t, water_t);
    }
    else if (Ventil_spilka.value && Cerpadlo_spilka.value) {
      doLiquid(Spilka_objem, Sklep_objem, water_t, water_t);
    }
    
    // odpar
    if (RMVP_teplota.value > 90 && RMVP_objem.value > 1 && RMVP_poklice.value) {
      odpar += 1;
      if (odpar >= odparMax) {
        odpar = 0;
        RMVP_objem.value -= 1;
        sendMsg(RMVP_objem.name, RMVP_objem.value);
      }
    }
    
    // sacharometr - principielne funguje jako doLiquid
    if (Spilka_objem.value > 1 && kvasnice) {
      doLiquid(water_o, Spilka_sachmetr, water_t, water_t);
    }
    else {
      int prev = Spilka_sachmetr.value;
      Spilka_sachmetr.value = 0;
      if (prev != Spilka_sachmetr.value) {
        kvasnice = false;
        sendMsg(Spilka_sachmetr.name, Spilka_sachmetr.value);
      }
    }
    
    // kvasnice
    if (Ventil_kvasnice.value) {
      if (Spilka_objem.value + 1 <= Spilka_objem.maxVal) {
        Spilka_objem.value += 1;
        kvasnice = true;
        Ventil_kvasnice.value = false;
        sendMsg(Spilka_objem.name, Spilka_objem.value);
      }
    };
    
    water_o.value = 499; // neomezeny zdroj vody
  }
  
  //input
  if (Serial.available() >= 3) {
    for(int i = 0; i < 3; i++) {
      inString[i] = Serial.read();
    }
    if ( strstr(inString, RMVP_ohrev.name) != NULL ) { 
      RMVP_ohrev.value = inString[2] == 'a';
    }
    else if ( strstr(inString, RMVP_michani.name) != NULL ) { 
      RMVP_michani.value = inString[2] == 'a';
    }
    else if ( strstr(inString, Chladic_zapnuto.name) != NULL ) { 
      Chladic_zapnuto.value = inString[2] == 'a';
    }
    else if ( strstr(inString, Spilka_klima.name) != NULL ) { 
      Spilka_klima.value = inString[2] == 'a';
    }
    else if ( strstr(inString, Sklep_klima.name) != NULL ) { 
      Sklep_klima.value = inString[2] == 'a';
    }
    else if ( strstr(inString, RMVP_poklice.name) != NULL ) { 
      RMVP_poklice.value = inString[2] == 'a';
    }
    else if ( strstr(inString, RMVP_slad.name) != NULL ) { 
      RMVP_slad.value = inString[2] == 'a';
    }
    else if ( strstr(inString, Cerpadlo_varna.name) != NULL ) { 
      Cerpadlo_varna.value = inString[2] == 'a';
    }
    else if ( strstr(inString, Cerpadlo_spilka.name) != NULL ) { 
      Cerpadlo_spilka.value = inString[2] == 'a';
    }
    else if ( strstr(inString, Ventil_voda_SK.name) != NULL ) { 
      Ventil_voda_SK.value = inString[2] == 'a';
    }
    else if ( strstr(inString, Ventil_voda_RMVP.name) != NULL ) { 
      Ventil_voda_RMVP.value = inString[2] == 'a';
    }
    else if ( strstr(inString, Ventil_SKi.name) != NULL ) { 
      Ventil_SKi.value = inString[2] == 'a';
    }
    else if ( strstr(inString, Ventil_RMVPi.name) != NULL ) { 
      Ventil_RMVPi.value = inString[2] == 'a';
    }
    else if ( strstr(inString, Ventil_SKo.name) != NULL ) { 
      Ventil_SKo.value = inString[2] == 'a';
    }
    else if ( strstr(inString, Ventil_RMVPo.name) != NULL ) { 
      Ventil_RMVPo.value = inString[2] == 'a';
    }
    else if ( strstr(inString, Ventil_SKo_s.name) != NULL ) { 
      Ventil_SKo_s.value = inString[2] == 'a';
    }
    else if ( strstr(inString, Ventil_RMVP_h.name) != NULL ) { 
      Ventil_RMVP_h.value = inString[2] == 'a';
    }
    else if ( strstr(inString, Ventil_mladina.name) != NULL ) { 
      Ventil_mladina.value = inString[2] == 'a';
    }
    else if ( strstr(inString, Ventil_kvasnice.name) != NULL ) { 
      Ventil_kvasnice.value = inString[2] == 'a';
    }
    else if ( strstr(inString, Ventil_spilka.name) != NULL ) { 
      Ventil_spilka.value = inString[2] == 'a';
    }
  }
}

void doLiquid(Value& from, Value& to, Value& temp_from, Value& temp_to) {
  from.counter += 1;
  Value pom = (from.work / from.on) > (to.work / to.on) ? to : from;
  if(from.counter >= pom.on) {
    from.counter = 0;
    if (from.value - pom.work >= 0 && to.value + pom.work <= to.maxVal) {
      if (temp_from.value != temp_to.value) {
        temp_to.value += temp_from.value > temp_to.value ? 1 : -1;
        sendMsg(temp_to.name, temp_to.value);
      }
      from.value -= pom.work;
      to.value += pom.work;
      sendMsg(to.name, to.value);
      sendMsg(from.name, from.value);
    }
  }
}

void doTemperature(Value& val, boolean on) {
  val.counter += 1;
  if(val.counter >= (on ? val.on : val.off)) {
    val.counter = 0;
    int add = on ? val.work : -val.work;
    int prev = val.value;
    val.value = min(val.maxVal, max(0, val.value + add));
    if (prev != val.value) {
      sendMsg(val.name, val.value);
    }
  }
}

void sendMsg(char head[], int message) {
  if ( strstr(head, "---") != NULL ) {
    return;
  }
  char m[40];
  sprintf(m, "%s%c%d", head, delim, message);
  Serial.print(m);
  delay(10);
}

boolean isSecond() {
  clock += 1;
  if (clock >= SECOND) {
    clock = 0;
    return true;
  }
  return false;
}

