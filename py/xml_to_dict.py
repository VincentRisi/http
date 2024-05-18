import xml.sax

sample_data = '''\
<Payment Id="361499" SessionNo="" SequenceNo="">
  <PaymentReference>V0859X0170074499</PaymentReference>
  <RelatedReference>0.6850790597412384</RelatedReference>
  <SendersReference>0.68507905974123</SendersReference>
  <Profile>
    <CapturedUser>CC312454</CapturedUser>
    <AuthorisedBy>GPS - STP</AuthorisedBy>
    <Direction>IN</Direction>
    <Status>SEND_MCPE</Status>
    <ProcessedBy/>
    <AssignedTo>GPS</AssignedTo>
  </Profile>
  <BOPRecs/>
  <SaffyNo>X0170074</SaffyNo>
  <InputDate>0001-01-01T00:00:00</InputDate>
  <OutputDate>0001-01-01T00:00:00</OutputDate>
  <ReceivedDate>2018-10-26T08:44:31.763</ReceivedDate>
  <OriginalBrnNo>0859</OriginalBrnNo>
  <ProcessingBrnNo>0859</ProcessingBrnNo>
  <RoutedFromBrnNo/>
  <Channel>SW</Channel>
  <ChannelType>BK</ChannelType>
  <AdditionalFields>
    <PaymentInfoField>
      <Id>573562</Id>
      <Type>UETR</Type>
      <Value>275cae7b-5018-4e45-adce-12ba82d3880a</Value>
    </PaymentInfoField>
  </AdditionalFields>
  <PaymentType>VostroZarOnUs</PaymentType>
  <SwiftType>202</SwiftType>
  <SwiftMessage>
  {1:F01NEDSZAJ0XCLS9819847312}
  {2:O2026381181026IRVTUS30BXXX15761658771810262746}
  {3:{103:ZCL}{113:7116}{108:181026394263}{111:001}{121:275cae7b-5018-4e45-adce-12ba82d3880a}}
  {4::20:0.6850790597412384:21:0.6850790597412384:32A:181026ZAR28:50K:NEDBANK LIMITED135 RIVONIA ROADSANDTON JOHANNESBURG:52A:NEDSZAJ0:53A:FIRNZAJ0AXXX:57A:NEDSZAJ0XXXX:58A:BOFAGB20CLS:72:/REC/SPCLS-}
  </SwiftMessage>
  <Priority>N/A</Priority>
  <AssignTo>GPS</AssignTo>
  <AssignTo>GPS2</AssignTo>
  <ValueDate>2018-10-26</ValueDate>
  <OriginalValueDate>2018-10-26</OriginalValueDate>
  <ProcessedDate>2018-10-26</ProcessedDate>
  <TmStamp>2018-10-26T08:44:33.45</TmStamp>
  <Principle Pink="gogo">
    Amount
    <Amount>28.0000</Amount>
    Currency
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
    <Amount>28.0000</Amount>
    <Currency>ZAR</Currency>
  </ZAREq>
  <Settlement>
    <Amount>28.0000</Amount>
    <Currency>ZAR</Currency>
  </Settlement>
  <TotalCharge>
    <Amount>0.0000</Amount>
    <Currency>ZAR</Currency>
  </TotalCharge>
  <SendersCharges/>
  <ReceiversCharges/>
  <SendersBank>
    <Id>3267816</Id>
    <SwiftAddress>IRVTUS30</SwiftAddress>
    <BankType>SenderBank</BankType>
    <Account>1986001288</Account>
    <Branch>XXX</Branch>
    <AccountType>VOSTRO</AccountType>
    <AccountCurrency>ZAR</AccountCurrency>
  </SendersBank>
  <RecipientBank>
    <Id>3267817</Id>
    <SwiftAddress>NEDSZAJ0</SwiftAddress>
    <BankType>RecipientBank</BankType>
    <Branch>XCL</Branch>
    <AccountCurrency>ZAR</AccountCurrency>
  </RecipientBank>
  <OrderingCustomer>
    <Id>3267818</Id>
    <BankType>OrderingCustomer</BankType>
    <Name>NEDBANK LIMITED</Name>
    <Address>135 RIVONIA ROAD/?LNSANDTON JOHANNESBURG</Address>
    <AccountCurrency>ZAR</AccountCurrency>
  </OrderingCustomer>
  <OrderingInstitution>
    <Id>3267819</Id>
    <SwiftAddress>NEDSZAJ0</SwiftAddress>
    <BankType>OrderingInstitution</BankType>
    <Branch>XXX</Branch>
  </OrderingInstitution>
  <SendersCorrespondent>
    <Id>3267820</Id>
    <SwiftAddress>FIRNZAJ0</SwiftAddress>
    <BankType>SendersCorrespondent</BankType>
    <Branch>AXX</Branch>
  </SendersCorrespondent>
  <FundsWith>
    <Id>3267823</Id>
    <SwiftAddress>ZYABZAJ0</SwiftAddress>
    <BankType>FundsWith</BankType>
    <Account>9831103505</Account>
    <SortCode/>
    <AccountType>RTGS</AccountType>
    <AccountCurrency>ZAR</AccountCurrency>
  </FundsWith>
  <Beneficiary>
    <Id>3267821</Id>
    <SwiftAddress>BOFAGB20</SwiftAddress>
    <BankType>Beneficiary</BankType>
    <Branch>CLS</Branch>
    <AccountCurrency>ZAR</AccountCurrency>
  </Beneficiary>
  <AccountWith>
    <Id>3267822</Id>
    <SwiftAddress>NEDSZAJ0</SwiftAddress>
    <BankType>AccountWith</BankType>
    <Branch>XXX</Branch>
    <AccountCurrency>ZAR</AccountCurrency>
  </AccountWith>
  <SenderToReceiverInfo>/REC/SPCLS</SenderToReceiverInfo>
  <DebitBank no="1">
    <Id>3267824x</Id>
    <SwiftAddress>ZYABZAJ0</SwiftAddress>
    <BankType>DebitBank</BankType>
    <Account>9831103505</Account>
    <SortCode/>
    <AccountType>RTGS</AccountType>
    <AccountCurrency>ZAR</AccountCurrency>
  </DebitBank>
  <DebitBank no="2">
    Mary had a little lamb
    Its fleece was white as snow
    And everywhere that ...
    <Id>3267824y</Id>
    <SwiftAddress>ZYABZAJ0</SwiftAddress>
    <BankType>DebitBank</BankType>
    <Account>9831103505</Account>
    <SortCode/>
    <AccountType>RTGS</AccountType>
    <AccountCurrency>ZAR</AccountCurrency>
  </DebitBank>
  <CreditBank>
    <Id>3267825</Id>
    <SwiftAddress>BOFAGB20</SwiftAddress>
    <BankType>CreditBank</BankType>
    <Name>BANK OF AMERICA CLS CONTROL BRANCH LONDON</Name>
    <Account>1986251713</Account>
    <Branch>CLS</Branch>
    <AccountType>VOSTRO</AccountType>
    <AccountCurrency>ZAR</AccountCurrency>
  </CreditBank>
  <BeneficiaryAccounts/>
  <PaymentFEC/>
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
    <OUT103Y>false</OUT103Y>
    <OUT202YCOV>false</OUT202YCOV>
    <ManualCaptured>false</ManualCaptured>
    <SpecialInstruction>false</SpecialInstruction>
    <SIM>false</SIM>
    <BulkUpload>false</BulkUpload>
    <CLS>false</CLS>
    <Vostro>true</Vostro>
    <NIS>false</NIS>
    <CashOps>false</CashOps>
    <GPI>true</GPI>
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
  <PaymentUserNotesITT/>
</Payment>
'''

