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

package vlab.crackle;

import java.io.Serializable;

/**
 *
 */
public class Operation implements Serializable
{
  public static final byte
          SIZE = 1, DYNAMIC = 2;
  private static final long serialVersionUID = 1L;
  public String name;
  public byte code;
  public boolean isConstant;
  public Field field;

  public Operation()
  {
    code = 0;
    name = "";
    isConstant = false;
    field = null;
  }
  //public boolean byPtr(Prototype prototype)
  //{
  //  if (isConstant == true || code != SIZE) return false;
  //  if (field == null)
  //  {
  //    for (int i=0; i<prototype.parameters.size(); i++)
  //    {
  //      Field f = (Field)prototype.parameters.elementAt(i);
  //      if (f.name.compareTo(name) == 0)
  //        return f.type.typeof == Type.BYREFPTR || f.type.typeof == Type.BYPTR;
  //    }
  //    return false;
  //  }    
  //  return field.type.typeof == Type.BYREFPTR || field.type.typeof == Type.BYPTR;
  //}
}
