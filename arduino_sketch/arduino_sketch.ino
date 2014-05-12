const int DELAY = 100;
const int SECOND = 1000/DELAY; // pocet iteraci do sekundy
int clock = 0; // pocita sekundy

const int odparMax = 10;
int odpar = 0;

boolean kvasnice1 = false;
boolean kvasnice2 = false;

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
Value water_t =          { "---",  1,  15, 0, 1,  1,  100 };
Value sachmetr_1_z =     { "---",  1, 499, 0, 1,  1,  999 };
Value sachmetr_2_z =     { "---",  1, 499, 0, 1,  1,  999 };

//outs
Value RMVP_teplota =     { "rmt",  1, 0, 0, 1,  10, 200 };
Value RMVP_objem =       { "rmo",  1, 0, 0, 1,  1,  100 };
Value SK_teplota =       { "skt",  1, 0, 0, 1,  10, 200 };
Value SK_objem =         { "sko",  1, 0, 0, 1,  1,  100 };
Value Chladic_teplota =  { "cht", -1, 5, 0, 2,  30, 40 };
Value Spilka_teplota =   { "spt", -1, 5, 0, 4,  50, 40 };
Value Spilka_n1_objem =  { "sp1",  1, 0, 0, 1,  1,  110 };
Value Spilka_n2_objem =  { "sp2",  1, 0, 0, 1,  1,  110 };
Value Spilka_sachmetr1 = { "sc1", -1, 30, 0, 5, 5,  35 };
Value Spilka_sachmetr2 = { "sc2", -1, 30, 0, 5, 5,  35 };
Value Sklep_teplota =    { "slt", -1, 5, 0, 4,  50, 40 };
Value Sklep_n1_objem =   { "sl1",  1, 0, 0, 1,  1,  110 };
Value Sklep_n2_objem =   { "sl2",  1, 0, 0, 1,  1,  110 };
Value Sklep_n3_objem =   { "sl3",  1, 0, 0, 1,  1,  110 };
Value Sklep_n4_objem =   { "sl4",  1, 0, 0, 1,  1,  110 };

//ins
#define numSwitches 34
Switch switches[numSwitches] = {
  { "ro", false }, //0 RMVP_ohrev
  { "rm", false }, //1 RMVP_michani
  { "rp", false }, //2 RMVP_poklice
  { "rn", false }, //3 RMVP_slad
  { "cc", false }, //4 Chladic_zapnuto
  { "sc", false }, //5 Spilka_klima
  { "sl", false }, //6 Sklep_klima
  { "cv", false }, //7 Cerpadlo_varna
  { "cs", false }, //8 Cerpadlo_spilka
  { "vs", false }, //9 Ventil_voda_SK
  { "vr", false }, //10 Ventil_voda_RMVP
  { "si", false }, //11 Ventil_SKi
  { "ri", false }, //12 Ventil_RMVPi
  { "so", false }, //13 Ventil_SKo
  { "rv", false }, //14 Ventil_RMVPo
  { "ss", false }, //15 Ventil_SKo_s
  { "rh", false }, //16 Ventil_RMVP_h
  { "ml", false }, //17 Ventil_mladina
  { "a1", false }, //18 Ventil_SPi_1
  { "a2", false }, //19 Ventil_SPi_2
  { "k1", false }, //20 Ventil_kvasnice1
  { "k2", false }, //21 Ventil_kvasnice2
  { "b1", false }, //22 Ventil_SPo_1
  { "b2", false }, //23 Ventil_SPo_2
  { "c1", false }, //24 Ventil_SKLEPi_1
  { "c2", false }, //25 Ventil_SKLEPi_2
  { "c3", false }, //26 Ventil_SKLEPi_3
  { "c4", false }, //27 Ventil_SKLEPi_4
  { "d1", false }, //28 Ventil_SKLEPo_1
  { "d2", false }, //29 Ventil_SKLEPo_2
  { "d3", false }, //30 Ventil_SKLEPo_3
  { "d4", false }, //31 Ventil_SKLEPo_4
  { "xr", false }, //32 Ventil_RMVP_OUT
  { "xs", false }  //33 Ventil_SK_OUT
};

