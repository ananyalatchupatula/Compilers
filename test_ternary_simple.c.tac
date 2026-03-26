**PROCEDURE: main
**BEGIN: Three Address Code Statements
read x_
temp0 = x_ > 0
if(temp0) goto Label0
Label1:
stemp0 = 0
goto Label2
Label0:
stemp0 = 1
Label2:
y_ = stemp0
write y_
**END: Three Address Code Statements
