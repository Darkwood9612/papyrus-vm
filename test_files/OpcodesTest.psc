 Scriptname OpcodesTest extends _TestObject

function Assert(Bool v) native global
function Print(String s) native global

; Variables For Integer Test
Int nullIntegerNumber
Int integerNumber = 381
Float floatNumber = 5.5

; Variables For Float Test
Float nuulFloatNumber

; Variables For String Test
String emptyString
String firstString = "ArNolD"
String secondString = "JeSiKa"
String resultStrcat = "ArNolDJeSiKa"

; Variables For If_And_While Test
String boolTrue = "Start if , condition true"
String boolFalse = "Start if , condition false"
Bool conditionDefaultFalse
Int chekValue
Int valueToAssign = 10

; Variables For Array Test
Int[] _array

_TestObject Property _TestObjectRef Auto
OpcodesTest Property OpcodeRef Auto

State FirstState

  Event OnBeginState()
    Print(" Start First State!")
    Assert(GetState() == "FirstState")
    GoToState("SecondState")
  EndEvent

  Event OnEndState()
    Print(" End First State!")
  EndEvent

EndState

State SecondState

  Event OnBeginState()
    Print(" Start Second State!")
    Assert(GetState() == "SecondState")
    GoToState("")
  EndEvent

  Event OnEndState()
    Print(" End Second State!")
  EndEvent

EndState

Event OnBeginState()
    Print(" Start Default State!")
    Assert(GetState() == "")
EndEvent

Event OnEndState()
  Print(" End Default State!")
EndEvent

Function Main()

IntTest()
FloatTest()
StringTest()
OperatorsTest()
ReturnTest()
AssignTest()
WhileTest()
ArrayTest()
IfTest()
CastTest()
CallParentTest()
PropertyTest()
FactorialTest()
StateTest()

EndFunction

Function StateTest()
Print(" Start State Test!")
Assert(GetState() == "")
GotoState("FirstState")

Print(" End State Test!")
EndFunction


Function FactorialTest()
Print(" Start Factorial Test!")

Assert(Factorial(1) == 1)
Assert(Factorial(2) == 2)
Assert(Factorial(3) == 6)
Assert(Factorial(4) == 24)
Assert(Factorial(5) == 120)
Assert(Factorial(6) == 720)

Print(" End Factorial Test!")
EndFunction

Function IntTest()
Print(" Start Int Test!")

Assert(nullIntegerNumber == 0)
Assert(integerNumber == 381)

Assert(floatNumber as int == 5)

Assert(1 + 2 == 3)
Assert(10 - 11 == -1)
Assert(5 * 5 == 25)
Assert(10 / 5 == 2)
Assert(10 / 4 == 2)
Assert(1 / 0 == 1)
Assert(0 / 0 == 1)
Assert(12 % 10 == 2)
Assert(5 % 0 == 0)
Assert(0 % 0 == 0)

Assert(1 + 1 + 1 == 3)
Assert(10 * 10 + 10 == 110)
Assert(5 * 5 / 10 - 1 == 1)
Assert((100 - 101) * (-50) == 50)
Assert(-(-100) == 100)

Print(" End Int Test!")
EndFunction

Function FloatTest()
Print(" Start Float Test!")

Assert(nuulFloatNumber == 0.0)

Assert(3.5 + 1.5 == 5.0)
Assert(11.5 - 6.5 == 5.0)
Assert(3.0 * 2.5 == 7.5)
Assert(27.5 / 5.0 == 5.5)

Assert(integerNumber as float == 381.0)

Print(" End Float Test!")
EndFunction

Function StringTest()
Print(" Start String Test!")

Assert(emptyString == "")

Assert("ABC" != "ABCD")
Assert("ABC" == "ABC")
Assert("ABC" == "abc")

Assert(strcat(firstString, secondString) == resultStrcat)

Print(" End String Test!")
EndFunction

Function OperatorsTest()
Print(" Start Operators{==, !=, >, >=, <, <=, !} Test!")

Assert(5 == 5)
Assert(!(6 == 5))
Assert(5.0 == 5.0)
Assert(!(6.0 == 5.0))

Assert(4 < 5)
Assert(!(6 < 5))
Assert(4.0 < 5.0)
Assert(!(6.0 < 5.0))

Assert(5 <= 5)
Assert(4 <= 5)
Assert(!(6 <= 5))
Assert(5.0 <= 5.0)
Assert(4.0 <= 5.0)
Assert(!(6.0 <= 5.0))

Assert(5 > 4)
Assert(!(6 > 7))
Assert(5.0 > 4.0)
Assert(!(5.0 > 6.0))

