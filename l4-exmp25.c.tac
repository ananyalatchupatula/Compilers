**PROCEDURE: main
**BEGIN: Three Address Code Statements
maxInput_ = 10
sum_ = 0
i_ = 2
number_ = 2
Label0:
temp0 = i_ < maxInput_
temp2 = ! temp0
if(temp2) goto Label1
write i_
temp1 = i_ + 2
i_ = temp1
goto Label0
Label1:
temp3 = number_ < 3
temp4 = ! temp3
if(temp4) goto Label3
number_ = 5
goto Label2
Label3:
Label2:
temp5 = sum_ + number_
sum_ = temp5
write sum_
**END: Three Address Code Statements
