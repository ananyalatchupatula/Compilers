**PROCEDURE: main
**BEGIN: Three Address Code Statements
a_ = 2
b_ = 4
c_ = 10
d_ = 3
e_ = 8
f_ = 7
temp0 = b_ + c_
temp1 = a_ >= temp0
temp2 = e_ - a_
temp3 = d_ < temp2
temp4 = temp1 && temp3
temp5 = f_ != d_
temp6 = temp4 || temp5
temp10 = ! temp6
if(temp10) goto Label1
temp7 = d_ * e_
temp8 = temp7 / f_
temp9 = a_ + temp8
a_ = temp9
goto Label0
Label1:
b_ = 1
Label0:
**END: Three Address Code Statements
