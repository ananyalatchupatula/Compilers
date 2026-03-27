**PROCEDURE: main
**BEGIN: Three Address Code Statements
x_ = 14
y_ = 36
a_ = x_
b_ = y_
Label0:
temp0 = b_ != 0
temp3 = ! temp0
if(temp3) goto Label1
t_ = b_
temp1 = a_ / b_
temp2 = temp1 * b_
b_ = temp2
a_ = t_
goto Label0
Label1:
gcd_ = a_
temp4 = x_ * y_
temp5 = temp4 / gcd_
lcm_ = temp5
**END: Three Address Code Statements
