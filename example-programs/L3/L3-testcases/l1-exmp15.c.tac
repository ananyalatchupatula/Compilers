**PROCEDURE: main
**BEGIN: Three Address Code Statements
a_ = 5
b_ = 10
temp0 = - 0.30
c_ = temp0
temp1 = - 2.00
d_ = temp1
temp2 = a_ <= b_
temp3 = c_ > d_
temp4 = temp2 || temp3
temp5 = ! temp4
if(temp5) goto Label0
stemp0 = "yes"
goto Label1
Label0:
stemp0 = "no"
Label1:
s_ = stemp0
write s_
**END: Three Address Code Statements
