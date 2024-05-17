/// ------------------------------------------------------------------
/// Copyright (c) 1996, 2007 Vincent Risi in Association 
///                          with Barone Budge and Dominick 
/// All rights reserved. 
/// This program and the accompanying materials are made available 
/// under the terms of the Common Public License v1.0 
/// which accompanies this distribution and is available at 
/// http://www.eclipse.org/legal/cpl-v10.html 
/// Contributors:
///    Vincent Risi
/// ------------------------------------------------------------------
/// System : JPortal
/// ------------------------------------------------------------------

package vlab.jportal.code;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.util.Properties;
import java.util.Vector;

import static vlab.jportal.TJCStructs.*;
import static vlab.jportal.Writer.*;
import vlab.jportal.*;

public class PostgreCCode extends Generator
{
  static PlaceHolder placeHolder;
  static boolean useTStyle = true;
  /**
   * Build of output data rec for standard procedures
   */
  static Vector nullVector = new Vector();
  static String structName = "";
  private static Properties properties;
  private static PrintWriter outLog;

  public static String description()
  {
    return "Generate tStyle C++ Code for PostgreSQL";
  }

  public static String documentation()
  {
    return "Generate tStyle C++ Code for PostgreSQL";
  }

  /**
   * Generates the procedure classes for each table present.
   */
  public static void generate(Database database, String output, PrintWriter outLog)
  {
    PostgreCCode.outLog = outLog;
    try
    {
      String propertiesName = format("%s%s.properties", output, database.name);
      InputStream input = new FileInputStream(propertiesName);
      properties = new Properties();
      properties.load(input);
    }
    catch (Exception ex)
    {
      properties = null;
    }
    for (int i = 0; i < database.flags.size(); i++)
    {
      String flag = database.flags.elementAt(i);
      flag = flag.toLowerCase();
      if (flag.equalsIgnoreCase("usetstyle") || flag.equalsIgnoreCase("use tstyle"))
        useTStyle = true;
    }
    useTStyle = getProperty("usetstyle", useTStyle);
    for (int i = 0; i < database.tables.size(); i++)
    {
      try
      {
        Table table = database.tables.elementAt(i);
        generate(table, output);
        generateSnips(table, output, outLog, false);
      }
      catch (Exception ex)
      {
        outLog.println(ex);
        ex.printStackTrace(outLog);
      }
    }
  }

  /**
   * Build of standard and user defined procedures
   */
  static void generate(Table table, String output)
  {
    boolean stdExtend = true;
    try
    {
      outLog.println("Code: " + fileName(output, table.useName().toLowerCase(), ".sh"));
      OutputStream outFile = new FileOutputStream(fileName(output, table.useName().toLowerCase(), ".sh"));
      try
      {
        writer = new PrintWriter(outFile);
        indent_size = 4;
        try
        {
          writeln("// This code was generated, do not modify it, modify it at source and regenerate it.");
          writeln("#ifndef _" + table.useName().toLowerCase() + "SH");
          writeln("#define _" + table.useName().toLowerCase() + "SH");
          writeln();
          writeln("#include \"machine.h\"");
          writeln("#include <stddef.h>");
          writeln("#include <stdlib.h>");
          writeln("#include <string.h>");
          writeln("#include \"padgen.h\"");
          writeln("#include \"pgapi.h\"");
          writeln("#include \"xmlrecord.h\"");
          writeln("#include \"pgdatabuild.h\"");
          writeln();
          if (table.hasStdProcs)
            stdExtend = generateMainStruct(table);
          generateOtherStructs(table);
          writeln("#endif");
          writer.flush();
        }
        finally
        {
          writer.flush();
        }
        outFile.close();
        outLog.println("Code: " + fileName(output, table.useName().toLowerCase(), ".cpp"));
        outFile = new FileOutputStream(
                fileName(output, table.useName().toLowerCase(), ".cpp"));
        writer = new PrintWriter(outFile);
        try
        {
          writeln("// This code was generated, do not modify it, modify it at source and regenerate it.");
          writeln();
          writeln("#include \"" + fileName("", table.useName().toLowerCase(), ".sh")
                  + "\"");
          writeln();
          generateImplementation(table, stdExtend);
        }
        finally
        {
          writer.flush();
        }
      }
      finally
      {
        outFile.close();
      }
    }
    catch (IOException e1)
    {
      outLog.println("Generate Procs IO Error");
    }
  }

  static boolean generateMainStruct(Table table)
  {
    for (int i = 0; i < table.comments.size(); i++)
    {
      String s = table.comments.elementAt(i);
      writeln("//" + s);
    }
    int filler = 0;
    structName = "t" + table.useName();
    writeln("struct t" + table.useName());
    writeln("{");
    boolean canExtend = true;
    Vector fields = table.fields;
    for (int i = 0; i < fields.size(); i++)
    {
      Field field = (Field) fields.elementAt(i);
      if (field.type == Field.BLOB)
        canExtend = false;
      writeln(1, padder(cppVar(field) + ";", 48) + generatePadding(field, filler++));
      if (isNull(field))
      {
        writeln(1, padder("int16  " + field.useName() + "IsNull;", 48) + generatePadding(filler++));
        filler++;
      }
    }
    generateMainStructInterface(table);
    generateSwapsInterface("", fields, null);
    String useName = table.useName();
    if (canExtend == true)
      generateHeaderInterface("", fields, useName, nullVector, null);
    else
      generateDataBuildInterface("", fields, useName, nullVector, null);
    writeln("};");
    writeln();
    return canExtend;
  }

