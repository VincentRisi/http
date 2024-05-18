from xmlutils import *

payment_xmldata='''\
<Payment Id="365188" SessionNo="" SequenceNo="">
  <PaymentReference>IP0900X017136688</PaymentReference>
  <SendersReference>b198ceb4fcfc8ccf</SendersReference>
  <Profile>
    <CapturedUser>CC093660</CapturedUser>
    <AuthorisedBy>GPS - STP</AuthorisedBy>
    <Direction>IN</Direction>
    <Status>SEND_MCPE</Status>
    <ProcessedBy/>
    <AssignedTo>GPS</AssignedTo>
  </Profile>
  <BOPRecs/>
  <SaffyNo>X0171366</SaffyNo>
  <InputDate>0001-01-01T00:00:00</InputDate>
  <OutputDate>0001-01-01T00:00:00</OutputDate>
  <ReceivedDate>2018-11-12T12:00:46.007</ReceivedDate>
  <OriginalBrnNo>0900</OriginalBrnNo>
  <ProcessingBrnNo>0900</ProcessingBrnNo>
  <RoutedFromBrnNo/>
  <FinancialBrnNo>0900</FinancialBrnNo>
  <Channel>SW</Channel>
  <ChannelType>CL</ChannelType>
  <AdditionalFields>
    <PaymentInfoField>
      <Id>582119</Id>
      <Type>UETR</Type>
      <Value>88bf24c6-28be-4b2d-98d9-747cbdd5fae3</Value>
    </PaymentInfoField>
    <PaymentInfoField>
      <Id>582120</Id>
      <Type>OperationCode</Type>
      <Value>CRED</Value>
    </PaymentInfoField>
    <PaymentInfoField>
      <Id>582121</Id>
      <Type>RemittanceInformation</Type>
      <Value>My Account in Other Bank</Value>
    </PaymentInfoField>
  </AdditionalFields>
  <PaymentType>ZarPayOn</PaymentType>
  <SwiftType>103</SwiftType>
  <SwiftMessage>{1:F01NEDSZAJ0XXXX7696307955}{2:O1031854181109IRVTUS30BXXX16808967841811090427N}{3:{108:33448321}{111:001}{121:88bf24c6-28be-4b2d-98d9-747cbdd5fae3}}{4::20:b198ceb4fcfc8ccf:23B:CRED:32A:181112ZAR6200,01:33B:ZAR6200,01:50K:/04554899000106CAPETOWN IRON AND STEELCIF-75225826000002TESTING ONESouth Africa:52A:/IRVTUS30:57A:ABSAZAJ0:59:/1158667043FREDERICK MICHEL BEHR37 FICHARDT STREET SASOLBURG SASASOLBURG SOUTH AFRICAZA:70:My Account in Other Bank:71A:SHA:71F:USD0,:71F:ZAR335,45-}</SwiftMessage>
  <Priority>N/A</Priority>
  <AssignTo>GPS</AssignTo>
  <ValueDate>2018-11-12</ValueDate>
  <OriginalValueDate>2018-11-12</OriginalValueDate>
  <ProcessedDate>2018-11-12</ProcessedDate>
  <TmStamp>2018-11-12T12:01:07.16</TmStamp>
  <Principle>
    <Amount>6200.0100</Amount>
    <Currency>ZAR</Currency>
  </Principle>
  <Instructed>
    <Amount>6200.0100</Amount>
    <Currency>ZAR</Currency>
    <Rate>0</Rate>
  </Instructed>
  <OriginalInstructed>
    <Amount>0</Amount>
    <Rate>0</Rate>
  </OriginalInstructed>
  <ZAREq>
    <Amount>6200.0100</Amount>
    <Currency>ZAR</Currency>
  </ZAREq>
  <Settlement>
    <Amount>6086.0100</Amount>
    <Currency>ZAR</Currency>
  </Settlement>
  <TotalCharge>
    <Amount>114.0000</Amount>
    <Currency>ZAR</Currency>
  </TotalCharge>
  <ChargeType>SHA</ChargeType>
  <OutChargeType>SHA</OutChargeType>
  <SendersCharges>
    <PaymentCharge>
      <Id>168168</Id>
      <Currency>USD</Currency>
      <Amount>0.0000</Amount>
      <VatInd>false</VatInd>
      <VatAmount>0.0000</VatAmount>
      <VatPercentage>0</VatPercentage>
      <ChargeOverrideInd>false</ChargeOverrideInd>
      <IsNedbankCharge>false</IsNedbankCharge>
      <BranchNo>0900</BranchNo>
      <ChargeAccNo/>
      <ChargeCode>71F</ChargeCode>
      <VatAccNo/>
      <Out71FCharge>false</Out71FCharge>
      <ForeignRate>0</ForeignRate>
      <ForeignAmount>0.0000</ForeignAmount>
    </PaymentCharge>
    <PaymentCharge>
      <Id>168169</Id>
      <Currency>ZAR</Currency>
      <Amount>335.4500</Amount>
      <VatInd>false</VatInd>
      <VatAmount>0.0000</VatAmount>
      <VatPercentage>0</VatPercentage>
      <ChargeOverrideInd>false</ChargeOverrideInd>
      <IsNedbankCharge>false</IsNedbankCharge>
      <BranchNo>0900</BranchNo>
      <ChargeAccNo/>
      <ChargeCode>71F</ChargeCode>
      <VatAccNo/>
      <Out71FCharge>false</Out71FCharge>
      <ForeignRate>0</ForeignRate>
      <ForeignAmount>0.0000</ForeignAmount>
    </PaymentCharge>
  </SendersCharges>
  <ReceiversCharges>
    <PaymentCharge>
      <Id>168170</Id>
      <Currency>ZAR</Currency>
      <Amount>99.1300</Amount>
      <Description>ITT PayOn Charges</Description>
      <VatInd>true</VatInd>
      <VatAmount>14.8700</VatAmount>
      <VatPercentage>15</VatPercentage>
      <ChargeOverrideInd>false</ChargeOverrideInd>
      <IsNedbankCharge>true</IsNedbankCharge>
      <BranchNo>0900</BranchNo>
      <ChargeAccNo>9000647000</ChargeAccNo>
      <ChargeCode>0100</ChargeCode>
      <EventDescription>0100</EventDescription>
      <VatAccNo>9000174000</VatAccNo>
      <Out71FCharge>true</Out71FCharge>
      <ForeignRate>0</ForeignRate>
      <ForeignAmount>0.0000</ForeignAmount>
    </PaymentCharge>
  </ReceiversCharges>
  <SendersBank>
    <Id>3302058</Id>
    <SwiftAddress>IRVTUS30</SwiftAddress>
    <BankType>SenderBank</BankType>
    <Account>1986001288</Account>
    <Branch>XXX</Branch>
    <AccountType>VOSTRO</AccountType>
    <AccountCurrency>ZAR</AccountCurrency>
  </SendersBank>
  <RecipientBank>
    <Id>3302059</Id>
    <SwiftAddress>NEDSZAJ0</SwiftAddress>
    <BankType>RecipientBank</BankType>
    <Branch>XXX</Branch>
    <AccountCurrency>ZAR</AccountCurrency>
  </RecipientBank>
  <OrderingCustomer>
    <Id>3302060</Id>
    <BankType>OrderingCustomer</BankType>
    <Name>CAPETOWN IRON AND STEEL</Name>
    <Address>CIF-75225826000002/?LNTESTING ONE/?LNSouth Africa</Address>
    <Account>04554899000106</Account>
    <AccountCurrency>ZAR</AccountCurrency>
  </OrderingCustomer>
  <OrderingInstitution>
    <Id>3302061</Id>
    <BankType>OrderingInstitution</BankType>
    <Account>IRVTUS30</Account>
  </OrderingInstitution>
  <FundsWith>
    <Id>3302064</Id>
    <SwiftAddress>IRVTUS30</SwiftAddress>
    <BankType>FundsWith</BankType>
    <Account>1986001288</Account>
    <Branch>XXX</Branch>
    <AccountType>VOSTRO</AccountType>
    <AccountCurrency>ZAR</AccountCurrency>
  </FundsWith>
  <Beneficiary>
    <Id>3302062</Id>
    <BankType>Beneficiary</BankType>
    <Name>FREDERICK MICHEL BEHR</Name>
    <Address>37 FICHARDT STREET SASOLBURG SA/?LNSASOLBURG SOUTH AFRICA/?LNZA</Address>
    <Account>1158667043</Account>
    <AccountCurrency>ZAR</AccountCurrency>
  </Beneficiary>
  <AccountWith>
    <Id>3302063</Id>
    <SwiftAddress>ABSAZAJ0</SwiftAddress>
    <BankType>AccountWith</BankType>
    <Branch>XXX</Branch>
    <SortCode>//ZA632005</SortCode>
    <AccountCurrency>ZAR</AccountCurrency>
  </AccountWith>
  <SenderToReceiverInfo>/REC/NTNRC/</SenderToReceiverInfo>
  <DebitBank>
    <Id>3302065</Id>
    <SwiftAddress>IRVTUS30</SwiftAddress>
    <BankType>DebitBank</BankType>
    <Account>1986001288</Account>
    <Branch>XXX</Branch>
    <AccountType>VOSTRO</AccountType>
    <AccountCurrency>ZAR</AccountCurrency>
  </DebitBank>
  <CreditBank>
    <Id>3302066</Id>
    <SwiftAddress>ZYABZAJ0</SwiftAddress>
    <BankType>CreditBank</BankType>
    <Name/>
    <Address>N/A</Address>
    <Account>9831103505</Account>
    <Branch>XXX</Branch>
    <SortCode/>
    <AccountType>RTGS</AccountType>
    <AccountCurrency>ZAR</AccountCurrency>
  </CreditBank>
  <BeneficiaryAccounts/>
  <PaymentFEC/>
  <OutSendersCorrespondent>
    <Id>3302068</Id>
    <SwiftAddress>ZYABZAJ0</SwiftAddress>
    <BankType>OutSendersCorrespondent</BankType>
    <Name/>
    <Address>N/A</Address>
    <Account>9831103505</Account>
    <Branch>XXX</Branch>
    <SortCode/>
    <AccountType>RTGS</AccountType>
    <AccountCurrency>ZAR</AccountCurrency>
  </OutSendersCorrespondent>
  <STP>true</STP>
  <PaymentOutflags>
    <OUT103>false</OUT103>
    <OUT205Sarb>false</OUT205Sarb>
    <OUT205Bank>false</OUT205Bank>
    <OUT202Bank>false</OUT202Bank>
    <OUT191>false</OUT191>
    <MT199>false</MT199>
    <Repaired>false</Repaired>
    <RoutedBackToSaffy>false</RoutedBackToSaffy>
    <PaymentCover>false</PaymentCover>
    <OUT103Y>true</OUT103Y>
    <OUT202YCOV>false</OUT202YCOV>
    <ManualCaptured>false</ManualCaptured>
    <SpecialInstruction>false</SpecialInstruction>
    <SIM>false</SIM>
    <BulkUpload>false</BulkUpload>
    <CLS>false</CLS>
    <Vostro>false</Vostro>
    <NIS>false</NIS>
    <CashOps>false</CashOps>
    <GPI>true</GPI>
    <GPIException>false</GPIException>
    <ManualVerification>false</ManualVerification>
    <AutoFEC>false</AutoFEC>
    <DbtAccInsufftFunds>false</DbtAccInsufftFunds>
    <PayAmountUnderIFILimit>true</PayAmountUnderIFILimit>
    <SiressPayment>false</SiressPayment>
    <NostroCcyCutOff>false</NostroCcyCutOff>
    <Field59F>false</Field59F>
    <LoanIQPayment>false</LoanIQPayment>
    <ReturnOfFunds>false</ReturnOfFunds>
    <ElectronicCharge>false</ElectronicCharge>
    <ReturnedFromNT>false</ReturnedFromNT>
  </PaymentOutflags>
  <Indicators>
    <Field57D>false</Field57D>
    <Field72>false</Field72>
    <CorrespondentLookup>false</CorrespondentLookup>
    <BranchLimitChecked>true</BranchLimitChecked>
    <CoverReceived>false</CoverReceived>
    <MT101Msg>false</MT101Msg>
    <NedTreasuryClient>false</NedTreasuryClient>
    <DefaultRecCodeDone>true</DefaultRecCodeDone>
    <GermanPension>false</GermanPension>
  </Indicators>
  <MT103>
    <Id>3302067</Id>
    <SwiftAddress>ABSAZAJ0</SwiftAddress>
    <BankType>MT103</BankType>
    <Name/>
    <Branch>XXX</Branch>
    <SortCode>/REC/NTNRC/</SortCode>
    <AccountCurrency>ZAR</AccountCurrency>
  </MT103>
  <PaymentUserNotesITT/>
</Payment>
'''