Assert(5 >= 5)
Assert(6 >= 5)
Assert(!(4 >= 5))
Assert(5.0 >= 5.0)
Assert(4.0 >= 3.0)
Assert(!(6.0 >= 7.0))

Print(" End Operators{==, !=, >, >=, <, <=, !} Test!")
EndFunction

Function ReturnTest()
Print(" Start Return Test!")

  Assert(5 == returnValue(5))

Print(" End Return Test!")
EndFunction

Function AssignTest()
Print(" Start Assign Test!")

Assert(chekValue == 0)
chekValue = valueToAssign
Assert(valueToAssign == chekValue)

Print(" End Assign Test!")
EndFunction

Function WhileTest()
Print(" Start While Test!")

Int whileStepCounter
Bool conditionForWhile = true

Assert(whileStepCounter == 0)
Print("Start While.....")
  While(conditionForWhile)
;Print("Start while in step: " + whileStepCounter as string)
    whileStepCounter = whileStepCounter + 1

    If(whileStepCounter >= 5)
      conditionForWhile = !conditionForWhile
    EndIf

  EndWhile
Print("End While.....")
Assert(whileStepCounter == 5)

Print(" End While Test!")
EndFunction

Function ArrayTest()
Print(" Start Array Test!")

_array = new Int[5]
Int len = _array.Length
Assert(len == 5)

Assert(_array[3] == 0)
_array[3] = 20
Assert(_array[3] == 20)

Int index = _array.Find(20, 2)
Assert(3 == index)

index = _array.Find(20, 4)
Assert(-1 == index)

index = _array.Find(20, 10)
Assert(-1 == index)

index = 0
index = _array.RFind(20 , -3)
Assert(-1 == index)

index = _array.RFind(20 , -7)
Assert(-1 == index)

index = _array.RFind(20)
Assert(3 == index)

index = _array.RFind(20, -1)
Assert(3 == index)

Assert(index)

Print(_array as String)

Print(" End Array Test!")
EndFunction

Function IfTest()
Print(" Start IF Test!")

Assert(conditionDefaultFalse == False)
Assert(ifAndElse(conditionDefaultFalse) == boolFalse)
Assert(ifAndElse(!conditionDefaultFalse) == boolTrue)

Bool conditionForIfTest

Assert(conditionForIfTest == false)

If(true && true)
  Assert(true)
Else
  Assert(false)
EndIf

If(false && true)
  Assert(false)
Else
  Assert(true)
EndIf

If(false && false)
  Assert(false)
Else
  Assert(true)
EndIf

If(true || true)
  Assert(true)
Else
  Assert(false)
EndIf

If(true || false)
  Assert(true)
Else
  Assert(false)
EndIf

If(false || false)
  Assert(false)
Else
  Assert(true)
EndIf

If(OpcodeRef)
  Assert(true)
Else
  Assert(false)
EndIf

Print(" End IF Test!")
EndFunction

Function CastTest()
Print(" Start Cast Test!")

Assert(5 == 5.5 as Int)
Assert(5 as Float == 5.0)

Assert("5" == 5 as String)
Assert("5.000000" == 5.0 as String)
Assert("True" == true as String)
Assert("False" == false as String)

Assert("5")
Assert(_array)
Assert(true)

Assert(False == !_array)
Assert(False == !"lol")
Assert(True == !emptyString)

Assert(False == !OpcodeRef)
Assert(True == OpcodeRef)
_TestObject strFormId = OpcodeRef as _TestObject
Print(" _TestObject id = " + strFormId)

Print(" End Cast Test!")
EndFunction

Function CallParentTest()


  Print(" Start CallParent Test!")

  Float value = 0.0

  Assert(value == 0.0)

  value = parent.ReturbBackValue(1.0)

  Assert(value == 1.0)

  Print(" End CallParent Test!")
EndFunction

Function PropertyTest()
  Print(" Start Property Test!")

  Int resultat = _TestObjectRef.parentProperty

  Assert(resultat == 0)

  _TestObjectRef.parentProperty = 15

  resultat = _TestObjectRef.parentProperty

   Assert(resultat == 15)

  Print(" End Property Test!")
EndFunction


Int Function returnValue(Int v)
  Return v
EndFunction


Int Function Factorial(Int n); функия для тестирования рекурсии !!!

  If n < 1
    Return 1
  Else
    Return n * Factorial(n - 1)
  EndIf

EndFunction

String Function strcat(String s1 , String s2 )
  Return s1 + s2
EndFunction

String Function ifAndElse(Bool condition)

    If(condition)
      Return boolTrue
    Else
      Return boolFalse
    Endif

EndFunction
