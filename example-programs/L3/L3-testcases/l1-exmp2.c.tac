**PROCEDURE: main
**BEGIN: Three Address Code Statements
averageWorkingHoursPerDay_ = 8.50
salary_ = 10000.00
read employeeDailyWorkingHours_
temp0 = employeeDailyWorkingHours_ > averageWorkingHoursPerDay_
temp3 = ! temp0
if(temp3) goto Label0
temp1 = salary_ * 0.20
stemp0 = temp1
goto Label1
Label0:
temp2 = salary_ * 0.10
stemp0 = temp2
Label1:
bonus_ = stemp0
temp4 = salary_ + bonus_
salary_ = temp4
write salary_
**END: Three Address Code Statements
