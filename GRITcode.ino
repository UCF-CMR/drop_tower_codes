//GRIT code
//v1.0.0
//Wesley Chambers
//20 July 2018
//
//v1.1.0
//29 September 2025
//Modification by Brandon Doyle:
//  - Falling event now triggers on falling edge of contact_pin_sig, not
//    simply logical LOW. This is to avoid immediately firing the solenoid if
//    someone should accidentally flip the arm switch before mounting GRIT.
//  - 4 minute cooldown replaced with infinite while loop. Is there a reason
//    we were letting GRIT reset itself? Maybe there is and I just haven't
//    thought of it! But if not, let's not let it do that, IMO.
//
//v1.1.1
//16 February 2026
//Brandon Doyle
//Bug fix:
//  - Parentheses corrected in FE and RE macros
//Modifications:
//  - Introduced version numbers.
//      Convention: v[GRIT version].[campaign].[software version]
//      -> GRIT version meaning the experiment version. We have a GRIT 2 planned.
//         We will save v2.0.0 for the initial software for that experiment.
//      -> Increment campaign # as sets of projects come and go. v1.1.x will
//         be Kayla Schang's big push in 2025 and 2026. I have no interest in playing
//         historian and figuring out what number we should really be on by now xD
//      Retroactively name:
//      -> v1.0.0: Wesley's version
//      -> v1.1.0: my September 29 version
//      -> v1.1.1: this version, Feb 2026
//  - Serial.print() the software name and version number in setup()
//  - Added visible (LED) end-state behavior. If you see this pattern, you know
//    that the Arduino believes the solenoid fired
//
//v1.1.2
//17 February 2026
//Brandon Doyle
//Modification:
//  - Edge detection: Added debouncing
//
//v1.1.3
//20 February 2026
//Brandon Doyle
//Modification:
//  - Spacing changed so I stop getting confused
//  - Make contact LED blink when armed and unconnected

const char softwareVersion[] = "v1.1.2";

//Macros for detecing signal rising/falling edges.
//2 bytes of debouncing: RE fires on 00001111, FE fires on 11110000
//#define RE(signal, state) ((state=(state<<1)|(signal&1)))==0x0F //Rising edge. Unused. Uncomment if needed.
#define FE(signal, state) ((state=(state<<1)|(signal&1)))==0xF0   //Falling edge.

//Pin assignments

int relay_pin_out = 7;

int armed_pin_sig = 2;

int contact_pin_vlt = 11;
int contact_pin_sig = 10;
int contact_pin_gnd = 9;

int armed_pin_LED = 4;
int contact_pin_LED = 3;

// Brandon moved these out of the loop:
bool ARMED_LOGIC   = false;
byte CONTACT_LOGIC = false;

//Pattern plays on the LEDs so you know the Arduino
//believes the solenoid fired:
void end_state_LED_pattern(){
  digitalWrite(contact_pin_LED,  LOW);
  digitalWrite(armed_pin_LED,   HIGH);
  delay(100);
  digitalWrite(contact_pin_LED, HIGH);
  digitalWrite(armed_pin_LED,    LOW);
  delay(100);
  digitalWrite(contact_pin_LED,  LOW);
  digitalWrite(armed_pin_LED,   HIGH);
  delay(100);
  digitalWrite(contact_pin_LED, HIGH);
  digitalWrite(armed_pin_LED,    LOW);
  delay(100);
  digitalWrite(contact_pin_LED,  LOW);
  digitalWrite(armed_pin_LED,    LOW);
  delay(500);
}

void setup() {
  // put your setup code here, to run once:
  
  pinMode(armed_pin_sig, INPUT);

  pinMode(relay_pin_out, OUTPUT);

  pinMode(contact_pin_sig, INPUT);
  pinMode(contact_pin_vlt, OUTPUT);
  pinMode(contact_pin_gnd, OUTPUT); 
  
  pinMode(armed_pin_LED, OUTPUT);
  pinMode(contact_pin_LED, OUTPUT);

  //Initialize pin states  
  
  digitalWrite(relay_pin_out, LOW);
  
  digitalWrite(contact_pin_vlt, HIGH);
  digitalWrite(contact_pin_gnd, LOW);

  digitalWrite(armed_pin_LED, LOW);
  digitalWrite(contact_pin_LED, HIGH);

  Serial.begin(9600);
  Serial.print(F("GRIT code "));
  Serial.println(softwareVersion);
  Serial.println(F(" "));
}

void loop() {
  // Read armed and contact logic
  ARMED_LOGIC   = digitalRead(armed_pin_sig);
  CONTACT_LOGIC = digitalRead(contact_pin_sig);

  // Show Logic by print and LED
  Serial.print("Armed Logic:");
  Serial.print(ARMED_LOGIC);
  Serial.print("\t Contact Logic:");
  Serial.println(CONTACT_LOGIC);

  //Contact Light indication
  if(CONTACT_LOGIC==true){
    digitalWrite(contact_pin_LED, HIGH);
  }
  if(CONTACT_LOGIC==false){
    digitalWrite(contact_pin_LED,  LOW);    
    delay(25);
    digitalWrite(contact_pin_LED,  HIGH);
    delay(25);
  }
  if(ARMED_LOGIC==false){
    digitalWrite(armed_pin_LED,  LOW);
  }


  while(ARMED_LOGIC==true){
    
    // Read armed and contact logic
    ARMED_LOGIC   = digitalRead(armed_pin_sig);
    //CONTACT_LOGIC = digitalRead(contact_pin_sig);  //Now handled in Falling Edge detection -Brandon

    //For debugging falling edge detection: 
    // for(int i=8;i--;i>=0){
    //   Serial.print(bitRead(CONTACT_LOGIC,i));
    // }
    // Serial.print("   ");
    // Serial.println(CONTACT_LOGIC==0xF0);

    digitalWrite(armed_pin_LED,  HIGH);
    // Falling event: triggers on falling edge (hehe) of signal from contact_pin_sig
    if( FE(digitalRead(contact_pin_sig),CONTACT_LOGIC) ){ // FE() stores current state and checks for falling edge
      //Activating Launcher
      delay(50);
      digitalWrite(relay_pin_out,   HIGH);

      //For debugging falling edge detection:
      // for(int i=8;i--;i>=0){
      //   Serial.print(bitRead(CONTACT_LOGIC,i));
      // }
      // Serial.print("   ");
      // Serial.println(CONTACT_LOGIC==0xF0);
      
      Serial.print("### Firing Solenoid ### \n");
      //Pulse length
      delay(250);
      digitalWrite(relay_pin_out, LOW);

      //delay(240000);   // 4 min cooldown (enough time to turn off the arm or power switch)
      while(true){       // infinity min cooldown.
        end_state_LED_pattern(); //Add specific end state patern
      }
    }
  } 
  
}
