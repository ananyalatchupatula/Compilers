**PROCEDURE: main
**BEGIN: Three Address Code Statements
a_ = 2
b_ = 3
c_ = 4
temp0 = b_ > a_
temp3 = ! temp0
if(temp3) goto Label1
temp1 = b_ > c_
temp2 = ! temp1
if(temp2) goto Label2
stemp0 = b_
goto Label3
Label2:
stemp0 = c_
Label3:
a_ = stemp0
goto Label0
Label1:
Label0:
write a_
**END: Three Address Code Statements
