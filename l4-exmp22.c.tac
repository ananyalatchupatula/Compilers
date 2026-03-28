**PROCEDURE: main
**BEGIN: Three Address Code Statements
a_ = 2.30
b_ = 3.40
temp0 = a_ + b_
temp1 = a_ - b_
temp2 = temp0 < temp1
temp5 = ! temp2
if(temp5) goto Label0
temp3 = a_ * b_
stemp0 = temp3
goto Label1
Label0:
temp4 = a_ - b_
stemp0 = temp4
Label1:
d_ = stemp0
**END: Three Address Code Statements
