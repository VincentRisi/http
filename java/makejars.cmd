@setlocal

@PATH="D:\java\jdk-21\bin"
           
@set source=C:\vlab\http\java\out\production\java
@set build=C:\vlab\http\java\build

@pushd %source%
jar cfe %build%\jportal.jar vlab/jportal/Compiler vlab/jportal/*.class vlab/jportal/code/*.class vlab/jportal/ddl/*.class vlab/jportal/util/*.class
@popd

@pushd %source%
jar cfe %build%\crackle.jar vlab/crackle/Compiler vlab/crackle/*.class vlab/crackle/http/*.class
@popd

@pause
