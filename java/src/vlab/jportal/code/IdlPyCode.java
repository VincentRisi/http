/// ------------------------------------------------------------------
/// Copyright (c) 1996, 2004 Vincent Risi in Association 
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
/// $Date: 2004/10/18 13:45:47 $
/// $Revision: 411.1 $ // YMM.Revision
/// ------------------------------------------------------------------

package vlab.jportal.code;

import java.io.*;
import java.util.Vector;
import static vlab.jportal.Writer.*;
import vlab.jportal.*;

public class IdlPyCode extends Generator
{
  public static String description()
  {
    return "Generate IDL PYTHON Code for 3 Tier Access";
  }
  public static String documentation()
  {
    return "Generate IDL Code for 3 Tier Access"
    + "\r\n- \"use package\" use package name for namespace"
    + "\r\n- \"use schema\" use schema name for namespace"
    ;
  }
  static boolean usePackage;
  static boolean useSchema;
  static void setFlags(Database database, PrintWriter outLog)
  {
    usePackage = false;
    useSchema = false;
    for (int i = 0; i < database.flags.size(); i++)
    {
      String flag = database.flags.elementAt(i);
      if (flag.equalsIgnoreCase("use package"))
        usePackage = true;
      else if (flag.equalsIgnoreCase("use package"))
        useSchema = true;
    }
    if (usePackage == true)
      outLog.println(" (use package)");
    if (useSchema == true)
      outLog.println(" (use schema)");
  }
  static String padder(String s, int length)
  {
    for (int i = s.length(); i < length-1; i++)
      s = s + " ";
    return s + " ";
  }
  /**
  * Generates the procedure classes for each table present.
  */
  public static void generate(Database database, String output, PrintWriter outLog)
  {
    setFlags(database, outLog);
    for (int i=0; i<database.tables.size(); i++)
    {
      Table table = database.tables.elementAt(i);
      generate(table, output, outLog);
    }
  }
  static Vector enumsGenerated;
  private static boolean hasEnum(String name)
  {
    for (int i = 0; i < enumsGenerated.size(); i++)
      if (name.compareTo((String)enumsGenerated.elementAt(i)) == 0)
        return true;
    return false;
  }
  private static void generatePragmaEnums(Field field)
  {
    if (hasEnum(field.name) == false)
    {
      enumsGenerated.addElement(field.name);
      StringBuffer buffer = new StringBuffer();
      buffer.append("pragma si enum " + field.useName());
      for (int j = 0; j < field.enums.size(); j++)
      {
        Enumerator num = field.enums.elementAt(j);
        buffer.append(" " + num.name + " " + num.value);
      }
      writeln(buffer.toString());
    }
  }
  private static void generatePragmaValueList(Field field)
  {
    if (hasEnum(field.name) == false)
    {
      enumsGenerated.addElement(field.name);
      StringBuffer buffer = new StringBuffer();
      buffer.append("pragma si enum " + field.useName());
      for (int j = 0; j < field.valueList.size(); j++)
      {
        String name = field.valueList.elementAt(j);
        buffer.append(" " + name + " " + j);
      }
      writeln(buffer.toString());
    }
  }
  private static void generatePragma(Table table)
  {
    writeln("pragma si table " 
                   + table.name + " " 
                   + table.fields.size()
                   );
    for (int i = 0; i < table.keys.size(); i++)
    {
      Key key = table.keys.elementAt(i);
      if (key.isPrimary == true)
      {
        write("pragma si prime");
        for (int j = 0; j < key.fields.size(); j++)
        {
          String name = key.fields.elementAt(j);
          write(" " + name);
        }
        writeln();
      }
    }
    for (int i = 0; i < table.fields.size(); i++)
    {
      Field field = table.fields.elementAt(i);
      String name = field.useName();
      writeln("pragma si field " 
                    + name
                    + " " + field.type
                    + " " + field.length 
                    + " " + field.precision 
                    + " " + field.scale
                    + " " + (field.isNull ? "opt" : "mand") 
                    );
      if (field.enums.size() > 0)
       generatePragmaEnums(field);
     if (field.valueList.size() > 0)
       generatePragmaValueList(field);
   }
  }
  private static void generatePragma(Table table, Proc proc)
  {
    writeln("pragma si proc " 
                  + table.name + " " + proc.name
                  + " " + (proc.inputs.size() + proc.outputs.size() + proc.dynamics.size())
                  );
    for (int i = 0; i < proc.inputs.size(); i++)
    {
      Field field = proc.inputs.elementAt(i);
      String name = field.useName();
      writeln("pragma si input " 
                    + name
                    + " " + field.type
                    + " " + field.length 
                    + " " + field.precision 
                    + " " + field.scale
                    + " " + (field.isNull ? "opt" : "mand")
                    );
      if (field.enums.size() > 0)
        generatePragmaEnums(field);
      if (field.valueList.size() > 0)
        generatePragmaValueList(field);
    }
    for (int i = 0; i < proc.outputs.size(); i++)
    {
      Field field = proc.outputs.elementAt(i);
      String name = field.useName();
      writeln("pragma si output " 
                    + name
                    + " " + field.type
                    + " " + field.length 
                    + " " + field.precision 
                    + " " + field.scale
                    + " " + (field.isNull ? "opt" : "mand")
                    );
      if (field.enums.size() > 0)
        generatePragmaEnums(field);
      if (field.valueList.size() > 0)
        generatePragmaValueList(field);
    }
    for (int i = 0; i < proc.dynamics.size(); i++)
    {
      String name = proc.dynamics.elementAt(i);
      Integer size = proc.dynamicSizes.elementAt(i);
      writeln("pragma si dynamic " 
                    + name
                    + " " + size.intValue()
                    );
    }
  }
  /**
  * Build of standard and user defined procedures
  */
  private static void generate(Table table, String output, PrintWriter outLog)
  {
    try
    {
      outLog.println("Code: "+output+table.useName().toLowerCase() + ".ii");
      OutputStream outFile = new FileOutputStream(output + table.useName().toLowerCase() + ".ii");
      try
      {
        writer = new PrintWriter(outFile);
        indent_size = 4;
        try
        {
          String packageName = "Bbd.Idl2.AnyDB";
          if (usePackage == true)
            packageName = table.database.packageName;
          writeln("// This code was generated by IdlPyCode, do not modify it, modify it at source and regenerate it.");
          writeln("pragma \"ii("+table.useName().toLowerCase() + ":" + packageName + ")\"");
          writeln();
          if (table.hasStdProcs)
          {
            writeln("struct D"+table.useName()+" \""+table.useName().toLowerCase()+".sh\"");
            writeln("struct O"+table.useName()+" \""+table.useName().toLowerCase()+".sh\"");
          }
          for (int i=0; i<table.procs.size(); i++)
          {
            Proc proc = table.procs.elementAt(i);
            if (proc.isData || proc.isStd || proc.isStdExtended() || proc.hasNoData())
              continue;
            writeln("struct D" + table.useName() + proc.upperFirst() + " \"" + table.useName().toLowerCase() + ".sh\"");
            if (proc.outputs.size() > 0)
              writeln("struct O" + table.useName() + proc.upperFirst() + " \"" + table.useName().toLowerCase() + ".sh\"");
          }
          writeln();
          for (int i=0; i<table.procs.size(); i++)
          {
            Proc proc = table.procs.elementAt(i);
            if (proc.isData)
              continue;
            if (proc.options.size() > 0)
            {
              for (int j = 0; j < proc.options.size(); j++)
              {
                String option = proc.options.elementAt(j);
                writeln("pragma \"" + table.name + proc.name + ":" + option + "\"");
              }
            }
            if (proc.isMultipleInput)
              generateBulkAction(table, proc);
            else if (proc.isInsert && proc.hasReturning)
              generateAction(table, proc);
            else if (proc.outputs.size() > 0)
              if (proc.isSingle)
                generateSingle(table, proc);
              else
                generateMultiple(table, proc);
            else
              generateAction(table, proc);
          }
          enumsGenerated = new Vector();
          if (table.hasStdProcs)
            generatePragma(table);
          for (int i = 0; i < table.procs.size(); i++)
          {
            Proc proc = table.procs.elementAt(i);
            if (proc.isData || proc.isStd || proc.isStdExtended() || proc.hasNoData())
              continue;
            generatePragma(table, proc);
          }
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
  private static void generateSingle(Table table, Proc proc)
  {
    String dataStruct;
    if (proc.isStd || proc.isStdExtended())
      dataStruct = table.useName();
    else
      dataStruct = table.useName()+proc.upperFirst();
    boolean hasInput = (proc.inputs.size() > 0 || proc.dynamics.size() > 0);
    writeln("int32 " + table.useName() + proc.upperFirst()
                   +"(D"+dataStruct+" *rec)");
    writeln("{");
    writeln("message: #");
    if (hasInput)
    {
      writeln("input");
      writeln(1, "rec;");
    }
    writeln("output");
    writeln(1, "rec;");
    writeln("code");
    writeln("PYTHON:");
    writeln("try:");
    writeln(1, "rec.execSelectOne();");
    writeln("except DBError as db:");
    writeln(1, "print ('DBError: value:{0} rc:{1} ociErr:{2}'.format(db.value, db.rc, db.ociErr))");
    writeln("return rec");
    writeln("endcode");
    writeln("}");
    writeln();
  }
  private static void generateMultiple(Table table, Proc proc)
  {
    String dataStruct;
    if (proc.isStd || proc.isStdExtended())
      dataStruct = table.useName();
    else
      dataStruct = table.useName()+proc.upperFirst();
    boolean hasInput = (proc.inputs.size() > 0 || proc.dynamics.size() > 0);
    write("void "+table.useName()+proc.upperFirst()+"(");
    if (hasInput)
      write("D"+dataStruct+"* inRec, ");
    writeln("int32* noOf, O"+dataStruct+"*& outRecs)");
    writeln("{");
    writeln("message: #");
    if (hasInput)
    {
      writeln("input");
      writeln(1, "inRec;");
    }
    writeln("output");
    writeln(1, "noOf;");
    writeln(1, "outRecs size(noOf);");
    writeln("code");
    writeln(1, "T"+table.useName()+proc.upperFirst()+" q(*connect, JP_MARK);");
    if (hasInput)
      writeln(1, "q.Exec(*inRec);");
    else
      writeln(1, "q.Exec();");
    writeln(1, "while (q.Fetch())");
    writeln(2, "AddList(outRecs, *noOf, *q.ORec(), (int32)q.NOROWS);");
    writeln("endcode");
    writeln("}");
    writeln();
  }
  private static void generateAction(Table table, Proc proc)
  {
    String dataStruct;
    if (proc.isStd || proc.isStdExtended())
      dataStruct = table.useName();
    else
      dataStruct = table.useName()+proc.upperFirst();
    boolean hasInput = (proc.inputs.size() > 0 || proc.dynamics.size() > 0);
    writeln("// use this function for transaction business logic");
    write("public void _"+table.useName()+proc.upperFirst()+"(");
    if (hasInput || proc.hasModifieds())
      write("D"+dataStruct+" *rec");
    writeln(")");
    writeln("{");
    writeln("message: #");
    if (hasInput || proc.hasModifieds())
      writeln("input rec;");
    if (proc.hasModifieds() || proc.hasReturning)
      writeln("output rec;");
    writeln("code");
    writeln(1, "T" + table.useName() + proc.upperFirst() + " q(*connect, JP_MARK);");
    if (hasInput || proc.hasModifieds())
      writeln(1, "q.Exec(*rec);");
    else
      writeln(1, "q.Exec();");
    if (proc.hasReturning)
    {
      writeln(1, "if (q.Fetch())");
      writeln(2, "*rec = *q.DRec();");
    }
    else if (proc.hasModifieds())
      writeln(1, "*rec = *q.DRec();");
    writeln("endcode");
    writeln("}");
    writeln();
    write("void " + table.useName() + proc.upperFirst() + "(");
    if (hasInput || proc.hasModifieds())
      write("D" + dataStruct + " *rec");
    writeln(")");
    writeln("{");
    writeln("message: #");
    if (hasInput || proc.hasModifieds())
      writeln("input rec;");
    if (proc.hasModifieds() || proc.hasReturning)
      writeln("output rec;");
    writeln("code");
    writeln(1, "try");
    writeln(1, "{");
    write(2, table.useName() + proc.upperFirst() + "(");
    if (hasInput || proc.hasModifieds() || proc.hasReturning)
      write("rec");
    writeln(");");
    writeln(2, "Commit();");
    writeln(1, "}");
    writeln(1, "catch(xCept ex)");
    writeln(1, "{");
    writeln(2, "logFile->Log(ex);");
    writeln(2, "Rollback();");
    writeln(2, "throw;");
    writeln(1, "}");
    writeln("endcode");
    writeln("}");
    writeln();
  }
  private static void generateBulkAction(Table table, Proc proc)
  {
    String dataStruct;
    if (proc.isStd || proc.isStdExtended())
      dataStruct = table.useName();
    else
      dataStruct = table.useName()+proc.upperFirst();
    writeln("// use this function for transaction business logic");
    write("public void _"+table.useName()+proc.upperFirst()+"(");
    write("int noOf, D"+dataStruct+" *recs");
    writeln(")");
    writeln("{");
    writeln("message: #");
    writeln("input noOf; recs size(noOf);");
    writeln("code");
    writeln(1, "T" + table.useName() + proc.upperFirst() + " q(*connect, JP_MARK);");
    writeln(1, "q.Exec(noOf, recs);");
    writeln("endcode");
    writeln("}");
    writeln();
    write("void " + table.useName() + proc.upperFirst() + "(");
    write("int noOf, D" + dataStruct + " *recs");
    writeln(")");
    writeln("{");
    writeln("message: #");
    writeln("input noOf; recs size(noOf);");
    writeln("code");
    writeln(1, "try");
    writeln(1, "{");
    write(2, table.useName() + proc.upperFirst() + "(");
    write("noOf, recs");
    writeln(");");
    writeln(2, "Commit();");
    writeln(1, "}");
    writeln(1, "catch(xCept ex)");
    writeln(1, "{");
    writeln(2, "logFile->Log(ex);");
    writeln(2, "Rollback();");
    writeln(2, "throw;");
    writeln(1, "}");
    writeln("endcode");
    writeln("}");
    writeln();
  }
}
