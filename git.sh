#!/bin/bash

# Pushes current version to github

git add macros/*
git add old/*
git add plots/*
git add purity/*.C
git add purity/*.cc
git add purity/*.h
git add purity/*.sh
git add *.cc
git add *.h
git add *.C
git add *.sh

DATE=`date +"%Y-%m-%d"` 
git commit -m "${DATE}"

git push -u origin master