void doKvasnice(Switch& sw, boolean& kvasnice, Value& nadrz);
void doSachmetr(int obj, boolean& kvasnice, Value& sachmetr, Value& zdroj);
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
    doTemperature(RMVP_teplota, switches[0].value);
    doTemperature(SK_teplota, false);
    doTemperature(Chladic_teplota, switches[4].value);
    doTemperature(Spilka_teplota, switches[5].value);
    doTemperature(Sklep_teplota, switches[6].value);
    
    // objemy - cerpani
    if (switches[10].value || switches[3].value) { // slad funguje jako voda
      doLiquid(water_o, RMVP_objem, water_t, RMVP_teplota);
    }
    else if (switches[9].value) {
      doLiquid(water_o, SK_objem, water_t, SK_teplota);
    }
    
    if (switches[11].value && (switches[14].value || switches[16].value) && switches[7].value) {
      doLiquid(RMVP_objem, SK_objem, RMVP_teplota, SK_teplota);
    }
    else if ((switches[13].value || switches[15].value) && switches[12].value && switches[7].value) {
      doLiquid(SK_objem, RMVP_objem, SK_teplota, RMVP_teplota);
    }
    else if ((switches[14].value || switches[16].value) && switches[17].value && switches[18].value && switches[7].value) {
      doLiquid(RMVP_objem, Spilka_n1_objem, water_t, water_t);
    }
    else if ((switches[14].value || switches[16].value) && switches[17].value && switches[19].value && switches[7].value) {
      doLiquid(RMVP_objem, Spilka_n2_objem, water_t, water_t);
    }
    else if (switches[32].value) {
      doLiquid(RMVP_objem, water_o, water_t, water_t);
    }
    else if (switches[33].value) {
      doLiquid(SK_objem, water_o, water_t, water_t);
    }
    
    if (switches[22].value && switches[24].value && switches[8].value) {
      doLiquid(Spilka_n1_objem, Sklep_n1_objem, water_t, water_t);
    }
    else if (switches[22].value && switches[25].value && switches[8].value) {
      doLiquid(Spilka_n1_objem, Sklep_n2_objem, water_t, water_t);
    }
    else if (switches[22].value && switches[26].value && switches[8].value) {
      doLiquid(Spilka_n1_objem, Sklep_n3_objem, water_t, water_t);
    }
    else if (switches[22].value && switches[27].value && switches[8].value) {
      doLiquid(Spilka_n1_objem, Sklep_n4_objem, water_t, water_t);
    }
    else if (switches[23].value && switches[24].value && switches[8].value) {
      doLiquid(Spilka_n2_objem, Sklep_n1_objem, water_t, water_t);
    }
    else if (switches[23].value && switches[25].value && switches[8].value) {
      doLiquid(Spilka_n2_objem, Sklep_n2_objem, water_t, water_t);
    }
    else if (switches[23].value && switches[26].value && switches[8].value) {
      doLiquid(Spilka_n2_objem, Sklep_n3_objem, water_t, water_t);
    }
    else if (switches[23].value && switches[27].value && switches[8].value) {
      doLiquid(Spilka_n2_objem, Sklep_n4_objem, water_t, water_t);
    }
    
    if (switches[28].value) {
      doLiquid(Sklep_n1_objem, water_o, water_t, water_t);
    }
    else if (switches[29].value) {
      doLiquid(Sklep_n2_objem, water_o, water_t, water_t);
    }
    else if (switches[30].value) {
      doLiquid(Sklep_n3_objem, water_o, water_t, water_t);
    }
    else if (switches[31].value) {
      doLiquid(Sklep_n4_objem, water_o, water_t, water_t);
    }
    
    // odpar
    if (RMVP_teplota.value > 90 && RMVP_objem.value > 1 && switches[2].value) {
      odpar += 1;
      if (odpar >= odparMax) {
        odpar = 0;
        RMVP_objem.value -= 1;
        sendMsg(RMVP_objem.name, RMVP_objem.value);
      }
    }
    
    // sacharometr - principielne funguje jako doLiquid
    doSachmetr(Spilka_n1_objem.value, kvasnice1, Spilka_sachmetr1, sachmetr_1_z);
    doSachmetr(Spilka_n2_objem.value, kvasnice2, Spilka_sachmetr2, sachmetr_2_z);
        
    // kvasnice
    doKvasnice(switches[20], kvasnice1, Spilka_n1_objem);
    doKvasnice(switches[21], kvasnice2, Spilka_n2_objem);
    
    water_o.value = 499; // neomezeny zdroj vody
    sachmetr_1_z.value = 499;
    sachmetr_2_z.value = 499;
  }
  
  //input
  if (Serial.available() >= 3) {
    for(int i = 0; i < 3; i++) {
      inString[i] = Serial.read();
    }
    for(int i = 0; i < numSwitches; i++) {
      if ( strstr(inString, switches[i].name) != NULL ) { 
        switches[i].value = inString[2] == 'a';
      }
    }
  }
}


void doKvasnice(Switch& sw, boolean& kvasnice, Value& nadrz) {
  if (sw.value) {
      if (nadrz.value + 1 <= nadrz.maxVal) {
        nadrz.value += 1;
        kvasnice = true;
        sw.value = false;
        sendMsg(nadrz.name, nadrz.value);
      }
    }
}

void doSachmetr(int obj, boolean& kvasnice, Value& sachmetr, Value& zdroj) {
  if (obj > 1 && kvasnice) {
      doLiquid(zdroj, sachmetr, water_t, water_t);
  }
  else {
    int prev = sachmetr.value;
    sachmetr.value = sachmetr.maxVal - 5;
    if (prev != sachmetr.value) {
      kvasnice = false;
      sendMsg(sachmetr.name, sachmetr.value);
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