chopped_data = '''\
<Document xmlns="urn:iso:std:iso:20022:tech:xsd:pain.001.001.07">
   <CstmrCdtTrfInitn>
      <GrpHdr>
         <MsgId>1903050822236102</MsgId>
         <CreDtTm>2019-03-05T08:25:21.153639+02:00</CreDtTm>
         <NbOfTxs>1</NbOfTxs>
         <InitgPty>
            <Nm>Nedbank</Nm>
            <Id>
               <OrgId>
                  <AnyBIC>NEDSZAJJ</AnyBIC>
                  <Othr>
                     <Id>ECOBANK</Id>
                  </Othr>
               </OrgId>
            </Id>
         </InitgPty>
      </GrpHdr>
      <PmtInf>
         <PmtInfId>1903050822236102</PmtInfId>
         <PmtMtd>TRF</PmtMtd>
         <NbOfTxs>1</NbOfTxs>
         <PmtTpInf>
            <SvcLvl>
               <Cd>SEPA</Cd>
            </SvcLvl>
            <CtgyPurp>
               <Cd>BENE</Cd>
            </CtgyPurp>
         </PmtTpInf>
         <ReqdExctnDt>2019-03-05</ReqdExctnDt>
         <Dbtr>
            <Nm>Margaret De Vries</Nm>
            <PstlAdr>
               <PstCd>04001</PstCd>
               <TwnNm>DURBAN</TwnNm>
               <Ctry>ZA</Ctry>
               <AdrLine>1402 CARILLON</AdrLine>
               <AdrLine>22 PRINCE STREET</AdrLine>
            </PstlAdr>
            <CtryOfRes>ZA</CtryOfRes>
         </Dbtr>
         <DbtrAcct>
            <Id>
               <Othr>
                  <Id>2305958897</Id>
               </Othr>
            </Id>
            <Ccy>ZAR</Ccy>
         </DbtrAcct>
         <DbtrAgt>
            <FinInstnId>
               <BICFI>NEDSZAJJ</BICFI>
            </FinInstnId>
         </DbtrAgt>
         <CdtTrfTxInf>
            <PmtId>
               <InstrId>1903050822236102</InstrId>
               <EndToEndId>Oliseh</EndToEndId>
            </PmtId>
            <Amt>
               <EqvtAmt>
                  <Amt Ccy="ZAR">10000.00</Amt>
                  <CcyOfTrf>USD</CcyOfTrf>
               </EqvtAmt>
            </Amt>
            <XchgRateInf>
               <UnitCcy>USD</UnitCcy>
               <XchgRate>14.38720</XchgRate>
               <RateTp>AGRD</RateTp>
               <CtrctId>1903050000791</CtrctId>
            </XchgRateInf>
            <IntrmyAgt1>
               <FinInstnId>
                  <BICFI>ECOCGHAC</BICFI>
               </FinInstnId>
            </IntrmyAgt1>
            <CdtrAgt>
               <FinInstnId>
                  <BICFI>ECOCFRPP</BICFI>
               </FinInstnId>
            </CdtrAgt>
            <CdtrAgtAcct>
               <Id>
                  <Othr>
                     <Id>5810010199</Id>
                  </Othr>
               </Id>
               <Ccy>USD</Ccy>
            </CdtrAgtAcct>
            <Cdtr>
               <Nm>EKERUCHE OLISEH KELVIN EKERUCHE OLISEH KELVIN</Nm>
               <PstlAdr>
                  <PstCd>NG9999</PstCd>
                  <TwnNm>Abraka</TwnNm>
                  <CtrySubDvsn>Abraka</CtrySubDvsn>
                  <Ctry>NG</Ctry>
                  <AdrLine>48 Ivie road</AdrLine>
                  <AdrLine>Abraka</AdrLine>
               </PstlAdr>
            </Cdtr>
            <CdtrAcct>
               <Id>
                  <Othr>
                     <Id>2920000347</Id>
                  </Othr>
               </Id>
               <Ccy>NGN</Ccy>
            </CdtrAcct>
            <RgltryRptg>
               <Dtls>
                  <Tp>B.4(A)(X)</Tp>
                  <Ctry>NG</Ctry>
                  <Cd>401</Cd>
                  <Amt Ccy="USD">695.06</Amt>
               </Dtls>
            </RgltryRptg>
         </CdtTrfTxInf>
      </PmtInf>
      <SplmtryData>
         <PlcAndNm>Charges</PlcAndNm>
         <Envlp>
            <Charges>
               <Charge>
                  <CommissionAccount>9579647003      </CommissionAccount>
                  <ChargeCode>0001</ChargeCode>
                  <Description>  FGN D+T : TTS INWDS</Description>
                  <Amount>350.00</Amount>
                  <VAT>Y</VAT>
                  <VATAmount>52.50</VATAmount>
                  <VATPercentage>15.00</VATPercentage>
                  <VATAccount>9579174000      </VATAccount>
               </Charge>
            </Charges>
         </Envlp>
      </SplmtryData>
   </CstmrCdtTrfInitn>
</Document>
'''

