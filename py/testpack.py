import sys
import yaml

messages = '''\
%YAML 1.2
---
Message:
  - Id: 65661
    SourceSysId: MCCA
    Reference: SPOTTRAN-FACTS-0
    QueueId: WSSMCAINTMTP
    MessageData: |
      {1:F01NEDSZAJ0XXXX6649391196}{2:O9000017190307ZYCQZAJ0AXXX06349038421903070404N}{3:{108:0933876451193050}}{4:
      :20:NEDSP19066000001
      :21:NEDSZAZYAAZA0306
      :25:20905149
      :32A:190307ZAR16,64
      :52A:NEDSZAJ0
      :72:/REC/NOREF/MRT/5119/GCD/20190306/OR
      //F/2019030600000330/BKB/ZYAAZAJ0/P
      //DT/20190307/PTM/001146/DCD/NEDSZA
      //J0/OCD/ZYCQZAJ0/CPL/3000
      -}  
    MessageType: 1
  - Id: 65662
    SourceSysId: MCCA
    Reference: SPOTTRAN-FACTS-0
    QueueId: WSSMCAINTMTP
    MessageData: |
      <MCAInterest>
        <Description>MCA-5810984185</Description>
        <WasteMvsCount>0</WasteMvsCount>
        <WasteMvsVatIncludedCount>0</WasteMvsVatIncludedCount>
        <WasteTibosCount>0</WasteTibosCount>
        <BillPurchCount>0</BillPurchCount>
        <MFADRCount>0</MFADRCount>
        <FABJ/>
        <FAAJ/>
        <FAAS>AS0810 MCA201903072000N  SSCNH    00000000000002000001000000           620000005810984185 20190307                            0000000000000423                      06-MAR-2019     INTPD060319CNH                       0000000000</FAAS>
        <FADR/>
      </MCAInterest>
    MessageType: 1
  - Id: 65663
    SourceSysId: MCCA
    Reference: SPOTTRAN-FACTS-0
    QueueId: WSSMCAINTMTP
    MessageData: |
      <MCAInterest>
        <Description>MCA-5810984185</Description>
        <WasteMvsCount>0</WasteMvsCount>
        <WasteMvsVatIncludedCount>0</WasteMvsVatIncludedCount>
        <WasteTibosCount>0</WasteTibosCount>
        <BillPurchCount>0</BillPurchCount>
        <MFADRCount>0</MFADRCount>
        <FABJ/>
        <FAAJ/>
        <FAAS>AS0810 MCA201903072000N  SSCNH    00000000000002000001000000           620000005810984185 20190307                            0000000000000423                      06-MAR-2019     INTPD060319CNH                       0000000000</FAAS>
        <FADR/>
      </MCAInterest>
    MessageType: 1
  - Id: 65678
    SourceSysId: GPS
    Reference: V086809340256426
    QueueId: GPS-VOSPAYON-MTP
    MessageData: |
      <Payment Id="377426" SessionNo="6650" SequenceNo="391368">
        <PaymentReference>V086809340256426</PaymentReference>
        <RelatedReference>2019CEM289878</RelatedReference>
        <SendersReference>2019CEM289878996</SendersReference>
        <Profile>
          <CapturedUser>SRV-GPS-QA</CapturedUser>
          <AuthorisedBy>GPS - STP</AuthorisedBy>
          <Direction>IN</Direction>
          <Status>SEND_MCPE</Status>
          <ProcessedBy/>
          <AssignedTo>GPS</AssignedTo>
        </Profile>
        <BOPRecs/>
        <SaffyNo>109340256</SaffyNo>
        <InputDate>0001-01-01T00:00:00</InputDate>
        <OutputDate>0001-01-01T00:00:00</OutputDate>
        <ReceivedDate>2019-03-07T09:24:49.467</ReceivedDate>
        <OriginalBrnNo>0868</OriginalBrnNo>
        <ProcessingBrnNo>0868</ProcessingBrnNo>
        <RoutedFromBrnNo/>
        <Channel>SW</Channel>
        <ChannelType>BK</ChannelType>
        <AdditionalFields/>
        <PaymentType>VostroZarPayOn</PaymentType>
        <SwiftType>202</SwiftType>
        <SwiftMessage>{1:F01NEDSZAJ0XSCI6650391368}{2:O2020924190307NEDSZAJ0XXXX71179608911903070924N}{3:{108:109340256}}{4::20:2019CEM289878996:21:2019CEM289878:32A:190307ZAR34000000,:53B:/1766000061:57A:SBZAZAJ0:58A:/002149168SBZAZAJ0:72:/REC/MPDEM-}</SwiftMessage>
        <Priority>N/A</Priority>
        <AssignTo>GPS</AssignTo>
        <ValueDate>2019-03-07</ValueDate>
        <OriginalValueDate>2019-03-07</OriginalValueDate>
        <ProcessedDate>2019-03-07</ProcessedDate>
        <TmStamp>2019-03-07T09:24:55.95</TmStamp>
        <Principle>
          <Amount>34000000.0000</Amount>
          <Currency>ZAR</Currency>
        </Principle>
        <Instructed>
          <Amount>0.0000</Amount>
          <Rate>0</Rate>
        </Instructed>
        <OriginalInstructed>
          <Amount>0</Amount>
          <Rate>0</Rate>
        </OriginalInstructed>
        <ZAREq>
          <Amount>34000000.0000</Amount>
          <Currency>ZAR</Currency>
        </ZAREq>
        <Settlement>
          <Amount>34000000.0000</Amount>
          <Currency>ZAR</Currency>
        </Settlement>
        <TotalCharge>
          <Amount>0.0000</Amount>
          <Currency>ZAR</Currency>
        </TotalCharge>
        <SendersCharges/>
        <ReceiversCharges/>
        <SendersBank>
          <Id>3408559</Id>
          <SwiftAddress>NEDSZAJ0</SwiftAddress>
          <BankType>SenderBank</BankType>
          <Branch>XXX</Branch>
          <AccountCurrency>ZAR</AccountCurrency>
        </SendersBank>
        <RecipientBank>
          <Id>3408560</Id>
          <SwiftAddress>NEDSZAJ0</SwiftAddress>
          <BankType>RecipientBank</BankType>
          <Branch>XSC</Branch>
          <AccountCurrency>ZAR</AccountCurrency>
        </RecipientBank>
        <SendersCorrespondent>
          <Id>3408561</Id>
          <BankType>SendersCorrespondent</BankType>
          <Account>1766000061</Account>
        </SendersCorrespondent>
        <FundsWith>
          <Id>3408569</Id>
          <SwiftAddress>NEDSZAJ0</SwiftAddress>
          <BankType>FundsWith</BankType>
          <Name>DEMAT MONEY MARKET INSTRUMENTS</Name>
          <Address>135 RIVONIA RDSANDTON CITY</Address>
          <Account>1766000061</Account>
          <SortCode/>
          <AccountType>CURRENT</AccountType>
          <AccountCurrency>ZAR</AccountCurrency>
        </FundsWith>
        <Beneficiary>
          <Id>3408562</Id>
          <SwiftAddress>SBZAZAJ0</SwiftAddress>
          <BankType>Beneficiary</BankType>
          <Account>002149168</Account>
          <Branch>XXX</Branch>
          <AccountCurrency>ZAR</AccountCurrency>
        </Beneficiary>
        <AccountWith>
          <Id>3408563</Id>
          <SwiftAddress>SBZAZAJ0</SwiftAddress>
          <BankType>AccountWith</BankType>
          <Name/>
          <Branch>XXX</Branch>
          <AccountCurrency>ZAR</AccountCurrency>
        </AccountWith>
        <SenderToReceiverInfo>/REC/MPDEM</SenderToReceiverInfo>
        <DebitBank>
          <Id>3408570</Id>
          <SwiftAddress>NEDSZAJ0</SwiftAddress>
          <BankType>DebitBank</BankType>
          <Name>DEMAT MONEY MARKET INSTRUMENTS</Name>
          <Address>135 RIVONIA RDSANDTON CITY</Address>
          <Account>1766000061</Account>
          <SortCode/>
          <AccountType>CURRENT</AccountType>
          <AccountCurrency>ZAR</AccountCurrency>
        </DebitBank>
        <CreditBank>
          <Id>3408571</Id>
          <SwiftAddress>ZYABZAJ0</SwiftAddress>
          <BankType>CreditBank</BankType>
          <Name/>
          <Address>N/A</Address>
          <Account>9831103505</Account>
          <Branch/>
          <SortCode/>
          <AccountType>RTGS</AccountType>
          <AccountCurrency>ZAR</AccountCurrency>
        </CreditBank>
        <BeneficiaryAccounts/>
        <PaymentFEC/>
        <OutSendersCorrespondent>
          <Id>3408573</Id>
          <SwiftAddress>ZYABZAJ0</SwiftAddress>
          <BankType>OutSendersCorrespondent</BankType>
          <Name/>
          <Address>N/A</Address>
          <Account>9831103505</Account>
          <Branch/>
          <SortCode/>
          <AccountType>RTGS</AccountType>
          <AccountCurrency>ZAR</AccountCurrency>
        </OutSendersCorrespondent>
        <STP>true</STP>
        <PaymentOutflags>
          <OUT103>false</OUT103>
          <OUT205Sarb>false</OUT205Sarb>
          <OUT205Bank>false</OUT205Bank>
          <OUT202Bank>true</OUT202Bank>
          <OUT191>false</OUT191>
          <MT199>false</MT199>
          <Repaired>false</Repaired>
          <RoutedBackToSaffy>false</RoutedBackToSaffy>
          <PaymentCover>false</PaymentCover>
          <OUT103Y>false</OUT103Y>
          <OUT202YCOV>false</OUT202YCOV>
          <ManualCaptured>false</ManualCaptured>
          <SpecialInstruction>false</SpecialInstruction>
          <SIM>false</SIM>
          <BulkUpload>false</BulkUpload>
          <CLS>false</CLS>
          <Vostro>true</Vostro>
          <NIS>true</NIS>
          <CashOps>false</CashOps>
          <GPI>false</GPI>
          <GPIException>false</GPIException>
          <ManualVerification>false</ManualVerification>
          <AutoFEC>false</AutoFEC>
          <DbtAccInsufftFunds>false</DbtAccInsufftFunds>
          <PayAmountUnderIFILimit>false</PayAmountUnderIFILimit>
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
          <Field72>true</Field72>
          <CorrespondentLookup>false</CorrespondentLookup>
          <BranchLimitChecked>true</BranchLimitChecked>
          <CoverReceived>false</CoverReceived>
          <MT101Msg>false</MT101Msg>
          <NedTreasuryClient>false</NedTreasuryClient>
          <DefaultRecCodeDone>true</DefaultRecCodeDone>
          <GermanPension>false</GermanPension>
        </Indicators>
        <MT202BANK>
          <Id>3408572</Id>
          <SwiftAddress>SBZAZAJ0</SwiftAddress>
          <BankType>MT202Bank</BankType>
          <Name/>
          <Branch>XXX</Branch>
          <SortCode>/REC/DTBNK/</SortCode>
          <AccountCurrency>ZAR</AccountCurrency>
        </MT202BANK>
        <PaymentUserNotesITT/>
      </Payment>
    MessageType: 1
...
'''

