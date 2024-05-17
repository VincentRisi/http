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
/// $Date: 2004/10/18 13:45:44 $
/// $Revision: 411.1 $ // YMM.Revision
/// ------------------------------------------------------------------

package vlab.jportal.ddl;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;

import static vlab.jportal.Writer.*;

import vlab.jportal.*;

public class Lite3DDL extends Generator
{
  private static String tableOwner;

  public static String description()
  {
    return "Generate Lite3 DDL";
  }

  public static String documentation()
  {
    return "Generate Lite3 DDL.";
  }

  /**
   * Generates the SQL for Lite3 Table creation.
   */
  public static void generate(Database database, String output, PrintWriter outLog)
  {
    try
    {
      String fileName;
      if (database.output.length() > 0)
        fileName = database.output;
      else
        fileName = database.name;
//      if (database.schema.length() > 0) // does not have multiple schemas - main or temp only
//        tableOwner = database.schema + ".";
//      else
        tableOwner = "";
      outLog.println("DDL: " + output + fileName + ".sql");
      try (OutputStream outFile = new FileOutputStream(output + fileName + ".sql"))
      {
        writer = new PrintWriter(outFile);
        indent_size = 4;
        //PrintWriter outData = new PrintWriter(outFile);
        for (int i = 0; i < database.tables.size(); i++)
          generateTable(database, database.tables.elementAt(i));
        writer.flush();
      }
    }
    catch (IOException e1)
    {
      outLog.println("Generate LITE3 SQL IO Error");
    }
  }

  /**
   * @param database
   * @param table
   */
  private static void generateTable(Database database, Table table)
  {
    String tablename = tableOwner + table.fixEscape();
    if (table.fields.size() > 0)
    {
      writeln("DROP TABLE IF EXISTS " + tablename + ";");
      writeln();
    }
    String comma = "( ";
    boolean primeDone = false;
    if (table.fields.size() > 0)
    {
      writeln("CREATE TABLE " + tablename);
      for (int i = 0; i < table.fields.size(); i++, comma = ", ")
      {
        Field field = table.fields.elementAt(i);
        if (field.isSequence && field.isPrimaryKey)
          primeDone = true;
        writeln(comma + field.fixEscape() + " " + varType(field));
      }
      for (int i = 0; i < table.keys.size(); i++)
      {
        Key key = table.keys.elementAt(i);
        if (key.isPrimary == true && primeDone == false)
          generatePrimary(table, key);
        if (key.isUnique == true)
          generateUnique(table, key);
      }
      if (table.links.size() > 0)
      {
        for (int i = 0; i < table.links.size(); i++)
        {
          Link link = table.links.elementAt(i);
          if (link.linkName.length() == 0)
            link.linkName = table.name + "_FK" + bSO(i);
          generateLink(link, table.name, "", i);
        }
      }
      if (table.options.size() > 0)
      {
        for (int i = 0; i < table.options.size(); i++)
        {
          String option = table.options.elementAt(i);
          if (option.toLowerCase().indexOf("constraint") == 0)
            writeln(", " + option);
        }
      }
      write(")");
      writeln(";");
      writeln();
      for (int i = 0; i < table.keys.size(); i++)
      {
        Key key = table.keys.elementAt(i);
        if (!key.isPrimary && !key.isUnique)
          generateIndex(table, key);
      }
    }
    for (int i = 0; i < table.views.size(); i++)
    {
      View view = table.views.elementAt(i);
      generateView(view, table, tableOwner);
    }
    for (int i = 0; i < table.procs.size(); i++)
    {
      Proc proc = table.procs.elementAt(i);
      if (proc.isData)
        generateProc(proc);
    }
  }

  /**
   * @param proc
   */
  private static void generateProc(Proc proc)
  {
    for (int i = 0; i < proc.lines.size(); i++)
    {
      Line l = proc.lines.elementAt(i);
      writeln(l.line);
    }
    writeln();
  }

  /**
   * @param table
   * @param key
   */
  private static void generateUnique(Table table, Key key)
  {
    String comma = " (";
    write(", UNIQUE");
    for (int i = 0; i < key.fields.size(); i++, comma = ", ")
    {
      String name = key.fields.elementAt(i);
      write(comma + name);
    }
    writeln(")");
  }

  /**
   * @param table
   * @param key
   */
  private static void generatePrimary(Table table, Key key)
  {
    String comma = " (";
    write(", PRIMARY KEY");
    for (int i = 0; i < key.fields.size(); i++, comma = ", ")
    {
      String name = key.fields.elementAt(i);
      write(comma + name);
    }
    writeln(")");
  }

