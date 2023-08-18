#!/bin/bash

# @Author: ABHIRUP RANJAN(110091866)
# SECTION: 3 COMP8567
# TOOK HELP FROM GOOGLE, STACKOVERFLOW AND OTHER TECHNICAL WEBSITES FOR DESIGNING THIS SCRIPT

# GLOBAL DECLARATION AND TIME FORMATTING
abhiTimeStampFormatEDT="%a %d %b %Y %I:%M:%S %p %Z"
abhiMakecompleteBackupPath="$HOME/home/backup/cb"
abhiBackupLogFilePath="$HOME/home/backup/backup.log"
abhiIncrementalBackupPath="$HOME/home/backup/ib"
abhiTimeStampFormatSysDEFAULT="%Y-%m-%d %H:%M:%S %Z"

# INITIALIZING THE GLOBAL VARIBAL USED FOR cb AND ib FOLDERS
abhiCBFileNameVar=20001
abhiIBFileNameVar=10001

# CREATING THE PARENT DIRECTORIES IF DOES NOT EXITS FOR THE PASSED PATH
mkdir -p "$abhiMakecompleteBackupPath"
mkdir -p "$abhiIncrementalBackupPath"

# CREATE AN EMPTY FILE IF DOES NOT EXITS OR UPDATE THE TIMESTAMP OF AN EXISTING FILE
touch "$abhiBackupLogFilePath"

# THE METHOD WILL CREATE A COMPLETE BACKUP FOR ONLY TXT FILES IF OR NOT ANY CHANGES ARE DONE
abhiCreateCompleteBackup() {
  # FINDS THE REGULAR FILES IN HOME DIRECTORY PATH -->TAR COMMAND IS USED TO CREATE A COMPRESSED FILE
  # OPTION c = CREATE A NEW ARCHIVE, z = COMPRESSION, v = DISPLAY THE FILE, f = ARCHIVE FILE
  # --null -T - READ THE NULL-TERMINATED FILEPATH FROM STANDARD INPUT
  find "$HOME" -name "*.txt" -type f -print0 | tar czvf "$abhiMakecompleteBackupPath/cb$abhiCBFileNameVar.tar" --null -T -
  # APPENDING THE INFO TO THE BACKUPLOG FILE
  echo "$1  cb$abhiCBFileNameVar.tar was created" >>"$abhiBackupLogFilePath"
  # INCREMENTING THE CBFILENAME VARIABLE COUNT BY 1
  abhiCBFileNameVar=$((abhiCBFileNameVar + 1))
}

# THE METHOD WILL ONLY CREATE A BACKUP FOR TXT ONLY IF A NEW TXT FILE IS CREATED OR ANY CHANGES/UPDATE ARE MADE IN EXISTING TXT FILE
abhiCreateIncrementalBackup() {
  # ONLY FINDS THOSE TXT FILES WHICH ARE MODIFIED AFTER PASSED DEFAULT SYSTEM DATE/TIME
  new_files=$(find "$HOME" -type f -name "*.txt" -newermt "$2")

  # IF EXECUTES ONLY IF ANY SUCH FILES ARE FOUND
  if [ -n "$new_files" ]; then
    # DEFING THE NAME OF THE INCREMENTAL BACKUP TAR FILE
    abhiBkupFName="ib$abhiIBFileNameVar.tar"
    # ONLY FINDS THOSE TXT FILES WHICH ARE MODIFIED AFTER PASSED DEFAULT SYSTEM DATE/TIME
    # -exec OPTION TO PROCESS EACH FILE INDIVIDUALLY
    find "$HOME" -type f -name "*.txt" -newermt "$2" -exec tar -czf "$abhiIncrementalBackupPath/$abhiBkupFName" {} +
    # APPENDING THE INFO TO THE BACKUPLOG FILE
    echo "$1  $abhiBkupFName was created" >>"$abhiBackupLogFilePath"
     # INCREMENTING THE IBFILENAME VARIABLE COUNT BY 1
    abhiIBFileNameVar=$((abhiIBFileNameVar + 1))
  # IF NO CHANGES HAS BEEN MADE IN ANY DIRECTORY THEN ELSE EXECUTES  
  else
    echo "$1  No changes-Incremental backup was not created" >>"$abhiBackupLogFilePath"
  fi
}

# CREATING AND INFINITE LOOP
for (( ; ; ))
do
  # TIME ZONE = 'America/New_York' IS USED FOR EDT
  # GETS THE CURRENT TIME AND FORMAT THE TIME BASED ON THE VALUE OF THE $abhiTimeStampFormatEDT VARIABLE
  getCurrentTimeFormat=$(TZ="America/New_York" date +"$abhiTimeStampFormatEDT")
  # GETS THE CURRENT TIME IN DEFAULT SYSTEM TIMEZONE AND FORMAT THE TIME BASED ON THE VALUE OF THE $abhiTimeStampFormatSysDEFAULT VARIABLE
  abhiGetActualTime=$(date +"$abhiTimeStampFormatSysDEFAULT")
  # FUNCION CALL
  abhiCreateCompleteBackup "$getCurrentTimeFormat"
  sleep 120


  for i in {1..3}; do
    # TIME ZONE = 'America/New_York' IS USED FOR EDT
    # GETS THE CURRENT TIME AND FORMAT THE TIME BASED ON THE VALUE OF THE $abhiTimeStampFormatEDT VARIABLE
    getCurrentTimeFormat=$(TZ="America/New_York" date +"$abhiTimeStampFormatEDT")
    # FUNCION CALL
    abhiCreateIncrementalBackup "$getCurrentTimeFormat" "$abhiGetActualTime"
    # GETS THE CURRENT TIME IN DEFAULT SYSTEM TIMEZONE AND FORMAT THE TIME BASED ON THE VALUE OF THE $abhiTimeStampFormatSysDEFAULT VARIABLE
    abhiGetActualTime=$(date +"$abhiTimeStampFormatSysDEFAULT")
    sleep 120
  done

# CLOSING THE 2nd FOR LOOP AND "&" WILL MAKE SURE THAT THIS SCRIPT RUNS IN BACKGROUND
done &