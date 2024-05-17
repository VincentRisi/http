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

public class OracleDDL extends Generator
{
  private static boolean addExit = false;
  private static boolean dropTablespace = false;

  /**
   * Generates the SQL for ORACLE Table creation.
   */
  public static String description()
  {
    return "Generate Oracle DDL";
  }

  public static String documentation()
  {
    return "Generate Oracle DDL.";
  }

  private static void setFlags(Database database, PrintWriter outLog)
  {
    for (int findex = 0; findex < database.flags.size(); findex++)
    {
      String flag = database.flags.elementAt(findex);
      {
        if (flag.equals("addExit") == true)
          addExit = true;
        else if (flag.equals("dropTablespace") == true)
          dropTablespace = true;
      }
    }
    if (addExit == false)
      outLog.println("-- No addExit flag detected.");
  }

  public static void generate(Database database, String output, PrintWriter outLog)
  {
    try
    {
      String tableOwner = "";
      String fileName;
      setFlags(database, outLog);
      if (database.output.length() > 0)
        fileName = database.output;
      else
        fileName = database.name;
      outLog.println("DDL: " + output + fileName + ".sql");
      try (OutputStream outFile = new FileOutputStream(output + fileName + ".sql"))
      {
        writer = new PrintWriter(outFile);
        if (database.schema.length() > 0)
          tableOwner = database.schema + ".";
        else if (database.userid.length() > 0)
          tableOwner = database.userid + ".";
        if (database.password.length() > 0)
        {
          if (addExit == false)
            outLog.println("-- An exit added because a CONNECT is generated.");
          writeln("CONNECT " + database.userid + "/" + database.password + "@" + database.server);
          writeln();
          addExit = true;
        }
        for (int i = 0; i < database.tables.size(); i++)
          generate(database.tables.elementAt(i));
        for (int i = 0; i < database.views.size(); i++)
          generate(database.views.elementAt(i), null, tableOwner);
        for (int i = 0; i < database.sequences.size(); i++)
          generate(database.sequences.elementAt(i), tableOwner);
        if (addExit == true)
        {
          writeln("exit");
          writeln();
        }
        writer.flush();
      }
    }
    catch (IOException e1)
    {
      outLog.println("Generate Oracle SQL IO Error");
    }
  }

  static String bSO(int i)
  {
    String x = "" + (101 + i);
    return x.substring(1);
  }

  static String fixDefaultValue(Field field)
  {
    int len = field.defaultValue.length();
    switch (field.type)
    {
      case Field.DATE:
        if (len == 8)
          return format("to_date('%s', 'YYYYMMDD')", field.defaultValue);
        return format("to_date('%s', 'YYYY-MM-DD')", field.defaultValue);
      case Field.DATETIME:
        if (len == 14)
          return format("to_date('%s', 'YYYYMMDDHH24MISS')", field.defaultValue);
        if (len >= 19)
          return format("to_date('%s', 'YYYY-MM-DD HH24:MI:SS')", field.defaultValue.substring(0, 19));
        break;
      case Field.TIME:
        if (len == 6)
          return format("to_date('%s', 'HH24MISS')", field.defaultValue);
        return format("to_date('%s', 'HH24:MI:SS')", field.defaultValue);
      case Field.TIMESTAMP:
      case Field.AUTOTIMESTAMP:
        return "CURRENT_TIMESTAMP";
      default:
        return field.defaultValue;
    }
    return field.defaultValue;
  }