class DictHandler(xml.sax.ContentHandler):
    def __init__(self, mem_char='.', att_char='/'):
        self.tags = []
        self.tags_kv = {}
        self.tags_value = {}
        self.result = {}
        self.mem_char = mem_char;
        self.att_char = att_char;
    def startElement(self, name, attrs):
        self.tags.append(name)
        self.tags_kv[name] = []
        self.tags_value[name] = []
        for attr in attrs.getNames():
            self.tags_kv[name].append((attr, attrs.getValue(attr)))
    def endElement(self, name):
        def _add_value(result, name, value):
            if name in result:
                if not isinstance(result[name], list):
                    prev_value = result[name]
                    result[name] = [prev_value]
                result[name].append(value)
            else:
                result[name]=value 
        dname = self.tags[0]
        for tag in self.tags[1:]:
            dname += self.mem_char + tag
        self.tags.pop()
        value = ''
        nl = ''
        for line in self.tags_value[name]:
            value += nl + line
            nl = '\n'
        _add_value(self.result, dname, value)
        if name in self.tags_kv:
            for k, v in self.tags_kv[name]:
                aname = dname+self.att_char+k
                _add_value(self.result, aname, v)
    def characters(self, content):
        store = content.strip()
        if len(store) > 0:
            name = self.tags[-1]
            self.tags_value[name].append(store)