streams = '''\
%YAML 1.2
---
Streams:
  - Id: 177781
    MessageId: 65678
    QueueId: GPS-SAFFY-FP
    SourceSysId: GPS
    StreamRef: SAFFYNO109340259
    StreamType: SWIFT
    StreamDescr: '202Vostro=SBZAZAJ0=190307ZAR34000000,'
    MessageData: |
      {1:F01NEDSZAJ0AXXX0000000000}{2:I202SBZAZAJ0XXXXN}{3:{103:ZDS}{113:6100}{108:        11006070}{111:001}{121:8274de8e-865e-437e-a03e-9f0cfe54d90a}}{4:
      :20:V086809340256426
      :21:2019CEM289878
      :32A:190307ZAR34000000,
      :52A:NEDSZAJ0
      :53A:ZYABZAJ0
      :57A:SBZAZAJ0
      :58A:/002149168
      SBZAZAJ0
      :72:/REC/MPDEM
      -}
    MessageType: 1
  - Id: 177782
    MessageId: 65678
    QueueId: WASTE-GPS-FP
    SourceSysId: GPS
    StreamRef: NONE
    StreamType: WASTE
    StreamDescr: 'V086809340256426 GPS DEBIT WASTE - CURRENT'
    MessageData: |
      APUPDNUTF0019APPMOR  08680000090100000000009323FAF 00    99  00000000000000000000000001000000176600006100000868114522019030700000000092501+0003400000000V0868093402564262019CEM289878996    V0868093402564262019CEM289878996V086809340256426 2019CEM28980000000000000000000000000000000000000000000000000000000000 00000000
    MessageType: 1
  - Id: 177783
    MessageId: 65678
    QueueId: WASTE-GPS-FP
    SourceSysId: GPS
    StreamRef: NONE
    StreamType: WASTE
    StreamDescr: 'V086809340256426 GPS CREDIT WASTE - CURRENT'
    MessageData: |
      APUPDNUTF0019APPMOR  08680000090100000000009323FAF 00    99  00000000000000000000000009000000983110350500000868263952019030700000000092501+0003400000000V0868093402564262019CEM289878       V0868093402564262019CEM289878   V086809340256426 2019CEM28980000000000000000000000000000000000000000000000000000000000 00000000    
    MessageType: 1
'''