payment_short_xml='''\
<Payment Id="365188" SessionNo="" SequenceNo="">
  <AdditionalFields>
    <PaymentInfoField>
      <Id>582119</Id>
      <Type>UETR</Type>
      <Value>88bf24c6-28be-4b2d-98d9-747cbdd5fae3</Value>
    </PaymentInfoField>
    <PaymentInfoField>
      <Id>582120</Id>
      <Type>OperationCode</Type>
      <Value>CRED</Value>
    </PaymentInfoField>
    <PaymentInfoField>
      <Id>582121</Id>
      <Type>RemittanceInformation</Type>
      <Value>My Account in Other Bank</Value>
    </PaymentInfoField>
  </AdditionalFields>
  <PaymentReference>IP0900X017136688</PaymentReference>
  <SendersReference>b198ceb4fcfc8ccf</SendersReference>
  <Profile>
    <CapturedUser>CC093660</CapturedUser>
    <AuthorisedBy>GPS - STP</AuthorisedBy>
    <Direction>IN</Direction>
    <Status>SEND_MCPE</Status>
    <ProcessedBy/>
    <AssignedTo>GPS</AssignedTo>
  </Profile>
  <SendersCharges>
    <PaymentCharge>
      <Id>168168</Id>
      <Currency>USD</Currency>
      <Amount>0.0000</Amount>
      <VatInd>false</VatInd>
      <VatAmount>0.0000</VatAmount>
      <VatPercentage>0</VatPercentage>
      <ChargeOverrideInd>false</ChargeOverrideInd>
      <IsNedbankCharge>false</IsNedbankCharge>
      <BranchNo>0900</BranchNo>
      <ChargeAccNo/>
      <ChargeCode>71F</ChargeCode>
      <VatAccNo/>
      <Out71FCharge>false</Out71FCharge>
      <ForeignRate>0</ForeignRate>
      <ForeignAmount>0.0000</ForeignAmount>
    </PaymentCharge>
    <PaymentCharge>
      <Id>168169</Id>
      <Currency>ZAR</Currency>
      <Amount>335.4500</Amount>
      <VatInd>false</VatInd>
      <VatAmount>0.0000</VatAmount>
      <VatPercentage>0</VatPercentage>
      <ChargeOverrideInd>false</ChargeOverrideInd>
      <IsNedbankCharge>false</IsNedbankCharge>
      <BranchNo>0900</BranchNo>
      <ChargeAccNo/>
      <ChargeCode>71F</ChargeCode>
      <VatAccNo/>
      <Out71FCharge>false</Out71FCharge>
      <ForeignRate>0</ForeignRate>
      <ForeignAmount>0.0000</ForeignAmount>
    </PaymentCharge>
  </SendersCharges>
  <MT103>
    <Id>3302067</Id>
    <SwiftAddress>ABSAZAJ0</SwiftAddress>
    <BankType>MT103</BankType>
    <Name/>
    <Branch>XXX</Branch>
    <SortCode>/REC/NTNRC/</SortCode>
    <AccountCurrency>ZAR</AccountCurrency>
  </MT103>
  <PaymentUserNotesITT/>
</Payment>
'''

