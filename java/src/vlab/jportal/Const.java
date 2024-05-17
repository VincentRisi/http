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

package vlab.jportal;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.Serializable;
import java.util.Vector;

public class Const implements Serializable
{
  private static final long serialVersionUID = 1L;
  public String name;
  public Vector<Value> values;
  public Const()
  {
    name = "";
    values = new Vector<Value>();
  }
  public void reader(DataInputStream ids) throws IOException
  {
    name = ids.readUTF();
    int noOf = ids.readInt();
    for (int i=0; i<noOf; i++)
    {
      Value v = new Value();
      v.reader(ids);
      values.addElement(v);
    }
  }
  public void writer(DataOutputStream ods) throws IOException
  {
    ods.writeUTF(name);
    ods.writeInt(values.size());
    for (int i=0; i<values.size(); i++)
    {
      Value v = values.elementAt(i);
      v.writer(ods);
    }
  }
}
