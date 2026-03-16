**PROCEDURE: main
**BEGIN: Three Address Code Statements
write "Enter first number: "
read a_
write "Enter second number: "
read b_
write "Enter third number: "
read c_
temp0 = a_ > b_
temp5 = ! temp0
if(temp5) goto Label0
temp1 = a_ > c_
temp2 = ! temp1
if(temp2) goto Label2
stemp1 = a_
goto Label3
Label2:
stemp1 = c_
Label3:
stemp0 = stemp1
goto Label1
Label0:
temp3 = b_ > c_
temp4 = ! temp3
if(temp4) goto Label4
stemp2 = b_
goto Label5
Label4:
stemp2 = c_
Label5:
stemp0 = stemp2
Label1:
d_ = stemp0
write "Enter largest number is: "
write d_
**END: Three Address Code Statements