  static void generateOtherStructs(Table table)
  {
    for (int i = 0; i < table.procs.size(); i++)
    {
      Proc proc = table.procs.elementAt(i);
      if (proc.isData || proc.isStd || proc.hasNoData())
        continue;
      if (proc.isStdExtended())
        continue;
      String work = "";
      String baseClass = "";
      boolean canExtend = true;
      Vector fields = proc.outputs;
      for (int j = 0; j < fields.size(); j++)
      {
        Field field = (Field) fields.elementAt(j);
        if (field.type == Field.BLOB)
          canExtend = false;
      }
      fields = proc.inputs;
      for (int j = 0; j < fields.size(); j++)
      {
        Field field = (Field) fields.elementAt(j);
        if (field.type == Field.BLOB)
          canExtend = false;
      }
      structName = "t" + table.useName() + proc.upperFirst();
      writeln("struct t" + table.useName() + proc.upperFirst() + work);
      writeln("{");
      int filler = 0;
      Vector inputs = proc.inputs;
      for (int j = 0; j < inputs.size(); j++)
      {
        Field field = (Field) inputs.elementAt(j);
        if (proc.hasOutput(field.name))
          continue;
        for (int c = 0; c < field.comments.size(); c++)
        {
          String s = field.comments.elementAt(c);
          writeln(1, "//" + s);
        }
        writeln(1, padder(cppVar(field) + ";", 48) + generatePadding(field, filler++));
        if (isNull(field))
          writeln(1, padder("int16  " + field.useName() + "IsNull;", 48) + generatePadding(filler++));
      }
      for (int j = 0; j < proc.dynamics.size(); j++)
      {
        String s = proc.dynamics.elementAt(j);
        Integer n = proc.dynamicSizes.elementAt(j);
        writeln(1, padder("char " + s + "[" + (n.intValue() + 1) + "];", 48) + charPadding(n.intValue() + 1, filler++));
      }
      fields = proc.outputs;
      if (fields.size() > 0)
      {
        for (int j = 0; j < fields.size(); j++)
        {
          Field field = (Field) fields.elementAt(j);
          for (int c = 0; c < field.comments.size(); c++)
          {
            String s = field.comments.elementAt(c);
            writeln(1, "//" + s);
          }
          writeln(1, padder(cppVar(field) + ";", 48) + generatePadding(field, filler++));
          if (isNull(field))
            writeln(1, padder("int16  " + field.useName() + "IsNull;", 48) + generatePadding(filler++));
        }
      }
      generateOtherInterface(table, proc);
      generateSwapsInterface(baseClass, inputs, proc);
      String useName = table.useName() + proc.upperFirst();
      if (canExtend == true)
        generateHeaderInterface(baseClass, inputs, useName, proc.dynamics, proc);
      else
        generateDataBuildInterface(baseClass, inputs, useName, proc.dynamics, proc);
      writeln("};");
      writeln();
    }
  }

  static void generateCommand(Proc proc)
  {
    Vector lines = placeHolder.getLines();
    int size = 1;
    for (int i = 0; i < lines.size(); i++)
    {
      String l = (String) lines.elementAt(i);
      if (l.charAt(0) == '"')
        size += (l.length() + 2);
      else
      {
        String var = l.trim();
        for (int j = 0; j < proc.dynamics.size(); j++)
        {
          String s = proc.dynamics.elementAt(j);
          if (var.compareTo(s) == 0)
          {
            Integer n = proc.dynamicSizes.elementAt(j);
            size += (n.intValue() + 2);
          }
        }
      }
    }
    writeln(1, "if (q_->command == 0)");
    writeln(2, "q_->command = new char [" + size + "];");
    writeln(1, "memset(q_->command, 0, sizeof(q_->command));");
    String strcat = "strcat(q_->command, ";
    String terminate = "";
    if (lines.size() > 0)
    {
      for (int i = 0; i < lines.size(); i++)
      {
        String l = (String) lines.elementAt(i);
        if (l.charAt(0) != '"')
        {
          terminate = ");";
          strcat = "  strcat(q_->command, ";
          writeln(terminate);
        } else if (i != 0)
          writeln(terminate);
        write(1, strcat + l);
        if (l.charAt(0) == '"')
        {
          terminate = "\"\\n\"";
          strcat = "                   ";
        }
      }
      writeln(");");
    }
  }

  /**
   * generate Holding variables
   */
  static void generateCppBind(Field field)
  {
    switch (field.type)
    {
      case Field.BOOLEAN:
      case Field.BYTE:
      case Field.SHORT:
        writeln(1, "int16 " + field.useName() + "_INT16;");
        break;
      case Field.INT:
      case Field.SEQUENCE:
      case Field.IDENTITY:
        writeln(1, "int32 " + field.useName() + "_INT32;");
        break;
      case Field.LONG:
        writeln(1, "int64 " + field.useName() + "_INT64;");
        break;
      case Field.FLOAT:
      case Field.DOUBLE:
        if (field.precision <= 15)
          writeln(1, "double " + field.useName() + "_DOUBLE;");
        break;
      case Field.DATE:
        writeln(1, "PgTDate " + field.useName() + "_PGDate;");
        break;
      case Field.TIME:
        writeln(1, "PgTTime " + field.useName() + "_PGTime;");
        break;
      case Field.DATETIME:
        writeln(1, "PgTDateTime " + field.useName() + "_PGDateTime;");
        break;
      case Field.TIMESTAMP:
        writeln(1, "PgTDateTime " + field.useName() + "_PGTimeStamp;");
        break;
    }
  }

  public static void generateEnumOrdinals(Table table)
  {
    for (int i = 0; i < table.fields.size(); i++)
    {
      Field field = table.fields.elementAt(i);
      if (field.enums.size() > 0)
      {
        writeln("enum e" + table.useName() + field.useName());
        String start = "{";
        for (int j = 0; j < field.enums.size(); j++)
        {
          Enumerator element = field.enums.elementAt(j);
          String evalue = "" + element.value;
          if (field.type == Field.ANSICHAR && field.length == 1)
            evalue = "'" + (char) element.value + "'";
          writeln(start + " " + table.useName() + field.useName() + element.name + " = " + evalue);
          start = ",";
        }
        writeln("};");
        writeln();
        writeln("inline const char *" + table.useName() + field.useName() + "Lookup(int no)");
        writeln("{");
        writeln(1, "switch(no)");
        writeln(1, "{");
        for (int j = 0; j < field.enums.size(); j++)
        {
          Enumerator element = field.enums.elementAt(j);
          String evalue = "" + element.value;
          if (field.type == Field.ANSICHAR && field.length == 1)
            evalue = "'" + (char) element.value + "'";
          writeln(1, "case " + evalue + ": return \"" + element.name + "\";");
        }
        writeln(1, "default: return \"<unknown value>\";");
        writeln(1, "}");
        writeln("}");
      }
    }
  }

  /**
   *
   */
  static void generateImplementation(Table table, boolean stdExtend)
  {
    for (int i = 0; i < table.procs.size(); i++)
    {
      Proc proc = table.procs.elementAt(i);
      if (proc.isData)
        continue;
      if (proc.isStd || proc.isStdExtended() || proc.hasNoData())
        generateImplementation(table, proc);
    }
    structName = format("t%s", table.name);
    if (stdExtend == true)
      generateHeaderImplementation("", table.fields, table.name, nullVector, null);
    else
      generateDataBuildImplementation("", table.fields, table.name, nullVector, null);
    generateSwapsImplementation("", table.fields, null);
    for (int i = 0; i < table.procs.size(); i++)
    {
      Proc proc = table.procs.elementAt(i);
      if (proc.isData)
        continue;
      if (proc.isStd || proc.isStdExtended() || proc.hasNoData())
        continue;
      generateImplementation(table, proc);
    }
  }

