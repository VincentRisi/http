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

import vlab.jportal.*;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.util.Vector;

import static vlab.jportal.Writer.*;

public class MSSqlDDL extends Generator
{
  protected static Vector flagsVector;
  private static String tableOwner;
  private static String tableSchema;

  public static String description()
  {
    return "Generate MSSql DDL";
  }

  public static String documentation()
  {
    return "Generate MSSql DDL";
  }

  /**
   * Generates the SQL for SQLServer Table creation.
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
      if (database.schema.length() > 0)
      {
        tableOwner = database.schema + ".";
        tableSchema = database.schema + "_";
      } else
      {
        tableOwner = "";
        tableSchema = "";
      }
      outLog.println("DDL: " + output + fileName + ".sql");
      try (OutputStream outFile = new FileOutputStream(output + fileName + ".sql"))
      {
        writer = new PrintWriter(outFile);
        indent_size = 4;
        writeln("USE " + database.name);
        writeln();
        for (int i = 0; i < database.tables.size(); i++)
          generateTable(database.tables.elementAt(i));
        for (int i = 0; i < database.views.size(); i++)
          generateView(database.views.elementAt(i));
        for (int i = 0; i < database.sequences.size(); i++)
          generateSequence(database.sequences.elementAt(i), tableOwner);
        writer.flush();
      }
    }
    catch (IOException e1)
    {
      outLog.println("Generate SQLServer SQL IO Error");
    }
  }

  private static void generateSequence(Sequence sequence, String tableOwner)
  {
    writeln("DROP SEQUENCE " + tableOwner + sequence.name + ";");
    writeln("GO");
    writeln();
    writeln("CREATE SEQUENCE " + tableOwner + sequence.name);
    writeln("  MINVALUE  " + sequence.minValue);
    writeln("  MAXVALUE  " + sequence.maxValue);
    writeln("  INCREMENT BY " + sequence.increment);
    if (sequence.cycleFlag)
      writeln("  CYCLE");
    if (sequence.orderFlag)
      writeln("  ORDER");
    writeln("  START WITH " + sequence.startWith);
    writeln("GO");
    writeln();
    if (tableOwner.length() > 0)
    {
      writeln("DROP SYNONYM " + sequence.name);
      writeln("GO");
      writeln();
      writeln("CREATE SYNONYM " + sequence.name + " FOR " + tableOwner + sequence.name);
      writeln("GO");
      writeln();
    }
  }

  static String useExtra(String name, String extra)
  {
    String work = name + extra;
    int last = name.length() - 1;
    if ((name.charAt(0) == '[' && name.charAt(last) == ']')
            || (name.charAt(0) == '\"' && name.charAt(last) == '\"'))
      work = name.substring(0, last - 1) + extra + name.substring(last);
    return work;
  }

  static void generateTable(Table table)
  {
    boolean bigSequence = false;
    String tableName = tableOwner + table.useLiteral();
    String comma = "  ";
    writeln("IF OBJECT_ID('" + tableName + "','U') IS NOT NULL");
    writeln("    DROP TABLE " + tableName);
    writeln("GO");
    writeln();
    writeln("CREATE TABLE " + tableName);
    writeln("(");
    for (int i = 0; i < table.fields.size(); i++, comma = ", ")
    {
      Field field = table.fields.elementAt(i);
      write(comma + varType(field, false, table.hasSequenceReturning));
      if (field.defaultValue.length() > 0)
        write(" CONSTRAINT  DF_" + tableName + "_" + field.useLiteral() + " DEFAULT " + field.defaultValue);
      if (!field.isNull)
        writeln(" NOT NULL");
      else
        writeln(" NULL");
      if (field.type == Field.BIGSEQUENCE)
        bigSequence = true;
    }
    for (int i = 0; i < table.keys.size(); i++)
    {
      Key key = table.keys.elementAt(i);
      if (key.isPrimary)
        generatePrimary(key, tableSchema + table.name);
      else if (key.isUnique)
        generateUnique(key, tableSchema + table.name);
    }
    for (int i = 0; i < table.links.size(); i++)
    {
      Link link = table.links.elementAt(i);
      generateLink(link, tableSchema + table.name);
    }
    writeln(")");
    writeln("GO");
    writeln();
    if (table.hasSequence)
    {
      writeln("DROP SEQUENCE " + tableOwner + table.name + "Seq");
      writeln("GO");
      writeln();
      writeln("CREATE SEQUENCE " + tableOwner + table.name + "Seq");
      if (bigSequence == true)
        writeln("  AS BIGINT START WITH 0 INCREMENT BY 1");
      else
        writeln("  AS INT START WITH 0 INCREMENT BY 1");
      writeln("GO");
      writeln();
      if (tableOwner.length() > 0)
      {
        writeln("DROP SYNONYM " + table.name + "SEQ");
        writeln("GO");
        writeln();
        writeln("CREATE SYNONYM " + table.name + "SEQ FOR " + tableOwner + table.name + "SEQ");
        writeln("GO");
        writeln();
      }
//      if (table.grants.size() > 0)
//      {
//        Grant grant = table.grants.elementAt(0);
//        for (int j = 0; j < grant.users.size(); j++)
//        {
//          String user = grant.users.elementAt(j);
//          writeln("GRANT SELECT ON " + tableOwner + tableName + "SEQ TO " + user + "");
//          writeln("GO");
//          writeln();
//        }
//      }
    }
    for (int i = 0; i < table.keys.size(); i++)
    {
      Key key = table.keys.elementAt(i);
      if ((!key.isPrimary && !key.isUnique))
        generateKey(key, tableName);
    }
    for (int i = 0; i < table.grants.size(); i++)
    {
      Grant grant = table.grants.elementAt(i);
      generateGrant(grant, tableName);
    }
    for (int i = 0; i < table.views.size(); i++)
    {
      View view = table.views.elementAt(i);
      generateView(view, table);
    }
    for (int i = 0; i < table.procs.size(); i++)
    {
      Proc proc = table.procs.elementAt(i);
      if (proc.isData)
        generateData(proc);
    }
  }

  /**
   * Generates SQL code for SQL Server Index
   */
  static void generateKey(Key key, String table)
  {
    String comma = "  ";
    if (key.isPrimary)
      writeln("CREATE UNIQUE CLUSTERED INDEX " + key.name + " ON " + table);
    else if (key.isUnique)
      writeln("CREATE UNIQUE INDEX " + key.name + " ON " + table);
    else if (key.isClustered)
      writeln("CREATE CLUSTERED INDEX " + key.name + " ON " + table);
    else
      writeln("CREATE INDEX " + key.name + " ON " + table);
    writeln("(");
    for (int i = 0; i < key.fields.size(); i++, comma = ", ")
    {
      String name = key.fields.elementAt(i);
      writeln(comma + name);
    }
    writeln(")");
    for (int i = 0; i < key.options.size(); i++)
    {
      String option = key.options.elementAt(i);
      writeln(option);
    }
    writeln("GO");
    writeln();
    if (key.isPrimary)
    {
      writeln("sp_primarykey " + table);
      for (int i = 0; i < key.fields.size(); i++)
      {
        String name = key.fields.elementAt(i);
        writeln(", " + name);
      }
      writeln("GO");
      writeln();
    }
  }

