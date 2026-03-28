**PROCEDURE: main
**BEGIN: Three Address Code Statements
a_ = 2
b_ = 3
c_ = 4
temp0 = b_ > a_
temp3 = ! temp0
if(temp3) goto Label2
temp1 = b_ > c_
temp2 = ! temp1
if(temp2) goto Label0
stemp0 = b_
goto Label1
Label0:
stemp0 = c_
Label1:
a_ = stemp0
goto Label0
Label2:
write a_
**END: Three Address Code Statements
