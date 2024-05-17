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

public class SwaggerCode extends vlab.jportal.Generator
{
  static String structName = "";

  public static String description()
  {
    return "Generate Swagger Schema";
  }

  public static String documentation()
  {
    return "Generate Swagger Schema";
  }

  static String padder(String s, int length)
  {
    for (int i = s.length(); i < length - 1; i++)
      s = s + " ";
    return s + " ";
  }

  /**
   * Generates the procedure classes for each table present.
   */
  public static void generate(Database database, String output, PrintWriter outLog)
  {
    for (int i = 0; i < database.tables.size(); i++)
    {
      Table table = database.tables.elementAt(i);
      generateTable(table, output, outLog);
    }
  }

  private static void generateTable(Table table, String output, PrintWriter outLog)
  {
    try
    {
      outLog.println("Code: " + output + table.useName().toLowerCase() + ".yaml");
      OutputStream outFile = new FileOutputStream(output + table.useName().toLowerCase() + ".yaml");
      try
      {
        writer = new PrintWriter(outFile);
        indent_size = 2;
        writeln("%YAML 1.2");
        writeln("---");
        writeln("# This code was generated, do not modify it, modify it at source and regenerate it.");
        writeln("definitions:");
        //writeln(1, "schemas:");
        try
        {
          if (table.hasStdProcs)
            generateStdOutputRec(table);
          generateUserOutputRecs(table);
          writeln("...");
        }
        finally
        {
          writer.flush();
          writer.close();
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

  private static void generateUserOutputRecs(Table table)
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
      fields = proc.outputs;
      if (fields.size() > 0)
      {
        String typeChar = "D";
        if (proc.hasDiscreteInput())
          typeChar = "O";
        work = " : public " + typeChar + table.useName() + proc.upperFirst();
        //baseClass = typeChar + table.useName() + proc.upperFirst();
        structName = typeChar + table.useName() + proc.upperFirst();
        writeln(1, structName + ":");
        writeln(2, "type: object");
        writeln(2, "properties:");
        if (fields.size() > 0)
          writeln(3, "# outputs");
        for (int j = 0; j < fields.size(); j++)
        {
          Field field = (Field) fields.elementAt(j);
          writeln(3, field.useName() + ":");
          fieldType(field, 4);
          if (isNull(field))
          {
            writeln(3, field.useName() + "IsNull:");
            writeln(4, "type: " + "integer");
            writeln(4, "format: int16");
          }
        }
        writeln(2, "required:");
        fieldsRequired(fields, 3);
      }
      if (proc.hasDiscreteInput())
      {
        structName = "D" + table.useName() + proc.upperFirst();
        writeln(1, structName + ":");
        writeln(2, "type: object");
        writeln(2, "properties:");
        Vector inputs = proc.inputs;
        if (inputs.size() > 0)
          writeln(3, "# inputs");
        for (int j = 0; j < inputs.size(); j++)
        {
          Field field = (Field) inputs.elementAt(j);
          //if (proc.hasOutput(field.name))
          //  continue;
          writeln(3, field.useName() + ":");
          fieldType(field, 4);
          if (isNull(field))
          {
            writeln(3, field.useName() + "IsNull:");
            writeln(4, "type: " + "integer");
            writeln(4, "format: int16");
          }
        }
        Vector outputs = proc.outputs;
        if (outputs.size() > 0)
          writeln(3, "# outputs");
        for (int j = 0; j < fields.size(); j++)
        {
          Field field = (Field) fields.elementAt(j);
          if (proc.hasInput(field.name))
            continue;
          writeln(3, field.useName() + ":");
          fieldType(field, 4);
          if (isNull(field))
          {
            writeln(3, field.useName() + "IsNull:");
            writeln(4, "type: " + "integer");
            writeln(4, "format: int16");
          }
        }
        if (proc.dynamics.size() > 0)
          writeln(3, "# dynamics");
        for (int j = 0; j < proc.dynamics.size(); j++)
        {
          String s = proc.dynamics.elementAt(j);
          Integer n = proc.dynamicSizes.elementAt(j);
          writeln(3, s + ":");
          writeln(4, "type: string");
          writeln(4, "maxLength: " + n.intValue());
        }
        writeln(2, "required:");
        fieldsRequired(inputs, 3);
        for (int j = 0; j < fields.size(); j++)
        {
          Field field = (Field) fields.elementAt(j);
          if (proc.hasInput(field.name))
            continue;
          writeln(3, "- " + field.useName());
          if (isNull(field))
            writeln(3, "- " + field.useName() + "IsNull");
        }
        for (int j = 0; j < proc.dynamics.size(); j++)
        {
          String s = proc.dynamics.elementAt(j);
          writeln(3, "- " + s);
        }
      }
    }
  }

  private static void generateStdOutputRec(Table table)
  {
    structName = "D" + table.useName();
    writeln(1, structName + ":");
    writeln(2, "type: object");
    writeln(2, "properties:");
    writeln(3, "# table fields");
    Vector fields = table.fields;
    for (int i = 0; i < fields.size(); i++)
    {
      Field field = (Field) fields.elementAt(i);
      writeln(3, field.useName() + ":");
      fieldType(field, 4);
      if (isNull(field))
      {
        writeln(3, field.useName() + "IsNull:");
        writeln(4, "type: " + "integer");
        writeln(4, "format: int16");
      }
    }
    writeln(2, "required:");
    fieldsRequired(fields, 3);
  }

  private static void fieldsRequired(Vector fields, int ind)
  {
    for (int i = 0; i < fields.size(); i++)
    {
      Field field = (Field) fields.elementAt(i);
      writeln(ind, "- " + field.useName());
      if (isNull(field))
        writeln(ind, "- " + field.useName() + "IsNull");
    }
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
      case Field.BIGIDENTITY:
      case Field.BIGSEQUENCE:
      case Field.BLOB:
      case Field.DATE:
      case Field.DATETIME:
      case Field.TIMESTAMP:
      case Field.AUTOTIMESTAMP:
      case Field.TIME:
        //case Field.XML:
        return true;
    }
    return false;
  }

  static void fieldType(Field field, int ind)
  {
    switch (field.type)
    {
      case Field.BOOLEAN:
      case Field.BYTE:
      case Field.SHORT:
        writeln(ind, "type: " + "integer");
        writeln(ind, "format: int16");
        break;
      case Field.INT:
      case Field.IDENTITY:
      case Field.SEQUENCE:
        writeln(ind, "type: " + "integer");
        writeln(ind, "format: int32");
        break;
      case Field.LONG:
      case Field.BIGIDENTITY:
      case Field.BIGSEQUENCE:
        writeln(ind, "type: " + "integer");
        writeln(ind, "format: int64");
        break;
      case Field.CHAR:
      case Field.ANSICHAR:
      case Field.TLOB:
      case Field.XML:
      case Field.DATE:
      case Field.TIME:
      case Field.USERSTAMP:
      case Field.DATETIME:
      case Field.TIMESTAMP:
      case Field.AUTOTIMESTAMP:
        writeln(ind, "type: " + "string");
        writeln(ind, format("maxLength: %d", field.length));
        break;
      case Field.BLOB:
      case Field.IMAGE:
        writeln(ind, "type: " + "string");
        writeln(ind, format("maxLength: %d", field.length));
        break;
      case Field.FLOAT:
      case Field.DOUBLE:
        if (field.precision > 15)
        {
          writeln(ind, "type: " + "string");
          writeln(ind, format("maxLength: %d", field.precision + 2));
          break;
        }
        writeln(ind, "type: " + "number");
        writeln(ind, "format: double");
        break;
      case Field.MONEY:
        writeln(ind, "type: " + "string");
        writeln(ind, format("maxLength: %d", 20));
        break;
      default:
        writeln(ind, "type: " + field.type);
        break;
    }
  }
}
