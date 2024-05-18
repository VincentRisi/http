import cx_Oracle
import os
import sys


class Connection():
    connect = None

    def __init__(self, dbName, dbUser, dbPass):
        self.dbName = dbName
        self.dbUser = dbUser
        self.dbPass = dbPass

        self.connectionStr = '{0}/{1}@{2}'.format(dbUser, dbPass, dbName)
    
        print ('connecting to db ' + self.dbName)
        self.connect = cx_Oracle.connect(self.connectionStr)

    def __init__(self, dbConnect):

        str = dbConnect.split('@')
        userPass = str[0].split('/')
        self.dbName = str[1]
        self.dbUser = userPass[0]
        self.dbPass = userPass[1]

        self.connectionStr = dbConnect
    
        print ('connecting to db ' + self.dbName)
        self.connect = cx_Oracle.connect(self.connectionStr)



 