  static void generate(Table table)
  {
    String tableOwner = "";
    if (table.database.schema.length() > 0)
      tableOwner = table.database.schema + ".";
    else if (table.database.server.length() > 0)
      tableOwner = table.database.server + ".";
    else if (table.database.userid.length() > 0)
      tableOwner = table.database.userid + ".";
    String comma = "( ";
    boolean hasNotNull = false;
    boolean bigSequence = false;
    if (table.fields.size() > 0)
    {
      writeln("DROP TABLE " + tableOwner + table.fixEscape() + " CASCADE CONSTRAINTS;");
      writeln();
      writeln("CREATE TABLE " + tableOwner + table.fixEscape());
      for (int i = 0; i < table.fields.size(); i++, comma = ", ")
      {
        Field field = table.fields.elementAt(i);
        writeln(comma + field.fixEscape() + " " + varType(field));
        if (field.defaultValue.length() > 0)
          hasNotNull = true;
        if (field.checkValue.length() > 0)
          hasNotNull = true;
        else if (!field.isNull)
          hasNotNull = true;
        if (field.type == Field.BIGSEQUENCE)
          bigSequence = true;
      }
      write(")");
      if (table.options.size() > 0)
      {
        for (int i = 0; i < table.options.size(); i++)
        {
          String option = table.options.elementAt(i);
          if (option.toUpperCase().startsWith("TABLESPACE") && dropTablespace == false)
          {
            int n = option.toUpperCase().indexOf("INDEX ");
            writeln();
            if (n > 0)
            {
              write(option.substring(0, n - 1));
              table.options.addElement(option.substring(n));
            } else
              write(option);
          }
        }
      }
      writeln(";");
      writeln();
      writeln("DROP PUBLIC SYNONYM " + table.fixEscape() + ";");
      writeln();
      writeln("CREATE PUBLIC SYNONYM " + table.fixEscape() + " FOR " + tableOwner + table.fixEscape() + ";");
      writeln();
      for (int i = 0; i < table.grants.size(); i++)
      {
        Grant grant = table.grants.elementAt(i);
        generate(grant, tableOwner + table.fixEscape());
      }
      if (table.hasSequence)
      {
        writeln("DROP SEQUENCE " + tableOwner + table.name + "Seq;");
        writeln();
        writeln("CREATE SEQUENCE " + tableOwner + table.name + "Seq");
        writeln("  MINVALUE 1");
        if (bigSequence == true)
          writeln("  MAXVALUE 999999999999999999");
        else
          writeln("  MAXVALUE 999999999");
        writeln("  CYCLE");
        writeln("  ORDER;");
        writeln();
        writeln("DROP PUBLIC SYNONYM " + table.name + "SEQ;");
        writeln();
        writeln("CREATE PUBLIC SYNONYM " + table.name + "SEQ FOR " + tableOwner + table.name + "SEQ;");
        writeln();
        if (table.grants.size() > 0)
        {
          Grant grant = table.grants.elementAt(0);
          for (int j = 0; j < grant.users.size(); j++)
          {
            String user = grant.users.elementAt(j);
            writeln("GRANT SELECT ON " + tableOwner + table.name + "SEQ TO " + user + ";");
            writeln();
          }
        }
      }
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
      generate(view, table, tableOwner);
    }
    if (hasNotNull == true)
    {
      writeln("ALTER TABLE " + tableOwner + table.name);
      writeln("MODIFY");
      comma = "( ";
      for (int i = 0; i < table.fields.size(); i++, comma = ", ")
      {
        Field field = table.fields.elementAt(i);
        if (field.isNull && field.defaultValue.length() == 0 && field.checkValue.length() == 0)
          continue;
        if (field.defaultValue.length() > 0)
        {
          writeln(format("%s%s DEFAULT %s", comma, field.fixEscape(), fixDefaultValue(field)));
          continue;
        }
        write(comma + field.fixEscape() + " CONSTRAINT " + table.name + "_NN" + bSO(i));
        if (field.checkValue.length() > 0)
          write(" CHECK (" + field.checkValue + ")");
        else
          write(" NOT NULL");
        writeln();
      }
      writeln(");");
      writeln();
    }
    if (table.keys.size() > 0)
    {
      String mComma = "( ";
      writeln("ALTER TABLE " + tableOwner + table.name);
      writeln("ADD");
      for (int i = 0; i < table.keys.size(); i++)
      {
        Key key = table.keys.elementAt(i);
        if (key.isPrimary)
          generatePrimary(table, key, mComma);
        else if (key.isUnique)
          generateUnique(table, key, mComma);
        mComma = ", ";
      }
      writeln(");");
      writeln();
    }
    if (table.links.size() > 0)
    {
      String mComma = "( ";
      writeln("ALTER TABLE " + tableOwner + table.name);
      writeln("ADD");
      for (int i = 0; i < table.links.size(); i++)
      {
        Link link = table.links.elementAt(i);
        if (link.linkName.length() == 0)
          link.linkName = table.name + "_FK" + bSO(i);
        generate(link, mComma);
        mComma = ", ";
      }
      writeln(");");
      writeln();
    }
    for (int i = 0; i < table.procs.size(); i++)
    {
      Proc proc = table.procs.elementAt(i);
      if (proc.isData)
        generate(proc);
    }
  }

