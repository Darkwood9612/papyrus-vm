Scriptname IntTest

function Assert(Bool v) native

Int function Factorial(Int n)
  if n < 1
    return 1
  else
    return n * Factorial(n - 1)
  endif
endfunction

function Main()
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

  Assert(Factorial(1) == 1)
  Assert(Factorial(2) == 2)
  Assert(Factorial(3) == 6)
  Assert(Factorial(4) == 24)
  Assert(Factorial(5) == 120)
  Assert(Factorial(6) == 720)

  Int i
  Assert(i == 0)

  Int j = 123
  Assert(j == 123)

endfunction
