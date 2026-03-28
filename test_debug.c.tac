**PROCEDURE: main
**BEGIN: Three Address Code Statements
read n_
temp0 = n_ >= 5
temp1 = n_ <= 10
temp2 = temp0 && temp1
temp6 = ! temp2
if(temp6) goto Label3
Label0:
temp3 = n_ > 0
temp5 = ! temp3
if(temp5) goto Label1
write n_
write " "
temp4 = n_ - 1
n_ = temp4
goto Label0
Label1:
goto Label2
Label3:
temp7 = n_ <= 1024
temp8 = - 1024
temp9 = n_ >= temp8
temp10 = temp7 && temp9
temp13 = ! temp10
if(temp13) goto Label6
Label4:
write n_
write " "
temp11 = n_ / 2
n_ = temp11
temp12 = n_ != 0
if(temp12) goto Label4
goto Label5
Label6:
write "Your number is: "
write n_
Label5:
Label2:
**END: Three Address Code Statements
