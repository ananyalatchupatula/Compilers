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
temp14 = ! temp5
if(temp14) goto Label0
temp15 = - c_
temp6 = temp15 * x_
temp7 = temp6 + x_
temp8 = temp7 + x_
temp16 = - c_
temp9 = b_ + temp16
temp10 = c_ / temp9
temp11 = temp8 - temp10
stemp0 = temp11
goto Label1
Label0:
temp12 = c_ * 20
temp13 = temp12 - 30
stemp0 = temp13
Label1:
x_ = stemp0
**END: Three Address Code Statements