  /**
   * Emits class method for processing the database activity
   */
  static void generateImplementation(Table table, Proc proc)
  {
    placeHolder = new PlaceHolder(proc, PlaceHolder.DOLLAR_NO, "");
    String fullName = table.useName() + proc.name;
    boolean standard = (proc.isStd || proc.isStdExtended() || proc.hasNoData());
    if (standard)
      fullName = format("%s::%s", table.useName(), proc.upperFirst());
    else
      fullName = format("%s%s::", table.useName(), proc.upperFirst());
    writeln("void t" + fullName + "Exec()");
    writeln("{");
    generateCommand(proc);
    writeln(1, "q_->Open(q_->command, " + proc.inputs.size() + ", " + proc.outputs.size() + ");");
    for (int j = 0; j < proc.inputs.size(); j++)
    {
      Field field = proc.inputs.elementAt(j);
      generateCppBind(field);
    }
    for (int j = 0; j < placeHolder.pairs.size(); j++)
    {
      PlaceHolderPairs pair = placeHolder.pairs.elementAt(j);
      Field field = pair.field;
      writeln(1, "q_->Bind" + (field.type == Field.BLOB ? "Blob" : "") + "("
              + padder("" + j + ",", 4)
              + cppBind(field, table.name, proc.isInsert)
              + ((isNull(field)) ? ", &" + field.useName() + "IsNull" : "")
              + ((field.type == Field.ANSICHAR) ? ", 1" : "")
              + ");");
    }
    writeln(1, "q_->Exec();");
    writeln("}");
    writeln();
    boolean skipExecWithParms = false;
    for (int j = 0; j < proc.inputs.size(); j++)
    {
      Field field = proc.inputs.elementAt(j);
      if (field.type == Field.BLOB)
      {
        skipExecWithParms = true;
        break;
      }
    }
    if (skipExecWithParms == false)
    {
      if ((proc.inputs.size() > 0) || proc.dynamics.size() > 0)
      {
        writeln("void t" + fullName + "Set(");
        generateWithParms(proc, 1);
        writeln(")");
        writeln("{");
        for (int j = 0; j < proc.inputs.size(); j++)
        {
          Field field = proc.inputs.elementAt(j);
          if ((field.type == Field.SEQUENCE && proc.isInsert)
                  || field.type == Field.IDENTITY || field.type == Field.TIMESTAMP
                  || field.type == Field.USERSTAMP)
            continue;
          writeln(1, cppCopy(field));
        }
        for (int j = 0; j < proc.dynamics.size(); j++)
        {
          String s = proc.dynamics.elementAt(j);
          writeln(1, "strncpy(" + s + ", a" + s + ", sizeof(" + s
                  + ")-1);");
        }
        //if (standard)
        //  writeln(1, format("%sExec();", proc.upperFirst()));
        //else
        //  writeln(1, "Exec();");
        writeln("}");
        writeln();
      }
    }
    if (proc.outputs.size() > 0)
    {
      writeln("bool t" + fullName + "Fetch()");
      writeln("{");
      writeln(1, "if (q_->Fetch() == false)");
      writeln(2, "return false;");
      for (int j = 0; j < proc.outputs.size(); j++)
      {
        Field field = proc.outputs.elementAt(j);
        writeln(1, "q_->Get(" + j + ", " + cppGet(field) + ");");
        if (isNull(field))
          writeln(1, "q_->GetNull(" + j + ", " + field.useName() + "IsNull);");
      }
      writeln(1, "return true;");
      writeln("}");
      if (proc.isSingle)
      {
        writeln("bool t" + fullName + "Single()");
        writeln("{");
        if (standard)
        {
          writeln(1, format("%sExec();", proc.upperFirst()));
          writeln(1, format("return %sFetch();", proc.upperFirst()));
        } else
        {
          writeln(1, "Exec();");
          writeln(1, "return Fetch();");
        }
        writeln("}");
      }
      writeln();
    }
  }

  private static String generatePadding(Field field, int fillerNo)
  {
    switch (field.type)
    {
      case Field.BOOLEAN:
      case Field.BYTE:
      case Field.SHORT:
        return "IDL2_INT16_PAD(" + fillerNo + ");";
      case Field.INT:
      case Field.SEQUENCE:
      case Field.IDENTITY:
        return "IDL2_INT32_PAD(" + fillerNo + ");";
      //case Field.LONG:
      //  return "// IDL2_INT64_PAD(" + fillerNo + ");";
      case Field.CHAR:
      case Field.ANSICHAR:
      case Field.USERSTAMP:
      case Field.TLOB:
      case Field.DATE:
      case Field.TIME:
      case Field.DATETIME:
      case Field.TIMESTAMP:
        return charPadding(field.length + 1, fillerNo);
      case Field.DOUBLE:
      case Field.FLOAT:
        if (field.precision > 15)
          return charPadding(field.precision + 3, fillerNo);
        break;
      case Field.MONEY:
        return charPadding(21, fillerNo);
    }
    return "";
  }

  public static String generatePadding(int fillerNo)
  {
    return "IDL2_INT16_PAD(" + fillerNo + ");";
  }

  /**
   * Build of output data rec for standard procedures
   */
  static void generateMainStructInterface(Table table)
  {
    boolean hasQuery = false;
    for (int i = 0; i < table.procs.size(); i++)
    {
      Proc proc = table.procs.elementAt(i);
      if (proc.isStd || proc.isStdExtended() || proc.hasNoData())
      {
        if (hasQuery == false)
        {
          writeln(1, "PgQuery *q_;");
          hasQuery = true;
        }
        generateMainStructInterface(table, proc);
      }
    }
  }

  static void generateMainStructInterface(Table table, Proc proc)
  {
    String dataStruct;
    dataStruct = "t" + table.useName();
    generateMainInterface(table, proc, dataStruct);
  }

  static void generateMainInterface(Table table, Proc proc, String dataStruct)
  {
    writeln(1, format("void %sExec();", proc.upperFirst()));
    boolean skipExecWithParms = false;
    for (int j = 0; j < proc.inputs.size(); j++)
    {
      Field field = proc.inputs.elementAt(j);
      if (field.type == Field.BLOB)
      {
        skipExecWithParms = true;
        break;
      }
    }
    if (skipExecWithParms == false)
    {
      if ((proc.inputs.size() > 0) || proc.dynamics.size() > 0)
      {
        writeln(1, format("void %sSet(", proc.upperFirst()));
        generateWithParms(proc, 2);
        writeln(1, ");");
      }
    }
    if (proc.outputs.size() > 0)
    {
      writeln(1, format("bool %sFetch();", proc.upperFirst()));
      if (proc.isSingle)
        writeln(1, format("bool %sSingle();", proc.upperFirst()));
    }
  }

  static void generateOtherInterface(Table table, Proc proc)
  {
    String dataStruct;
    if (proc.isStdExtended() || proc.isStd)
      dataStruct = "t" + table.useName();
    else
      dataStruct = "t" + table.useName() + proc.upperFirst();
    generateOtherInterface(table, proc, dataStruct);
  }