  /**
   * Generates foreign key SQL Code for DB2
   */
  static void generateLink(Link link, String tableName, String owner, int no)
  {
    String comma = "( ";
    String linkname = "FK" + no + link.linkName.toUpperCase();
    writeln(", CONSTRAINT " + make18(linkname) + " FOREIGN KEY");
    for (int i = 0; i < link.fields.size(); i++, comma = "    , ")
    {
      String name = link.fields.elementAt(i);
      writeln(comma + name);
    }
    write(") REFERENCES " + owner + link.name);
    if (link.linkFields.size() > 0)
    {
      comma = "(";
      for (int i = 0; i < link.linkFields.size(); i++)
      {
        String name = link.linkFields.elementAt(i);
        write(comma + name);
        comma = ", ";
      }
      write(")");
    }
    if (link.isDeleteCascade)
      write(" ON DELETE CASCADE");
    writeln();
  }

  static String bSO(int i)
  {
    String x = "" + (101 + i);
    return x.substring(1);
  }

  static String make18(String data)
  {
    if (data.length() <= 18)
      return data;
    String x = "_UOIEAY";
    for (int i = 0; i < x.length(); i++)
    {
      char lookup = x.charAt(i);
      int n = data.indexOf(lookup);
      while (n != -1)
      {
        if (n == 0)
          data = data.substring(1);
        else if (n == data.length() - 1)
          data = data.substring(0, n);
        else
          data = data.substring(0, n) + data.substring(n + 1);
        if (data.length() <= 18)
          return data;
        n = data.indexOf(lookup);
      }
    }
    return data.substring(0, 18);
  }

  /**
   * @param name
   * @param extra
   */
  static String useExtra(String name, String extra)
  {
    String work = name + extra;
    int last = name.length() - 1;
    if (name.charAt(0) == '\"' && name.charAt(last) == '\"')
      work = name.substring(0, last - 1) + extra + name.substring(last);
    return work;
  }

  private static void generateView(View view, Table table, String tableOwner)
  {
    String viewname = tableOwner + useExtra(table.fixEscape(), view.name);
    writeln("DROP VIEW IF EXISTS " + viewname);
    writeln("");
    writeln("CREATE VIEW " + viewname);
    writeln("AS (");
    for (int i = 0; i < view.lines.size(); i++)
    {
      String line = (String) view.lines.elementAt(i);
      writeln(line);
    }
    writeln(");");
    writeln();
  }

  /**
   * @param table
   * @param key
   */
  private static void generateIndex(Table table, Key key)
  {
    String comma = "( ";
    String keyname = key.name.toUpperCase();
    if (keyname.indexOf(table.name.toUpperCase()) == -1)
      keyname = table.name.toUpperCase() + "_" + keyname;
    writeln("DROP INDEX IF EXISTS " + tableOwner + table.name + keyname + ";");
    writeln("");
    write("CREATE INDEX " + tableOwner + table.name + keyname + " ON " + tableOwner + table.name);
    for (int i = 0; i < key.fields.size(); i++, comma = ", ")
    {
      String name = key.fields.elementAt(i);
      writeln(comma + name);
    }
    writeln(");");
    writeln();
  }

  /**
   * @param field
   * @return
   */
  private static String varType(Field field)
  {
    //if (field.isNull == true)
    //  return "NULL";
    String notNull = (field.isNull == true) ? "" : " NOT NULL";
    String primeKey = "";
    String autoInc = "";
    String defaultValue = "";

    if (field.defaultValue.length() > 0)
      defaultValue = " DEFAULT " + field.defaultValue;
    if (field.isSequence && field.isPrimaryKey)
    {
      primeKey = " PRIMARY KEY";
      autoInc = " AUTOINCREMENT";
      defaultValue = "";
    }
    String work = switch (field.type)
            {
              case Field.BYTE, Field.SHORT, Field.INT, Field.IDENTITY, Field.SEQUENCE, Field.LONG, Field.BIGIDENTITY, Field.BIGSEQUENCE -> "INTEGER";
              case Field.BOOLEAN -> "BOOLEAN";
              case Field.ANSICHAR, Field.TLOB, Field.XML, Field.BIGXML -> "TEXT";
              case Field.CHAR -> " VARCHAR(" + field.length + ")";
              case Field.UTF8 -> format(" NVARCHAR(%d)", field.length * 6);
              case Field.UNICODE -> format(" NVARCHAR(%d)", (field.length + 1) * 4);
              case Field.USERSTAMP -> " VARCHAR(50)";
              case Field.UID -> " VARCHAR(36)";
              case Field.FLOAT, Field.DOUBLE, Field.MONEY -> "REAL";
              case Field.BLOB -> "BLOB";
              case Field.DATE -> "DATE";
              case Field.DATETIME -> "DATETIME";
              case Field.TIME -> "TIME";
              case Field.TIMESTAMP -> "DATETIME";
              default -> "unknown";
            };
    return work + notNull + defaultValue + primeKey + autoInc;
  }
}