  /**
   * Generates SQL code for ORACLE Primary Key create
   */
  static void generatePrimary(Key key, String tableName)
  {
    String comma = "    ";
    writeln(", CONSTRAINT PK_" + tableName + "_" + key.name + " PRIMARY KEY (");
    for (int i = 0; i < key.fields.size(); i++, comma = "  , ")
    {
      String name = key.fields.elementAt(i);
      writeln(comma + name);
    }
    writeln("  )");
  }

  /**
   * Generates SQL code for ORACLE Unique Key create
   */
  static void generateUnique(Key key, String tableName)
  {
    String comma = "    ";
    writeln(", CONSTRAINT UK_" + tableName + "_" + key.name + (key.isClustered ? " CLUSTERED UNIQUE (" : " UNIQUE ("));
    for (int i = 0; i < key.fields.size(); i++, comma = "  , ")
    {
      String name = key.fields.elementAt(i);
      writeln(comma + name);
    }
    writeln("  )");
  }

  /**
   * Generates foreign key SQL Code appended to table
   */
  static void generateLink(Link link, String table)
  {
    String comma = "    ";
    String temp = "";
    for (int i = 0; i < link.fields.size(); i++)
    {
      String name = link.fields.elementAt(i);
      temp += "_" + name;
    }
    writeln(", CONSTRAINT FK_" + table + "_" + link.useName() + temp + " FOREIGN KEY (");
    for (int i = 0; i < link.fields.size(); i++, comma = "   ,")
    {
      String name = link.fields.elementAt(i);
      writeln(comma + name);
    }
    writeln("  )");
    if (link.linkFields.size() > 0)
    {
      writeln("  REFERENCES " + tableOwner + link.name + "(");
      comma = "    ";
      for (int i = 0; i < link.linkFields.size(); i++, comma = "   ,")
      {
        String name = link.linkFields.elementAt(i);
        writeln(comma + name);
      }
      writeln("  )");
    } else
    {
      writeln("  REFERENCES " + tableOwner + link.name);
    }
    if (link.isDeleteCascade)
    {
      writeln("    ON DELETE CASCADE");
    }
    if (link.isUpdateCascade)
    {
      writeln("    ON UPDATE CASCADE");
    }
    for (int i = 0; i < link.options.size(); i++)
    {
      String option = (String) link.options.elementAt(i);
      writeln("    " + option);
    }
  }

