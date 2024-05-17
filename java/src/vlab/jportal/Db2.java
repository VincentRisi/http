/// ------------------------------------------------------------------
/// Copyright (c) from 1996 Vincent Risi 
///
/// All rights reserved. 
/// This program and the accompanying materials are made available 
/// under the terms of the Common Public License v1.0 
/// which accompanies this distribution and is available at 
/// http://www.eclipse.org/legal/cpl-v10.html 
/// Contributors:
///    Vincent Risi
/// ------------------------------------------------------------------
package vlab.jportal;

import java.io.Serializable;
import java.util.Vector;

import static vlab.jportal.Writer.format;

public class Db2 implements Serializable
{
  private static final long serialVersionUID = 1L;
  public Db2()
  {
  }
  static public StringBuffer insertCommand(Proc proc)
  {
    String name = proc.table.tableName();
    StringBuffer _command = new StringBuffer();
    boolean returnSeq = false;
    Vector<Field> fields = proc.table.fields;
    for (Field field : fields)
    {
      if (field.isSequence)
      {
        _command.append(format("select %s from final table (\n", field.useName()));
        returnSeq = true;
      }
    }
    _command.append(format("insert into %s\n", name));
    String comma = "( ";
    for (Field field : fields)
    {
      _command.append(format("%s%s\n", comma, field.useName()));
      comma = ", ";
    }
    _command.append(")\n");
    _command.append("values\n");
    comma = "( ";
    for (Field field : fields)
    {
      if (field.isSequence)
        _command.append(format("%snextval for %sSeq", comma, name));
      else
        _command.append(format("%s:%s", comma, field.useName()));
      comma = ", ";
    }
    if (returnSeq)
      _command.append(")");
    _command.append(")\n");
    return _command;
  }
}



