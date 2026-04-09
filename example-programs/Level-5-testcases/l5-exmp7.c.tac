**PROCEDURE: f1_
**BEGIN: Three Address Code Statements
stemp0 = p_
goto Label1
Label1:
return stemp0
**END: Three Address Code Statements
**PROCEDURE: main
**BEGIN: Three Address Code Statements
temp0 = 10 > 5
x_ = temp0
temp2 = f1_(x_)
x_ = temp2
**END: Three Address Code Statements
