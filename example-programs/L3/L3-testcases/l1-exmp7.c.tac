**PROCEDURE: main
**BEGIN: Three Address Code Statements
temp0 = b_ > c_
temp1 = ! temp0
if(temp1) goto Label0
stemp0 = b_
goto Label1
Label0:
stemp0 = c_
Label1:
a_ = stemp0
write 4
**END: Three Address Code Statements