  static void generateOtherInterface(Table table, Proc proc, String dataStruct)
  {
    writeln(1, "PgQuery *q_;");
    writeln(1, "void Exec();");
    boolean skipExecWithParms = false;
    for (int j = 0; j < proc.inputs.size(); j++)
    {
      Field field = proc.inputs.elementAt(j);
      if (field.type == Field.BLOB)
      {
        skipExecWithParms = true;
        break;
      }
    }
    if (skipExecWithParms == false)
    {
      if ((proc.inputs.size() > 0) || proc.dynamics.size() > 0)
      {
        writeln(1, "void Set(");
        generateWithParms(proc, 2);
        writeln(1, ");");
      }
    }
    if (proc.outputs.size() > 0)
    {
      writeln(1, "bool Fetch();");
      if (proc.isSingle)
        writeln(1, "bool Single();");
    }
  }

  static void generateWithParms(Proc proc, int tab)
  {
    String comma = "  ";
    for (int j = 0; j < proc.inputs.size(); j++)
    {
      Field field = proc.inputs.elementAt(j);
      if ((field.type == Field.SEQUENCE && proc.isInsert)
              || field.type == Field.IDENTITY || field.type == Field.TIMESTAMP
              || field.type == Field.USERSTAMP)
        continue;
      writeln(tab, comma + "const " + cppParm(field));
      comma = ", ";
    }
    for (int j = 0; j < proc.dynamics.size(); j++)
    {
      String s = proc.dynamics.elementAt(j);
      writeln(tab, comma + "const char*   a" + s);
      comma = ", ";
    }
  }

  private static void generateDataBuildInterface(String baseClass, Vector inputs, String useName, Vector dynamics, Proc proc)
  {
    writeln("#if defined(_DATABUILD_H_)");
    writeln(1, "static int NoBuildFields();");
    writeln(1, format("void BuildData(DataBuilder &dBuild, char *name=\"%s\");", useName));
    writeln(1, format("void SetData(DataBuilder &dBuild, char *name=\"%s\");", useName));
    writeln("protected:");
    writeln(1, "void _buildAdds(DataBuilder &dBuild);");
    writeln(1, "void _buildSets(DataBuilder &dBuild);");
    writeln("public:");
    writeln("#endif");
  }

  private static void generateDataBuildImplementation(String baseClass, Vector inputs, String useName, Vector dynamics, Proc proc)
  {
    writeln("#if defined(_DATABUILD_H_)");
    int inputNo = inputs.size();
    if (baseClass.length() > 0)
    {
      for (int j = 0; j < inputs.size(); j++)
      {
        Field field = (Field) inputs.elementAt(j);
        if (proc != null && proc.hasOutput(field.name))
          continue;
        inputNo++;
      }
      writeln(format("int %s::NoBuildFields() {return %s::NoBuildFields()+%d;}", structName, baseClass, (inputNo + dynamics.size())));
    } else
      writeln(format("int %s::NoBuildFields() {return %d;}", structName, (inputNo + dynamics.size())));
    writeln(format("void %s::_buildAdds(DataBuilder &dBuild)", structName));
    writeln("{");
    if (baseClass.length() > 0)
      writeln(1, baseClass + "::_buildAdds(dBuild);");
    for (int j = 0; j < inputs.size(); j++)
    {
      Field field = (Field) inputs.elementAt(j);
      if (proc != null && proc.hasOutput(field.name))
        continue;
      if (field.type == Field.BLOB)
        writeln(1, "dBuild.add(\"" + field.useName() + "\", " + field.useName() + ".len, " + field.useName() + ".data" + nullAdd(field) + ");");
      else
        writeln(1, "dBuild.add(\"" + field.useName() + "\", " + field.useName() + nullAdd(field) + ");");
    }
    for (int j = 0; j < dynamics.size(); j++)
    {
      String str = (String) dynamics.elementAt(j);
      writeln(1, "dBuild.add(\"" + str + "\", " + str + ");");
    }
    writeln("}");
    writeln(format("void %s::BuildData(DataBuilder &dBuild, char *name)", structName));
    writeln("{");
    writeln(1, "dBuild.name(name);");
    writeln(1, "_buildAdds(dBuild);");
    writeln("}");
    writeln(format("void %s::_buildSets(DataBuilder &dBuild)", structName));
    writeln("{");
    if (baseClass.length() > 0)
      writeln(1, baseClass + "::_buildSets(dBuild);");
    for (int j = 0; j < inputs.size(); j++)
    {
      Field field = (Field) inputs.elementAt(j);
      if (proc != null && proc.hasOutput(field.name))
        continue;
      if (field.type == Field.BLOB)
        writeln(1, "dBuild.set(\"" + field.useName() + "\", " + field.useName() + ".len, " + field.useName() + ".data, sizeof(" + field.useName() + ".data)" + nullSet(field) + ");");
      else
        writeln(1, "dBuild.set(\"" + field.useName() + "\", " + field.useName() + ", sizeof(" + field.useName() + ")" + nullSet(field) + ");");
    }
    for (int j = 0; j < dynamics.size(); j++)
    {
      String str = (String) dynamics.elementAt(j);
      writeln(1, "dBuild.set(\"" + str + "\", " + str + ", sizeof(" + str + "));");
    }
    writeln("}");
    writeln(format("void %s::SetData(DataBuilder &dBuild, char *name)", structName));
    writeln("{");
    writeln(1, "dBuild.name(name);");
    writeln(1, "_buildSets(dBuild);");
    writeln("}");
    writeln("#endif");
    writeln();
  }

  private static void generateHeaderInterface(String baseClass, Vector inputs, String useName, Vector dynamics, Proc proc)
  {
    writeln("#if defined(_TBUFFER_H_)");
    writeln(1, "void ToXML(TBAmp &XRec, char* Attr=0, char* Outer=\"" + useName + "\");");
    writeln("protected:");
    writeln(1, "void _toXML(TBAmp &XRec);");
    writeln("public:");
    writeln("#endif");
    writeln("#if defined(__XMLRECORD_H__)");
    writeln(1, "void FromXML(TBAmp &XRec);");
    writeln("protected:");
    writeln(1, "void _fromXML(TBAmp &XRec, TXMLRecord &msg);");
    writeln("public:");
    writeln("#endif");

    generateDataBuildInterface(baseClass, inputs, useName, dynamics, proc);
  }

