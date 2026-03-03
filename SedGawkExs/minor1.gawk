# Giovanni Rebosio gdr0051
NR>2 {
if($3==""||$4==""||$5=="")
{
	avg="F (missing scores)"
}
else
{
	numAvg=($3+$4+$5)/3
	if(numAvg<70)
	{
		avg="F"
	}	
	if(numAvg>69&&numAvg<80)
	{
		avg="C"
	}
	if(numAvg>79&&numAvg<90)
	{
		avg="B"
	}
	if(numAvg>89)
	{
		avg="A"
	}
}

if($3=="")
{
	field3="--"
}
else
{
	field3=$3
}
if($4=="")
{
	field4="--"
}
else
{
	field4=$4
}
if($5=="")
{
	field5="--"
}
else
{
	field5=$5
}


printf("%s	%s	%s	%s	%s => %s\n", $1, $2, field3, field4, field5, avg)}