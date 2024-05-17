@setlocal

PATH="C:\java\jdk1.6.0_43\bin"
pushd C:\vlab\python\jtools\crackle\
java -classpath C:\vlab\devtools\javacc-7.0.10\bootstrap\javacc.jar javacc IDL.jj
