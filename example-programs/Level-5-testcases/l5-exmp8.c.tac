**PROCEDURE: f1_
**BEGIN: Three Address Code Statements
stemp0 = s_
goto Label1
Label1:
return stemp0
**END: Three Address Code Statements
**PROCEDURE: main
**BEGIN: Three Address Code Statements
s1_ = "Hello"
temp1 = f1_(s1_)
s2_ = temp1
write s1_
**END: Three Address Code Statements