bancs_xmldata = '''\
<?xml version="1.0" encoding="UTF-8"?><CASH>
<CASHMOVEMENT>
<MVMNT_ID>57434429</MVMNT_ID>
<REF_NUM>6067735</REF_NUM>
<RLTD_REF>mishra0A2</RLTD_REF>
<ACNT_REF>1882001079</ACNT_REF>
<DR_CR>2</DR_CR>
<TRANS_CODE>1480</TRANS_CODE>
<RVRSL_IND>2</RVRSL_IND>
<POS_TYP>24</POS_TYP>
<MVMNT_GEN_DT>20181115</MVMNT_GEN_DT>
<MVMNT_GEN_TM>143319</MVMNT_GEN_TM>
<TRADE_DT>20181109</TRADE_DT>
<OU_ID>GGNED</OU_ID>
<COUNTRY>ZA</COUNTRY>
<BRANCH>0841</BRANCH>
<BP_ID>BP10012404</BP_ID>
<ROUTE>1</ROUTE>
<CLNT_TYPE>0</CLNT_TYPE>
<TXN_CRNCY>ZAR</TXN_CRNCY>
<TXN_AMNT>100</TXN_AMNT>
<EXCHNG_RT>1</EXCHNG_RT>
<LOC_CRNCY_EQ>100</LOC_CRNCY_EQ>
<BAS_CRNCY>ZAR</BAS_CRNCY>
<BAS_CRNCY_RATE>1</BAS_CRNCY_RATE>
<BAS_CRNCY_EQVLNT_AMNT>100</BAS_CRNCY_EQVLNT_AMNT>
<QUANTITY>5</QUANTITY>
<TXN_TYP>SE</TXN_TYP>
<FUNC_CODE>SD</FUNC_CODE>
<CONTRA_AC>8882014614001</CONTRA_AC>
<NARRATIVE>DVP NPN mishra0A2 5</NARRATIVE>
<BATCH>BaNCS</BATCH>
</CASHMOVEMENT>
<CASHMOVEMENT>
<MVMNT_ID>57434428</MVMNT_ID>
<REF_NUM>6067735</REF_NUM>
<RLTD_REF>mishra0A2</RLTD_REF>
<ACNT_REF>1986202259</ACNT_REF>
<DR_CR>1</DR_CR>
<TRANS_CODE>6308</TRANS_CODE>
<RVRSL_IND>2</RVRSL_IND>
<POS_TYP>1</POS_TYP>
<MVMNT_GEN_DT>20181115</MVMNT_GEN_DT>
<MVMNT_GEN_TM>143319</MVMNT_GEN_TM>
<TRADE_DT>20181109</TRADE_DT>
<OU_ID>GGNED</OU_ID>
<COUNTRY>DK</COUNTRY>
<BRANCH>0841</BRANCH>
<BP_ID>BP11013678</BP_ID>
<ROUTE>2</ROUTE>
<CLNT_TYPE>0</CLNT_TYPE>
<TXN_CRNCY>ZAR</TXN_CRNCY>
<TXN_AMNT>100</TXN_AMNT>
<EXCHNG_RT>1</EXCHNG_RT>
<LOC_CRNCY_EQ>100</LOC_CRNCY_EQ>
<BAS_CRNCY>ZAR</BAS_CRNCY>
<BAS_CRNCY_RATE>1</BAS_CRNCY_RATE>
<BAS_CRNCY_EQVLNT_AMNT>100</BAS_CRNCY_EQVLNT_AMNT>
<QUANTITY>5</QUANTITY>
<TXN_TYP>SE</TXN_TYP>
<FUNC_CODE>SD</FUNC_CODE>
<CONTRA_AC>100124040000004</CONTRA_AC>
<NARRATIVE>DVP NPN mishra0A2 5</NARRATIVE>
<BATCH>BaNCS</BATCH>
</CASHMOVEMENT>
</CASH>
'''

