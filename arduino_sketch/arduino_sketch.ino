//outs
const char RMVP_teplota[] = "rmt";
const char RMVP_objem[] = "rmo";
const char SK_teplota[] = "skt";
const char SK_objem[] = "sko";
const char Chladic_teplota[] = "cht";
const char Spilka_teplota[] = "spt";
const char Spilka_sachmetr[] = "sps";
const char Sklep_teplota[] = "slt";

//ins
const char RMVP_ohrev[] = "ro";
const char RMVP_michani[] = "rm";
const char RMVP_poklice[] =  "rp";
const char RMVP_slad[] = "rn";
const char Cerpadlo_varna[] = "cv";
const char Cerpadlo_spilka[] = "cs";
const char Ventil_voda_SK[] = "vs";
const char Ventil_voda_RMVP[] = "vr";
const char Ventil_SKi[] = "si";
const char Ventil_RMVPi[] = "ri";
const char Ventil_SKo[] = "so";
const char Ventil_RMVPo[] = "rv";
const char Ventil_SKo_s[] = "ss";
const char Ventil_RMVP_h[] = "rh";
const char Ventil_mladina[] = "ml";
const char Ventil_kvasnice[] = "kv";
const char Ventil_spilka[] = "sp";

const char delim = '_';

char inString[3];

char* msg(const char head[], char delim, int message);

void setup() { 
  Serial.begin(57600);
}

void loop() {
  if (Serial.available() >= 3) {
    for(int i = 0; i < 3; i++) {
      inString[i] = Serial.read();
    }
    if ( strstr(inString, RMVP_ohrev) != NULL ) { 
      Serial.print(msg(RMVP_teplota, 1 + (int)inString[2]));
    }
    else if ( strstr(inString, RMVP_michani) != NULL ) { 
      Serial.print(msg(RMVP_teplota, 2 + (int)inString[2]));
    }
    else if ( strstr(inString, RMVP_poklice) != NULL ) { 
      Serial.print(msg(RMVP_teplota, 3 + (int)inString[2]));
    }
    else if ( strstr(inString, RMVP_slad) != NULL ) { 
      Serial.print(msg(RMVP_objem, 1 + (int)inString[2]));
    }
    else if ( strstr(inString, Cerpadlo_varna) != NULL ) { 
      Serial.print(msg(RMVP_objem, 2 + (int)inString[2]));
    }
    else if ( strstr(inString, Cerpadlo_spilka) != NULL ) { 
      Serial.print(msg(SK_teplota, 1 + (int)inString[2]));
    }
    else if ( strstr(inString, Ventil_voda_SK) != NULL ) { 
      Serial.print(msg(SK_teplota, 2 + (int)inString[2]));
    }
    else if ( strstr(inString, Ventil_voda_RMVP) != NULL ) { 
      Serial.print(msg(SK_objem, 1 + (int)inString[2]));
    }
    else if ( strstr(inString, Ventil_SKi) != NULL ) { 
      Serial.print(msg(SK_objem, 2 + (int)inString[2]));
    }
    else if ( strstr(inString, Ventil_RMVPi) != NULL ) { 
      Serial.print(msg(Chladic_teplota, 1 + (int)inString[2]));
    }
    else if ( strstr(inString, Ventil_SKo) != NULL ) { 
      Serial.print(msg(Chladic_teplota, 2 + (int)inString[2]));
    }
    else if ( strstr(inString, Ventil_RMVPo) != NULL ) { 
      Serial.print(msg(Spilka_teplota, 1 + (int)inString[2]));
    }
    else if ( strstr(inString, Ventil_SKo_s) != NULL ) { 
      Serial.print(msg(Spilka_teplota, 2 + (int)inString[2]));
    }
    else if ( strstr(inString, Ventil_RMVP_h) != NULL ) { 
      Serial.print(msg(Spilka_sachmetr, 1 + (int)inString[2]));
    }
    else if ( strstr(inString, Ventil_mladina) != NULL ) { 
      Serial.print(msg(Spilka_sachmetr, 2 + (int)inString[2]));
    }
    else if ( strstr(inString, Ventil_kvasnice) != NULL ) { 
      Serial.print(msg(Sklep_teplota, 1 + (int)inString[2]));
    }
    else if ( strstr(inString, Ventil_spilka) != NULL ) { 
      Serial.print(msg(Sklep_teplota, 2 + (int)inString[2]));
    }
  }
  delay(50);
}

char* msg(const char head[], int message) {
  char m[40];
  sprintf(m, "%s%c%d", head, delim, message);
  return m;
}
