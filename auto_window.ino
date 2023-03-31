

#include <Servo.h>

#include <LiquidCrystal.h>
#include <Time.h>
#include <IRremote.h>
#include <Arduino.h>
int remoP = A5;
IRrecv irrecv(remoP);  
decode_results results;




void downBli();
void sound();
void upBli();
void closeWin();
void openWin();
void onLight();
void offLight();
void customlcd();
void customed();
void uncustom();




Servo blind;
Servo window;
Servo light;



//motor pin number
int bliMoP = 9;
int winMoP = 8;
int ligMoP = 10;



//sensor pin number
int rainP = A2;

int outPhoP = A0;
int inPhoP = A1;
//buzzer pin number
int buzP = 6;

//lcd pin number
int en = 11;
int  rs = 12;
int d4 = 5;
int d5 = 4;
int d6 = 3;
int d7 = 7;

//save what to operate, blind, window, light, by remote control
int operateWhat = 0; 

//save what to custom, blind up or down
int bliCusUporDown = 0;

//criteria value to compare value of sensor
int highLight = 200;
int lowLight = 600;


//save now state of blind, window, light, open or close
int blindN = 0;
int windowN = 0;
int lightN = 0;

//save time
unsigned long saveTime;
unsigned long nowTime;

//Timer for continuous sensor value save
unsigned long lightSaveTime = -1;
unsigned long rainSaveTime = -1;
unsigned long buzSaveTime = -1;



//Store sensor values by time period and use to check the amount of change over time
int lightSave[3] = { 0 };//Save every 10 minutes or more than 10 minutes
int rainSave[3] = { 0 };//Save every 3 minutes or more than 10 minutes
int tmp;

//save whether user has operated by remote control
//if remote cotroled, value is 1
int operBli = 0;
int operWin = 0;
int operLig = 0;

//save time that user has operated window, blind, light by remote control
unsigned long motorTimeBli = 0;
unsigned long motorTimeWin = 0;
unsigned long motorTimeLig = 0;

int i = 0;
int soundH = 659;//for buzzer



LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int screen = 0;
void setup() {

    blind.attach(bliMoP);
    window.attach(winMoP);
    light.attach(ligMoP);

    pinMode(outPhoP, INPUT);
    pinMode(inPhoP, INPUT);    

    pinMode(rainP, INPUT);

    lcd.begin(16, 2);
    lcd.setCursor(0, 0);

    pinMode(buzP, OUTPUT);

    int screen = 0;

    irrecv.enableIRIn();
    Serial.begin(9600);
}

