///
/// System : JPortal
/// $Source: /main/nedcor/cvsroot/dev/jportal/DelphiCode.java,v $
/// $Author: vince $
/// $Date: 2003/02/12 16:02:30 $
/// $Revision: 1.4 $
///

package vlab.jportal.code;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.PrintWriter;
import java.util.Vector;

import static vlab.jportal.Writer.*;
import vlab.jportal.*;


public class DelphiCode extends Generator
{
  protected static PrintWriter outLog;
  static PlaceHolder placeHolder;
 
  public static String description()
  {
    return "Generate Delphi Lite3 Code";
  }

  public static String documentation()
  {
    return "Generate Delphi Lite3 Code";
  }
  
  public static void generate(Database database, String output, PrintWriter outLog)
  {
    DelphiCode.outLog = outLog;
    for (int i=0; i<database.tables.size(); i++)
    {
      try
      {
        Table table = (Table) database.tables.elementAt(i);
        generate(table, output);
      }
      catch (Exception ex)
      {
        outLog.println(ex);
        ex.printStackTrace(outLog);
      }
    }
  }
  static String fileName(String output, String node, String ext)
  {
    return output + node + ext;
  }  /**
  * Build of standard and user defined procedures
  */
  static void generate(Table table, String output) throws Exception
  {
    outLog.println("Code: "+output+table.useName() + ".pas");
    try (PrintWriter outData = new PrintWriter(new FileOutputStream(fileName(output, table.useName(), ".pas"))))
    {
      writer = outData;
      indent_size = 2;
        //PrintWriter outData = new PrintWriter(outFile);
        writeln("unit "+table.useName()+";");
        writeln("// This code was generated, do not modify it, modify it at source and regenerate it.");
        writeln();
        writeln("{$mode objfpc}{$H+}");
        writeln();
        writeln("interface");
        writeln();
        writeln("uses Classes, SysUtils, SQLDB, SQLite3Conn;");
        writeln();
        if (table.hasStdProcs)
          generateStdInterface(table);
        generateOtherInterface(table);
        for (int i=0; i<table.procs.size(); i++)
        {
          Proc proc = (Proc) table.procs.elementAt(i);
          if (proc.isData)
            continue;
          if (proc.dynamics.size() < 1)
          {
            writeln("var");
            writeln(1, table.useName()+proc.upperFirst()+" : AnsiString =");
            generateSQLCode(proc);
            writeln();
          }
        }
        writeln("implementation");
        writeln();
        if (table.hasStdProcs)
          generateStdImplementation(table);
        generateOtherImplementation(table);
        writeln("end.");
        outData.flush();
    }
  }
  /**
  * Build of all required standard procedures
  */
  static void generateStdInterface(Table table)
  {
    for (int i=0; i < table.comments.size(); i++)
    {
      String s = (String) table.comments.elementAt(i);
      writeln("//"+s);
    }
    writeln("type T"+table.useName()+" = Class");
    writeln(1, "Conn : TSQLite3Connection;");
    writeln(1, "Tran : TSQLTransaction;");
    for (int i=0; i<table.fields.size(); i++)
    {
      Field field = (Field) table.fields.elementAt(i);
      if (field.comments.size() > 0)
      {
        for (int c=0; c < field.comments.size(); c++)
        {
          String s = (String) field.comments.elementAt(c);
          writeln(1,"//"+s);
        }
      }
      writeln(1, delphiVar(field)+";");
      if (field.isNull) // && notString(field))
        writeln(1, field.useName()+"IsNull : boolean;");
    }
    writeln(1, "constructor Create(const aConn : TSQLite3Connection; const aTran : TSQLTransaction);");
    for (int i=0; i<table.procs.size(); i++)
    {
      Proc proc = (Proc) table.procs.elementAt(i);
      if (proc.isData)
        continue;
      if (proc.isStd || proc.hasNoData())
        generateInterface(proc);
    }
    writeln("end;");
    writeln();
  }
  /**
  * Build of all required standard procedures
  */
  static void generateStdImplementation(Table table)
  {
    writeln("constructor T"+table.useName()+".Create(const aConn : TSQLite3Connection; const aTran : TSQLTransaction);");
    writeln("begin");
    writeln(1, "Conn := aConn;");
    writeln(1, "Tran := aTran;");
    for (int i=0; i<table.fields.size(); i++)
    {
      Field field = (Field) table.fields.elementAt(i);
      writeln(1, initDelphiVar(field));
      if (field.isNull) // && notString(field))
        writeln(1, field.useName()+"IsNull := false;");
    }
    writeln("end;");
    writeln();
    for (int i=0; i<table.procs.size(); i++)
    {
      Proc proc = (Proc) table.procs.elementAt(i);
      if (proc.isData)
        continue;
      if (proc.isStd || proc.hasNoData())
        generateImplementation(proc, table.useName(), table.useName());
    }
  }
  /**
  * Build of user defined procedures
  */
  static void generateOtherInterface(Table table)
  {
    for (int i=0; i<table.procs.size(); i++)
    {
      Proc proc = (Proc) table.procs.elementAt(i);
      if (proc.isData || proc.isStd || proc.hasNoData())
        continue;
      for (int j=0; j<proc.comments.size(); j++)
      {
        String comment = (String) proc.comments.elementAt(j);
        writeln("//"+comment);
      }
      writeln("type T" + table.useName() + proc.upperFirst() + " = Class");
      writeln(1, "Conn : TSQLite3Connection;");
      writeln(1, "Tran : TSQLTransaction;");
      for (int j=0; j<proc.inputs.size(); j++)
      {
        Field field = (Field) proc.inputs.elementAt(j);
        for (int c=0; c < field.comments.size(); c++)
        {
          String s = (String) field.comments.elementAt(c);
          writeln(1, "//"+s);
        }
        writeln("  "+delphiVar(field)+";");
        if (field.isNull) // && notString(field))
          writeln(1, field.useName()+"IsNull : boolean;");
      }
      for (int j=0; j<proc.outputs.size(); j++)
      {
        Field field = (Field) proc.outputs.elementAt(j);
        if (!proc.hasInput(field.name))
        {
          for (int c=0; c < field.comments.size(); c++)
          {
            String s = (String) field.comments.elementAt(c);
            writeln(1, "//"+s);
          }
          writeln("  "+delphiVar(field)+";");
          if (field.isNull) // && notString(field))
            writeln(1, field.useName()+"IsNull : boolean;");
        }
      }
      for (int j=0; j<proc.dynamics.size(); j++)
      {
        String s = (String) proc.dynamics.elementAt(j);
        writeln(1, s+" ;");
      }
      writeln("  constructor Create(const aConn : TSQLite3Connection; const aTran : TSQLTransaction);");
      generateInterface(proc);
      writeln("end;");
      writeln();
    }
  }
  static void generateOtherImplementation(Table table)
  {
    for (int i=0; i<table.procs.size(); i++)
    {
      Proc proc = (Proc) table.procs.elementAt(i);
      if (proc.isData || proc.isStd || proc.hasNoData())
        continue;
      writeln("constructor T"+table.useName()+proc.name+".Create(const aConn : TSQLite3Connection; const aTran : TSQLTransaction);");
      writeln("begin");
      writeln(1, "Conn := aConn;");
      writeln(1, "Tran := aTran;");
      for (int j=0; j<proc.inputs.size(); j++)
      {
        Field field = (Field) proc.inputs.elementAt(j);
        writeln("  "+initDelphiVar(field));
        if (field.isNull) // && notString(field))
          writeln(1, field.useName()+"IsNull := false;");
      }
      for (int j=0; j<proc.outputs.size(); j++)
      {
        Field field = (Field) proc.outputs.elementAt(j);
        if (!proc.hasInput(field.name))
          writeln("  "+initDelphiVar(field));
        if (field.isNull) // && notString(field))
          writeln(1, field.useName()+"IsNull := false;");
      }
      for (int j=0; j<proc.dynamics.size(); j++)
      {
        String s = (String) proc.dynamics.elementAt(j);
        writeln(1, s+" := '';");
      }
      writeln("end;");
      writeln();
      generateImplementation(proc, table.useName(), table.useName()+proc.name);
    }
  }
  static void generateWithParms(Proc proc)
  {
    String semicolon = "  ";
    for (int j=0; j<proc.inputs.size(); j++)
    {
      Field field = (Field) proc.inputs.elementAt(j);
      writeln(1, semicolon+"const a"+delphiVar(field));
      semicolon = "; ";
      if (field.isNull) // && notString(field))
        writeln(1, semicolon+"const a"+field.useName()+"IsNull : boolean");
    }
    for (int j=0; j<proc.dynamics.size(); j++)
    {
      String s = (String) proc.dynamics.elementAt(j);
      writeln(1, semicolon+"const a"+s+" : AnsiString");
    }
  }
  /** Emits class method for processing the database activity */
  static void generateInterface(Proc proc)
  {
    if (proc.comments.size() > 0)
    {
      for (int i=0; i<proc.comments.size(); i++)
      {
        String comment = (String) proc.comments.elementAt(i);
        writeln(1, "//"+comment);
      }
    }
    if (proc.hasNoData())
    {
      writeln(1, "class procedure "+proc.upperFirst()+"(const Conn : TConnector);");
    }
    else if (proc.outputs.size() == 0)
    {
      writeln(1, "procedure "+proc.upperFirst()+";");
      if ((proc.inputs.size() > 0) || proc.dynamics.size() > 0)
      {
        writeln(1, "procedure "+proc.upperFirst()+"(");
        generateWithParms(proc);
        writeln(1, "); overload;");
      }
    }
    else if (proc.isSingle)
    {
      writeln(1, "function "+proc.upperFirst()+" : Boolean;");
      if ((proc.inputs.size() > 0) || proc.dynamics.size() > 0)
      {
        writeln(1, "function "+proc.upperFirst()+"(");
        generateWithParms(proc);
        writeln(1, ") : Boolean; overload;");
      }
    }
    else
    {
      writeln(1, "function "+proc.upperFirst()+" : TSQLQuery;");
      if ((proc.inputs.size() > 0) || proc.dynamics.size() > 0)
      {
        writeln(1, "function "+proc.upperFirst()+"(");
        generateWithParms(proc);
        writeln(1, ") : TSQLQuery; overload;");
      }
      writeln(1, "function next"+proc.upperFirst()+"(const Query : TSQLQuery) : Boolean;");
    }
  }
  static int questionsSeen;
  static String question(Proc proc, String line)
  {
    String result = "";
    int p;
    while ((p = line.indexOf("?")) > -1)
    {
      if (p > 0)
      {
        result = result + line.substring(0, p);
        line = line.substring(p);
      }
      Field field = (Field) proc.inputs.elementAt(questionsSeen++);
      if (field.type == field.IDENTITY && proc.isInsert)
        field = (Field) proc.inputs.elementAt(questionsSeen++);
      result = result + ":" + field.name;
      line = line.substring(1);
    }
    result = result + line;
    return result;
  }
  /**
  * Emits class method for processing the database activity
  */
  static void generateImplementation(Proc proc, String tableName, String fullName)
  {
    String with;
    if (proc.hasNoData())
      writeln("class procedure T"+fullName+"."+proc.upperFirst()+";");
    else if (proc.outputs.size() == 0)
      writeln("procedure T"+fullName+"."+proc.upperFirst()+";");
    else if (proc.isSingle)
      writeln("function T"+fullName+"."+proc.upperFirst()+" : Boolean;");
    else
      writeln("function T"+fullName+"."+proc.upperFirst()+" : TSQLQuery;");
    if (proc.dynamics.size() > 0 || proc.outputs.size() == 0 || proc.isSingle)
      writeln("var");
    if (proc.outputs.size() == 0 || proc.isSingle)
      writeln(1, "Query : TSQLQuery;");
    if (proc.dynamics.size() > 0)
      writeln(1, tableName+proc.upperFirst()+" : AnsiString;");
    writeln("begin");
    if (proc.dynamics.size() > 0)
    {
      writeln(1, tableName+proc.upperFirst()+" :=");
      generateSQLCode(proc);
    }
    if (proc.outputs.size() == 0 || proc.isSingle)
    {
      writeln(1, "Query := TSQLQuery.Create(nil);");
      writeln(1, "try");
      writeln(2, "Query.Database := Conn;");
      //writeln("    with Query do begin");
      with = "Query";
    }
    else
    {
      writeln(1, "result := TSQLQuery.Create(nil);");
      writeln(1, "try");
      writeln(2, "result.Database := Conn;");
      //writeln("    with result do begin");
      with = "result";
    }
    //writeln(2, with+".DatabaseName := Conn.DatabaseName;");
    writeln(2, with+".SQL.Text := "+tableName+proc.upperFirst()+";");
    for (int j=0; j<proc.inputs.size(); j++)
    {
      Field field = (Field) proc.inputs.elementAt(j);
      if (proc.isInsert)
      {
        if (field.isSequence)
          writeln("    "+field.useName()+" := Conn.getSequence('"+proc.table.name+"');");
      }
      if (field.type == field.TIMESTAMP)
        writeln(2, field.useName()+" := Conn.getTimeStamp;");
      if (field.type == field.USERSTAMP)
        writeln(2, field.useName()+" := Conn.getUserStamp;");
      if (field.isNull) // && notString(field))
      {
        writeln(2, "if not "+field.useName()+"IsNull then begin");
        writeln(3, with+"."+delphiInputs(field));
        writeln(2, "end else begin");
        writeln(3, "Query.Params.ParamByName('"+field.name+"').Clear;");
        writeln(3, "Query.Params.ParamByName('"+field.name+"').DataType := "+delphiDataType(field)+";");
        writeln(3, "Query.Params.ParamByName('"+field.name+"').Bound := true;");
        writeln(2, "end;");
      }
      else
        writeln(2, with+"."+delphiInputs(field));
    }
    if (proc.outputs.size() == 0)
      writeln(2, with+".ExecSQL;");
    else
    {
      writeln(2, with+".Open;");
      if (proc.isSingle)
      {
        writeln(2, "if not "+with+".eof then begin");
        for (int j=0; j<proc.outputs.size(); j++)
        {
          Field field = (Field) proc.outputs.elementAt(j);
          generateDelphiOutput(field, 1, with);
        }
        writeln(3, "result := true;");
        writeln(2, "end");
        writeln(2, "else");
        writeln(3, "result := false;");
      }
    }
    //writeln("    end;");
    if (proc.outputs.size() == 0 || proc.isSingle)
    {
      writeln(1, "finally");
      writeln(2, "Query.Free;");
    }
    else
    {
      writeln(1, "except");
      writeln(2, "result.Free;");
    }
    writeln(1, "end;");
    writeln("end;");
    writeln();
    if (proc.inputs.size() > 0 || proc.dynamics.size() > 0)
    {
      if (proc.outputs.size() == 0)
      {
        writeln("procedure T" + fullName + "." + proc.upperFirst() + "(");
        generateWithParms(proc);
        writeln("); overload;");
      }
      else if (proc.isSingle)
      {
        writeln("function T" + fullName + "." + proc.upperFirst() + "(");
        generateWithParms(proc);
        writeln(") : Boolean; overload;");
      }
      else
      {
        writeln("function T" + fullName + "." + proc.upperFirst() + "(");
        generateWithParms(proc);
        writeln(") : TSQLQuery;");
      }
      writeln("begin");
      for (int j=0; j<proc.inputs.size(); j++)
      {
        int Indent;
        Field field = (Field) proc.inputs.elementAt(j);
        Indent = 0;
        if (field.isNull) // && notString(field))
        {
          writeln(1, field.useName()+"IsNull := a"+field.useName()+"IsNull;");
          writeln(1, "if not "+field.useName()+"IsNull then");
          Indent = 1;
        }
        writeln(Indent+1, field.useName()+" := a"+field.useName()+";");
      }
      for (int j=0; j<proc.dynamics.size(); j++)
      {
        String s = (String) proc.dynamics.elementAt(j);
        writeln(1, s+" := a"+s+";");
      }
      if (proc.outputs.size() == 0)
        writeln(1, proc.upperFirst()+";");
      else
        writeln(1, "result := "+proc.upperFirst()+";");
      writeln("end;");
      writeln();
    }
    if (proc.outputs.size() != 0 && !proc.isSingle)
    {
      writeln("function T"+fullName+".next"+proc.upperFirst()+"(const Query : TSQLQuery) : Boolean;");
      writeln("begin");
      writeln(1, "if not Query.eof then begin");
      for (int j=0; j<proc.outputs.size(); j++)
      {
        Field field = (Field) proc.outputs.elementAt(j);
        generateDelphiOutput(field, 0, "Query");
      }
      writeln(2, "result := true;");
      writeln(2, "Query.next;");
      writeln(1, "end");
      writeln(1, "else");
      writeln(2, "result := false;");
      writeln("end;");
      writeln();
    }
  }
  /**
  * Emits SQL Code
  */
  static void generateDelphiOutput(Field field, int gap, String with)
  {
    if (field.isNull) // && notString(field))
    {
      writeln(2+gap, field.useName()+"IsNull := "+with+".FieldByName('"+field.name+"').isNull;");
      writeln(2+gap, "if not "+field.useName()+"IsNull then");
      writeln(3+gap, ""+delphiOutputs(field, with));
    }
    else
      writeln(3+gap, delphiOutputs(field, with));
  }
  /**
  * Emits SQL Code
  */
  static void generateSQLCode(Proc proc)
  {
    questionsSeen = 0;
    for (int i=0; i < proc.lines.size(); i++)
    {
      String x;
      if (i+1 < proc.lines.size())
        x = " +";
      else
        x = ";";
      Line l = (Line) proc.lines.elementAt(i);
      if (l.isVar)
        writeln(2, l.line+x);
      else
      {
        String out = "'"+question(proc, l.line)+"'"+x;
        writeln(2, out);
      }
    }
  }
  /**
  * Translates field type to delphi data member type
  */
  static String delphiVar(Field field)
  {
    switch(field.type)
    {
    case Field.BOOLEAN:
      return field.useName() + " : Boolean";
    case Field.BYTE:
      return field.useName() + " : Shortint";
    case Field.SHORT:
      return field.useName() + " : Smallint";
    case Field.INT:
    case Field.SEQUENCE:
    case Field.IDENTITY:
      return field.useName() + " : Integer";
    case Field.LONG:
      return field.useName() + " : Longint";
    case Field.CHAR:
    case Field.ANSICHAR:
      return field.useName() + " : AnsiString";
    case Field.DATE:
    case Field.DATETIME:
    case Field.TIME:
    case Field.TIMESTAMP:
      return field.useName() + " : TDateTime";
    case Field.FLOAT:
    case Field.DOUBLE:
      return field.useName() + " : Double";
    case Field.BLOB:
    case Field.TLOB:
      return field.useName() + " : AnsiString";
    case Field.MONEY:
      return field.useName() + " : Double";
    case Field.USERSTAMP:
      return field.useName() + " : AnsiString";
    }
    return field.useName() + " : <unsupported>";
  }
  /**
  * returns the data member initialisation code (not always neccessary in java but
  * still we do it)
  */
  static String initDelphiVar(Field field)
  {
    switch(field.type)
    {
    case Field.BOOLEAN:
      return field.useName() +" := false;";
    case Field.DATE:
    case Field.DATETIME:
    case Field.TIMESTAMP:
      return field.useName() +" := Date;";
    case Field.FLOAT:
    case Field.DOUBLE:
    case Field.MONEY:
      return field.useName() +" := 0.0;";
    case Field.BYTE:
    case Field.SHORT:
    case Field.INT:
    case Field.LONG:
    case Field.SEQUENCE:
    case Field.IDENTITY:
      return field.useName() +" := 0;";
    case Field.TIME:
      return field.useName() +" := Time;";
    case Field.CHAR:
    case Field.ANSICHAR:
    case Field.USERSTAMP:
    case Field.BLOB:
    case Field.TLOB:
      return field.useName() +" := '';";
    }
    return field.useName() +"<unsupported>";
  }
  /**
  */
  static String delphiInputs(Field field)
  {
    switch(field.type)
    {
    case Field.BOOLEAN:
      return "Params.ParamByName('"+field.name+"').AsBoolean := "+field.useName()+";";
    case Field.DATE:
      return "Params.ParamByName('"+field.name+"').AsDateTime := "+field.useName()+";";
    case Field.DATETIME:
      return "Params.ParamByName('"+field.name+"').AsDateTime := "+field.useName()+";";
    case Field.FLOAT:
    case Field.DOUBLE:
    case Field.MONEY:
      return "Params.ParamByName('"+field.name+"').AsFloat := "+field.useName()+";";
    case Field.BYTE:
    case Field.SHORT:
    case Field.INT:
    case Field.LONG:
      return "Params.ParamByName('"+field.name+"').AsInteger := "+field.useName()+";";
    case Field.IDENTITY:
    case Field.SEQUENCE:
      return "Params.ParamByName('"+field.name+"').AsInteger := "+field.useName()+";";
    case Field.TIME:
    case Field.TIMESTAMP:
      return "Params.ParamByName('"+field.name+"').AsDateTime := "+field.useName()+";";
    case Field.CHAR:
    case Field.ANSICHAR:
    case Field.USERSTAMP:
      return "Params.ParamByName('"+field.name+"').AsString := "+field.useName()+";";
    case Field.BLOB:
      return "Params.ParamByName('"+field.name+"').AsBlob := "+field.useName()+";";
    case Field.TLOB:
      return "Params.ParamByName('"+field.name+"').AsMemo := "+field.useName()+";";
    }
    return field.useName() +"<unsupported>";
  }
  /**
  */
//  static boolean notString(Field field)
//  {
//    switch(field.type)
//    {
//    case Field.BOOLEAN:
//    case Field.DATE:
//    case Field.DATETIME:
//    case Field.FLOAT:
//    case Field.DOUBLE:
//    case Field.MONEY:
//    case Field.BYTE:
//    case Field.SHORT:
//    case Field.INT:
//    case Field.LONG:
//    case Field.IDENTITY:
//    case Field.SEQUENCE:
//    case Field.TIME:
//    case Field.TIMESTAMP:
//    case Field.BLOB:
//    case Field.TLOB:
//      return true;
//    }
//    return false;
//  }
  /**
  */
  static String delphiOutputs(Field field, String with)
  {
    switch(field.type)
    {
    case Field.BOOLEAN:
      return field.useName() +" := "+with+".FieldByName('"+field.name+"').AsBoolean;";
    case Field.BYTE:
    case Field.SHORT:
    case Field.INT:
    case Field.SEQUENCE:
    case Field.IDENTITY:
    case Field.LONG:
      return field.useName() +" := "+with+".FieldByName('"+field.name+"').AsInteger;";
    case Field.DATE:
      return field.useName() +" := "+with+".FieldByName('"+field.name+"').AsDateTime;";
    case Field.DATETIME:
      return field.useName() +" := "+with+".FieldByName('"+field.name+"').AsDateTime;";
    case Field.FLOAT:
    case Field.DOUBLE:
      return field.useName() +" := "+with+".FieldByName('"+field.name+"').AsFloat;";
    case Field.MONEY:
      return field.useName() +" := "+with+".FieldByName('"+field.name+"').AsCurrency;";
    case Field.TIME:
    case Field.TIMESTAMP:
      return field.useName() +" := "+with+".FieldByName('"+field.name+"').AsDateTime;";
    case Field.CHAR:
    case Field.ANSICHAR:
    case Field.USERSTAMP:
      return field.useName() +" := "+with+".FieldByName('"+field.name+"').AsString;";
    case Field.BLOB:
    case Field.TLOB:
      return field.useName() +" := "+with+".FieldByName('"+field.name+"').AsString;";
    }
    return field.useName() +"<unsupported>";
  }
  /**
  */
  static String delphiDataType(Field field)
  {
    switch(field.type)
    {
    case Field.BOOLEAN:
    case Field.DATE:
      return "ftDate";
    case Field.DATETIME:
    case Field.TIMESTAMP:
      return "ftDateTime";
    case Field.FLOAT:
    case Field.DOUBLE:
    case Field.MONEY:
      return "ftFloat";
    case Field.BYTE:
    case Field.SHORT:
    case Field.INT:
    case Field.LONG:
      return "ftInteger";
    case Field.TIME:
      return "ftTime";
    }
    return "ftString";
  }
}