responses = '''\
%YAML 1.2
---
Response:
  - Id: 150822
    MessageId: 65678
    StreamId: 177782
    StreamRef: NONE
    QueueId: WASTE-GPS-TP
    SourceQueue: WASTE-GPS-FP
    MessageData: |
      APUPDNUTF0019APPMOR  08680000090100000000009323FAF 00    99  00000000000000000000000001000000176600006100000868114522019030700000000092501+0003400000000V0868093402564262019CEM289878996    V0868093402564262019CEM289878996V086809340256426 2019CEM28980000000000000000000000000000000000000000000000000000000000 00000000
    MessageType: 1
  - Id: 150823
    MessageId: 65678
    StreamId: 177783
    StreamRef: NONE
    QueueId: WASTE-GPS-TP
    SourceQueue: WASTE-GPS-FP
    MessageData: |
      APUPDNUTF0019APPMOR  08680000090100000000009323FAF 00    99  00000000000000000000000009000000983110350500000868263952019030700000000092501+0003400000000V0868093402564262019CEM289878       V0868093402564262019CEM289878   V086809340256426 2019CEM28980000000000000000000000000000000000000000000000000000000000 00000000
    MessageType: 1
  - Id: 150823
    MessageId: 65678
    StreamId: 177781
    StreamRef: SAFFYNO109340259
    QueueId: GPS-SAFFY-TP
    SourceQueue: GPS-SAFFY-TP
    MessageData: |
      APUPDNUTF0019APPMOR  08680000090100000000009323FAF 00    99  00000000000000000000000009000000983110350500000868263952019030700000000092501+0003400000000V0868093402564262019CEM289878       V0868093402564262019CEM289878   V086809340256426 2019CEM28980000000000000000000000000000000000000000000000000000000000 00000000
    MessageType: 1
'''

