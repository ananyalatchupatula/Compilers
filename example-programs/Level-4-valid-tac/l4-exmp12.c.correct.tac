**PROCEDURE: main
**BEGIN: Three Address Code Statements
b_ = 2.10
c_ = 8.30
temp0 = b_ / c_
temp1 = temp0 / a_
temp2 = temp1 * b_
temp3 = a_ + temp2
temp4 = temp3 > 0.00
temp6 = ! temp4
if(temp6) goto Label0
temp5 = a_ - 1.00
a_ = temp5
goto Label0
Label0:
**END: Three Address Code Statements