  /**
   * Generates SQL code for ORACLE Primary Key create
   */
  static void generatePrimary(Table table, Key key, String mcomma)
  {
    String comma = "  ( ";
    String keyname = key.name.toUpperCase();
    if (!keyname.contains(table.name.toUpperCase()))
      keyname = table.name.toUpperCase() + "_" + keyname;
    writeln(mcomma + "CONSTRAINT " + keyname + " PRIMARY KEY");
    for (int i = 0; i < key.fields.size(); i++, comma = "  , ")
    {
      String name = key.fields.elementAt(i);
      writeln(comma + name);
    }
    writeln("  )");
    for (int i = 0; i < key.options.size(); i++)
    {
      String option = key.options.elementAt(i);
      if (option.toUpperCase().startsWith("INDEX IN"))
      {
        write("TABLESPACE " + option.substring(9));
        break;
      }
    }
  }

  /**
   * Generates SQL code for ORACLE Unique Key create
   */
  static void generateUnique(Table table, Key key, String mcomma)
  {
    String comma = "  ( ";
    String keyname = key.name.toUpperCase();
    if (!keyname.contains(table.name.toUpperCase()))
      keyname = table.name.toUpperCase() + "_" + keyname;
    writeln(mcomma + "CONSTRAINT " + keyname + " UNIQUE");
    for (int i = 0; i < key.fields.size(); i++, comma = "  , ")
    {
      String name = key.fields.elementAt(i);
      writeln(comma + name);
    }
    writeln("  )");
    for (int i = 0; i < key.options.size(); i++)
    {
      String option = key.options.elementAt(i);
      if (option.toUpperCase().startsWith("INDEX IN"))
      {
        write("TABLESPACE " + option.substring(9));
        break;
      }
    }
  }

  /**
   * Generates SQL code for ORACLE Index create
   */
  static void generateIndex(Table table, Key key)
  {
    String tableOwner = "";
    if (table.database.userid.length() > 0)
      tableOwner = table.database.userid + ".";
    String comma = "( ";
    String keyname = key.name.toUpperCase();
    if (!keyname.contains(table.name.toUpperCase()))
      keyname = table.name.toUpperCase() + "_" + keyname;
    writeln("DROP INDEX " + keyname + ";");
    writeln("");
    writeln("CREATE INDEX " + keyname + " ON " + tableOwner + table.name);
    for (int i = 0; i < key.fields.size(); i++, comma = ", ")
    {
      String name = key.fields.elementAt(i);
      writeln(comma + name);
    }
    write(")");
    for (int i = 0; i < key.options.size(); i++)
    {
      writeln();
      String option = key.options.elementAt(i);
      if (option.toUpperCase().startsWith("INDEX IN"))
      {
        write("TABLESPACE " + option.substring(9));
        break;
      }
    }
    writeln(";");
    writeln();
  }

  /**
   * Generates foreign key SQL Code for Oracle
   */
  static void generate(Link link, String mComma)
  {
    String comma = "  ( ";
    writeln(mComma + "CONSTRAINT " + link.linkName + " FOREIGN KEY");
    for (int i = 0; i < link.fields.size(); i++, comma = "  , ")
    {
      String name = link.fields.elementAt(i);
      writeln(comma + name);
    }
    writeln("  ) REFERENCES " + link.name);
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
    //writeln(";");
    writeln();
  }