def process(dict):
    with open(r'c:\nedbank\custom\python3\bbd-stdfront\source\misc-tests\xmlrecord-test\keyvalue2.bak', 'wt') as ofile:
        no1, no2, no3 = 1, 1, 1
        for key in dict:
            if dict[key] != '':
                value = dict[key]
                no1 = 0
                if not isinstance(value, list):
                    print (f'    bi.Fill(2, "{key}", {no1}, {no2+3}, {no3}, "{value}", "SAMSON", "20190619134601", "");')
                    ofile.write(f"{key}='{value}'\n")
                else:
                    for i, v in enumerate(value):
                        print (f'    bi.Fill(2, "{key}", {no1+i}, {no2+3}, {no3}, "{v}", "SAMSON", "20190619134601", "");')
                        ofile.write(f"{key}-{i}='{v}'\n")
                no2 += 1
                no3 += 1
        a = 4

def main1():
    with open(r'c:\nedbank\custom\python3\bbd-stdfront\source\misc-tests\xmlrecord-test\Document.xml', 'rt') as ifile:
        fred_data = ifile.read()
    handler = DictHandler()
    xml.sax.parseString(fred_data, handler)
    dict = handler.result
    process(dict)

def main2():
    from xmlutils import xmlparse
    with open(r'c:\nedbank\custom\python3\bbd-stdfront\source\misc-tests\xmlrecord-test\Document.xml', 'rt') as ifile:
        fred_data = ifile.read()
    dict = xmlparse(fred_data)
    process(dict)

def main3():
    import xmlutils
    with open(r'C:\nedbank\custom\python3\bbd-stdfront\source\misc-tests\xmlrecord-test\eximbills.xml', 'rt') as ifile:
        eximbills = ifile.read()
        print (eximbills)
    dict = xmlutils.xmlparse(eximbills)
    rec = xmlutils.dict_to_rec(dict)
    print ('**********************')
    xmlutils.printClass(rec, 'Transaction')
    print ('**********************')
    lines = xmlutils.dict_to_xml(dict)
    for line in lines:
        print (line)
    print ('**********************')
    lines = xmlutils.dict_to_json(dict)
    print (lines)
    print ('**********************')
    lines = xmlutils.dict_to_yaml(dict)
    print (lines)
    print ('**********************')
    xml = xmlutils.xmlbuild(dict)
    print (xml)

if __name__ == '__main__':
    main3()
