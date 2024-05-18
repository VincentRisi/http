## ORACLE MERGE in python sin tax.
## f'''
## MERGE INTO members x
## USING (SELECT {self.member_id} member_id
##             ,'{self.first_name}' first_name
##             ,'{self.last_name}' last_name
##             , {self.rank} rank FROM dual) y
## ON (x.member_id  = y.member_id)
## WHEN MATCHED THEN
##     UPDATE SET x.first_name = y.first_name, 
##                x.last_name = y.last_name, 
##                x.rank = y.rank
##     WHERE x.first_name <> y.first_name
##        OR x.last_name <> y.last_name
##        OR x.rank <> y.rank 
## WHEN NOT MATCHED THEN
##     INSERT(x.member_id, x.first_name, x.last_name, x.rank)  
##     VALUES(y.member_id, y.first_name, y.last_name, y.rank)
## '''    