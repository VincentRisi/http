/// ------------------------------------------------------------------
/// Copyright (c) from 1996
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

import java.io.FileOutputStream;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.util.Vector;

import static vlab.jportal.TJCStructs.*;
import static vlab.jportal.Writer.*;
import vlab.jportal.*;

public class MySqlCCode extends Generator
{
  static public PlaceHolder placeHolder;

  public static String description()
  {
    return "Generate MySql C Code";
  }

  public static String documentation()
  {
    return "Generate MySql C Code";
  }

  /**
   * Generates the procedure classes for each table present.
   */
  public static void generate(Database database, String output, PrintWriter outLog)
  {
    try
    {
      for (int i = 0; i < database.tables.size(); i++)
      {
        Table table = database.tables.elementAt(i);
        generate(table, output, outLog);
      }
    }
    catch (Exception ex)
    {
      outLog.println(ex);
      ex.printStackTrace(outLog);
    }
  }

  private static String fileName(String output, String node, String ext)
  {
    node = node.toLowerCase();
    return output + node + ext;
  }

  /**
   * Build of standard and user defined procedures
   */
  static void generate(Table table, String output, PrintWriter outLog) throws Exception
  {
    outLog.println("Code: " + fileName(output, table.useName().toLowerCase(), ".sh"));
    try (OutputStream outFile = new FileOutputStream(fileName(output, table.useName().toLowerCase(), ".sh")))
    {
      writer = new PrintWriter(outFile);
      writeln("/* ---------------------------------------------------------------------------------");
      writeln(" * This code was generated, do not modify it, modify it at source and regenerate it.");
      writeln(" * ---------------------------------------------------------------------------------");
      writeln(" */");
      writeln("#ifndef " + table.useName().toLowerCase() + "H");
      writeln("#define " + table.useName().toLowerCase() + "H");
      writeln();
      writeln("#include <stddef.h>");
      writeln("#include \"myapi.h\"");
      writeln();
      if (table.hasStdProcs)
        generateStdOutputRec(table);
      generateUserOutputRecs(table);
      generateInterface(table);
      writeln("#endif");
      writer.flush();
    }
    outLog.println("Code: " + fileName(output, table.useName().toLowerCase(), ".cpp"));
    try (OutputStream outFile = new FileOutputStream(fileName(output, table.useName().toLowerCase(), ".cpp")))
    {
      writer = new PrintWriter(outFile);
      writeln("/* ---------------------------------------------------------------------------------");
      writeln(" * This code was generated, do not modify it, modify it at source and regenerate it.");
      writeln(" * ---------------------------------------------------------------------------------");
      writeln(" */");
      writeln();
      writeln("#include \"" + fileName("", table.useName().toLowerCase(), ".h") + "\"");
      writeln();
      generateImplementation(table);
      writer.flush();
    }
  }

  static void generateInterface(Table table)
  {
    for (int i = 0; i < table.procs.size(); i++)
    {
      Proc proc = table.procs.elementAt(i);
      if (proc.isData)
        continue;
      generateInterface(table, proc);
    }
  }

  /**
   * Emits class method for processing the database activity
   */
  static void generateInterface(Table table, Proc proc)
  {
    String dataStruct;
    if (proc.isStdExtended())
      dataStruct = "T_" + table.useName();
    else
      dataStruct = "T_" + table.useName() + "_" + proc.name.toUpperCase();
    String query = "";
    if (proc.outputs.size() > 0)
      query = ", DB_QUERY *query";
    write("int " + table.useName().toLowerCase() + "_" + proc.name.toLowerCase() + "_execute");
    if (proc.hasNoData() || (proc.inputs.size() == 0 && proc.dynamics.size() == 0))
      writeln("(DB_CONNECTOR *connector" + query + ");");
    else
      writeln("(DB_CONNECTOR *connector" + query + ", " + dataStruct + " *data);");
    if (proc.outputs.size() > 0)
    {
      writeln("/**");
      writeln(" * Return of -1 indicates MySql error.");
      writeln(" *            0 indicates no (more) data found.");
      writeln(" *            1 indicates data found.");
      writeln(" */");
      writeln("int " + table.useName().toLowerCase() + "_" + proc.name.toLowerCase()
              + "_fetch(DB_QUERY *query, " + dataStruct + " *data);");
      if (proc.isSingle == false)
      {
        writeln("/**");
        writeln(" * This function loads the full result associated with 'data'.");
        writeln(" * Use with the data_seek function below, row will populate 'data' supplied here.");
        writeln(" */");
        writeln("int " + table.useName().toLowerCase() + "_" + proc.name.toLowerCase()
                + "_load_result(DB_QUERY *query, " + dataStruct + " *data);");
        writeln("/**");
        writeln(" * Use in conjunction with the load_result function above.");
        writeln(" * The 'data' record will be populated with the result row at offset.");
        writeln(" */");
        writeln("int " + table.useName().toLowerCase() + "_" + proc.name.toLowerCase()
                + "_data_seek(DB_QUERY *query, my_ulonglong offset);");
      }
    }
    writeln();
  }