void loop() {

     //check light of outside and inside
    int inLight = analogRead(inPhoP);
    int outLight = analogRead(outPhoP);


    nowTime = millis() / 1000;

    //save value of light sensor at intervals of more than 10 minutes
    if ((millis() - lightSaveTime) / (1000 * 60 * 10) >= 1)
        // if((millis()-moterTimeWin)/10)==50)
    {

        //shift values
        for (int i = 1;i >= 0;i--) 
        {
            lightSave[i + 1] = lightSave[i];
        }
        lightSave[0] = analogRead(outPhoP);

        //save now time for checking next time interval
        lightSaveTime = millis();

    }

    //save value of rain sensor at intervals of more than 3 minutes
    if ((millis() - rainSaveTime) / (1000 * 60 * 3) >= 1)
    {

        //shift values
        for (int i = 1;i >= 0;i--)
        {
            rainSave[i + 1] = rainSave[i];
        }
        rainSave[0] = analogRead(rainP);

        //save now time for checking next time interval
        rainSaveTime = millis();


    }





    //for decode of ir remote
    if (irrecv.decode(&results))
    {
    }
    irrecv.resume();



    //when remote's button pressed
    if (irrecv.decode(&results))
    {
        //when remote's button 1, 2, 3 pressed, show main options
        if ((results.value == 0xFD08F7 /*button1*/ || results.value == 0xFD8877/*button2*/ || results.value == 0xFD48B7/*button3*/) && screen == 0) 
        {

            screen = 1;
            lcd.clear();
            lcd.print("1:custom 2:remote");

        }

        //custom the criteria value selected, show what to custom 
        else if (screen == 1 && results.value == 0xFD08F7/*button1*/)
        {
            screen = 4;
            lcd.clear();
            lcd.print("1:blind down");
            lcd.setCursor(0, 1);
            lcd.print("2:blind up");

        }

        //choosed to custom the criteria value to down the blind, just for save
        else if (screen == 4 && results.value == 0xFD08F7/*button1*/)
        {
            screen = 8;
            bliCusUporDown = 1;
            customlcd();
        }
        //choosed to custom the criteria the value to raise the blind, just for save
        else if (screen == 4 && results.value == 0xFD8877/*button2*/)
        {
            screen = 8;
            bliCusUporDown = 2;
            customlcd();
        }

        //choosed to custom criteria the value to down the blind to now light sensor value
        else if (screen == 8 && results.value == 0xFD08F7/*button1*/ && bliCusUporDown == 1)
        {
            
                highLight = analogRead(outPhoP);
                customed();
            
        }

        //choosed to custom criteria the value to up the blind to now light sensor value
        else if (screen == 8 && results.value == 0xFD08F7/*button1*/ && bliCusUporDown == 2)
        {
            
                lowLight = analogRead(outPhoP);
                customed();
                
            
        }

        //choosed not to custom the criteria value
        else if (screen == 8 && results.value == 0xFD8877/*button2*/  && bliCusUporDown==1 )
        {
            bliCusUporDown = 0;
            uncustom();
        }

        //choosed not to custom the criteria value
        else if (screen == 8 && results.value == 0xFD8877/*button2*/ && bliCusUporDown == 2)
        {
            bliCusUporDown = 0;
            uncustom();
        }


        //show the remote control by user page  
        else if (screen == 1 && results.value == 0xFD8877/*button2*/)
        {
            screen = 2;
            lcd.clear();
            lcd.print("1:blind 2:window 3:light");

        }

        //print that user choosed to directly control blind by remote
        else if (screen == 2 && results.value == 0xFD08F7/*button1*/)
        {
            lcd.clear();
            lcd.print("blind choosen");
            screen = 5;
            //to save blind choosen
            operateWhat = 1;//save the blind is choosen
            lcd.setCursor(0, 1);

        }

        //print that user choosed to directly control window by remote
        else if (screen == 2 && results.value == 0xFD8877/*button2*/)
        {
            lcd.clear();
            lcd.print("window choosen");
            screen = 5;
            //to save window choosen
            operateWhat = 2;
        }

        //print that user choosed to directly control light by remote
        else if (screen == 2 && results.value == 0xFD48B7)
        {
            lcd.clear();
            lcd.print("light chosen");
            screen = 5;

            //to save light choosen
            operateWhat = 3;
        }

        //choosed to control blind, window, light 
        else if (screen == 5 && (results.value == 0xFD08F7/*button1*/ || results.value == 0xFD8877/*button2*/  || results.value == 0xFD48B7/*button3*/))
        {
            //control blind
            if (operateWhat == 1)
            {
                //check blind raised or down
                if (blindN == 0) //blindN==0 -> blind raised now ->move blind to down
                {

                    operBli = 1;
                    downBli();
                    screen = 9;
                    noTone(buzP);
                }
                else        //move blind up
                {
                    operBli = 1;
                    screen = 9;
                    upBli();
                    noTone(buzP);
                }
            }

            //control window
            else if (operateWhat == 2)
            {
                //check window close or open
                if (windowN % 2 == 0) //windowN==0 -> window is open now
                {
                    operWin = 1;
                    closeWin();
                    screen = 9;
                    noTone(buzP);
                }
                else                //window is closed now
                {
                    operWin = 1;
                    openWin();
                    screen = 9;
                    noTone(buzP);
                }



            }

            //control light
            else if (operateWhat == 3)
            {
                if (lightN == 0)    //now light off
                {
                    operLig = 1;
                    onLight();
                    screen = 9;
                    noTone(buzP);

                }
                else if (lightN == 1) //now light on
                {
                    operLig = 1;
                    offLight();
                    screen = 9;
                    noTone(buzP);
                }
            }

            operateWhat = 0;
            screen = 9;
        }

        //reset all about screen, ready to go to first screen
        else if (screen == 9)   //go to main screen, remove things written on lcd 
        {
            lcd.clear();
            screen = 0;
            operateWhat = 0;
            bliCusUporDown = 0;
        }
    }
    else if (screen == 0)   //go to main screen, remove things written on lcd 
    {
        lcd.clear();
    }







//if not operating by user, Operates based on sensor value
//operBli==0 -> not operating by user
    if (operBli == 0)
    {

        //check sensor value and Operates based on sensor value
        //when too bright outdoor
        if ((lightSave[2] - lightSave[1] > 0 && lightSave[1] - lightSave[0] > 0) && (outLight < highLight))  
        {
            //check blind up or down
            if (blindN % 2 == 0) //when blind up
            {
                lcd.setCursor(0, 1);
                lcd.print("too bright");
                downBli();
                noTone(buzP);
            }
        }
        //when too dark outdoor or too dark indoor
        else if ((lightSave[2] - lightSave[1] < 0 && lightSave[1] - lightSave[0] < 0) && ((outLight > lowLight)|| (inLight>700)))
        {
            //check blind up or down
            if (blindN % 2 == 1)  //when blind down
            {
                lcd.setCursor(0, 1);
                lcd.print("too dark");
                upBli();
                noTone(buzP);
            }
        }

        //}
    }

    //else now operating by user
    //30 minutes after the user has operated the blind
    else if ((millis() - motorTimeBli) / (1000 * 60 * 30) == 1 && operBli == 1)
    {
        motorTimeBli = 0;
        operBli = 0;

    }




    //if not operating by user, Operates based on sensor value
    //operLig==0 -> not operating by user
    if (operLig == 0)
    {
        //check sensor value and Operates based on sensor value
        //when too dark outdoor
        if ((lightSave[2] - lightSave[1] < 0 && lightSave[1] - lightSave[0] < 0) && (outLight > lowLight))
        {
            if (lightN == 0)    //if light is off, turn on the light
            {
                lcd.setCursor(0, 1);
                lcd.print("too dark");
                onLight();
                noTone(buzP);
            }
        }
    }

    //else now operating by user
    //30 minutes after the user has operated the light
    else if ((millis() - motorTimeWin) / (1000 * 60 * 30) >= 1 && operLig == 1)
    {
        motorTimeLig = 0;
        operLig = 0;

    }


    //if not operating by user, Operates based on sensor value
    //operLig==0 -> not operating by user
    if (operWin == 0)
    {
        //when now rain
        if (analogRead(rainP) < 500)
        {
            //before more than 6minutes, not rain but now and 3minutes ago rains 
            if (rainSave[2] >500 && rainSave[1]<500)
            {
                //if window is open now, close window
                if (windowN == 0)
                {
                    lcd.setCursor(0, 1);
                    lcd.print("now rainy");
                    closeWin();

                    noTone(buzP);
                }
            }


        }

    }
    //else now operating by user
    //30 minutes after the user has operated the window
    else if ((millis() - motorTimeWin) / (1000 * 60 * 30) == 1 && operWin == 1)
    {
        motorTimeWin = 0;
        operWin = 0;

    }


}