  private static void generateHeaderImplementation(String baseClass, Vector inputs, String useName, Vector dynamics, Proc proc)
  {
    writeln("#if defined(_TBUFFER_H_)");
    writeln(format("void %s::_toXML(TBAmp &XRec)", structName));
    writeln("{");
    if (baseClass.length() > 0)
      writeln(1, baseClass + "::_toXML(XRec);");
    for (int j = 0; j < inputs.size(); j++)
    {
      Field field = (Field) inputs.elementAt(j);
      if (proc != null && proc.hasOutput(field.name))
        continue;
      writeln(1, toXMLTFormat(field));
    }
    for (int j = 0; j < dynamics.size(); j++)
    {
      String str = (String) dynamics.elementAt(j);
      String front = "XRec.append(\"  <" + str + ">\");";
      String back = "XRec.append(\"</" + str + ">\");";
      writeln(1, front + "XRec.ampappend(" + str + ");" + back);
    }
    writeln("}");
    writeln(format("void %s::ToXML(TBAmp &XRec, char* Attr, char* Outer)", structName));
    writeln("{");
    writeln(1, "XRec.append(\"<\");XRec.append(Outer);if (Attr) XRec.append(Attr);XRec.append(\">\\n\");");
    writeln(1, "_toXML(XRec);");
    writeln(1, "XRec.append(\"</\");XRec.append(Outer);XRec.append(\">\\n\");");
    writeln("}");
    writeln("#endif");
    writeln();
    writeln("#if defined(__XMLRECORD_H__)");
    writeln(format("void %s::_fromXML(TBAmp &XRec, TXMLRecord &msg)", structName));
    writeln("{");
    writeln(1, "TBAmp work;");
    if (baseClass.length() > 0)
      writeln(1, baseClass + "::_fromXML(XRec, msg);");
    for (int j = 0; j < inputs.size(); j++)
    {
      Field field = (Field) inputs.elementAt(j);
      if (proc != null && proc.hasOutput(field.name))
        continue;
      write(1, "msg.GetValue(\"" + field.useName() + "/value\", work);");
      writeln(fromXMLTFormat(field));
    }
    for (int j = 0; j < dynamics.size(); j++)
    {
      String str = (String) dynamics.elementAt(j);
      write(1, "msg.GetValue(\"" + str + "/value\", work);");
      writeln("memcpy(" + str + ", work.data, sizeof(" + str + ")-1);");
    }
    writeln("}");
    writeln(format("void %s::FromXML(TBAmp &XRec)", structName));
    writeln("{");
    writeln(1, "TXMLRecord msg;");
    writeln(1, "msg.Load(XRec);");
    writeln(1, "memset(this, 0, sizeof(*this));");
    writeln(1, "_fromXML(XRec, msg);");
    writeln("}");
    writeln("#endif");
    writeln();
    generateDataBuildImplementation(baseClass, inputs, useName, dynamics, proc);
  }

  private static void generateSwapsInterface(String baseClass, Vector fields, Proc proc)
  {
    writeln(1, "void Clear();");
    writeln(1, format("%s (PgConnector &conn)", structName));
    writeln(1, "{");
    writeln(2, "Clear();");
    writeln(2, "q_ = new PgQuery(conn);");
    writeln(1, "}");
    writeln(1, format("~%s ()", structName));
    writeln(1, "{");
    writeln(2, "delete q_;");
    writeln(1, "}");
    writeln("#ifdef swapbytesH");
    writeln(1, "void Swaps();");
    writeln("#endif");
  }

  private static void generateSwapsImplementation(String baseClass, Vector fields, Proc proc)
  {
    writeln(format("void %s::Clear()", structName));
    writeln("{");
    if (baseClass.length() > 0)
      writeln(1, baseClass + "::Clear();");
    for (int j = 0; j < fields.size(); j++)
    {
      Field field = (Field) fields.elementAt(j);
      if (proc != null && proc.hasOutput(field.name))
        continue;
      writeln(1, cppInit(field));
    }
    if (proc != null)
    {
      for (int j = 0; j < proc.dynamics.size(); j++)
      {
        String s = proc.dynamics.elementAt(j);
        writeln(1, "memset(" + s + ", 0, sizeof(" + s + "));");
      }
    }
    writeln("}");
    writeln();
    writeln("#ifdef swapbytesH");
    writeln(format("void %s::Swaps()", structName));
    writeln("{");
    if (baseClass.length() > 0)
      writeln(1, baseClass + "::Swaps();");
    for (int j = 0; j < fields.size(); j++)
    {
      Field field = (Field) fields.elementAt(j);
      if (proc != null && proc.hasOutput(field.name))
        continue;
      if (notString(field) == false)
        continue;
      if (isStruct(field) == false)
        writeln(1, "SwapBytes(" + field.useName() + ");");
      else
        writeln(1, field.useName() + ".Swaps();");
      if (isNull(field))
        writeln(1, "SwapBytes(" + field.useName() + "IsNull);");
    }
    writeln("}");
    writeln("#endif");
    writeln();
  }

  static String fileName(String output, String node, String ext)
  {
    return output + node + ext;
  }

  static String fromXMLDOFormat(Field field)
  {
    switch (field.type)
    {
      case Field.CHAR:
      case Field.ANSICHAR:
      case Field.USERSTAMP:
      case Field.TLOB:
      case Field.DATE:
      case Field.TIME:
      case Field.DATETIME:
      case Field.TIMESTAMP:
        return "memcpy(" + field.useName() + ", work.data, sizeof(" + field.useName() + ")-1);";
      case Field.BOOLEAN:
      case Field.BYTE:
        return field.useName() + " = (int8)atol(work.data);";
      case Field.SHORT:
        return field.useName() + " = (int16)atol(work.data);";
      case Field.INT:
      case Field.SEQUENCE:
      case Field.IDENTITY:
        return field.useName() + " = (int32)atol(work.data);";
      case Field.LONG:
        return field.useName() + " = (int64)atol(work.data);";
      case Field.FLOAT:
      case Field.DOUBLE:
        if (field.precision > 15)
          return "memcpy(" + field.useName() + ", work.data, sizeof(" + field.useName() + ")-1);";
        return field.useName() + " = atof(work.data);";
      case Field.MONEY:
        return "memcpy(" + field.useName() + ", work.data, sizeof(" + field.useName() + ")-1);";
    }
    return "// " + field.useName() + " <unsupported>";
  }