def payment_test():
    payment = xmlparse(payment_xmldata)
    print (xmlbuild(payment))
    
    def do_rec():
        Payment = dict_to_rec(payment)
        printClass(Payment, 'Payment')
        print (Payment.__slots__)
        return
    
    def do_xml():
        Payment = dict_to_xml(payment)
        for line in Payment:
            print (line)
        return
    
    def do_json():
        Payment = dict_to_json(payment)
        print (Payment)
        yy = json.loads(Payment)
        print (json.dumps(yy, indent=2))
        return
    
    def do_yaml():
        Payment = dict_to_yaml(payment)
        print (Payment)
        return
    
    do_rec()
    do_xml()
    do_json()
    do_yaml()

def bancs_test():
    bancs = xmlparse(bancs_xmldata)
    print (xmlbuild(bancs))
    
    def do_rec():
        Bancs = dict_to_rec(bancs)
        printClass(Bancs, 'Bancs')
        return
    
    def do_xml():
        Bancs = dict_to_xml(bancs)
        for line in Bancs:
            print (line)
        return
    
    def do_json():
        Bancs = dict_to_json(bancs)
        print (Bancs)
        yy = json.loads(Bancs)
        print (json.dumps(yy, indent=2))
        return
    
    def do_yaml():
        Bancs = dict_to_yaml(bancs)
        print (Bancs)
        return
    
    do_rec()
    do_xml()
    do_json()
    do_yaml()

def main():
    payment_test()
    bancs_test()

if __name__ == '__main__':
    main()
