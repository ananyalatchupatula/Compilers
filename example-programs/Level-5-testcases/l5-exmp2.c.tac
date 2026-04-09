**PROCEDURE: f1_
**BEGIN: Three Address Code Statements
temp0 = b_ + c_
d_ = temp0
temp1 = d_ + 2
stemp0 = temp1
goto Label1
Label1:
return stemp0
**END: Three Address Code Statements
**PROCEDURE: g_
**BEGIN: Three Address Code Statements
stemp0 = b_
goto Label3
Label3:
return stemp0
**END: Three Address Code Statements
**PROCEDURE: main
**BEGIN: Three Address Code Statements
a_ = 3
a_ = f1_(a_, a_)
write a_
**END: Three Address Code Statements
