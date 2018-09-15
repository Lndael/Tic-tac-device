#include "TM1637.h"
#include <AmperkaKB.h>

// создаём объект для работы с матричной клавиатурой
// указывая номера arduino подключенные к шлейфу клавиатуры
// начиная с первого вывода шлейфа
AmperkaKB KB(4, 0, 2, 14, 12, 13, 3);
int masterFlag;
char inputChar;
int disarmPass;
int disarmTry;
int last_time;
boolean initial, initial2, pass_ent_f, time_ent_f, stopw_m_f, def_f,
    button_flag;
int minutes, seconds, milseconds;
#define masterPass 5425 // пароль инженерного меню
#define CLK 16
#define DIO 5
#define zipPin D8
TM1637 disp(CLK, DIO);
int timeShift;
int stop_m_f;
bool start_f;

void setup()
{
  // открываем монитор Serial порта
  Serial.begin(115200);
  // указываем тип клавиатуры
  KB.begin(KB4x3);
  // второй необзятельный параметр:
  // время длительного зажатия кнопки
  // по умолчанию 2000, изменим на 5000 мс
  // KB.begin(KB4x3, 5000);
  disp.init();
  disp.set(7);
  pinMode(zipPin, OUTPUT);
  start_f = 1;
  masterFlag = 1;
}

void get_time()
{
  if (start_f == 1)
  {
    last_time = millis();
    start_f = 0;
  }
  Serial.println("timer");
  if (millis() - last_time >= 500)
  {
    last_time = millis();
    milseconds++;
    if (milseconds == 1)
    {
      disp.point(POINT_OFF);
      if (minutes == 0 && seconds < 10)
      {
        tone(zipPin, 4000, 100);
      }
    }
    else
    {
      disp.point(POINT_ON);
    }
    if (milseconds >= 2)
    {
      milseconds = 0;
      seconds--;
      tone(zipPin, 4000, 100);
      if (seconds < 0)
      {
        seconds = 59;
        minutes--;
      }
    }
  }
}

int enterCode() // возвращает веденный код с клавиатуры
{
  Serial.println("enter PASS");
  bool i = true;
  char keyPressed;
  int intKeyPressed;
  int inputCode = 0;
  while (i == true)
  {
    keyPressed = '0';
    yield();
    KB.read();
    if (KB.justPressed())
    {
      keyPressed = KB.getChar;
      switch (keyPressed)
      {
      case '*':
        delay(50);
        i = false;
        break;
      case '#':
        inputCode = 0;
        break;
      default:
        delay(50);
        intKeyPressed = keyPressed - '0';
        inputCode = inputCode * 10 + intKeyPressed;
        Serial.println(inputCode);
        disp.clearDisplay();
        disp.displayInt(inputCode);
      }
    }
  }
  return inputCode;
}

void time_ent()
{
  disp.point(POINT_ON); //включить двоеточие
  int time_str;
  time_str = enterCode();
  minutes = ((time_str - time_str % 100) /
             100); //разбиение введенного числа на минуты и секунды
  seconds = (time_str % 100);
  if (seconds > 59)
  {
    seconds = 59;
  }
  milseconds = 0;
  last_time = millis();
  disp.point(POINT_OFF);
}

void setDisarmPass() // устанавливает пароль на обезвреживание
{
  disp.displayByte(_d, _A, _P, _S);
  disarmPass = enterCode();
  disp.clearDisplay();
}

bool checkMasterPass() // проверка мастер-пароля
{
  disp.displayByte(_A, _d, _P, _S);
  Serial.println("Check master pass");
  if (enterCode() == masterPass)
  {
    disp.displayByte(_G, _0, _0, _d);
    delay(1000);
    disp.clearDisplay();
    return true;
  }
  else
  {
    while (true)
    {
      int8_t text[] = {_b, _A, _d, _empty, _A, _d,
                       _empty, _P, _A, _S, _S, _empty};
      disp.runningString(text, 12, 300);
      delay(500);
      yield();
    }
  }
}

void loop()
{
  if (masterFlag == 1)
  {
    masterFlag = 0;
    if (checkMasterPass() == 1)
    {
      setDisarmPass();
      time_ent();
    }
    else
    {
      while (true)
      {
        int8_t welcome_banner[] = {_b, _A, _d, _empty, _P, _A, _S, _S, _empty};
        disp.runningString(welcome_banner, 9, 300);
        yield();
      }
    }
  }
  KB.read();
  if (KB.isHold())
  {
    if (KB.getChar == '#')
    {
      disp.displayByte(_C, _O, _d, _E);
      if (enterCode() == disarmPass)
      {
        while (true)
        {
          int8_t text[] = {_G, _r, _E, _A, _t, _empty};
          disp.runningString(text, 6, 300);
          delay(100);
          yield();
        }
      }
      disp.clearDisplay();
    }
  }
  get_time();
  disp.display(0, minutes / 10);
  disp.display(1, minutes % 10);
  disp.display(2, seconds / 10);
  disp.display(3, seconds % 10);
  if (minutes == 0 && seconds == 0)
  {
    disp.point(POINT_OFF);
    disp.displayByte(_b, _A, _N, _G);
    tone(zipPin, 50, 3000);
    disp.clearDisplay();
    while (true)
    {
      yield();
    }
  }
}