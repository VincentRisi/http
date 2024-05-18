import DBPORTAL as INTRINSICS
import DB_DOMAIN as domain

connect = INTRINSICS.logon('../../../../build/bin/putty3.bin', 'npu', 'npudev', 'dn29')
bankFile = domain.DBDomainBankFile(connect)
banks = bankFile.loadBankFile()
for bank in banks:
	print (f'swift:{bank.SwiftAddress} bank:{bank.BankName}')
