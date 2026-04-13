**PROCEDURE: main
**BEGIN: Three Address Code Statements
read x_
temp0 = x_ > 0
temp1 = ! temp0
if(temp1) goto Label0
stemp0 = 1
goto Label1
Label0:
stemp0 = 0
Label1:
y_ = stemp0
write y_
**END: Three Address Code Statements
