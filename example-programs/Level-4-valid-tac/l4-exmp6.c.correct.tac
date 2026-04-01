**PROCEDURE: main
**BEGIN: Three Address Code Statements
a_ = 5
b_ = 1
temp0 = 3 > 2
temp1 = ! temp0
if(temp1) goto Label4
stemp0 = a_
goto Label5
Label4:
stemp0 = 4
Label5:
temp2 = stemp0 > 5
temp3 = ! temp2
if(temp3) goto Label1
c_ = 10
goto Label0
Label1:
temp4 = a_ == 5
temp5 = ! temp4
if(temp5) goto Label3
c_ = 2
goto Label2
Label3:
Label2:
Label0:
**END: Three Address Code Statements