  /**
   *
   */
  static void generateImplementation(Table table)
  {
    for (int i = 0; i < table.procs.size(); i++)
    {
      Proc proc = table.procs.elementAt(i);
      if (proc.isData)
        continue;
      generateImplementation(table, proc);
    }
  }

  static void generateCommand(Proc proc)
  {
    placeHolder = new PlaceHolder(proc, PlaceHolder.QUESTION, "");
    Vector lines = placeHolder.getLines();
    if (lines.size() > 0)
    {
      writeln("static char* " + proc.table.useName().toUpperCase() + "_" + proc.name.toUpperCase() + "_COMMAND =");
      for (int i = 0; i < lines.size(); i++)
      {
        String l = (String) lines.elementAt(i);
        if (l.charAt(0) != '"')
          writeln("\"[>" + l.trim() + "<]\\n\"");
        else
          writeln(l + "\"\\n\"");
      }
      writeln(";");
      int dynamicsize = placeHolder.getTotalDynamicSize();
      writeln("#define " + proc.table.useName().toUpperCase() + "_"
              + proc.name.toUpperCase() + "_DYNAMIC_SIZE " + dynamicsize);
    }
  }

  /**
   * Emits class method for processing the database activity
   */
  static void generateImplementation(Table table, Proc proc)
  {
    generateCommand(proc);
    Vector pairs = placeHolder.getPairs();
    String NOBINDS = table.name.toUpperCase() + "_" + proc.name.toUpperCase() + "_NOBINDS";
    String NODEFINES = table.name.toUpperCase() + "_" + proc.name.toUpperCase() + "_NODEFINES";
    writeln("#define " + NOBINDS + " " + pairs.size());
    writeln("#define " + NODEFINES + " " + proc.outputs.size());
    writeln();
    String dataStruct;
    if (proc.isStdExtended())
      dataStruct = "T_" + table.useName();
    else
      dataStruct = "T_" + table.useName() + "_" + proc.name.toUpperCase();
    String query = "";
    if (proc.outputs.size() > 0)
      query = ", DB_QUERY *query";
    write("int " + table.useName().toLowerCase() + "_" + proc.name.toLowerCase() + "_execute");
    if (proc.hasNoData() || (proc.inputs.size() == 0 && proc.dynamics.size() == 0))
      writeln("(DB_CONNECTOR *connector" + query + ")");
    else
      writeln("(DB_CONNECTOR *connector" + query + ", " + dataStruct + " *data)");
    writeln("{");
    writeln("  int rc = 0;");
    if (proc.outputs.size() == 0)
    {
      writeln("  DB_QUERY _query;");
      writeln("  DB_QUERY *query = &_query;");
    } else if (proc.inputs.size() == 0 && proc.dynamics.size() == 0)
    {
      writeln("  " + dataStruct + " *data;");
      writeln("  int data_length = sizeof(" + dataStruct + ");");
    }
    String COMMAND = proc.table.useName().toUpperCase() + "_" + proc.name.toUpperCase() + "_COMMAND";
    String DYNAMIC_SIZE = proc.table.useName().toUpperCase() + "_" + proc.name.toUpperCase() + "_DYNAMIC_SIZE";
    writeln("  rc = db_query_init(query, connector");
    writeln("                   , " + COMMAND);
    writeln("                   , " + DYNAMIC_SIZE);
    writeln("                   , " + NOBINDS);
    writeln("                   , " + NODEFINES);
    writeln("                   );");
    for (int i = 0; i < proc.dynamics.size(); i++)
    {
      String var = proc.dynamics.elementAt(i);
      writeln("  if (rc == 0) rc = db_query_dynamic(query, \"[>" + var + "<]\", data->" + var + ");");
    }
    for (int i = 0; i < pairs.size(); i++)
    {
      PlaceHolderPairs pair = (PlaceHolderPairs) pairs.elementAt(i);
      Field field = pair.field;
      String nullField = "0";
      if (isNull(field) == true)
        nullField = "&data->" + field.useName() + "_is_null";
      writeln("  if (rc == 0) rc = db_query_bind_" + bindType(field) + "(query, " + i
              + ", &data->" + field.useName() + bindLength(field)
              + ", " + nullField
              + ");");
    }
    writeln("  if (rc == 0) rc = db_query_execute(query);");
    if (proc.outputs.size() == 0)
      writeln("  if (rc == 0) rc = db_query_close(query);");
    writeln("  return rc;");
    writeln("}");
    writeln();
    if (proc.outputs.size() > 0)
    {
      writeln("/**");
      writeln(" * Return of -1 indicates MySql error.");
      writeln(" *            0 indicates no (more) data found.");
      writeln(" *            1 indicates data found.");
      writeln(" */");
      writeln("int " + table.useName().toLowerCase() + "_" + proc.name.toLowerCase()
              + "_fetch(DB_QUERY *query, " + dataStruct + " *data)");
      writeln("{");
      writeln("  int rc = 0;");
      writeln("  int rebind = 0;");
      writeln("  if (data != query->data)");
      writeln("  {");
      for (int i = 0; i < proc.outputs.size(); i++)
      {
        Field field = proc.outputs.elementAt(i);
        String nullField = "0";
        if (isNull(field) == true)
          nullField = "&data->" + field.useName() + "_is_null";
        writeln("    if (rc == 0) rc = db_query_define_" + bindType(field) + "(query, " + i
                + ", &data->" + field.useName() + bindLength(field)
                + ", " + nullField
                + ");");
      }
      writeln("    query->data = data;");
      writeln("    rebind = 1;");
      writeln("  }");
      writeln("  if (rc == 0) return db_query_fetch(query, rebind);");
      writeln("  return -1;");
      writeln("}");
      writeln();
      if (proc.isSingle == false)
      {
        writeln("/**");
        writeln(" * This function loads the full result associated with 'data'.");
        writeln(" * Use with the data_seek function below, row will populate 'data' supplied here.");
        writeln(" */");
        writeln("int " + table.useName().toLowerCase() + "_" + proc.name.toLowerCase()
                + "_load_result(DB_QUERY *query, " + dataStruct + " *data)");
        writeln("{");
        writeln("  int rc = 0;");
        for (int i = 0; i < proc.outputs.size(); i++)
        {
          Field field = proc.outputs.elementAt(i);
          String nullField = "0";
          if (isNull(field) == true)
            nullField = "&data->" + field.useName() + "_is_null";
          writeln("  if (rc == 0) rc = db_query_define_" + bindType(field) + "(query, " + i
                  + ", &data->" + field.useName() + bindLength(field)
                  + ", " + nullField
                  + ");");
        }
        writeln("  query->data = data;");
        writeln("  if (rc == 0) rc = db_query_load_result(query);");
        writeln("  return rc;");
        writeln("}");
        writeln();
        writeln("/**");
        writeln(" * Use in conjunction with the load_result function above.");
        writeln(" * The 'data' record will be populated with the result row at offset.");
        writeln(" */");
        writeln("int " + table.useName().toLowerCase() + "_" + proc.name.toLowerCase()
                + "_data_seek(DB_QUERY *query, int offset)");
        writeln("{");
        writeln("  db_query_data_seek(query, offset);");
        writeln("  return 0;");
        writeln("}");
        writeln();
      }
    }
  }

