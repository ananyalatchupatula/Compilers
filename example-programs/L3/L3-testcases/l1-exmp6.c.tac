**PROCEDURE: main
**BEGIN: Three Address Code Statements
c_ = 2
b_ = 3
s_ = "Hi"
z_ = 3.50
write 5
write 5.60
write "Hi"
write x_
read b_
temp0 = x_ < b_
temp1 = b_ > c_
temp2 = c_ == b_
temp3 = ! temp2
temp4 = temp1 && temp3
temp5 = temp0 || temp4
temp16 = ! temp5
if(temp16) goto Label0
temp6 = - c_
temp7 = temp6 * x_
temp8 = temp7 + x_
temp9 = temp8 + x_
temp10 = - c_
temp11 = b_ + temp10
temp12 = c_ / temp11
temp13 = temp9 - temp12
stemp0 = temp13
goto Label1
Label0:
temp14 = c_ * 20
temp15 = temp14 - 30
stemp0 = temp15
Label1:
x_ = stemp0
**END: Three Address Code Statements
