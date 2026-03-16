**PROCEDURE: main
**BEGIN: Three Address Code Statements
write "Enter your percentage "
read marks_
temp0 = marks_ < 40.00
temp13 = ! temp0
if(temp13) goto Label0
stemp0 = "F"
goto Label1
Label0:
temp1 = marks_ >= 40.00
temp2 = marks_ < 50.00
temp3 = temp1 && temp2
temp12 = ! temp3
if(temp12) goto Label2
stemp1 = "D"
goto Label3
Label2:
temp4 = marks_ >= 50.00
temp5 = marks_ < 60.00
temp6 = temp4 && temp5
temp11 = ! temp6
if(temp11) goto Label4
stemp2 = "C"
goto Label5
Label4:
temp7 = marks_ >= 60.00
temp8 = marks_ < 70.00
temp9 = temp7 && temp8
temp10 = ! temp9
if(temp10) goto Label6
stemp3 = "B"
goto Label7
Label6:
stemp3 = "A"
Label7:
stemp2 = stemp3
Label5:
stemp1 = stemp2
Label3:
stemp0 = stemp1
Label1:
grade_ = stemp0
write "The grade obtained is: "
write grade_
**END: Three Address Code Statements
