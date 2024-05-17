@setlocal

@PATH="C:\java\jdk1.6.0_43\bin"
           
@set source=C:\vlab\python\jtools\out\production\jtools
@set build=C:\vlab\python\jtools\build

@pushd %source%
jar cfe %build%\jportal.jar vlab/jportal/Compiler vlab/jportal/*.class vlab/jportal/code/*.class vlab/jportal/ddl/*.class
@popd

@pushd %source%
jar cfe %build%\crackle.jar vlab/crackle/Compiler vlab/crackle/*.class vlab/crackle/http/*.class
@popd

@PATH="C:\java\jdk-11\bin"

@pushd %source%
jar cfe %build%\jportal-11.jar vlab/jportal/Compiler vlab/jportal/*.class vlab/jportal/code/*.class vlab/jportal/ddl/*.class
@popd

@pause