  /**
   * Generates grants for Oracle
   */
  static void generate(Grant grant, String object)
  {
    for (int i = 0; i < grant.perms.size(); i++)
    {
      String perm = grant.perms.elementAt(i);
      for (int j = 0; j < grant.users.size(); j++)
      {
        String user = grant.users.elementAt(j);
        writeln("GRANT " + perm + " ON " + object + " TO " + user + ";");
        writeln();
      }
    }
  }

  /**
   * Generates views for Oracle
   */
  static void generate(View view, Table table, String tableOwner)
  {
    String viewName = view.name;
    if (table != null)
      viewName = table.useExtra(view.name);
    writeln("CREATE OR REPLACE FORCE VIEW " + viewName);
    String comma = "( ";
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
    writeln(");");
    writeln();
    for (int i = 0; i < view.users.size(); i++)
    {
      String user = (String) view.users.elementAt(i);
      writeln("GRANT SELECT ON " + viewName + " TO " + user + ";");
    }
    writeln();
    writeln("DROP PUBLIC SYNONYM " + viewName + ";");
    writeln();
    writeln("CREATE PUBLIC SYNONYM " + viewName + " FOR " + tableOwner + viewName + ";");
    writeln();
  }

  /**
   * Generates pass through data for Oracle
   */
  static void generate(Proc proc)
  {
    for (int i = 0; i < proc.lines.size(); i++)
    {
      Line l = proc.lines.elementAt(i);
      writeln(l.line);
    }
    writeln();
  }

  /**
   * Generates pass through data for Oracle
   */
  static void generate(Sequence sequence, String tableOwner)
  {
    writeln("DROP SEQUENCE " + sequence.name + ";");
    writeln();
    writeln("CREATE SEQUENCE " + sequence.name);
    writeln("  MINVALUE  " + sequence.minValue);
    writeln("  MAXVALUE  " + sequence.maxValue);
    writeln("  INCREMENT BY " + sequence.increment);
    if (sequence.cycleFlag)
      writeln("  CYCLE");
    if (sequence.orderFlag)
      writeln("  ORDER");
    writeln("  START WITH " + sequence.startWith + ";");
    writeln();
    writeln("DROP PUBLIC SYNONYM " + sequence.name + ";");
    writeln();
    writeln("CREATE PUBLIC SYNONYM " + sequence.name + " FOR " + tableOwner + sequence.name + ";");
    writeln();
  }

  /**
   * Translates field type to Oracle SQL column types
   */
  static String varType(Field field)
  {
    switch (field.type)
    {
      case Field.BYTE:
        return "NUMBER(3)";
      case Field.SHORT:
        return "NUMBER(5)";
      case Field.INT:
      case Field.SEQUENCE:
        return "NUMBER(10)";
      case Field.LONG:
      case Field.BIGSEQUENCE:
        return "NUMBER(18)";
      case Field.CHAR:
      case Field.USERSTAMP:
        return "VARCHAR2(" + field.length + ")";
      case Field.ANSICHAR:
        return "CHAR(" + field.length + ")";
      case Field.UTF8:
        return format("NVARCHAR2(%d)", (field.length) * 6);
      case Field.DATE:
      case Field.DATETIME:
      case Field.TIME:
      case Field.TIMESTAMP:
        return "DATE";
      case Field.FLOAT:
      case Field.DOUBLE:
        if (field.scale != 0)
          return "NUMBER(" + field.precision + ", " + field.scale + ")";
        else if (field.precision != 0)
          return "NUMBER(" + field.precision + ")";
        return "NUMBER";
      case Field.BLOB:
        return "BLOB";
      case Field.TLOB:
        return "CLOB";
      case Field.IMAGE:
        return "LONG RAW";
      case Field.UNICODE:
        return format("NVARCHAR2(%d)", (field.length + 1) * 4);
      case Field.MONEY:
        return "NUMBER(15,2)";
      case Field.IDENTITY:
        return "<not supported>";
      case Field.XML:
      case Field.BIGXML:
        return "XMLTYPE";
      default:
        return "<not yet supported>";
    }
  }
}
