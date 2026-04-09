**PROCEDURE: f1_
**BEGIN: Three Address Code Statements
temp0 = b_ + c_
d_ = temp0
h_()
temp1 = d_ + 2
stemp0 = temp1
goto Label1
Label1:
return stemp0
**END: Three Address Code Statements
**PROCEDURE: g_
**BEGIN: Three Address Code Statements
h_()
b_ = 1
temp1 = f1_(b_, b_)
b_ = temp1
stemp0 = b_
goto Label3
Label3:
return stemp0
**END: Three Address Code Statements
**PROCEDURE: h_
**BEGIN: Three Address Code Statements
c_ = 1
**END: Three Address Code Statements
**PROCEDURE: main
**BEGIN: Three Address Code Statements
a_ = 3
h_()
temp4 = f1_(a_, a_)
a_ = temp4
temp5 = g_()
b_ = temp5
h_()
temp6 = f1_(a_, a_)
a_ = temp6
temp7 = g_()
b_ = temp7
write a_
**END: Three Address Code Statements
