#!/bin/bash  

# THIS WILL CHECK IF ANY EXTENSION FILE HAS BEEN PROVIDED IN THE CMD LINE  
if [ $# -eq 0 ]; then  
  echo "ERROR: THERE HAS BEEN NO FILES PROVIDED WITH EXTENSION .TXT(MAX 5 FILE)!!"  
  exit 1 
fi 

# USER DEFINED VARIABLES TO STORE THE DATA/VALUES  
outputFName="output.txt"  
data=""  

# LOGIC TO CONACATINATE THE TXT FILES IN ORDER PROVOIDED  
until [ -z "$1" ]; do  
	# LOGIC TO CHECK IF THE FILES ARE WITH EXTENSION AS .TXT  
  	if [ ! -f "$1" ] || [ "${1: -4}" != ".txt" ]; then  
    	echo "ERROR IN FILE FORMAT: $1 PASSED FILE IS NOT IN .TXT FORMAT OR EITHER FILE DOESNOT EXISTS!!"  
    	exit 1  
  	fi 

	# INITIALIZE THE COUNTER VARIABLE TO KEEP TRACK OF TXT FILE  
	txtCounterVar=0 

	# LOOP FOR ALL PASSED ARGUMENTS
	for file in "$@"; do  
    	# CHECKS IF EXTENSION IS .TXT 
        ((txtCounterVar++))  
        # IF MORE THAN 5 .TXT FILES THEN DISPLAY ERROR  
        if ((txtCounterVar > 5)); then  
            echo "ERROR IN NO OF ARGUMENTS/FILE PASSED: KINDLY NOTE TXT FILE UPTO TO 5 ARE ALLOWED!!"  
            exit 1  
        fi  
	done  
  	# CONCATENATE THE CONTENT OF THE FILE  
  	data+=$(cat "$1")
  	# FOR NEW LINE 
  	data+="\n"  
  	shift  
done   

# SAVING THE FINAL RESULT IN THE OUTPUT.TXT FORMAT 
echo -e "$data" > "$outputFName"  
echo "DATA FROM ALL PASSED FILE HAS BEEN CONCATINATED TO FILE:  $outputFName" 