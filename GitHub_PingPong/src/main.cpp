#include <Arduino.h>
#include <LedControl.h>

#define DIN 12
#define CS 11
#define CLK 10

#define DataPin  2
#define LatchPin 3
#define ClockPin 4

#define TasterU 13
#define TasterO 9

LedControl Anzeige = LedControl(DIN, CLK, CS, 1);
  
void setup() {
  //Allgemeines
  pinMode(TasterU, INPUT_PULLUP);
  pinMode(TasterO, INPUT_PULLUP);
  Serial.begin(9600);

  //Matrix
  Anzeige.shutdown(0, false);
  Anzeige.setIntensity(0, 8);
  Anzeige.clearDisplay(0);

  //7seg
  pinMode(DataPin, OUTPUT);
  pinMode(LatchPin, OUTPUT);
  pinMode(ClockPin, OUTPUT);
  
  //Ausgabe Serieller Monitor
  Serial.begin(9600);
}

byte pixel[8][8] =
{
  //0  1  2  3  4  5  6  7                 //  erste Zahl => Reihe       zweite Zahl => Spalte 
  
  {0, 0, 0, 0, 0, 0, 0, 0},  //|0
  {0, 0, 0, 0, 0, 0, 0, 0},  //|1
  {0, 0, 0, 0, 0, 0, 0, 0},  //|2
  {0, 0, 0, 0, 0, 0, 0, 0},  //|3
  {0, 0, 0, 0, 0, 0, 0, 0},  //|4
  {0, 0, 0, 0, 0, 0, 0, 0},  //|5
  {0, 0, 0, 0, 0, 0, 0, 0},  //|6
  {0, 0, 0, 0, 0, 0, 0, 0},  //|7
};
  
void AusgabeMatrix()
{
  for(byte row = 0; row < 8; row++)
  {
    int mulfaktor = 1;
    int dezzahl = 0;
    for(byte i = 8; i > 0; i--)
    {
      dezzahl =  dezzahl + (pixel[row][(i-1)] * mulfaktor);
      mulfaktor = mulfaktor * 2;
    }
    Serial.println(dezzahl);
    Anzeige.setRow(0, row, dezzahl);
  
  }
}


class BorderPlayer
{
  
  private: byte currentPosition = 2;

    
  public: 
    void BorderUp()
    {
    
      if(currentPosition > 0)
      {
         currentPosition--;
      }
    
    }
    void BorderDown()
    {
      if(currentPosition < 5)
      {
        currentPosition++;
      }
    }

    byte getBorderPosition()
    {
      return currentPosition;
    }

  public:
   void setNewBorderOnMatrix()
   {
     //rechte Border
      pixel[currentPosition][7] = 1;
      pixel[currentPosition + 1][7] = 1;
      pixel[currentPosition + 2][7] = 1;

     //linke Border
     pixel[currentPosition][0] = 1;
     pixel[currentPosition + 1][0] = 1;
     pixel[currentPosition + 2][0] = 1;
   }
    
};
void PrintGameOver()
{
    Anzeige.setIntensity(0, 1);
        for (byte row = 0; row < 8; row++)
        {
          Anzeige.setRow(0, row, B11111111);
        }
        delay(700);
      
      Anzeige.setIntensity(0, 8);
}
void BlinkFullMatrix()
{
  Anzeige.setIntensity(0, 1);
      for (size_t i = 0; i < 3; i++)
        {
          for (byte row = 0; row < 8; row++)
        {
          Anzeige.setRow(0, row, B11111111);
        }
        delay(100);
        for (byte row = 0; row < 8; row++)
        {
          Anzeige.setRow(0, row, B00000000);
        }
        delay(150);
      }
      Anzeige.setIntensity(0, 8);
}


int gameovers = 0;
class Ball
{
  private:
  bool rechtsfliegend;
  char winkel;
  BorderPlayer currentborder;

  public:
  Ball()
  {
    rechtsfliegend = true;
    winkel = 'm';
    currentSpalte = 3;
    currentReihe = 3;
  }

  private:
  byte currentSpalte;
  byte currentReihe;

  public:
  void BallLogistic(BorderPlayer border)
  {
    currentborder = border;
    if (BallGegenBorder())
    {
      rechtsfliegend = !rechtsfliegend;
    }
   
    OberUnterGrenze();

    PixelReiheSetzen();
    PixelSpalteSetzen();

    setNewBallOnMatrix();

  }

private:
  bool BallGegenBorder()
  {
    if (currentSpalte == 1 || currentSpalte == 6)
    {
      if (pixel[currentReihe][0] == 1 || pixel[currentReihe][7] == 1)
      {    
        if(currentReihe == currentborder.getBorderPosition() + 2)
        winkel = 'r';
        if(currentReihe == currentborder.getBorderPosition() + 1)
        winkel = 'm';
        if(currentReihe == currentborder.getBorderPosition())
        winkel = 'h';  
        return true;
      }
      else
      {
        GameOver();
        return false;
      }
    }
    else
      return false;
  }

  void OberUnterGrenze()
  {
    if (currentReihe == 0)
    {
      winkel = 'r';
      
    }
    if(currentReihe == 7)
    {
      winkel = 'h';
      
    }
    
  }

  void PixelReiheSetzen()
    {

      if (winkel == 'h')
      {
        currentReihe--;
      }
      if(winkel == 'r')
      {
        currentReihe++;
      }
      
    }
    void PixelSpalteSetzen()
    {
      if(rechtsfliegend)
      currentSpalte++;
      else
      currentSpalte--;

    }

    void GameOver()
    {
      gameovers++;
      if (gameovers <= 9)
      {
        PrintGameOver();
        
      }
      else
      {
        BlinkFullMatrix();
        PrintGameOver();
        gameovers = 0;
      }
      currentSpalte = 3;
      currentReihe = 3;
      rechtsfliegend = true;
      winkel = 'm';
      setNewBallOnMatrix();
      AusgabeMatrix();
      delay(10);
    }
    


  public:
   void setNewBallOnMatrix()
   {
      pixel[currentReihe][currentSpalte] = 1;
   }

};






BorderPlayer border;
Ball ball;

void Tasterabfrage()
{
  //TasterO
  if(!(digitalRead(TasterO)))
  {
    Serial.println("            TasterO: High");
    border.BorderUp();
  }
  
  //TasterU
  if(!(digitalRead(TasterU)))
  {
    Serial.print("TasterU: High");
    border.BorderDown();
  }
  
}

void ClearMatrix()
{
  for(byte spalte = 0; spalte < 8; spalte++)
      {
        for(byte reihe = 0; reihe < 8; reihe++)
        {
          pixel[reihe][spalte] = 0;
        }
      }
}

int siebseg_Ziffern[10] =
{
  B00111111, //0
  B00000110, //1
  B01011011, //2
  B01001111, //3
  B01100110, //4
  B01101101, //5
  B01111101, //6
  B00000111, //7
  B01111111, //8
  B01101111  //9
};
void Ausgabe7Seg()
{
  digitalWrite(LatchPin, LOW);
  shiftOut(DataPin, ClockPin, LSBFIRST, siebseg_Ziffern[gameovers]);
  digitalWrite(LatchPin, HIGH);
}

int i = 0;
void loop() {
  ClearMatrix();
  Tasterabfrage();
  border.setNewBorderOnMatrix();

  if (i % 5 != 0)
  {
    ball.BallLogistic(border);
  }
  else
  {
    ball.setNewBallOnMatrix();
  }
  AusgabeMatrix();
  Ausgabe7Seg();

  i++;
  delay(110);
}