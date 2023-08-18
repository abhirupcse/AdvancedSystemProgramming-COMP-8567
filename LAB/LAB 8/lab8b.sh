#!/bin/bash

# CHECK THE SYNTEX 
if [ $# -lt 1 ]; then
  echo "SYNOPSIS: $0 <directory_path> [<extension>]"
  exit 1
fi

abhiDirPath=$1
abhiExtension=$2

# COUNT THE TOTAL NUMBR OF FILES FOR PASSED EXTENSION IN THE DIRECTORY
if [ -z "$abhiExtension" ]; then
  count=$(find "$abhiDirPath" -type f | wc -l)
else
  count=$(find "$abhiDirPath" -type f -name "*$abhiExtension" | wc -l)
fi

# DISPLAYING THE COUNT OF THE FILES
if [ $count -eq 0 ]; then
  if [ -z "$abhiExtension" ]; then
    echo "THERE ARE NO FILES FOUND IN THE DIRECTORY PATH $abhiDirPath"
  else
    echo "THERE ARE NO FILE WITH PASSED EXTENSION $abhiExtension IN THE DIRECTORY PATH: $abhiDirPath"
  fi
else
  if [ -z "$abhiExtension" ]; then
    echo "TOTAL NO OF FILES IN THE DIRECTORY PATH: $abhiDirPath IS= $count"
  else
    echo "TOTAL NO OF FILES WITH EXTENSION $abhiExtension IN DIRECTORY PATH $abhiDirPath IS = $count"
  fi
fi