  /**
   * Translates field type to cpp data member type
   */
  static String cppVar(Field field)
  {
    switch (field.type)
    {
      case Field.BYTE:
        return "char   " + field.useName();
      case Field.SHORT:
        return "short  " + field.useName();
      case Field.BOOLEAN:
      case Field.INT:
      case Field.SEQUENCE:
      case Field.IDENTITY:
        return "int    " + field.useName();
      case Field.LONG:
        return "long   " + field.useName();
      case Field.CHAR:
      case Field.ANSICHAR:
        return "char   " + field.useName() + "[" + (field.length + 1) + "]";
      case Field.USERSTAMP:
        return "char   " + field.useName() + "[51]";
      case Field.BLOB:
      case Field.TLOB:
        return "TJLob  " + field.useName();
      case Field.DATE:
        return "char   " + field.useName() + "[9]";
      case Field.TIME:
        return "char   " + field.useName() + "[7]";
      case Field.DATETIME:
      case Field.TIMESTAMP:
        return "char   " + field.useName() + "[15]";
      case Field.FLOAT:
      case Field.DOUBLE:
      case Field.MONEY:
        return "double " + field.useName();
    }
    return field.useName() + " <unsupported>";
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

  static String bindLength(Field field)
  {
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
        return "";
    }
    return "[0], " + field.length;
  }

  static String bindType(Field field)
  {
    switch (field.type)
    {
      case Field.BYTE:
        return "tinyint";
      case Field.SHORT:
        return "short";
      case Field.BOOLEAN:
      case Field.INT:
      case Field.SEQUENCE:
      case Field.IDENTITY:
        return "int";
      case Field.LONG:
        return "long";
      case Field.CHAR:
      case Field.ANSICHAR:
        return "char";
      case Field.USERSTAMP:
        return "char";
      case Field.BLOB:
        return "blob";
      case Field.TLOB:
        return "tlob";
      case Field.DATE:
        return "date";
      case Field.TIME:
        return "time";
      case Field.DATETIME:
        return "datetime";
      case Field.TIMESTAMP:
        return "timestamp";
      case Field.FLOAT:
      case Field.DOUBLE:
        return "double";
      case Field.MONEY:
        return "double";
    }
    return field.useName() + " <unsupported>";
  }
}
