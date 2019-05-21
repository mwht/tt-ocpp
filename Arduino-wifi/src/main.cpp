#include <Bridge.h>
#include <HttpClient.h>
#include <Wire.h>
#include <Process.h>
#include "Vector.h"
#include "Things.h"

char x = 0;
char tagId = 0;
Process process;
Vector<char> queue;
volatile int licznik = 0;
volatile int flag = -1;

void sendString(String s)
{
  for(int i = 0; i < s.length(); i++)
  {
    queue.PushBack(s.charAt(i));
  }
}

void get(String thing)
{
  // Serial.println(thing);
  process.begin("curl");
  process.addParameter("-X");
  process.addParameter("POST");
  process.addParameter("-d");
  process.addParameter("{}");
  process.addParameter("-H");
  process.addParameter("Accept: application/json");
  process.addParameter("-H");
  process.addParameter("Content-Type: application/json");
  process.addParameter("-k");
  process.addParameter(thing);
  process.run();
  while(process.available() > 0)
  {
    char c = process.read();
    queue.PushBack(c);
    Serial.print(c);
  }
  queue.PushBack('\n');
  Serial.print("\n");
}

void receiveEvent(int bytes) {
  x = Wire.read();    // read one character from the I2C
  Serial.println((int)x);
}

void requestEvent(int bytes){
  if(!queue.Empty())
  {
    if(licznik > queue.Size())
    {
      licznik = 0;
      queue.Clear();
    }
    char c = queue[licznik];
    Wire.write(c);
    licznik++;
    return;
  }
  if(x == 0)
    return;
  if(x == 102)
  {
    flag = 255;
    x = 0;
    return;
  }
  if(flag == 255 && x != 0)
  {
    //Serial.println((int)x);
    tagId = x;
    x = 0;
    flag = 102;
    return;
  }
  if(x>0)
  {
    flag = x;
  }
  // if(x == '2')
  // {
  //   flag = 2;
  // }
  x = 0;
}

void setup() {
  Bridge.begin();
  Wire.begin(9);
  Serial.begin(9600);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void loop() {
  if(flag >0 && flag <= 100)
  {
    get((String)sendBattery + String(flag));
    flag = -1;
  }
  if(flag == 101)
  {
    get(getTagInfo);
    flag = -1;
  }
  if(flag == 102)
  {
    get((String)AuthorizePhone + String((int)tagId));
    flag = -1;
  }

  //delay(100);
}