  static String fromXMLTFormat(Field field)
  {
    switch (field.type)
    {
      case Field.CHAR:
      case Field.ANSICHAR:
      case Field.USERSTAMP:
      case Field.TLOB:
      case Field.DATE:
      case Field.TIME:
      case Field.DATETIME:
      case Field.TIMESTAMP:
        return "memcpy(" + field.useName() + ", work.data, sizeof(" + field.useName() + ")-1);";
      case Field.BOOLEAN:
      case Field.BYTE:
        return field.useName() + " = (int8)atol(work.data);";
      case Field.SHORT:
        return field.useName() + " = (int16)atol(work.data);";
      case Field.INT:
      case Field.SEQUENCE:
      case Field.IDENTITY:
        return field.useName() + " = (int32)atol(work.data);";
      case Field.LONG:
        return field.useName() + " = (int64)atol(work.data);";
      case Field.FLOAT:
      case Field.DOUBLE:
        if (field.precision > 15)
          return "memcpy(" + field.useName() + ", work.data, sizeof(" + field.useName() + ")-1);";
        return field.useName() + " = atof(work.data);";
      case Field.MONEY:
        return "memcpy(" + field.useName() + ", work.data, sizeof(" + field.useName() + ")-1);";
    }
    return "// " + field.useName() + " <unsupported>";
  }

  static int getLength(Field field)
  {
    switch (field.type)
    {
      case Field.BOOLEAN:
      case Field.BYTE:
      case Field.SHORT:
        return 2;
      case Field.INT:
      case Field.SEQUENCE:
      case Field.IDENTITY:
        return 4;
      case Field.LONG:
        return 8;
      case Field.CHAR:
      case Field.ANSICHAR:
      case Field.USERSTAMP:
      case Field.TLOB:
        return field.length + 1;
      case Field.BLOB:
        return 8;
      case Field.DATE:
        return 9;
      case Field.TIME:
        return 7;
      case Field.DATETIME:
      case Field.TIMESTAMP:
        return 15;
      case Field.FLOAT:
      case Field.DOUBLE:
        if (field.precision > 15)
          return field.precision + 3; // allow for - . and null terminator
        return 8;
      case Field.MONEY:
        return 21;
    }
    return 4;
  }

  private static boolean getProperty(String propName, boolean propDefault)
  {
    if (properties == null)
      return propDefault;
    String propValue = properties.getProperty(propName);
    if (propValue == null)
      return propDefault;
    return propValue.equalsIgnoreCase("true");
  }

  private static String charPadding(int no, int fillerNo)
  {
    int n = 8 - (no % 8);
    if (n != 8)
      return "IDL2_CHAR_PAD(" + fillerNo + "," + n + ");";
    return "";
  }

  /**
   * Translates field type to cpp data member type
   */
  static String cppBind(Field field, String tableName, boolean isInsert)
  {
    switch (field.type)
    {
      case Field.BOOLEAN:
      case Field.BYTE:
      case Field.SHORT:
        return field.useName() + ", " + field.useName() + "_INT16";
      case Field.INT:
        return field.useName() + ", " + field.useName() + "_INT32";
      case Field.LONG:
        return field.useName() + ", " + field.useName() + "_INT64";
      case Field.FLOAT:
      case Field.DOUBLE:
        if (field.precision > 15)
          return field.useName() + ", " + (field.precision) + ", " + (field.scale);
        return field.useName() + ", " + field.useName() + "_DOUBLE, " + (field.precision) + ", " + (field.scale);
      case Field.MONEY:
        return field.useName() + ", 18, 2";
      case Field.SEQUENCE:
        if (isInsert)
          return "q_->Sequence(" + field.useName() + ", \"" + tableName + "_" + field.useName() + "_seq\"), " + field.useName() + "_INT32";
        else
          return field.useName() + ", " + field.useName() + "_INT32";
      case Field.TLOB:
        return field.useName() + ", " + (field.length);
      case Field.CHAR:
        return field.useName() + ", " + (field.length);
      case Field.ANSICHAR:
        return field.useName() + ", " + (field.length);
      case Field.USERSTAMP:
        return "q_->UserStamp(" + field.useName() + ", sizeof(" + field.useName() + ")), 50";
      case Field.DATE:
        return "q_->Date(" + field.useName() + "_PGDate, " + field.useName()
                + ")";
      case Field.TIME:
        return "q_->Time(" + field.useName() + "_PGTime, " + field.useName()
                + ")";
      case Field.DATETIME:
        return "q_->DateTime(" + field.useName() + "_PGDateTime, " + field.useName()
                + ")";
      case Field.TIMESTAMP:
        return "q_->TimeStamp(" + field.useName() + "_PGTimeStamp, " + field.useName()
                + ")";
      case Field.BLOB:
        return "(char*)&" + field.useName() + ", sizeof(" + field.useName() + ".data)";
    }
    return field.useName() + ", <unsupported>";
  }

  /**
   * Translates field type to cpp data member type
   */
  static String cppBindArray(Field field, String tableName)
  {
    switch (field.type)
    {
      case Field.BOOLEAN:
      case Field.BYTE:
      case Field.SHORT:
        return "(int16*)  (q_->data+" + field.useName().toUpperCase() + "_POS)";
      case Field.INT:
      case Field.SEQUENCE:
      case Field.IDENTITY:
        return "(int32*)    (q_->data+" + field.useName().toUpperCase() + "_POS)";
      case Field.LONG:
        return "(int64*)   (q_->data+" + field.useName().toUpperCase() + "_POS)";
      case Field.CHAR:
        return "(char*)   (q_->data+" + field.useName().toUpperCase() + "_POS), "
                + (field.length + 1);
      case Field.ANSICHAR:
        return "(char*)   (q_->data+" + field.useName().toUpperCase() + "_POS), "
                + (field.length + 1) + ", 1";
      case Field.USERSTAMP:
        return "(char*)   (q_->data+" + field.useName().toUpperCase() + "_POS), 51";
      case Field.DATE:
        return "(PgTDate*)(q_->data+" + field.useName().toUpperCase() + "_POS)";
      case Field.TIME:
        return "(PgTTime*)(q_->data+" + field.useName().toUpperCase() + "_POS)";
      case Field.DATETIME:
      case Field.TIMESTAMP:
        return "(PgTDateTime*)(q_->data+" + field.useName().toUpperCase() + "_POS)";
      case Field.FLOAT:
      case Field.DOUBLE:
        if (field.precision > 15)
          return "(char*)   (q_->data+" + field.useName().toUpperCase() + "_POS), " + (field.precision + 3);
        return "(double*) (q_->data+" + field.useName().toUpperCase() + "_POS)";
      case Field.MONEY:
        return "(char*)   (q_->data+" + field.useName().toUpperCase() + "_POS), "
                + (field.precision + 3);
    }
    return field.useName() + " <unsupported>";
  }

