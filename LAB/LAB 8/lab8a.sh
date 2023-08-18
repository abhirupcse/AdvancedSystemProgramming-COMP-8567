#!/bin/bash

# THIS WILL CHECK IF THE GIVEN TEXT FILE sample.txt EXITS IN THE GIVEN PATH DIRECTORY
if [ -e sample.txt ]; then
  # THIS WILL CHEECK THE USER
  if [ $(stat -c '%U' sample.txt) == $(whoami) ]; then
    # SETTING THE FILE WRITE PERMISSION FOR THE sample.txt
    if [ -w sample.txt ]; then
      # HERE WE ARE APPENDING THE CONTENT RETRIEVED FROM ls -1 INTO sample.txt
      ls -1 >> sample.txt
      echo "CONTENT of ls -1 HAS BEEN APPENDED TO THE FILE sample.txt!!!"
    else
      # SETTING THE sample.txt FILE PERMISSION TO WRITE
      chmod u+w sample.txt
      echo "WRITE PERMISSION HAS BEEN GIVEN IN THE FILE sample.txt!"
      ls -1 >> sample.txt
      echo "CONTENT OF ls -1 HAS BEEN WRITTEN TO THE FILE sample.txt!!!"
    fi
  else
    # DISPLAYING THE MESSAEGE IF USER IS NOT THE OWNER OF THE FILE sample.txt file
    echo "YOU ARE IDENTIFIED AS THE OWNER OF THE FILE sample.txt"
  fi
else
  # THIS BLOCK WILL RUN AND WILL CREATE A FILE IF NOT EXITS
  touch sample.txt
  echo "THE FILE sample.txt DOES NOT EXITS HENCE AN EMPTY FILE WAS CREATED!!!"
fi
