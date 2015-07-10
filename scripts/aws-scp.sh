#!/bin/sh
#Author:         charliezhao
#Filename:       aws-scp.sh
#Last modified:  2015-06-29 06:12
#Description:     

if [ $# -eq 0 ]
then
    echo $0 '<file name>'
    echo "it used to copy file to aws ~/tmp"
    exit 1
fi

scp -r -i ~/aws/AWSForCharlie.pem.txt $1 ubuntu@52.69.150.171:~/tmp/