  /**
   * Translates field type to cpp data member type
   */
  static String cppCopy(Field field)
  {
    switch (field.type)
    {
      case Field.BOOLEAN:
      case Field.BYTE:
      case Field.SHORT:
      case Field.INT:
      case Field.LONG:
      case Field.SEQUENCE:
        return field.useName() + " = a" + field.useName() + ";";
      case Field.FLOAT:
      case Field.DOUBLE:
        if (field.precision > 15)
          return "strncpy(" + field.useName() + ", a" + field.useName()
                  + ", sizeof(" + field.useName() + ")-1);";
        return field.useName() + " = a" + field.useName() + ";";
      case Field.MONEY:
        return "strncpy(" + field.useName() + ", a" + field.useName()
                + ", sizeof(" + field.useName() + ")-1);";
      case Field.CHAR:
      case Field.TLOB:
      case Field.DATE:
      case Field.TIME:
      case Field.DATETIME:
        return "strncpy(" + field.useName() + ", a" + field.useName()
                + ", sizeof(" + field.useName() + ")-1);";
      case Field.ANSICHAR:
        return "memcpy(" + field.useName() + ", a" + field.useName()
                + ", sizeof(" + field.useName() + "));";
      case Field.BLOB:
        return field.useName() + " = a" + field.useName() + ";";
      case Field.USERSTAMP:
      case Field.IDENTITY:
      case Field.TIMESTAMP:
        return "// " + field.useName() + " -- generated";
    }
    return field.useName() + " <unsupported>";
  }

  /**
   * Translates field type to cpp data member type
   */
  static String cppDefine(Field field)
  {
    switch (field.type)
    {
      case Field.BOOLEAN:
      case Field.BYTE:
      case Field.SHORT:
        return "(int16*) (q_->data+" + field.useName().toUpperCase() + "_POS)";
      case Field.INT:
      case Field.IDENTITY:
      case Field.SEQUENCE:
        return "(int32*) (q_->data+" + field.useName().toUpperCase() + "_POS)";
      case Field.LONG:
        return "(int64*) (q_->data+" + field.useName().toUpperCase() + "_POS)";
      case Field.CHAR:
      case Field.TLOB:
        return "(char*)  (q_->data+" + field.useName().toUpperCase() + "_POS), "
                + (field.length + 1);
      case Field.ANSICHAR:
        return "(char*)  (q_->data+" + field.useName().toUpperCase() + "_POS), "
                + (field.length + 1) + ", 1";
      case Field.USERSTAMP:
        return "(char*)  (q_->data+" + field.useName().toUpperCase() + "_POS), 51";
      case Field.BLOB:
        return "(char*)  (q_->data+" + field.useName().toUpperCase() + "_POS), sizeof(" + field.useName() + ")";
      case Field.DATE:
        return "(PgTDate*)(q_->data+" + field.useName().toUpperCase() + "_POS)";
      case Field.TIME:
        return "(PgTTime*)(q_->data+" + field.useName().toUpperCase() + "_POS)";
      case Field.DATETIME:
      case Field.TIMESTAMP:
        return "(PgTDateTime*)(q_->data+" + field.useName().toUpperCase() + "_POS)";
      case Field.FLOAT:
      case Field.DOUBLE:
        if (field.precision > 15)
          return "(char*)  (q_->data+" + field.useName().toUpperCase() + "_POS), " + (field.precision + 3);
        return "(double*)(q_->data+" + field.useName().toUpperCase() + "_POS)";
      case Field.MONEY:
        return "(char*)  (q_->data+" + field.useName().toUpperCase() + "_POS), 21";
    }
    return field.useName() + " <unsupported>";
  }

  /**
   * Translates field type to cpp data member type
   */
  static String cppGet(Field field)
  {
    switch (field.type)
    {
      case Field.BOOLEAN:
      case Field.BYTE:
      case Field.SHORT:
      case Field.INT:
      case Field.SEQUENCE:
      case Field.IDENTITY:
      case Field.LONG:
        return field.useName();
      case Field.FLOAT:
      case Field.DOUBLE:
        if (field.precision > 15)
          return field.useName() + ", " + (field.precision + 3);
        return field.useName();
      case Field.MONEY:
        return field.useName() + ", 21";
      case Field.CHAR:
      case Field.ANSICHAR:
      case Field.TLOB:
        return field.useName() + ", " + (field.length + 1);
      case Field.USERSTAMP:
        return field.useName() + ", 51";
      case Field.BLOB:
        return field.useName() + ".len, " + field.useName() + ".data, sizeof(" + field.useName() + ")";
      case Field.DATE:
        return "PgDate(" + field.useName() + ")";
      case Field.TIME:
        return "PgTime(" + field.useName() + ")";
      case Field.DATETIME:
      case Field.TIMESTAMP:
        return "PgDateTime(" + field.useName() + ")";
    }
    return field.useName() + " <unsupported>";
  }

  static String cppInit(Field field)
  {
    switch (field.type)
    {
      case Field.BOOLEAN:
      case Field.BYTE:
      case Field.SHORT:
      case Field.INT:
      case Field.SEQUENCE:
      case Field.IDENTITY:
      case Field.LONG:
        return field.useName() + " = 0;";
      //return "long   " + field.useName();
      case Field.CHAR:
      case Field.ANSICHAR:
      case Field.TLOB:
      case Field.USERSTAMP:
      case Field.DATE:
      case Field.TIME:
      case Field.DATETIME:
      case Field.TIMESTAMP:
        return "memset(" + field.useName() + ", 0, sizeof(" + field.useName() + "));";
      case Field.BLOB:
        return "memset(&" + field.useName() + ", 0, sizeof(" + field.useName() + "));";
      case Field.FLOAT:
      case Field.DOUBLE:
        if (field.precision <= 15)
          return field.useName() + " = 0.0;";
        return "memset(" + field.useName() + ", 0, sizeof(" + field.useName() + "));";
      case Field.MONEY:
        return "memset(" + field.useName() + ", 0, sizeof(" + field.useName() + "));";
    }
    return field.useName() + " <unsupported>";
  }

  static String cppLength(Field field)
  {
    switch (field.type)
    {
      case Field.BOOLEAN:
      case Field.BYTE:
      case Field.SHORT:
        return "sizeof(int16)";
      case Field.INT:
      case Field.SEQUENCE:
      case Field.IDENTITY:
        return "sizeof(int32)";
      case Field.LONG:
        return "sizeof(int64)";
      case Field.CHAR:
      case Field.ANSICHAR:
      case Field.TLOB:
        return "" + (field.length + 1);
      case Field.BLOB:
        return "sizeof(PgBlob<" + field.length + ">)";
      case Field.USERSTAMP:
        return "51";
      case Field.DATE:
        return "sizeof(PgTDate)";
      case Field.TIME:
        return "sizeof(PgTTime)";
      case Field.DATETIME:
      case Field.TIMESTAMP:
        return "sizeof(PgTDateTime)";
      case Field.FLOAT:
      case Field.DOUBLE:
        if (field.precision > 15)
          return "" + (field.precision + 3);
        return "sizeof(double)";
      case Field.MONEY:
        return "21";
    }
    return "0";
  }

