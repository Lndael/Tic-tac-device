#include <AmperkaKB.h>
#include <cstring>
// создаём объект для работы с матричной клавиатурой
// указывая номера arduino подключенные к шлейфу клавиатуры
// начиная с первого вывода шлейфа
AmperkaKB KB(16, 5, 4, 0, 14, 12, 13);
bool masterFlag = 1;
int ledPin = 15; // адрес светодиода
char inputChar;
int disarmPass;
int disarmTry;
#define masterPass 5425 // пароль инженерного меню

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
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

int enterCode() // возвращает веденный код с клавиатуры
{
  Serial.println("Enter code");
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
        i = false;
        break;
      case '#':
        code = 0;
      default:
        intKeyPressed = keyPressed - '0';
        code = code * 10 + intKeyPressed;
      }
    }
  }
  return code;
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
    return true;
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
        digitalWrite(ledPin, HIGH);
        delay(1000);
        digitalWrite(ledPin, LOW);
        delay(1000);
        yield();
      }
    }
  }

  if (enterCode() == disarmPass)
  {
    {
      while (true)
      {
        digitalWrite(ledPin, HIGH);
        delay(100);
        digitalWrite(ledPin, LOW);
        delay(100);
        yield();
      }
    }
  }
  else
  {
    while (true)
    {
      digitalWrite(ledPin, HIGH);
      yield();
    }
  }
}