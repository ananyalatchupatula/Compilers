**PROCEDURE: main
**BEGIN: Three Address Code Statements
temp0 = a_ > 20
temp1 = ! temp0
if(temp1) goto Label1
b_ = 20
goto Label0
Label1:
c_ = 28
Label0:
Label2:
temp2 = b_ > 20
temp3 = c_ < 30
temp4 = temp2 && temp3
temp6 = ! temp4
if(temp6) goto Label3
temp5 = b_ + 1
b_ = temp5
goto Label2
Label3:
Label4:
temp7 = c_ + 2
c_ = temp7
temp8 = c_ > 30
if(temp8) goto Label4
**END: Three Address Code Statements
