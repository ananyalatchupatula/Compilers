**PROCEDURE: main
**BEGIN: Three Address Code Statements
write "Enter value of a: "
read a_
write "Enter value of b: "
read b_
temp0 = a_ * a_
temp1 = b_ * b_
temp2 = temp0 > temp1
d_ = temp2
temp3 = b_ * b_
temp4 = a_ * a_
temp5 = temp3 > temp4
e_ = temp5
temp7 = ! d_
if(temp7) goto Label0
stemp0 = "a"
goto Label1
Label0:
temp6 = ! e_
if(temp6) goto Label2
stemp1 = "b"
goto Label3
Label2:
stemp1 = "none"
Label3:
stemp0 = stemp1
Label1:
z_ = stemp0
write z_
**END: Three Address Code Statements
