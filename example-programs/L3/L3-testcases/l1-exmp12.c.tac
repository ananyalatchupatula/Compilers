**PROCEDURE: main
**BEGIN: Three Address Code Statements
write "Enter first number: "
read a_
write "Enter second number: "
read b_
temp0 = a_ >= b_
temp1 = ! temp0
if(temp1) goto Label0
stemp0 = a_
goto Label1
Label0:
stemp0 = b_
Label1:
c_ = stemp0
write "The larger number is: "
write c_
**END: Three Address Code Statements