  /**
   * Generates foreign key SQL Code for SQL Server
   */
  static void generateSpLink(Link link, String table)
  {
    writeln("sp_foreignkey " + table + ", " + link.name);
    for (int i = 0; i < link.fields.size(); i++)
    {
      String name = link.fields.elementAt(i);
      writeln(", " + name);
    }
    writeln("GO");
    writeln();
  }

  /**
   * Generates grant SQL Code for SQL Server
   */
  static void generateGrant(Grant grant, String object)
  {
    for (int i = 0; i < grant.perms.size(); i++)
    {
      String perm = grant.perms.elementAt(i);
      for (int j = 0; j < grant.users.size(); j++)
      {
        String user = grant.users.elementAt(j);
        writeln("GRANT " + perm + " ON " + object + " TO " + user);
        writeln("GO");
        writeln();
      }
    }
  }

  /**
   * Generates view SQL Code for SQL Server
   */
  private static void generateView(View view, String viewName)
  {
    writeln("IF OBJECT_ID('" + viewName + "','V') IS NOT NULL");
    writeln("    DROP VIEW " + viewName);
    writeln("GO");
    writeln();
    writeln("CREATE VIEW " + viewName);
    writeln("(");
    String comma = "  ";
    for (int i = 0; i < view.aliases.size(); i++)
    {
      String alias = (String) view.aliases.elementAt(i);
      writeln(comma + alias);
      comma = ", ";
    }
    writeln(") AS");
    writeln("(");
    for (int i = 0; i < view.lines.size(); i++)
    {
      String line = (String) view.lines.elementAt(i);
      writeln(line);
    }
    writeln(")");
    writeln("GO");
    writeln();
    for (int i = 0; i < view.users.size(); i++)
    {
      String user = (String) view.users.elementAt(i);
      writeln("GRANT SELECT ON " + viewName + " TO " + user);
    }
    if (view.users.size() > 0)
    {
      writeln("GO");
      writeln();
    }
  }