  /**
   * Translates field type to cpp data member type
   */
  static String cppParm(Field field)
  {
    switch (field.type)
    {
      case Field.BOOLEAN:
      case Field.BYTE:
      case Field.SHORT:
        return "int16  a" + field.useName();
      case Field.INT:
      case Field.SEQUENCE:
      case Field.IDENTITY:
        return "int32   a" + field.useName();
      case Field.LONG:
        return "int64  a" + field.useName();
      case Field.CHAR:
      case Field.TLOB:
      case Field.ANSICHAR:
        return "char*  a" + field.useName();
      case Field.USERSTAMP:
        return "char*  a" + field.useName();
      case Field.DATE:
        return "char*  a" + field.useName();
      case Field.TIME:
        return "char*  a" + field.useName();
      case Field.DATETIME:
      case Field.TIMESTAMP:
        return "char*  a" + field.useName();
      case Field.FLOAT:
      case Field.DOUBLE:
        if (field.precision > 15)
          return "char*  a" + field.useName();
        return "double a" + field.useName();
      case Field.MONEY:
        return "char*  a" + field.useName();
    }
    return field.useName() + " <unsupported>";
  }

  /**
   * Translates field type to cpp data member type
   */
  static String cppVar(Field field)
  {
    switch (field.type)
    {
      case Field.BOOLEAN:
      case Field.BYTE:
      case Field.SHORT:
        return "int16  " + field.useName();
      case Field.INT:
      case Field.IDENTITY:
      case Field.SEQUENCE:
        return "int32  " + field.useName();
      case Field.LONG:
        return "int64  " + field.useName();
      //return "long   " + field.useName();
      case Field.CHAR:
      case Field.ANSICHAR:
      case Field.TLOB:
        return "char   " + field.useName() + "[" + (field.length + 1) + "]";
      case Field.USERSTAMP:
        return "char   " + field.useName() + "[" + (field.length + 1) + "]";
      case Field.BLOB:
        return "PgBlob<" + field.length + "> " + field.useName();
      case Field.DATE:
        return "char   " + field.useName() + "[9]";
      case Field.TIME:
        return "char   " + field.useName() + "[7]";
      case Field.DATETIME:
      case Field.TIMESTAMP:
        return "char   " + field.useName() + "[15]";
      case Field.FLOAT:
      case Field.DOUBLE:
        if (field.precision > 15)
          return "char   " + field.useName() + "[" + (field.precision + 3) + "]";
        return "double " + field.useName();
      case Field.MONEY:
        return "char   " + field.useName() + "[21]";
    }
    return field.useName() + " <unsupported>";
  }

  static boolean isLob(Field field)
  {
    return field.type == Field.BLOB;
  }

  static boolean isNull(Field field)
  {
    if (field.isNull == false)
      return false;
    switch (field.type)
    {
      case Field.BOOLEAN:
      case Field.FLOAT:
      case Field.DOUBLE:
      case Field.MONEY:
      case Field.BYTE:
      case Field.SHORT:
      case Field.INT:
      case Field.LONG:
      case Field.IDENTITY:
      case Field.SEQUENCE:
      case Field.BLOB:
      case Field.DATE:
      case Field.DATETIME:
      case Field.TIMESTAMP:
      case Field.TIME:
        return true;
    }
    return false;
  }

  static boolean isStruct(Field field)
  {
    return field.type == Field.BLOB;
  }

  static boolean notString(Field field)
  {
    switch (field.type)
    {
      case Field.BOOLEAN:
      case Field.BYTE:
      case Field.SHORT:
      case Field.INT:
      case Field.LONG:
      case Field.IDENTITY:
      case Field.SEQUENCE:
      case Field.BLOB:
        return true;
      case Field.FLOAT:
      case Field.DOUBLE:
        return field.precision <= 15;
    }
    return false;
  }

  private static String nullAdd(Field field)
  {
    if (isNull(field))
      return ", " + field.useName() + "IsNull";
    return "";
  }

  private static String nullSet(Field field)
  {
    if (isNull(field))
      return ", " + field.useName() + "IsNull";
    return "";
  }

  static String padder(String s, int length)
  {
    for (int i = s.length(); i < length - 1; i++)
      s = s + " ";
    return s + " ";
  }

  static String toXMLDOFormat(Field field)
  {
    String front = "XRec.append(\"  <" + field.useName() + ">\");";
    String back = "XRec.append(\"</" + field.useName() + ">\");";
    switch (field.type)
    {
      case Field.CHAR:
      case Field.ANSICHAR:
      case Field.USERSTAMP:
      case Field.TLOB:
      case Field.DATE:
      case Field.TIME:
      case Field.DATETIME:
      case Field.TIMESTAMP:
        return front + "XRec.ampappend(" + field.useName() + ");" + back;
      case Field.BOOLEAN:
      case Field.BYTE:
      case Field.SHORT:
      case Field.INT:
        return front + "XRec.ampappend(JP_XML_FORMAT((int32)" + field.useName() + ").result);" + back;
      case Field.LONG:
        return front + "XRec.ampappend(JP_XML_FORMAT((int64)" + field.useName() + ").result);" + back;
      case Field.FLOAT:
      case Field.DOUBLE:
        if (field.precision > 15)
          return front + "XRec.ampappend(" + field.useName() + ");" + back;
        return front + "XRec.ampappend(JP_XML_FORMAT((double)" + field.useName() + ").result);" + back;
      case Field.MONEY:
        return front + "XRec.ampappend(" + field.useName() + ");" + back;
    }
    return "// " + field.useName() + " <unsupported>";
  }

  static String toXMLTFormat(Field field)
  {
    String front = "XRec.append(\"  <" + field.useName() + " value='\");";
    String back = "XRec.append(\"'/>\");";
    switch (field.type)
    {
      case Field.CHAR:
      case Field.ANSICHAR:
      case Field.USERSTAMP:
      case Field.TLOB:
      case Field.DATE:
      case Field.TIME:
      case Field.DATETIME:
      case Field.TIMESTAMP:
        return front + "XRec.ampappend(" + field.useName() + ");" + back;
      case Field.BOOLEAN:
      case Field.BYTE:
      case Field.SHORT:
      case Field.INT:
        return front + "XRec.ampappend(JP_XML_FORMAT((int32)" + field.useName() + ").result);" + back;
      case Field.LONG:
        return front + "XRec.ampappend(JP_XML_FORMAT((int64)" + field.useName() + ").result);" + back;
      case Field.FLOAT:
      case Field.DOUBLE:
        if (field.precision > 15)
          return front + "XRec.ampappend(" + field.useName() + ");" + back;
        return front + "XRec.ampappend(JP_XML_FORMAT((double)" + field.useName() + ").result);" + back;
      case Field.MONEY:
        return front + "XRec.ampappend(" + field.useName() + ");" + back;
    }
    return "// " + field.useName() + " <unsupported>";
  }
}
