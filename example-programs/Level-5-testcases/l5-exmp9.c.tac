**PROCEDURE: main
**BEGIN: Three Address Code Statements
a_ = 2
D1234_ = 3
temp0 = a_ > 5
temp1 = ! temp0
if(temp1) goto Label2
stemp0 = 10
goto Label4
Label4:
return stemp0
goto Label1
Label2:
stemp1 = a_
goto Label5
Label5:
return stemp1
Label1:
**END: Three Address Code Statements
