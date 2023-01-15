#include <stdlib.h>
#include <string.h>

int pot1 = A0;
int pot2 = A1;

int read1;
int read2;

char entry1[5];
char entry2[5];

char out_string[15];

int i;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);
}

void loop() {
  
  //while(!Serial.available());
  
  


  read1 = map(analogRead(pot1), 0, 1023, 0, 255);
  read2 = map(analogRead(pot2), 0, 1023, 0, 255);

  sprintf(entry1, "%d", read1);
  sprintf(entry2, "%d", read2);
  

  sprintf(out_string, "A%3s-B%3s", entry1, entry2);
  for(i = 1; i < 4; i++)
  {
    if((out_string[i] < '0') || (out_string[i] > '9'))
    {
      out_string[i] = '0';
    }
  }
  for(i = 6; i < 8; i++)
  {
    if((out_string[i] < '0') || (out_string[i] > '9'))
    {
      out_string[i] = '0';
    }
  }

  //out_string[11] == '\n';
  //out_string[12] == '\0';
  
  Serial.println(out_string);
  
  delay(5);

  /*
  read1 = map(analogRead(pot1), 0, 1023, 0, 255);
  sprintf(entry1, "%d", read1);

  Serial.println(read1);
  
*/
  
}