  static void generateView(View view, Table table)
  {
    String viewName = tableOwner + useExtra(table.useLiteral(), view.name);
    generateView(view, viewName);
  }

  static void generateView(View view)
  {
    generateView(view, view.name);
  }

  static void generateData(Proc proc)
  {
    for (int i = 0; i < proc.lines.size(); i++)
    {
      Line l = proc.lines.elementAt(i);
      if (l.line.startsWith("--start"))
        writeln("BEGIN TRANSACTION;");
      else
        writeln(l.line);
    }
    writeln();
  }

  /**
   * Translates field type to SQLServer SQL column types
   */
  static String varType(Field field, boolean typeOnly, boolean hasSequenceReturning)
  {
    switch (field.type)
    {
      case Field.BOOLEAN:
        return field.useLiteral() + " BIT";
      case Field.BYTE:
        return field.useLiteral() + " TINYINT";
      case Field.SHORT:
        return field.useLiteral() + " SMALLINT";
      case Field.INT:
        return field.useLiteral() + " INT";
      case Field.LONG:
        return field.useLiteral() + " BIGINT";
      case Field.SEQUENCE:
        return field.useLiteral() + " INTEGER";
      case Field.BIGSEQUENCE:
        return field.useLiteral() + " BIGINT";
      case Field.IDENTITY:
        if (typeOnly == true)
          return field.useLiteral() + " INTEGER";
        else
          return field.useLiteral() + " INTEGER IDENTITY(1,1)";
      case Field.BIGIDENTITY:
        if (typeOnly == true)
          return field.useLiteral() + " BIGINT";
        else
          return field.useLiteral() + " BIGINT IDENTITY(1,1)";
      case Field.CHAR:
        if (field.length > 8000)
        {
          return field.useLiteral() + " VARCHAR(MAX)";
        }
        return field.useLiteral() + " VARCHAR(" + field.length + ")";
      case Field.ANSICHAR:
        return field.useLiteral() + " CHAR(" + field.length + ")";
      case Field.UTF8:
        int utf8len = (field.length) * 6;
        if (utf8len < 4000)
          return format("%s NVARCHAR(%d)", field.useLiteral(), utf8len);
        else
          return format("%s NVARCHAR(MAX)", field.useLiteral());
      case Field.UNICODE:
        int unicodelen = (field.length + 1) * 4;
        if (unicodelen < 4000)
          return format("%s NVARCHAR2(%d)", field.useLiteral(), unicodelen);
        else
          return format("%s NVARCHAR(MAX)", field.useLiteral());
      case Field.WCHAR:
        if (field.length > 4000)
        {
          return field.useLiteral() + " NVARCHAR(MAX)";
        }
        return field.useLiteral() + " NVARCHAR(" + field.length + ")";
      case Field.WANSICHAR:
        return field.useLiteral() + " NCHAR(" + field.length + ")";
      case Field.DATE:
        return field.useLiteral() + " DATETIME";
      case Field.DATETIME:
        return field.useLiteral() + " DATETIME";
      case Field.TIME:
        return field.useLiteral() + " DATETIME";
      case Field.TIMESTAMP:
        return field.useLiteral() + " DATETIME";
      case Field.FLOAT:
      case Field.DOUBLE:
        if (field.precision > 15)
          return field.useLiteral() + " DECIMAL(" + field.precision + "," + field.scale + ")";
        return field.useLiteral() + " FLOAT";
      case Field.BLOB:
        return field.useLiteral() + " IMAGE";
      case Field.TLOB:
        return field.useLiteral() + " TEXT";
      case Field.BIGXML:
      case Field.XML:
        return field.useLiteral() + " XML";
      case Field.MONEY:
        return field.useLiteral() + " MONEY";
      case Field.USERSTAMP:
        return field.useLiteral() + " VARCHAR(50)";
      case Field.UID:
        return field.useLiteral() + " UNIQUEIDENTIFIER";
    }
    return "unknown";
  }
}