replies = '''\
%YAML 1.2
---
Reply:
  - Id: 12973
    MessageId: 65678
    QueueId: GPS-RFP
    SourceSysId: GPS
    Reference: V086809340256426
    MessageData: |
      [{(PUTTYREPLY:OK)}]
    MessageType: 1
  - Id: 12974
    MessageId: 65678
    QueueId: GPS-RFP
    SourceSysId: GPS
    Reference: V086809340256426
    MessageData: |
      <STREAMRESPONSES COUNT="3" STATUS="SUCCESS">
        <STREAM0 TIMESTAMP="20190307092624" STATUS="2" DESCR="202Vostro=SBZAZAJ0=190307ZAR34000000," REF="SAFFYNO109340259" QUEUEID="GPS-SAFFY-FP" DATECREATED="20190307092501" ID="701772" TYPE="SWIFT"/>
        <STREAM2 ID="701774" QUEUEID="WASTE-GPS-FP" DATECREATED="20190307092501" TYPE="WASTE" TIMESTAMP="20190307092502" STATUS="2" REF="NONE" DESCR="V086809340256426 GPS CREDIT WASTE - CURRENT"/>
        <STREAM1 STATUS="2" ID="701773" TIMESTAMP="20190307092502" DESCR="V086809340256426 GPS DEBIT WASTE - CURRENT" QUEUEID="WASTE-GPS-FP" TYPE="WASTE" REF="NONE" DATECREATED="20190307092501"/>
      </STREAMRESPONSES>
    MessageType: 1
'''

def main():
    m1 = yaml.load(messages)
    print (m1)
    m2 = yaml.load(streams)
    print (m2)
    m3 = yaml.load(responses)
    print (m3)
    m4 = yaml.load(replies)
    print (m4)
    ifile = open('testpack.yaml', 'rt')
    m0 = yaml.load(ifile)
    ifile.close()
    print (m0)

if __name__ == '__main__':
    main()