//functions
// 
//  
//make sound during moving blind, window, light
void sound()
{
    tone(buzP, soundH);

}





//operating blind down
void downBli()
{
    tone(buzP, soundH);

    lcd.setCursor(0, 0);
    lcd.print("blind down");
    sound();
    motorTimeBli = millis();


    //blind down
    blind.write(180);
    delay(2000);
    blind.write(90);


    blindN = 1; 
    lcd.clear();


}


void upBli()
{
    tone(buzP, soundH);
    lcd.setCursor(0, 0);
    lcd.print("blind raise");
    sound();
    motorTimeBli = millis();

    //blind up
    blind.write(0);
    delay(2000);
    blind.write(90);


    blindN = 0;

    lcd.clear();


}

void closeWin()
{
    lcd.setCursor(0, 0);
    lcd.print("window close");
    sound();
    motorTimeWin = millis();


    //window close
    window.write(180);
    delay(2000);
    window.write(90);
    delay(100);

    windowN = 1;
    lcd.clear();
}

void openWin()
{
    lcd.setCursor(0, 0);
    lcd.print("window open");
    sound();
    motorTimeWin = millis();

    //window open
    window.write(0);
    delay(2000);
    window.write(90);
    delay(100);

    windowN = 0;
    lcd.clear();
}

void onLight()
{
    lcd.setCursor(0, 0);
    lcd.print("Light on");
    sound();
    i = 0;
    motorTimeLig = millis();


    while (i <= 30)
    {
        if (millis() - motorTimeLig / 10 >= 1)
        {
            light.write(i);
            i = i + 10;
            motorTimeLig = millis();
        }
    }

    lightN = 1;
    lcd.clear();
}

void offLight()
{
    lcd.setCursor(0, 0);
    lcd.print("Light off");
    sound();
    motorTimeLig = millis();
    i = 30;

    light.write(90);
    light.write(0);
    lightN = 0;
    while (i >= 0)
    {
        if (millis() - motorTimeLig / 10 >= 1)
        {
            light.write(i);
            i = i - 10;
            motorTimeLig = millis();
        }
    }
    lcd.clear();

}

void customlcd()
{
    lcd.clear();
    lcd.print("1:set 2:cancle");

}

void customed()
{
    lcd.clear();
    lcd.print("value is costomized");
    screen = 9;
    bliCusUporDown = 0;
}

void uncustom()
{
    lcd.clear();
    lcd.print("canceled, go to the main");
    bliCusUporDown = 0;
    screen = 9;
}
