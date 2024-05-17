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

public class MSSql implements Serializable
{
  private static final long serialVersionUID = 1L;
  public MSSql()
  {
  }
  static public StringBuffer insertCommand(Proc proc)
  {
    String name = proc.table.tableName();
    StringBuffer _command = new StringBuffer();
    _command.append(format("insert into %s\n", name));
    Vector<Field> fields = proc.table.fields;
    boolean returnSeq = false;
    Field retField = null;
    String comma = "( ";
    for (Field field : fields)
    {
      if (field.isSequence)
      {
        returnSeq = true;
        retField = field;
      }
      else if (field.isIdentity)
      {
        retField = field;
        continue;
      }
      _command.append(format("%s%s\n", comma, field.useName()));
      comma = ", ";
    }
    _command.append(")\n");
    if (retField != null)
      _command.append(format("output inserted.%s", retField.useName()));
    _command.append("values\n");
    comma = "( ";
    for (Field field : fields)
    {
      if (field.isIdentity)
        continue;
      if (field.isSequence)
        _command.append(format("%snextval for %sSeq", comma, name));
      else
        _command.append(format("%s:%s", comma, field.useName()));
      comma = ", ";
    }
    _command.append(")\n");
    return _command;
  }
}



