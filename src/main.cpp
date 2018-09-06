#include "AmperkaKB.h"
#include "TM1637.h"
// создаём объект для работы с матричной клавиатурой
// указывая номера arduino подключенные к шлейфу клавиатуры
// начиная с первого вывода шлейфа
AmperkaKB KB(D0, D1, D2, D3, D4, D5, D6);
bool masterFlag = 1;
char inputChar;
int disarmPass;
int disarmTry;
#define masterPass 5425 // пароль инженерного меню
#define CLK D7
#define DIO 3
TM1637 disp(CLK, DIO);

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
  disp.set(4);
}

int enterCode() // возвращает веденный код с клавиатуры
{
  Serial.println("enter PASS");
  disp.displayByte(_P, _A, _S, _S);
  boolean i = true;
  char keyPressed;
  int intKeyPressed;
  int code;
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
        disp.clearDisplay();
        disp.displayInt(code);
        delay(3000);
        i = false;
        return code;
        break;
      case '#':
        code = 0;
      default:
        intKeyPressed = keyPressed - '0';
        code = code * 10 + intKeyPressed;
      }
    }
  }
}

void setDisarmPass() // устанавливает пароль на обезвреживание
{
  disarmPass = enterCode();
  masterFlag = 0;
}

bool checkMasterPass() // проверка мастер-пароля
{
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
      int8_t text[] = {_b, _A, _d, _empty, _A, _d, _empty, _P, _A, _S, _S, _empty};
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
    if (checkMasterPass() == 1)
      setDisarmPass();
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

  if (enterCode() == disarmPass)
  {
    {
      while (true)
      {
        int8_t text[] = {_G, _r, _E, _A, _t, _empty};
        disp.runningString(text, 6, 300);
        delay(100);
        yield();
      }
    }
  }
  else
  {
    while (true)
    {
      disp.displayByte(_b, _A, _N, _G);
      yield();
    }
  }
}